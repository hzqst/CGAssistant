#include "scriptform.h"
#include "ui_scriptform.h"
#include "mywebview.h"
#include <QWebEngineView>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>

#include "../CGALib/gameinterface.h"
extern CGA::CGAInterface *g_CGAInterface;

ScriptForm::ScriptForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptForm)
{
    ui->setupUi(this);

    m_webview = new MyWebView(this);
    ui->verticalLayout_debug->addWidget(m_webview);
    m_webview->hide();

    m_output = new QTextEdit(this);
    ui->verticalLayout_debug->addWidget(m_output);
    m_output->setReadOnly(true);
    m_output->show();

    m_bDebugging = false;
    m_bListening = false;
    m_port = 0;

    m_node = new QProcess();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("PATH", QDir::currentPath());
    m_node->setProcessEnvironment(env);

    connect(m_node, &QProcess::started, this, &ScriptForm::OnNodeStarted);
    connect(m_node, &QProcess::errorOccurred, this, &ScriptForm::OnNodeStartError);
    connect(m_node, SIGNAL(readyReadStandardError()), this, SLOT(OnNodeReadyReadStdErr()));
    connect(m_node, SIGNAL(readyReadStandardOutput()), this, SLOT(OnNodeReadyReadStdOut()));
    connect(m_node, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnNodeFinish(int, QProcess::ExitStatus)));
}

void ScriptForm::OnNodeStarted()
{
    if(m_bDebugging){
        ui->label_status->setText(tr("Debugging with pid %1...").arg(m_node->processId()));
    } else {
        ui->label_status->setText(tr("Running with pid %1...").arg(m_node->processId()));
    }
    ui->pushButton_kill->setEnabled(true);
}

void ScriptForm::OnNodeStartError(QProcess::ProcessError error)
{
    ui->label_status->setText(tr("Failed to start node, error: %1").arg(m_node->errorString()));
    ui->pushButton_run->setEnabled(true);
    ui->pushButton_debug->setEnabled(true);
    ui->pushButton_kill->setEnabled(false);
}

void ScriptForm::OnNodeReadyReadStdOut()
{
    if(!m_bListening)
        return;
    if(!m_bDebugging)
    {
        QString data = m_node->readAllStandardOutput();
        m_output->moveCursor(QTextCursor::End);
        m_output->insertPlainText(data);
    }
}

void ScriptForm::OnNodeReadyReadStdErr()
{
    if(!m_bListening)
        return;
    if(m_bDebugging)
    {
        QByteArray data = m_node->readAllStandardError();
        QLatin1String pattern("chrome-devtools://devtools/bundled/inspector.html");
        int findStart = data.indexOf(pattern);
        if(findStart != -1)
        {
            findStart += pattern.size();
            int findEnd = data.indexOf(QLatin1String("\r\n"), findStart);
            QString url = QLatin1String("qrc:/devtools/bundled/inspector.html");
            if(findEnd == -1)
                url += data.mid(findStart);
            else
                url += data.mid(findStart, findEnd-findStart);

            m_webview->load(QUrl(url));

            m_bListening = false;
        }
    }
    else
    {
        QString data = m_node->readAllStandardError();
        m_output->moveCursor(QTextCursor::End);
        m_output->insertPlainText(data);
    }
}

void ScriptForm::OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    ui->label_status->setText(tr("Node finished with exitCode %1").arg(exitCode));

    ui->pushButton_load->setEnabled(true);
    ui->pushButton_run->setEnabled(true);
    ui->pushButton_debug->setEnabled(true);
    ui->pushButton_kill->setEnabled(false);

    //restore everything...
    g_CGAInterface->FixMapWarpStuck(2);
}

void ScriptForm::OnCloseWindow()
{
    if(m_node->state() == QProcess::Running)
    {
        m_node->kill();
        m_node->waitForFinished();
    }
    m_node->deleteLater();
    m_node = NULL;
}

ScriptForm::~ScriptForm()
{
    delete ui;
}

void ScriptForm::on_pushButton_load_clicked()
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("Load Script"));
    fileDialog->setDirectory(".");
    fileDialog->setNameFilter(tr("JavaScript Files(*.js)"));
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    if(fileDialog->exec() == QDialog::Accepted)
    {
        QString filePath = fileDialog->selectedFiles()[0];
        QFile file(filePath);
        if(file.exists())
        {
            m_scriptPath = filePath;
            ui->label_status->setText(tr("Ready to launch"));
            ui->pushButton_run->setEnabled(true);
            ui->pushButton_debug->setEnabled(true);
            ui->pushButton_kill->setEnabled(false);
            ui->lineEdit_scriptPath->setText(filePath);
        } else {
            QMessageBox::critical(this, tr("Error"), tr("Failed to load script file.\nerror: %1").arg(file.errorString()), QMessageBox::Ok, 0);
        }
    }
}

void ScriptForm::on_pushButton_run_clicked()
{
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_debug->setEnabled(false);
    ui->pushButton_kill->setEnabled(false);
    ui->pushButton_load->setEnabled(false);

    ui->label_status->setText(tr("Starting node..."));

    m_bDebugging = false;
    m_bListening = true;

    m_output->clear();
    m_output->show();
    m_webview->hide();

    QStringList args;
    QFileInfo fileInfo(m_scriptPath);

    args.append(fileInfo.fileName());
    if(m_port != 0)
        args.append(QString::number(m_port));

    m_node->setWorkingDirectory(fileInfo.dir().absolutePath());
    m_node->start("node.exe", args);
}

void ScriptForm::on_pushButton_debug_clicked()
{
    ui->pushButton_run->setEnabled(false);
    ui->pushButton_debug->setEnabled(false);
    ui->pushButton_kill->setEnabled(false);
    ui->pushButton_load->setEnabled(false);

    ui->label_status->setText(tr("Starting node..."));

    m_bDebugging = true;
    m_bListening = true;

    m_output->hide();
    m_webview->show();

    QStringList args;
    QFileInfo fileInfo(m_scriptPath);

    args.append("--inspect-brk");
    args.append(fileInfo.fileName());
    if(m_port != 0)
        args.append(QString::number(m_port));

    m_node->setWorkingDirectory(fileInfo.dir().absolutePath());
    m_node->start("node.exe", args);
}

void ScriptForm::on_pushButton_kill_clicked()
{
    if(ui->pushButton_kill->isEnabled()){
        ui->pushButton_kill->setEnabled(false);

        m_node->kill();
        m_webview->load(QUrl());

        if(m_bDebugging)
            m_output->clear();

        m_output->show();
        m_webview->hide();
    }
}

void ScriptForm::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd)
{
    m_port = port;
}
