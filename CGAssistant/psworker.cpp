#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include "psworker.h"
#include "../CGALib/gameinterface.h"

#include "MINT.h"

extern CGA::CGAInterface *g_CGAInterface;

Q_DECLARE_METATYPE(CProcessItemList)

CRetryAttachProcessTimer::CRetryAttachProcessTimer(quint32 ProcessId, quint32 ThreadId, QObject *parent) : QTimer(parent)
{
    m_ProcessId = ProcessId;
    m_ThreadId = ThreadId;
    m_retry = 0;
}

CProcessWorker::CProcessWorker(QObject *parent) : QObject(parent)
{
    qRegisterMetaType<CProcessItemList>("CProcessItemList");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnQueueQueryProcess()));
    timer->start(500);
    connect(timer, SIGNAL(timeout()), this, SLOT(OnCheckFreezeProcess()));
    timer->start(1000);

    m_AttachMutex = NULL;
    m_AttachHwnd = 0;
    m_AttachGamePort = 0;
    m_AutoAttachPID = 0;
    m_AutoAttachTID = 0;
    m_LastGameAnimTick = 0;
    m_AnimTickFreezeTime = 0;
    m_MaxFreezeTime = 30;
}

quint32 CProcessWorker::GetAttachedHwnd()
{
    return (quint32)m_AttachHwnd;
}

bool CProcessWorker::IsProcessAttached(quint32 ProcessId)
{
    bool bAttached = false;
    WCHAR szMutex[32];
    wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
    HANDLE hAttachMutex = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szMutex);
    if(hAttachMutex)
    {
        bAttached = true;
        CloseHandle(hAttachMutex);
    }
    return bAttached;
}

bool CProcessWorker::InjectByMsgHook(quint32 ThreadId, quint32 hWnd, QString &dllPath, int &errorCode, QString &errorString)
{
    HMODULE hModule = NULL;
    do
    {
        hModule = LoadLibraryW((LPCWSTR)dllPath.utf16());
        if (!hModule){
            errorCode = GetLastError();
            errorString = tr("InjectByMsgHook failed with LoadLibraryW, errorCode: %1").arg(errorCode);
            break;
        }

        PVOID pfnProc = (PVOID)GetProcAddress(hModule, "_GetMsgProc@12");
        if (!pfnProc){
            errorCode = GetLastError();
            errorString = tr("InjectByMsgHook failed with GetProcAddress, errorCode: %1").arg(errorCode);
            break;
        }

        HHOOK hHook = SetWindowsHookExW(WH_GETMESSAGE, (HOOKPROC)pfnProc, hModule, ThreadId);
        if (!hHook){
            errorCode = GetLastError();
            errorString = tr("InjectByMsgHook failed with SetWindowsHookExW, errorCode: %1").arg(errorCode);
            break;
        }

        if(!PostMessageW((HWND)hWnd, WM_NULL, 0, 0)){
            errorCode = GetLastError();
            errorString = tr("InjectByMsgHook failed with PostMessageW, errorCode: %1").arg(errorCode);
            break;
        }

        errorCode = 0;
    } while(0);

    if (hModule)
        FreeLibrary(hModule);

    return (errorCode == 0) ? true : false;
}

bool CProcessWorker::ReadSharedData(quint32 ProcessId, int &port, quint32 &hWnd)
{
    bool bSuccess = false;

    WCHAR szLockName[32];
    wsprintfW(szLockName, L"CGASharedDataLock_%d", ProcessId);

    HANDLE hDataLock = OpenMutexW(MUTEX_ALL_ACCESS, FALSE, szLockName);
    if(hDataLock)
    {
        if(WAIT_OBJECT_0 == WaitForSingleObject(hDataLock, 500))
        {
            WCHAR szMappingName[32];
            wsprintfW(szMappingName, L"CGASharedData_%d", ProcessId);
            HANDLE hFileMapping = CreateFileMappingW(INVALID_HANDLE_VALUE, NULL, PAGE_READONLY, 0, sizeof(CGA::CGAShare_t), szMappingName);
            if (hFileMapping)
            {
                LPVOID pViewOfFile = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, 0);
                if (pViewOfFile)
                {
                    CGA::CGAShare_t *data = (CGA::CGAShare_t *)pViewOfFile;

                    port = data->Port;
                    hWnd = (quint32)data->hWnd;
                    bSuccess = true;
                    UnmapViewOfFile(pViewOfFile);
                }
                CloseHandle(hFileMapping);
            }
            ReleaseMutex(hDataLock);
            CloseHandle(hDataLock);
        }
    }

    return bSuccess;
}

void CProcessWorker::OnRetryAttachProcess()
{
    auto timer = (CRetryAttachProcessTimer *)sender();

    int port = 0;
    quint32 hwnd = 0;
    if(!ReadSharedData(timer->m_ProcessId, port, hwnd))
    {
        if(timer->m_retry > 10)
        {
            timer->stop();
            timer->deleteLater();
            NotifyAttachProcessFailed(timer->m_ProcessId, timer->m_ThreadId, -2, tr("Timeout for reading shared data."));
            return;
        }
        else
        {
            ++ timer->m_retry;
        }
    } else {
        timer->stop();
        timer->deleteLater();
        ConnectToServer(timer->m_ProcessId, timer->m_ThreadId, port, hwnd);
    }
}

void CProcessWorker::ConnectToServer(quint32 ProcessId, quint32 ThreadId, int port, quint32 hwnd)
{
    if(!CreateAttachMutex(ProcessId, ThreadId))
        return;

    if(g_CGAInterface->Connect(port))
    {
        m_AttachHwnd = (HWND)hwnd;
        m_AttachGamePort = port;
        g_CGAInterface->GetNextAnimTickCount(m_LastGameAnimTick);
        //g_CGAInterface->RegisterServerShutdownNotify(std::bind(&CProcessWorker::NotifyServerShutdown, this, std::placeholders::_1));
        NotifyAttachProcessOk(ProcessId, ThreadId, port, hwnd);
        QTimer::singleShot(100, this, SLOT(OnQueueQueryProcess()));
    }
    else
    {
        Disconnect();
        NotifyAttachProcessFailed(ProcessId, ThreadId, -3, tr("Could not connect to local RPC server."));
    }
}

void CProcessWorker::Disconnect()
{
    g_CGAInterface->Disconnect();
    if(m_AttachMutex != NULL)
    {
        CloseHandle(m_AttachMutex);
        m_AttachMutex = NULL;
    }
    m_AttachHwnd = NULL;
    m_AttachGamePort = 0;
}

void CProcessWorker::OnAutoAttachProcess(quint32 ProcessId, quint32 ThreadId)
{
    m_AutoAttachPID = ProcessId;
    m_AutoAttachTID = ThreadId;
}

void CProcessWorker::OnQueueAttachProcess(quint32 ProcessId, quint32 ThreadId, quint32 hWnd, QString dllPath)
{
    int port = 0;
    quint32 hwnd = 0;
    if(!ReadSharedData(ProcessId, port, hwnd))
    {
        int errorCode = -1;
        QString errorString;
        if(!InjectByMsgHook(ThreadId, hWnd, dllPath, errorCode, errorString))
        {
            NotifyAttachProcessFailed(ProcessId, ThreadId, errorCode, errorString);
            return;
        }
        CRetryAttachProcessTimer *timer = new CRetryAttachProcessTimer(ProcessId, ThreadId, this);
        connect(timer,SIGNAL(timeout()),this,SLOT(OnRetryAttachProcess()));
        timer->start( 500 );
    }
    else
    {
        //Already attached to game
        Disconnect();
        ConnectToServer(ProcessId, ThreadId, port, hwnd);
    }
}

bool CProcessWorker::CreateAttachMutex(quint32 ProcessId, quint32 ThreadId)
{
    WCHAR szMutex[32];
    wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
    auto AttachMutex = CreateMutexW(NULL, TRUE, szMutex);
    if(AttachMutex == NULL)
    {
        NotifyAttachProcessFailed(ProcessId, ThreadId, -4, tr("Game already attached by another instance of CGAssistant."));
        return false;
    }

    m_AttachMutex = AttachMutex;
    return true;
}

void CProcessWorker::OnQueueQueryProcess()
{
    CProcessItemList list;

    const wchar_t szFindGameClass[] = { 39764, 21147, 23453, 36125, 0 };

    HWND hWnd = NULL;
    DWORD pid, tid;
    WCHAR szText[256];

    while ((hWnd = FindWindowExW(NULL, hWnd, szFindGameClass, NULL)) != NULL)
    {
        if((tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&pid)) != 0 && pid != GetCurrentProcessId())
        {
            if(GetWindowTextW(hWnd, szText, 256))
            {
                bool attached = IsProcessAttached(pid);
                CProcessItemPtr item(new CProcessItem((quint32)pid, (quint32)tid, (quint32)hWnd, szText, attached));
                list.append(item);

                if(!attached && m_AutoAttachPID == pid && m_AutoAttachTID == tid){
                    OnQueueAttachProcess( (quint32)pid, (quint32)tid, (quint32)hWnd, QString("cgahook.dll") );
                    m_AutoAttachPID = 0;
                    m_AutoAttachTID = 0;
                }
            }
        }
    }


    NotifyQueryProcess(list);
}

void CProcessWorker::OnNotifyFillMaxFreezeTime(int freezetime)
{
    m_MaxFreezeTime = freezetime;
}

void CProcessWorker::OnKillProcess()
{
    if(!m_AttachHwnd)
        return;

    if(!IsWindow(m_AttachHwnd))
        return;

    DWORD pid, tid;
    tid = GetWindowThreadProcessId(m_AttachHwnd, &pid);
    if(!pid || !tid){
        return;
    }

    HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if(ProcessHandle)
    {
        if(STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
        {
            Disconnect();
        }
        CloseHandle(ProcessHandle);
    }
}

void CProcessWorker::OnCheckFreezeProcess()
{
    if(!m_AttachHwnd)
        return;

    if(!IsWindow(m_AttachHwnd))
        return;

    DWORD pid, tid;
    tid = GetWindowThreadProcessId(m_AttachHwnd, &pid);
    if(!pid || !tid){
        m_AttachHwnd = NULL;
        m_AttachGamePort = 0;
        return;
    }

    bool freeze = false;
    double tick;
    if(g_CGAInterface->IsConnected() && g_CGAInterface->GetNextAnimTickCount(tick))
    {
        if(m_LastGameAnimTick == tick)
        {
            m_AnimTickFreezeTime ++;
            if(m_AnimTickFreezeTime >= m_MaxFreezeTime)
                freeze = true;
        }
        else
        {
            m_AnimTickFreezeTime = 0;
            m_LastGameAnimTick = tick;
        }
    }
    else
    {
        m_AnimTickFreezeTime ++;
        if(m_AnimTickFreezeTime >= m_MaxFreezeTime)
            freeze = true;
    }

    //the game is freezed somehow, kill it
    if(freeze)
    {
        HANDLE ProcessHandle = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        if(ProcessHandle)
        {
            if(STATUS_SUCCESS == NtTerminateProcess(ProcessHandle, 0))
            {
                Disconnect();
            }
            CloseHandle(ProcessHandle);
        }
    }
}

void CProcessWorker::OnHttpGetGameProcInfo(QJsonDocument *doc)
{
    QJsonObject obj;

    ULONG pid, tid;
    if(m_AttachHwnd && IsWindow(m_AttachHwnd) && (tid = GetWindowThreadProcessId(m_AttachHwnd, &pid)) != 0)
    {
        obj.insert("errcode", 0);
        obj.insert("pid", (int)pid);
        obj.insert("tid", (int)tid);
        obj.insert("gameport", (int)m_AttachGamePort);
        obj.insert("hwnd", (int)m_AttachHwnd);
    }
    else
    {
        obj.insert("errcode", 1);
        obj.insert("message", tr("not attached to any game process yet."));
    }
    doc->setObject(obj);
}

bool IsProcessThreadPresent(quint32 ProcessId, quint32 ThreadId)
{
    bool bPresent = false;
    HANDLE ThreadHandle = OpenThread(THREAD_QUERY_LIMITED_INFORMATION, FALSE, ThreadId);
    if(ThreadHandle)
    {
        THREAD_BASIC_INFORMATION tbi;
        if(STATUS_SUCCESS == NtQueryInformationThread(ThreadHandle, ThreadBasicInformation, &tbi, sizeof(tbi), NULL))
        {
             if(tbi.ClientId.UniqueProcess == (HANDLE)ProcessId && tbi.ExitStatus == STATUS_PENDING){
                bPresent = true;
             }
        }
        CloseHandle(ThreadHandle);
    }
    return bPresent;
}
