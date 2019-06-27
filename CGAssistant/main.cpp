#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QTime>
//#include <QTextStream>
#include <QTranslator>
#include <QNetworkProxyFactory>

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


/*void customMessageHandler(QtMsgType type,const QMessageLogContext &context, const QString &msg)
{
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;

    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
    break;
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
    break;
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QFile outFile("cga.txt");
    outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    QTextStream ts(&outFile);
    ts << txt << endl;
    outFile.close();
}
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    if(translator.load(":/lang.qm"))
        a.installTranslator(&translator);

    QNetworkProxyFactory::setUseSystemConfiguration(false);

    //qInstallMessageHandler(customMessageHandler);

    g_CGAInterface = CGA::CreateInterface();

    SetUnhandledExceptionFilter(MinidumpCallback);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + qrand());

    MainWindow w;
    w.show();

    return a.exec();
}
