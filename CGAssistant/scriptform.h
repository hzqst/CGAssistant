#ifndef SCRIPTFORM_H
#define SCRIPTFORM_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class ScriptForm;
}
class QTextEdit;

class ScriptForm : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptForm(QWidget *parent = 0);
    ~ScriptForm();

public slots:
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd);
    void on_pushButton_load_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_debug_clicked();
    void on_pushButton_kill_clicked();
    void OnNotifyFillLoadScript(QString path, bool autorestart, bool injuryprot, bool soulprot);

private slots:
    void OnNodeStarted();
    void OnNodeStartError(QProcess::ProcessError error);
    void OnNodeReadyReadStdOut();
    void OnNodeReadyReadStdErr();
    void OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnCloseWindow();
    void OnAutoRestart();
private:
    Ui::ScriptForm *ui;
    QTextEdit *m_output;
    QString m_scriptPath;
    QString m_chromePath;
    QProcess *m_node;

    bool m_bListening;
    bool m_bDebugging;
    quint32 m_port;
};

#endif // SCRIPTFORM_H
