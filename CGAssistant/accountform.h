#ifndef ACCOUNTFORM_H
#define ACCOUNTFORM_H

#include <QWidget>
#include <QProcess>
#include <QTime>
#include <windows.h>

namespace Ui {
class AccountForm;
}

class AccountForm : public QWidget
{
    Q_OBJECT

public:
    explicit AccountForm(QWidget *parent = nullptr);
    ~AccountForm();

    bool QueryAccount(QString &label, QString &errorMsg);

private slots:
    void OnPOLCNReadyReadStdOut();
    void OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnPOLCNStarted();
    void OnAutoLogin();
    void on_pushButton_getgid_clicked();    
    void on_pushButton_logingame_clicked();
public slots:
    void OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid, int bigserver, int server, int character, bool autologin, bool skipupdate, bool autochangeserver);
    void OnHttpLoadAccount(QString query, QByteArray postdata, QJsonDocument* doc);
    void OnNotifyConnectionState(int state, QString msg);
signals:
    void NotifyAutoAttachProcess(quint32 pid, quint32 tid);
    void NotifyKillProcess();

private:
    Ui::AccountForm *ui;
    QProcess *m_POLCN;
    QByteArray m_StdOut;
    int m_serverid;
    quint32 m_game_pid;
    quint32 m_game_tid;
    QString m_glt;
    QTime m_loginquery;
    QTime m_loginresult;
    HANDLE m_polcn_lock;
    HANDLE m_polcn_map;
    int m_login_failure;
};

#endif // ACCOUNTFORM_H
