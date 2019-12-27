#ifndef PSWORKER_H
#define PSWORKER_H

#include <QSharedPointer>
#include <QTimer>
#include <Windows.h>

class CProcessItem
{
public:
    CProcessItem(quint32 pid, quint32 tid, quint32 hWnd, const wchar_t *szTitle, bool bAttached){
        m_ProcessId = pid;
        m_ThreadId = tid;
        m_hWnd = hWnd;
        m_Title = QString::fromWCharArray(szTitle);
        m_bAttached = bAttached;
    }
    quint32 m_ProcessId;
    quint32 m_ThreadId;
    quint32 m_hWnd;
    QString m_Title;
    bool m_bAttached;
};

typedef QSharedPointer<CProcessItem> CProcessItemPtr;
typedef QList<CProcessItemPtr> CProcessItemList;

class CRetryAttachProcessTimer : public QTimer
{
    Q_OBJECT
public:
    explicit CRetryAttachProcessTimer(quint32 ProcessId, QObject *parent = Q_NULLPTR);

    quint32 m_ProcessId;
    int m_retry;
};

class CProcessWorker : public QObject
{
    Q_OBJECT
public:
    explicit CProcessWorker(QObject *parent = NULL);
public slots:
    void OnQueueAttachProcess(quint32 ProcessId, quint32 ThreadId, quint32 hWnd, QString dllPath);
    void OnQueueQueryProcess();
    void OnCheckFreezeProcess();
    void OnAutoAttachProcess(quint32 ProcessId, quint32 ThreadId);
    void OnNotifyFillMaxFreezeTime(int freezetime);
private slots:
    void OnRetryAttachProcess();

signals:
    void NotifyAttachProcessOk(quint32 ProcessId, quint32 port, quint32 hWnd);
    void NotifyAttachProcessFailed(quint32 ProcessId, int errorCode, QString errorString);
    void NotifyQueryProcess(CProcessItemList list);
    void NotifyServerShutdown(int port);
private:
    bool IsProcessAttached(quint32 ProcessId);
    bool InjectByMsgHook(quint32 ThreadId, quint32 hWnd, QString &szDllPath, int &errorCode, QString &errorString);
    bool ReadSharedData(quint32 ProcessId, int &port, quint32 &hWnd);
    void ConnectToServer(quint32 ProcessId, int port, quint32 hWnd);
    bool CreateAttachMutex(quint32 ProcessId);
    void Disconnect();
private:
    HANDLE m_AttachMutex;
    HWND m_AttachHwnd;

    quint32 m_AutoAttachPID;
    quint32 m_AutoAttachTID;
    double m_LastGameAnimTick;
    int m_AnimTickFreezeTime;
    int m_MaxFreezeTime;
};

#endif // PSWORKER_H
