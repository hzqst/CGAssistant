#ifndef ACCOUNTFORM_H
#define ACCOUNTFORM_H

#include <QWidget>
#include <QProcess>
#include <QTime>

namespace Ui {
class AccountForm;
}

class AccountForm : public QWidget
{
    Q_OBJECT

public:
    explicit AccountForm(QWidget *parent = nullptr);
    ~AccountForm();

private slots:
    void OnPOLCNReadyReadStdOut();
    void OnPOLCNFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnPOLCNStarted();
    void OnAutoLogin();
    void on_pushButton_getgid_clicked();    
    void on_pushButton_logingame_clicked();
public slots:
    void OnNotifyFillAutoLogin(int game, QString user, QString pwd, QString gid, int bigserver, int server, int character, bool autologin, bool skipupdate);

signals:
    void NotifyAutoAttachProcess(quint32 pid, quint32 tid);

private:
    Ui::AccountForm *ui;
    QProcess *m_POLCN;
    int m_serverid;
    quint32 m_game_pid;
    quint32 m_game_tid;
    QString m_glt;
    QTime m_loginquery;
    QTime m_loginresult;
};

#endif // ACCOUNTFORM_H
