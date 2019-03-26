#ifndef SCRIPTFORM_H
#define SCRIPTFORM_H

#include <QWidget>
#include <QProcess>

namespace Ui {
class ScriptForm;
}
class QTextEdit;
class MyWebView;

class ScriptForm : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptForm(QWidget *parent = 0);
    ~ScriptForm();

public slots:
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd);

private slots:
    void on_pushButton_load_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_debug_clicked();
    void OnNodeStarted();
    void OnNodeStartError(QProcess::ProcessError error);
    void OnNodeReadyReadStdOut();
    void OnNodeReadyReadStdErr();
    void OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnCloseWindow();
    void on_pushButton_kill_clicked();

private:
    Ui::ScriptForm *ui;
    MyWebView *m_webview;
    QTextEdit *m_output;
    QString m_scriptPath;
    QProcess *m_node;

    bool m_bListening;
    bool m_bDebugging;
    int m_port;
};

#endif // SCRIPTFORM_H
