#include "accountform.h"
#include "ui_accountform.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

#include "../CGALib/gameinterface.h"

extern CGA::CGAInterface *g_CGAInterface;

bool IsProcessThreadPresent(quint32 ProcessId, quint32 ThreadId);

AccountForm::AccountForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountForm)
{
    ui->setupUi(this);

    m_POLCN = new QProcess();
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    m_POLCN->setProcessEnvironment(env);

    connect(m_POLCN, SIGNAL(started()), this, SLOT(OnPOLCNStarted()));
    connect(m_POLCN, SIGNAL(readyReadStandardOutput()), this, SLOT(OnPOLCNReadyReadStdOut()));
    connect(m_POLCN, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(OnPOLCNFinish(int, QProcess::ExitStatus)));

    ui->comboBox_gameType->addItem(QObject::tr("cg_item_6000 (HN)"), QVariant(4));
    ui->comboBox_gameType->addItem(QObject::tr("cg_item_6000 (WT)"), QVariant(40));
    ui->comboBox_gameType->addItem(QObject::tr("cg_se_6000"), QVariant(1));
    ui->comboBox_gameType->addItem(QObject::tr("cg_se_3000"), QVariant(11));

    ui->comboBox_bigserver->addItem(QObject::tr("Aries"), QVariant(0));
    ui->comboBox_bigserver->addItem(QObject::tr("Gemini"), QVariant(2));

    for(int i = 0;i < 10; ++i)
        ui->comboBox_server->addItem(QObject::tr("#%1").arg(i + 1), QVariant(i));

    ui->comboBox_character->addItem(QObject::tr("Left"), QVariant(0));
    ui->comboBox_character->addItem(QObject::tr("Right"), QVariant(1));

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnAutoLogin()));
    timer->start(1000);

    m_game_pid = 0;
    m_game_tid = 0;
    m_serverid = 0;
}

AccountForm::~AccountForm()
{
    delete ui;
}

void AccountForm::OnAutoLogin()
{
    if(g_CGAInterface->IsConnected())
    {
        if(ui->checkBox_autoLogin->isChecked())
        {
            int gameStatus = 0;
            int worldStatus = 0;
            if(g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
            {
                if(worldStatus == 2 && gameStatus == 1)
                {
                    if(m_loginresult.elapsed() > 1000*60 || m_glt.isEmpty())
                    {
                        on_pushButton_getgid_clicked();
                    }
                    else
                    {
                        on_pushButton_logingame_clicked();
                    }
                }
            }
        }
        else
        {
            g_CGAInterface->LoginGameServer("", "", 0, 0, 0, 0);
        }
    }
    else
    {
        if(ui->checkBox_autoLogin->isChecked() && (!m_game_pid || !IsProcessThreadPresent(m_game_pid, m_game_tid)) )
        {
            on_pushButton_getgid_clicked();
        }
    }
}

void AccountForm::OnPOLCNReadyReadStdOut()
{

}

void AccountForm::OnPOLCNStarted()
{
     ui->label_status->setText(tr("POLCN_Launcher running with pid %1...").arg(m_POLCN->processId()));
     ui->textEdit_output->setText(tr("Querying GID list..."));
}

void AccountForm::OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    auto out = m_POLCN->readAllStandardOutput();

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(out, &err);
    if(err.error == QJsonParseError::NoError && doc.isObject())
    {
        QJsonObject obj = doc.object();
        if(obj.contains("result"))
        {
            auto result = obj.take("result");
            if(result == 0)
            {
                ui->textEdit_output->setText(tr("Get GID ok.\n"));
                ui->textEdit_output->append(out);

                auto lastgid = ui->comboBox_gid->currentText();
                ui->comboBox_gid->clear();
                auto gid_array = obj.take("gid").toArray();
                for(auto i = 0; i<gid_array.count(); ++i)
                {
                    auto gid_object = gid_array[i].toObject();
                    ui->comboBox_gid->addItem(gid_object.take("name").toString());
                }

                if(!lastgid.isEmpty())
                {
                    for(auto i = 0; i< ui->comboBox_gid->count(); ++i)
                    {
                        if(lastgid == ui->comboBox_gid->itemText(i))
                            ui->comboBox_gid->setCurrentIndex(i);
                    }
                }

                m_serverid = obj.take("serverid").toInt();
                m_glt = obj.take("glt").toString();
                m_loginresult.start();

                if(obj.find("game_pid") != obj.end())
                {
                    m_game_pid = (quint32)obj.take("game_pid").toInt();
                    m_game_tid = (quint32)obj.take("game_tid").toInt();
                    NotifyAutoAttachProcess(m_game_pid, m_game_tid);
                }

                if(ui->checkBox_autoLogin->isChecked())
                {
                    on_pushButton_logingame_clicked();
                }
            }
            else
            {
                ui->textEdit_output->setText(tr("Login refused by server.\n"));
                ui->textEdit_output->append(out);
            }
        }
        else
        {
            ui->textEdit_output->setText(tr("Fail to parse POLCN_Launcher output, missing \"result\".\n"));
            ui->textEdit_output->append(out);
        }
    }
    else
    {
        ui->textEdit_output->setText(tr("Fail to parse POLCN_Launcher output.\n"));
        ui->textEdit_output->append(out);
    }

    ui->label_status->setText(tr("POLCN_Launcher finished with exitCode %1").arg(exitCode));
}

void AccountForm::on_pushButton_getgid_clicked()
{
    if(m_POLCN->state() != QProcess::ProcessState::Running)
    {
        QString argstr = QString("-account %1 -pwd %2 -gametype %3 -rungame %4 -skipupdate %5")
                .arg(ui->lineEdit_account->text())
                .arg(ui->lineEdit_password->text())
                .arg(ui->comboBox_gameType->currentData().toInt())
                .arg(g_CGAInterface->IsConnected() ? "0" : "1")
                .arg(ui->checkBox_disableUpdater->isChecked() ? "1" : "0");

        m_POLCN->setNativeArguments(argstr);
        m_POLCN->start("POLCN_Launcher.exe");
    }
}

void AccountForm::on_pushButton_logingame_clicked()
{
    if(g_CGAInterface->IsConnected())
    {
        g_CGAInterface->LoginGameServer(ui->comboBox_gid->currentText().toStdString(),
                                        m_glt.toStdString(),
                                        m_serverid,
                                        ui->comboBox_bigserver->currentData().toInt(),
                                        ui->comboBox_server->currentData().toInt(),
                                        ui->comboBox_character->currentData().toInt());
    }
}

void AccountForm::OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid, int bigserver, int server, int character, bool autologin, bool skipupdate)
{
    if(game == 4)
        ui->comboBox_gameType->setCurrentIndex(0);
    if(game == 40)
        ui->comboBox_gameType->setCurrentIndex(1);
    if(game == 1)
        ui->comboBox_gameType->setCurrentIndex(2);
    if(game == 11)
        ui->comboBox_gameType->setCurrentIndex(3);

    if(!user.isEmpty())
        ui->lineEdit_account->setText(user);
    if(!pwd.isEmpty())
        ui->lineEdit_password->setText(pwd);
    if(!gid.isEmpty())
        ui->comboBox_gid->setEditText(gid);

    if(bigserver >= 1 && bigserver <= 2)
        ui->comboBox_bigserver->setCurrentIndex(bigserver - 1);

    if(server >= 1 && server <= 10)
        ui->comboBox_server->setCurrentIndex(server - 1);

    if(character >= 1 && character <= 2)
        ui->comboBox_character->setCurrentIndex(character - 1);

    if(autologin)
        ui->checkBox_autoLogin->setChecked(true);

    if(skipupdate)
        ui->checkBox_disableUpdater->setChecked(true);
}
