#ifndef SCRIPTFORM_H
#define SCRIPTFORM_H

#include <QWidget>
#include <QProcess>

#include <QTime>

namespace Ui {
class ScriptForm;
}
class QTextEdit;
class QPlainTextEdit;

class ScriptForm : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptForm(QWidget *parent = 0);
    ~ScriptForm();
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dropEvent(QDropEvent *event);
    void UpdateGameTextUI(bool show);
signals:
    void ReportNavigatorPath(QString json);
    void ReportNavigatorFinish(int exitCode);

public slots:
    void OnNotifyAttachProcessOk(quint32 ProcessId, quint32 ThreadId, quint32 port, quint32 hWnd);
    void on_pushButton_load_clicked();
    void on_pushButton_run_clicked();
    void on_pushButton_debug_clicked();
    void on_pushButton_term_clicked();
    void OnNotifyFillLoadScript(QString path, int autorestart, bool freezestop, bool injuryprot, bool soulprot, int consolemaxlines);
    void RunNavigatorScript(int x, int y, int enter, QString *result);
    void StopNavigatorScript();
    void OnHttpLoadScript(QString query, QByteArray postdata, QJsonDocument* doc);
private slots:
    void OnNodeStarted();
    void OnNodeStartError(QProcess::ProcessError error);
    void OnNodeReadyRead();
    void OnNodeFinish(int exitCode, QProcess::ExitStatus exitStatus);
    void OnCloseWindow();
    void OnAutoRestart();
    void on_pushButton_suspend_clicked();
    void OnSetFreezeDuration(int value);

private:
    Ui::ScriptForm *ui;
    QPlainTextEdit *m_output;
    QString m_scriptPath;
    QString m_chromePath;
    QProcess *m_node;

    QString m_PathString;
    bool m_bPathBegin;
    bool m_bNavigating;
    bool m_bListening;
    bool m_bDebugging;
    bool m_bSuspending;
    quint32 m_port;

    int m_ConsoleMaxLines;

    int m_LastMapX;
    int m_LastMapY;
    int m_LastMapIndex;
    QTime m_LastMapChange;
};

#endif // SCRIPTFORM_H
