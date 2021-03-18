#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTranslator>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QJsonDocument>

#include "qhttpserver.hpp"
#include "qhttpserverresponse.hpp"
#include "qhttpserverrequest.hpp"

#include "../CGALib/gameinterface.h"
#include "player.h"

CGA::CGAInterface *g_CGAInterface = NULL;

#include <windows.h>
#include <dbghelp.h>

#pragma comment(lib,"dbghelp.lib")

LONG WINAPI MinidumpCallback(EXCEPTION_POINTERS* pException)
{
    HANDLE hDumpFile = CreateFile(L"minidump.mdmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hDumpFile != INVALID_HANDLE_VALUE) {

        MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
        dumpInfo.ExceptionPointers = pException;
        dumpInfo.ThreadId = GetCurrentThreadId();
        dumpInfo.ClientPointers = TRUE;

        MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithFullMemory), &dumpInfo, NULL, NULL);
        CloseHandle(hDumpFile);
    }

    //MessageBox(NULL, L"A fatal error occured, sorry but we have to terminate this program.\nSee minidump for more information.", L"Fatal Error", MB_ICONWARNING);

    return EXCEPTION_EXECUTE_HANDLER;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + qrand());

    qputenv("CGA_DIR_PATH", QCoreApplication::applicationDirPath().toLocal8Bit());

    qputenv("CGA_GAME_PORT", "");

    qputenv("CGA_GUI_PORT", "");

    QCommandLineOption gameType("gametype", "", "gametype");

    QCommandLineOption loginUser("loginuser", "", "loginuser");

    QCommandLineOption loginPwd("loginpwd", "", "loginpwd");

    QCommandLineOption gid("gid", "", "gid");

    QCommandLineOption bigserver("bigserver", "", "bigserver");

    QCommandLineOption server("server", "", "server");

    QCommandLineOption character("character", "", "character");

    QCommandLineOption autologin("autologin");

    QCommandLineOption skipupdate("skipupdate");

    QCommandLineOption autochangeserver("autochangeserver");

    QCommandLineOption loadscript("loadscript", "", "loadscript");

    QCommandLineOption scriptautorestart("scriptautorestart");

    QCommandLineOption scriptautoterm("scriptautoterm");

    QCommandLineOption injuryprotect("injuryprotect");

    QCommandLineOption soulprotect("soulprotect");

    QCommandLineOption loadsettings("loadsettings", "", "loadsettings");

    QCommandLineOption killfreeze("killfreeze", "", "killfreeze", "30");

    QCommandLineParser parser;

    parser.setSingleDashWordOptionMode(QCommandLineParser::ParseAsLongOptions);

    parser.addOption(gameType);
    parser.addOption(loginUser);
    parser.addOption(loginPwd);
    parser.addOption(gid);
    parser.addOption(bigserver);
    parser.addOption(server);
    parser.addOption(character);
    parser.addOption(autologin);
    parser.addOption(skipupdate);
    parser.addOption(autochangeserver);
    parser.addOption(loadscript);
    parser.addOption(scriptautorestart);
    parser.addOption(scriptautoterm);
    parser.addOption(injuryprotect);
    parser.addOption(soulprotect);
    parser.addOption(loadsettings);
    parser.addOption(killfreeze);
    parser.process(a);

    QTranslator translator;

    if(translator.load(":/lang.qm"))
        a.installTranslator(&translator);

    g_CGAInterface = CGA::CreateInterface();

    SetUnhandledExceptionFilter(MinidumpCallback);

     using namespace qhttp::server;

    QHttpServer qserver(qApp);

    MainWindow w;

    using namespace qhttp::server;

    for(unsigned int qport = 14396; qport < 14396 + 1000; ++qport)
    {
        if(qserver.listen(QHostAddress::LocalHost, qport, [&w](QHttpRequest* req, QHttpResponse* res) {
            req->collectData();

            req->onEnd([req, res, &w](){

                res->addHeader("connection", "close");

                if(req->method() == qhttp::THttpMethod::EHTTP_GET)
                {
                    auto path = req->url().path();
                    if(path.indexOf("/cga/") == 0)
                    {
                        auto subreq = path.mid(sizeof("/cga/") - 1);

                        if(0 == subreq.compare("GetGameProcInfo"))
                        {
                            QJsonDocument doc;
                            w.HttpGetGameProcInfo(&doc);

                            res->setStatusCode(qhttp::ESTATUS_OK);
                            res->end(doc.toJson());
                            return;
                        }
                        else if(0 == subreq.compare("GetSettings"))
                        {
                           QJsonDocument doc;
                           w.HttpGetSettings(&doc);

                           res->setStatusCode(qhttp::ESTATUS_OK);
                           res->end(doc.toJson());
                           return;
                        }
                    }
                }
                else if(req->method() == qhttp::THttpMethod::EHTTP_POST)
                {
                   auto path = req->url().path();
                   if(path.indexOf("/cga/") == 0)
                   {
                       auto subreq = path.mid(sizeof("/cga/") - 1);
                       if(0 == subreq.compare("LoadSettings"))
                       {
                           auto reqData = req->collectedData();

                           QJsonDocument doc;

                           w.HttpLoadSettings(req->url().query(), reqData, &doc);

                           res->setStatusCode(qhttp::ESTATUS_OK);
                           res->end(doc.toJson());
                           return;
                       }
                       else if(0 == subreq.compare("LoadScript"))
                       {
                           auto reqData = req->collectedData();

                           QJsonDocument doc;

                           w.HttpLoadScript(req->url().query(), reqData, &doc);

                           res->setStatusCode(qhttp::ESTATUS_OK);
                           res->end(doc.toJson());
                           return;
                       }
                       else if(0 == subreq.compare("LoadAccount"))
                       {
                           auto reqData = req->collectedData();

                           QJsonDocument doc;

                           w.HttpLoadAccount(req->url().query(), reqData, &doc);

                           res->setStatusCode(qhttp::ESTATUS_OK);
                           res->end(doc.toJson());
                           return;
                       }
                   }
                }

                res->setStatusCode(qhttp::ESTATUS_BAD_REQUEST);
                res->end(QByteArray("invalid request"));
            });

        }))
        {
            QByteArray qportString = QString("%1").arg(qport).toLocal8Bit();
            qputenv("CGA_GUI_PORT", qportString);
            break;
        }
    }

    w.show();

    w.NotifyFillAutoLogin(parser.value(gameType).toInt(),
                          parser.value(loginUser),
                          parser.value(loginPwd),
                          parser.value(gid),
                          parser.value(bigserver).toInt(),
                          parser.value(server).toInt(),
                           parser.value(character).toInt(),
                           parser.isSet(autologin) ? true : false,
                          parser.isSet(skipupdate) ? true : false,
                          parser.isSet(autochangeserver) ? true : false);

    w.NotifyFillLoadScript(parser.value(loadscript),
                           parser.isSet(scriptautorestart) ? true : false,
                           parser.isSet(injuryprotect) ? true : false,
                           parser.isSet(soulprotect) ? true : false);

    w.NotifyFillLoadSettings(parser.value(loadsettings));

    w.NotifyFillMaxFreezeTime(parser.value(killfreeze).toInt());

    return a.exec();
}
