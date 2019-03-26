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

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()) + qrand());

    MainWindow w;
    w.show();

    return a.exec();
}
