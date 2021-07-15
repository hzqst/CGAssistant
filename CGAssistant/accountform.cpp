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

typedef struct
{
    uint32_t cga_pid;
    uint32_t polcn_pid;
    char gid[128];
}polcn_view_t;

typedef struct
{
    char glt[128];
}glt_view_t;

AccountForm::AccountForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AccountForm)
{
    ui->setupUi(this);

    m_polcn_lock = NULL;
    m_polcn_map = NULL;

    m_glt_lock = NULL;
    m_glt_map = NULL;

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
    ui->comboBox_character->addItem(QObject::tr("Highest Lv"), QVariant(2));
    ui->comboBox_character->addItem(QObject::tr("Lowest Lv"), QVariant(3));

    ui->comboBox_CharaEye->addItem(QObject::tr("1"), QVariant(0));
    ui->comboBox_CharaEye->addItem(QObject::tr("2"), QVariant(1));
    ui->comboBox_CharaEye->addItem(QObject::tr("3"), QVariant(2));
    ui->comboBox_CharaEye->addItem(QObject::tr("4"), QVariant(3));
    ui->comboBox_CharaEye->addItem(QObject::tr("5"), QVariant(4));

    ui->comboBox_CharaMou->addItem(QObject::tr("1"), QVariant(0));
    ui->comboBox_CharaMou->addItem(QObject::tr("2"), QVariant(1));
    ui->comboBox_CharaMou->addItem(QObject::tr("3"), QVariant(2));
    ui->comboBox_CharaMou->addItem(QObject::tr("4"), QVariant(3));
    ui->comboBox_CharaMou->addItem(QObject::tr("5"), QVariant(4));

    ui->comboBox_CharaColor->addItem(QObject::tr("1"), QVariant(0));
    ui->comboBox_CharaColor->addItem(QObject::tr("2"), QVariant(1));
    ui->comboBox_CharaColor->addItem(QObject::tr("3"), QVariant(2));
    ui->comboBox_CharaColor->addItem(QObject::tr("4"), QVariant(3));

    ui->comboBox_Chara->addItem(QObject::tr("bawu"), QVariant(0));
    ui->comboBox_Chara->addItem(QObject::tr("kazi"), QVariant(1));
    ui->comboBox_Chara->addItem(QObject::tr("xin"), QVariant(2));
    ui->comboBox_Chara->addItem(QObject::tr("tuobu"), QVariant(3));
    ui->comboBox_Chara->addItem(QObject::tr("kai"), QVariant(4));
    ui->comboBox_Chara->addItem(QObject::tr("feite"), QVariant(5));
    ui->comboBox_Chara->addItem(QObject::tr("boke"), QVariant(6));
    ui->comboBox_Chara->addItem(QObject::tr("wulu"), QVariant(7));
    ui->comboBox_Chara->addItem(QObject::tr("moeko"), QVariant(8));
    ui->comboBox_Chara->addItem(QObject::tr("ami"), QVariant(9));
    ui->comboBox_Chara->addItem(QObject::tr("meigu"), QVariant(10));
    ui->comboBox_Chara->addItem(QObject::tr("li"), QVariant(11));
    ui->comboBox_Chara->addItem(QObject::tr("kayi"), QVariant(12));
    ui->comboBox_Chara->addItem(QObject::tr("ailu"), QVariant(13));

    ui->comboBox_Chara->addItem(QObject::tr("xiedi"), QVariant(14 + 0));
    ui->comboBox_Chara->addItem(QObject::tr("pite"), QVariant(14 + 1));
    ui->comboBox_Chara->addItem(QObject::tr("zuozang"), QVariant(14 + 2));
    ui->comboBox_Chara->addItem(QObject::tr("niersen"), QVariant(14 + 3));
    ui->comboBox_Chara->addItem(QObject::tr("beidite"), QVariant(14 + 4));
    ui->comboBox_Chara->addItem(QObject::tr("lansiluote"), QVariant(14 + 5));
    ui->comboBox_Chara->addItem(QObject::tr("weisikaier"), QVariant(14 + 6));
    ui->comboBox_Chara->addItem(QObject::tr("shala"), QVariant(14 + 7));
    ui->comboBox_Chara->addItem(QObject::tr("aya"), QVariant(14 + 8));
    ui->comboBox_Chara->addItem(QObject::tr("fuerdiya"), QVariant(14 + 9));
    ui->comboBox_Chara->addItem(QObject::tr("xiala"), QVariant(14 + 10));
    ui->comboBox_Chara->addItem(QObject::tr("pingping"), QVariant(14 + 11));
    ui->comboBox_Chara->addItem(QObject::tr("geleisi"), QVariant(14 + 12));
    ui->comboBox_Chara->addItem(QObject::tr("hemi"), QVariant(14 + 13));

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

bool AccountForm::IsGltExpired()
{
    QString glt;

    if(m_glt.isEmpty())
        return true;

    if(m_loginresult.elapsed() > 1000*60)
        return true;

    if(m_glt_map && m_glt_lock)
    {
        WaitForSingleObject(m_glt_lock, INFINITE);
        auto glt_view = MapViewOfFile(m_glt_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
        if(glt_view)
        {
            glt_view_t *view = (glt_view_t *)glt_view;

            glt = QString(view->glt);

            UnmapViewOfFile(glt_view);
        }
        ReleaseMutex(m_glt_lock);
    }

    if(0 != glt.compare(m_glt))
        return true;

    return false;
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

    if(!ui->checkBox_createChara->isChecked())
    {
        CGA::cga_create_chara_t req;
        g_CGAInterface->CreateCharacter(req);
    }

    if(!ui->checkBox_autoLogin->isChecked())
    {
        if(g_CGAInterface->IsConnected()){
            g_CGAInterface->LoginGameServer("", "", 0, 0, 0, 0);
        }

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
            if((worldStatus == 2 && gameStatus == 1) || (worldStatus == 3 && gameStatus == 11))
            {
                if(IsGltExpired())
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

                if(!m_glt_lock)
                {
                    m_glt_lock = CreateMutexW(NULL, FALSE, L"CGAGltLock");
                }

                if(!m_glt_map)
                {
                    m_glt_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAGltMap");
                }

                if(m_glt_map && m_glt_lock)
                {
                    WaitForSingleObject(m_glt_lock, INFINITE);
                    auto glt_view = MapViewOfFile(m_glt_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
                    if(glt_view)
                    {
                        glt_view_t *view = (glt_view_t *)glt_view;

                        memset(view->glt, 0, sizeof(view->glt));

                        strncpy(view->glt, m_glt.toLocal8Bit().data(), 127);
                        view->glt[127] = 0;

                        UnmapViewOfFile(glt_view);
                    }
                    ReleaseMutex(m_glt_lock);
                }

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

bool AccountForm::QueryAccount(QString &label, QString &errorMsg)
{
    if(m_POLCN->state() == QProcess::ProcessState::Running)
    {
        //qDebug("running");
        return false;
    }

    if(!m_polcn_lock)
    {
        m_polcn_lock = CreateMutexW(NULL, TRUE, L"CGAPolcnLock");
        if(m_polcn_lock && GetLastError() == ERROR_ALREADY_EXISTS)
        {
           // qDebug("lock1");
            auto polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
            if(polcn_map)
            {
                auto polcn_view = MapViewOfFile(polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
                if(polcn_view)
                {
                    polcn_view_t *view = (polcn_view_t *)polcn_view;

                    label = tr("Waiting for other CGAssistant to login...");
                    errorMsg = tr("Waiting for other CGAssistant to login ...\nCGA_PID=#%1, POLCN_PID=#%2, GID=%3")
                            .arg( view->cga_pid ).arg( view->polcn_pid ).arg( QString(view->gid) );

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
            //qDebug("lock2");

            label = tr("Failed to acquire POLCN login lock...");
            errorMsg = tr("Failed to acquire POLCN login lock, error code %1...").arg( GetLastError() );
            return false;
        }
    }

    //qDebug("query");

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

    if(!m_polcn_map)
    {
        m_polcn_map = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 0x1000, L"CGAPPolcnMap");
    }

    if(m_polcn_map)
    {
        auto polcn_view = MapViewOfFile(m_polcn_map, FILE_MAP_WRITE | FILE_MAP_READ, 0, 0, 0x1000 );
        if(polcn_view)
        {
            polcn_view_t *view = (polcn_view_t *)polcn_view;

            view->cga_pid = (uint32_t)QApplication::applicationPid();
            view->polcn_pid = (uint32_t)m_POLCN->processId();

            memset(view->gid, 0, sizeof(view->gid));

            strncpy(view->gid, ui->comboBox_gid->currentText().toLocal8Bit().data(), 127);
            view->gid[127] = 0;

            UnmapViewOfFile(polcn_view);
        }
    }

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
        if(ui->checkBox_createChara->isChecked()){
            CGA::cga_create_chara_t req;

            req.name = ui->lineEdit_CharaName->text().toStdString();
            req.character = ui->comboBox_Chara->currentData().toInt();
            req.eye = ui->comboBox_CharaEye->currentData().toInt();
            req.mouth = ui->comboBox_CharaMou->currentData().toInt();
            req.color = ui->comboBox_CharaColor->currentData().toInt();

            QStringList points = ui->lineEdit_CharaPoints->text().split(",");

            if(points.size() == 5){
                req.endurance = points[0].toInt();
                req.strength = points[1].toInt();
                req.defense = points[2].toInt();
                req.agility = points[3].toInt();
                req.magical = points[4].toInt();
            }

            QStringList elements = ui->lineEdit_CharaElements->text().split(",");

            if(elements.size() == 4){
                req.earth = elements[0].toInt();
                req.water = elements[1].toInt();
                req.fire = elements[2].toInt();
                req.wind = elements[3].toInt();
            }

            g_CGAInterface->CreateCharacter(req);
        } else {
            CGA::cga_create_chara_t req;
            g_CGAInterface->CreateCharacter(req);
        }

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
            if(ui->comboBox_server->currentIndex() + 1 >= 10)
                ui->comboBox_server->setCurrentIndex(0);
            else
                ui->comboBox_server->setCurrentIndex(ui->comboBox_server->currentIndex() + 1);
        }
        if(m_login_failure >= 10 && ui->checkBox_autoKillGame->isChecked())
        {
            NotifyKillProcess();
        }
    }
    else if(state == 1 || state == 2)
    {
        m_login_failure = 0;
    }
}

void AccountForm::OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
                                        int bigserver, int server, int character,
                                        bool autologin, bool skipupdate, bool autochangeserver,bool autokillgame,
                                        bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
                                        QString create_chara_points, QString create_chara_elements, QString create_chara_name)
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

    if(character >= 1 && character <= 4)
        ui->comboBox_character->setCurrentIndex(character - 1);

    if(autologin)
        ui->checkBox_autoLogin->setChecked(true);

    if(skipupdate)
        ui->checkBox_disableUpdater->setChecked(true);

    if(autochangeserver)
        ui->checkBox_autoChangeServer->setChecked(true);

    if(autokillgame)
        ui->checkBox_autoKillGame->setChecked(true);

    if(create_chara)
    {
        ui->checkBox_createChara->setChecked(true);
        on_checkBox_createChara_stateChanged(1);
    }

    if(create_chara_chara)
        ui->comboBox_Chara->setCurrentIndex(create_chara_chara-1);

    if(create_chara_eye)
        ui->comboBox_CharaEye->setCurrentIndex(create_chara_eye-1);
    if(create_chara_mou)
        ui->comboBox_CharaMou->setCurrentIndex(create_chara_mou-1);
    if(create_chara_color)
        ui->comboBox_CharaColor->setCurrentIndex(create_chara_color-1);

    if(!create_chara_points.isEmpty())
        ui->lineEdit_CharaPoints->setText(create_chara_points);

    if(!create_chara_elements.isEmpty())
        ui->lineEdit_CharaElements->setText(create_chara_elements);

    if(!create_chara_name.isEmpty()){
        ui->lineEdit_CharaName->setText(create_chara_name);
    }
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
       if(newobj.contains("autocreatechara")){
           auto qautocreatechara = newobj.take("autocreatechara");
            if(qautocreatechara.isBool()){
              ui->checkBox_createChara->setChecked(qautocreatechara.toBool());
              on_checkBox_createChara_stateChanged(qautocreatechara.toBool() ? 1 : 0);
            }
       }
       if(newobj.contains("createcharachara")){
           auto qcreatecharachara = newobj.take("createcharachara");

           int createcharachara = qcreatecharachara.toInt();
           if(createcharachara >= 1 && createcharachara <= 28)
               ui->comboBox_Chara->setCurrentIndex(createcharachara-1);
       }
       if(newobj.contains("createcharaeye")){
           auto qcreatecharaeye = newobj.take("createcharaeye");

           int createcharaeye = qcreatecharaeye.toInt();
           if(createcharaeye >= 1 && createcharaeye <= 5)
               ui->comboBox_CharaEye->setCurrentIndex(createcharaeye - 1);
       }
       if(newobj.contains("createcharamouth")){
           auto qcreatecharamouth = newobj.take("createcharamouth");

           int createcharamouth = qcreatecharamouth.toInt();
           if(createcharamouth >= 1 && createcharamouth <= 5)
               ui->comboBox_CharaMou->setCurrentIndex(createcharamouth - 1);
       }
       if(newobj.contains("createcharacolor")){
           auto qcreatecharacolor = newobj.take("createcharacolor");

           int createcharacolor = qcreatecharacolor.toInt();
           if(createcharacolor >= 1 && createcharacolor <= 5)
               ui->comboBox_CharaColor->setCurrentIndex(createcharacolor - 1);
       }
       if(newobj.contains("createcharapoints")){
           auto qcreatecharapoints = newobj.take("createcharapoints");
           if(qcreatecharapoints.isString()){
               auto points = qcreatecharapoints.toString();
               if(!points.isEmpty())
               {
                    ui->lineEdit_CharaPoints->setText(points);
               }
           }
       }
       if(newobj.contains("createcharaelements")){
           auto qcreatecharaelements = newobj.take("createcharaelements");
           if(qcreatecharaelements.isString()){
               auto elements = qcreatecharaelements.toString();
               if(!elements.isEmpty())
               {
                    ui->lineEdit_CharaElements->setText(elements);
               }
           }
       }
       if(newobj.contains("createcharaname")){
           auto qcreatecharaname = newobj.take("createcharaname");
           if(qcreatecharaname.isString()){
               auto name = qcreatecharaname.toString();
               if(!name.isEmpty())
               {
                    ui->lineEdit_CharaName->setText(name);
               }
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

void AccountForm::on_checkBox_createChara_stateChanged(int arg1)
{
    if(arg1){
        ui->comboBox_Chara->setEnabled(true);
        ui->comboBox_CharaEye->setEnabled(true);
        ui->comboBox_CharaMou->setEnabled(true);
        ui->comboBox_CharaColor->setEnabled(true);
        ui->lineEdit_CharaName->setEnabled(true);
        ui->lineEdit_CharaPoints->setEnabled(true);
        ui->lineEdit_CharaElements->setEnabled(true);
    } else {
        ui->comboBox_Chara->setEnabled(false);
        ui->comboBox_CharaEye->setEnabled(false);
        ui->comboBox_CharaMou->setEnabled(false);
        ui->comboBox_CharaColor->setEnabled(false);
        ui->lineEdit_CharaName->setEnabled(false);
        ui->lineEdit_CharaPoints->setEnabled(false);
        ui->lineEdit_CharaElements->setEnabled(false);
    }
}
