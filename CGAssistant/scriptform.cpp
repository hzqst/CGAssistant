#include "scriptform.h"
#include "ui_scriptform.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTimer>
#include "../CGALib/gameinterface.h"

extern CGA::CGAInterface *g_CGAInterface;

ScriptForm::ScriptForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScriptForm)
{
    ui->setupUi(this);

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

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoRestart()));
    timer->start(1000);
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
    else {
        QByteArray data = m_node->readAllStandardOutput();
        QLatin1String pattern("Debugger listening on ws://");
        qDebug(data.data());
        int findStart = data.indexOf(pattern);
        if(findStart != -1)
        {
            findStart += pattern.size();
            int findEnd = data.indexOf(QLatin1String("\r\n"), findStart);
            QString url = QLatin1String("chrome-devtools://devtools/bundled/js_app.html?experiments=true&v8only=true&ws=");
            if(findEnd == -1)
                url += data.mid(findStart);
            else
                url += data.mid(findStart, findEnd-findStart);

            m_output->insertPlainText(QString("Visit %1").arg(url));

            m_bListening = false;
        }
    }
}

void ScriptForm::OnNodeReadyReadStdErr()
{
    if(!m_bListening)
        return;
    if(m_bDebugging)
    {
        QByteArray data = m_node->readAllStandardError();
        QLatin1String pattern("Debugger listening on ws://");

        qDebug(data.data());
        int findStart = data.indexOf(pattern);
        if(findStart != -1)
        {
            findStart += pattern.size();
            int findEnd = data.indexOf(QLatin1String("\r\n"), findStart);
            QString url = QLatin1String("chrome-devtools://devtools/bundled/js_app.html?experiments=true&v8only=true&ws=");
            if(findEnd == -1)
                url += data.mid(findStart);
            else
                url += data.mid(findStart, findEnd-findStart);

            m_output->insertPlainText(QString("Visit %1").arg(url));

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

void ScriptForm::OnAutoRestart()
{
    if(g_CGAInterface->IsConnected())
    {
        int ingame = 0;
        if(g_CGAInterface->IsInGame(ingame) && ingame)
        {
            CGA::cga_player_info_t playerinfo;
            if(g_CGAInterface->GetPlayerInfo(playerinfo))
            {
                if(playerinfo.health != 0 && ui->checkBox_injuryProt->isChecked())
                {
                    if(m_node->state() == QProcess::Running)
                    {
                        on_pushButton_kill_clicked();
                        return;
                    }
                }
                if(playerinfo.souls != 0 && ui->checkBox_soulProt->isChecked())
                {
                    if(m_node->state() == QProcess::Running)
                    {
                        on_pushButton_kill_clicked();
                        return;
                    }
                }
            }

            if(ui->checkBox_autorestart->isChecked() && m_node->state() != QProcess::Running && !m_scriptPath.isEmpty())
            {
                int worldStatus = 0, gameStatus = 0;
                if(g_CGAInterface->GetWorldStatus(worldStatus) && g_CGAInterface->GetGameStatus(gameStatus) && worldStatus == 9 && gameStatus == 3)
                {
                    on_pushButton_run_clicked();
                    return;
                }
            }
        }
        else
        {
            if(m_node->state() == QProcess::Running)
            {
                on_pushButton_kill_clicked();
                return;
            }
        }
    }
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
    if(ui->pushButton_run->isEnabled() && m_node->state() != QProcess::Running){
        ui->pushButton_run->setEnabled(false);
        ui->pushButton_debug->setEnabled(false);
        ui->pushButton_kill->setEnabled(false);
        ui->pushButton_load->setEnabled(false);

        ui->label_status->setText(tr("Starting node..."));

        m_bDebugging = false;
        m_bListening = true;

        m_output->clear();

        QStringList args;
        QFileInfo fileInfo(m_scriptPath);

        args.append(fileInfo.fileName());
        if(m_port != 0)
            args.append(QString::number(m_port));

        m_node->setWorkingDirectory(fileInfo.dir().absolutePath());
        m_node->start("node.exe", args);
    }
}

void ScriptForm::on_pushButton_debug_clicked()
{
    if(ui->pushButton_debug->isEnabled() && m_node->state() != QProcess::Running){
        ui->pushButton_run->setEnabled(false);
        ui->pushButton_debug->setEnabled(false);
        ui->pushButton_kill->setEnabled(false);
        ui->pushButton_load->setEnabled(false);

        ui->label_status->setText(tr("Starting node in debug mode..."));

        m_bDebugging = true;
        m_bListening = true;

        QStringList args;
        QFileInfo fileInfo(m_scriptPath);

        args.append("--inspect");
        args.append(fileInfo.fileName());
        if(m_port != 0)
            args.append(QString::number(m_port));

        m_node->setWorkingDirectory(fileInfo.dir().absolutePath());
        m_node->start("node.exe", args);
    }
}

void ScriptForm::on_pushButton_kill_clicked()
{
    if(ui->pushButton_kill->isEnabled() && m_node->state() == QProcess::Running){
        ui->pushButton_kill->setEnabled(false);

        m_node->kill();

        if(m_bDebugging)
            m_output->clear();
    }
}

void ScriptForm::OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd)
{
    m_port = port;
}

void ScriptForm::OnNotifyFillLoadScript(QString path, bool autorestart, bool injuryprot, bool soulprot)
{
    if(!path.isEmpty())
    {
        QFile file(path);
        if(file.exists())
        {
            m_scriptPath = path;
            ui->label_status->setText(tr("Ready to launch"));
            ui->pushButton_run->setEnabled(true);
            ui->pushButton_debug->setEnabled(true);
            ui->pushButton_kill->setEnabled(false);
            ui->lineEdit_scriptPath->setText(path);
            on_pushButton_run_clicked();
        }
    }

    if(autorestart)
        ui->checkBox_autorestart->setChecked(true);
    if(injuryprot)
        ui->checkBox_injuryProt->setChecked(true);
    if(soulprot)
        ui->checkBox_soulProt->setChecked(true);
}
