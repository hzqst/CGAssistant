#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTime>
#include <QTranslator>
#include <QCommandLineOption>
#include <QCommandLineParser>

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

    QCommandLineOption gameType("gametype", "", "gametype");

    QCommandLineOption loginUser("loginuser", "", "loginuser");

    QCommandLineOption loginPwd("loginpwd", "", "loginpwd");

    QCommandLineOption gid("gid", "", "gid");

    QCommandLineOption bigserver("bigserver", "", "bigserver");

    QCommandLineOption server("server", "", "server");

    QCommandLineOption character("character", "", "character");

    QCommandLineOption autologin("autologin");

    QCommandLineOption polcnskipupdate("polcnskipupdate");

    QCommandLineOption loadscript("loadscript", "", "loadscript");

    QCommandLineOption scriptautorestart("scriptautorestart");

    QCommandLineOption injuryprotect("injuryprotect");

    QCommandLineOption soulprotect("soulprotect");

    QCommandLineOption loadsettings("loadsettings", "", "loadsettings");

    QCommandLineOption killfreeze("killfreeze", "", "killfreeze", "15");

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
    parser.addOption(polcnskipupdate);
    parser.addOption(loadscript);
    parser.addOption(scriptautorestart);
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

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + qrand());

    MainWindow w;
    w.show();

    w.NotifyFillAutoLogin(parser.value(gameType).toInt(),
                          parser.value(loginUser),
                          parser.value(loginPwd),
                          parser.value(gid),
                          parser.value(bigserver).toInt(),
                          parser.value(server).toInt(),
                           parser.value(character).toInt(),
                           parser.isSet(autologin) ? true : false,
                          parser.isSet(polcnskipupdate) ? true : false);

    w.NotifyFillLoadScript(parser.value(loadscript),
                           parser.isSet(scriptautorestart) ? true : false,
                           parser.isSet(injuryprotect) ? true : false,
                           parser.isSet(soulprotect) ? true : false);

    w.NotifyFillLoadSettings(parser.value(loadsettings));

    w.NotifyFillMaxFreezeTime(parser.value(killfreeze).toInt());

    return a.exec();
}
