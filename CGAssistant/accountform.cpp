#include "accountform.h"
#include "ui_accountform.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <windows.h>

#include "../CGALib/gameinterface.h"

extern CGA::CGAInterface *g_CGAInterface;

bool IsProcessThreadPresent(quint32 ProcessId, quint32 ThreadId);

AccountForm::AccountForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountForm)
{
    ui->setupUi(this);

    m_polcn_lock = NULL;
    m_polcn_map = NULL;

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
    if(m_loginquery.elapsed() > 15 * 1000)
    {
        if(m_POLCN->state() == QProcess::ProcessState::Running)
        {
            m_POLCN->kill();
            return;
        }
    }

    if(!ui->checkBox_autoLogin->isChecked())
    {
        if(g_CGAInterface->IsConnected())
            g_CGAInterface->LoginGameServer("", "", 0, 0, 0, 0);

        ui->label_status->setText(tr("Auto-Login off."));
        if(m_polcn_lock)
        {
            CloseHandle(m_polcn_lock);
            m_polcn_lock = NULL;
        }
        if(m_polcn_map)
        {
            CloseHandle(m_polcn_map);
            m_polcn_map = NULL;
        }
        return;
    }

    if(g_CGAInterface->IsConnected())
    {
        int ingame = 0;
        if(g_CGAInterface->IsInGame(ingame) && ingame == 1)
        {
            ui->textEdit_output->setText(tr("Game-Login succeeded.\n"));
            ui->label_status->setText(tr("Game-Login succeeded."));
            if(m_polcn_lock)
            {
                CloseHandle(m_polcn_lock);
                m_polcn_lock = NULL;
            }
            if(m_polcn_map)
            {
                CloseHandle(m_polcn_map);
                m_polcn_map = NULL;
            }
            return;
        }

        int gameStatus = 0;
        int worldStatus = 0;
        if(g_CGAInterface->GetGameStatus(gameStatus) && g_CGAInterface->GetWorldStatus(worldStatus))
        {
            if(worldStatus == 2 && gameStatus == 1)
            {
                if(m_loginresult.elapsed() > 1000*60 || m_glt.isEmpty())
                {
                    on_pushButton_getgid_clicked();
                    return;
                }
                else
                {
                    on_pushButton_logingame_clicked();
                    return;
                }
            }
        }
    }
    else if(!m_game_pid || !IsProcessThreadPresent(m_game_pid, m_game_tid))
    {
        on_pushButton_getgid_clicked();
    }
}

void AccountForm::OnPOLCNReadyReadStdOut()
{
    m_StdOut += m_POLCN->readAllStandardOutput();
}

void AccountForm::OnPOLCNStarted()
{
     ui->label_status->setText(tr("POLCN_Launcher running with pid %1...").arg(m_POLCN->processId()));
     ui->textEdit_output->setText(tr("Querying GID list..."));
}

void AccountForm::OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_StdOut += m_POLCN->readAllStandardOutput();

    QJsonParseError err;
    auto doc = QJsonDocument::fromJson(m_StdOut, &err);
    if(err.error == QJsonParseError::NoError && doc.isObject())
    {
        QJsonObject obj = doc.object();
        if(obj.contains("result"))
        {
            auto result = obj.take("result");
            if(result == 0)
            {
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

                ui->textEdit_output->setText(tr("Get GID ok.\n"));
                ui->textEdit_output->append(m_StdOut);

                if(obj.find("game_pid") != obj.end())
                {
                    m_game_pid = (quint32)obj.take("game_pid").toInt();
                    m_game_tid = (quint32)obj.take("game_tid").toInt();
                    NotifyAutoAttachProcess(m_game_pid, m_game_tid);

                    ui->textEdit_output->append(tr("Wait for game process to login...\n"));
                }

                if(ui->checkBox_autoLogin->isChecked())
                {
                    on_pushButton_logingame_clicked();
                }
            }
            else
            {
                ui->textEdit_output->setText(tr("Login refused by server.\n"));
                ui->textEdit_output->append(m_StdOut);
            }
        }
        else
        {
            ui->textEdit_output->setText(tr("Fail to parse POLCN_Launcher output, missing \"result\".\n"));
            ui->textEdit_output->append(m_StdOut);
        }
    }
    else
    {
        ui->textEdit_output->setText(tr("Fail to parse POLCN_Launcher output.\n"));
        ui->textEdit_output->append(m_StdOut);
    }

    ui->label_status->setText(tr("POLCN_Launcher finished with exitCode %1").arg(exitCode));
}

bool IsProcessThreadPresent(quint32 ProcessId, quint32 ThreadId);

bool AccountForm::QueryAccount(QString &label, QString &errorMsg)
{
    if(m_POLCN->state() == QProcess::ProcessState::Running)
        return false;
    if(!m_polcn_lock)
    {
        m_polcn_lock = CreateMutexW(NULL, TRUE, L"CGAPolcnLock");
        if(m_polcn_lock && GetLastError() == ERROR_ALREADY_EXISTS)
        {
            auto polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
            if(polcn_map)
            {
                auto polcn_view = MapViewOfFile(polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
                if(polcn_view)
                {
                    uint32_t polcn_cga_pid = *(uint32_t *)((char *)polcn_view + 0);

                    label = tr("Waiting for other CGAssistant to login...");
                    errorMsg = tr("Waiting for CGAssistant pid #%1 to login...").arg( polcn_cga_pid );

                    UnmapViewOfFile(polcn_view);
                }
                else
                {
                    label = tr("Waiting for other CGAssistant to release POLCN login lock...");
                    errorMsg = tr("Waiting for other CGAssistant to release POLCN login lock...");
                }
                CloseHandle(polcn_map);
            }
            else
            {
                label = tr("Waiting for other CGAssistant to release POLCN login lock...");
                errorMsg = tr("Waiting for other CGAssistant to release POLCN login lock...");
            }
            CloseHandle(m_polcn_lock);
            m_polcn_lock = NULL;
            return false;
        }
        else if(!m_polcn_lock)
        {
            label = tr("Failed to acquire POLCN login lock...");
            errorMsg = tr("Failed to acquire POLCN login lock, error code %1...").arg( GetLastError() );
            return false;
        }
    }

    if(!m_polcn_map)
    {
        m_polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
    }

    if(m_polcn_map)
    {
        auto polcn_view = MapViewOfFile(m_polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
        if(polcn_view)
        {
            *(uint32_t *)((char *)polcn_view + 0) = GetCurrentProcessId();
            UnmapViewOfFile(polcn_view);
        }
    }

    m_loginquery = QTime::currentTime();
    m_StdOut.clear();

    QString argstr = QString("-account %1 -pwd %2 -gametype %3 -rungame %4 -skipupdate %5")
            .arg(ui->lineEdit_account->text())
            .arg(ui->lineEdit_password->text())
            .arg(ui->comboBox_gameType->currentData().toInt())
            .arg(((IsProcessThreadPresent(m_game_pid, m_game_tid) || g_CGAInterface->IsConnected())) ? "0" : "1")
            .arg(ui->checkBox_disableUpdater->isChecked() ? "1" : "0");

    m_POLCN->setNativeArguments(argstr);
    m_POLCN->start("POLCN_Launcher.exe");
    return true;
}

void AccountForm::on_pushButton_getgid_clicked()
{
    QString label, errorMsg;
    QueryAccount(label, errorMsg);

    if(!label.isEmpty())
        ui->label_status->setText(label);

    if(!errorMsg.isEmpty())
        ui->textEdit_output->setText(errorMsg);
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

void AccountForm::OnNotifyConnectionState(int state, QString msg)
{
    if((state == 10000 || state == 0) && !msg.isEmpty())
    {
        m_login_failure ++;
        if(ui->checkBox_autoChangeServer->isChecked())
        {
            if(m_login_failure > 8)
            {
                NotifyKillProcess();
            }
            else
            {
                if(ui->comboBox_server->currentIndex() + 1 >= 8)
                    ui->comboBox_server->setCurrentIndex(0);
                else
                    ui->comboBox_server->setCurrentIndex(ui->comboBox_server->currentIndex() + 1);
            }
        }
    }
    else if(state == 1)
    {
        m_login_failure = 0;
    }
}

void AccountForm::OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid, int bigserver, int server, int character, bool autologin, bool skipupdate, bool autochangeserver)
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

    if(autochangeserver)
        ui->checkBox_autoChangeServer->setChecked(true);
}

void AccountForm::OnHttpLoadAccount(QString query, QByteArray postdata, QJsonDocument* doc)
{
    QJsonObject obj;

    QJsonParseError err;
    auto newdoc = QJsonDocument::fromJson(postdata, &err);
    if(err.error == QJsonParseError::NoError && newdoc.isObject())
    {
       obj.insert("errcode", 0);
       auto newobj = newdoc.object();
       if(newobj.contains("user")){
           auto quser = newobj.take("user");
           if(quser.isString()){
               auto user = quser.toString();
               if(!user.isEmpty())
               {
                    ui->lineEdit_account->setText(user);
               }
           }
       }
       if(newobj.contains("pwd")){
           auto qpwd = newobj.take("pwd");
           if(qpwd.isString()){
               auto pwd = qpwd.toString();
               if(!pwd.isEmpty())
               {
                    ui->lineEdit_password->setText(pwd);
               }
           }
       }
       if(newobj.contains("gid")){
           auto qgid = newobj.take("gid");
           if(qgid.isString()){
               auto gid = qgid.toString();
               if(!gid.isEmpty())
               {
                    ui->comboBox_gid->setCurrentText(gid);
               }
           }
       }
       if(newobj.contains("game")){
           auto qgame = newobj.take("game");

           int game = qgame.toInt();
           if(game == 4)
               ui->comboBox_gameType->setCurrentIndex(0);
           if(game == 40)
               ui->comboBox_gameType->setCurrentIndex(1);
           if(game == 1)
               ui->comboBox_gameType->setCurrentIndex(2);
           if(game == 11)
               ui->comboBox_gameType->setCurrentIndex(3);
       }
       if(newobj.contains("bigserver")){
           auto qbigserver = newobj.take("bigserver");

           int bigserver = qbigserver.toInt();
           if(bigserver >= 1 && bigserver <= 2)
               ui->comboBox_bigserver->setCurrentIndex(bigserver - 1);
       }
       if(newobj.contains("server")){
           auto qserver = newobj.take("server");

           int server = qserver.toInt();
           if(server >= 1 && server <= 10)
               ui->comboBox_server->setCurrentIndex(server - 1);
       }
       if(newobj.contains("character")){
           auto qcharacter = newobj.take("character");

           int character = qcharacter.toInt();
           if(character >= 1 && character <= 2)
               ui->comboBox_character->setCurrentIndex(character - 1);
       }
       if(newobj.contains("autologin")){
           auto qautologin = newobj.take("autologin");
            if(qautologin.isBool()){
              ui->checkBox_autoLogin->setChecked(qautologin.toBool());
            }
       }
       if(newobj.contains("skipupdate")){
           auto qskipupdate = newobj.take("skipupdate");
            if(qskipupdate.isBool()){
              ui->checkBox_disableUpdater->setChecked(qskipupdate.toBool());
            }
       }
       if(newobj.contains("autochangeserver")){
           auto qautochangeserver = newobj.take("autochangeserver");
            if(qautochangeserver.isBool()){
              ui->checkBox_autoChangeServer->setChecked(qautochangeserver.toBool());
            }
       }
    }
    else
    {
       obj.insert("errcode", 1);
       obj.insert("message", tr("json parse error"));
    }
end:
   doc->setObject(obj);
}
