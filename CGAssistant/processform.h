#ifndef PROCESSFORM_H
#define PROCESSFORM_H

#include <QThread>
#include <QWidget>
#include "processtable.h"
#include "psworker.h"

namespace Ui {
class ProcessForm;
}

class ProcessForm : public QWidget
{
    Q_OBJECT

public:
    explicit ProcessForm(CProcessWorker *worker, QWidget *parent = 0);
    ~ProcessForm();

private slots:
    void on_pushButton_attach_clicked();
    void on_checkBox_syncwnd_stateChanged(int arg1);

    void on_comboBox_size_currentIndexChanged(int index);

public slots:
    void OnCloseWindow();
    void OnNotifyAttachProcessFailed(quint32 ProcessId, quint32 ThreadId, int errorCode, QString errorString);
    void OnNotifyQueryProcess(CProcessItemList list);
    void OnNotifyChangeWindow(Qt::WindowStates st);
signals:
    void QueueAttachProcess(quint32 ProcessId, quint32 Thread, quint32 hWnd, QString dllPath);
private:
    CProcessWorker *m_worker;
    Ui::ProcessForm *ui;
    CProcessTableModel *m_model;
    bool m_sync;
};

#endif // PROCESSFORM_H
