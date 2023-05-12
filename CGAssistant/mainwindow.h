#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QJsonDocument>
#include <QThread>
#include "player.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    virtual void closeEvent(QCloseEvent *event);
    virtual void changeEvent(QEvent * event);
private:
    Ui::MainWindow *ui;
    QThread m_playerWorkerThread;
    QThread m_processWorkerThread;
    QThread m_battleWorkerThread;

signals:
    void NotifyCloseWindow();
    void NotifyChangeWindow(Qt::WindowStates);
    void NotifyFillAutoLogin(int game, QString user, QString pwd, QString gid,
                             int bigserver, int server, int character,
                             bool autologin, bool skipupdate, bool autochangeserver,bool autokillgame,
                             bool create_chara, int create_chara_chara, int create_chara_eye, int create_chara_mou, int create_chara_color,
                             QString create_chara_points, QString create_chara_elements, QString create_chara_name);
    void NotifyFillLoadScript(QString path, int autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines);
    void NotifyFillLoadSettings(QString path);
    void NotifyFillStaticSettings(int freezetime, int chatmaxlines);
    void HttpGetGameProcInfo(QJsonDocument* doc);
    void HttpGetSettings(QJsonDocument* doc);
    void HttpLoadSettings(QString query, QByteArray postdata, QJsonDocument* doc);
    void HttpLoadScript(QString query, QByteArray postdata, QJsonDocument* doc);
    void HttpLoadAccount(QString query, QByteArray postdata, QJsonDocument* doc);
private slots:
    void on_tabWidget_currentChanged(int index);
    void OnNotifyGetPlayerInfo(QSharedPointer<CGA_PlayerInfo_t> player);
    void OnNotifyGetInfoFailed(bool bIsConnected, bool bIsInGame);
    void OnNotifyLoginProgressStart();
    void OnNotifyLoginProgressEnd();
private:
    bool m_bIsInLoginProgress;

    QIcon m_base_icon;
    QIcon m_login_icon;
    QIcon m_notingame_icon;
};

#endif // MAINWINDOW_H
