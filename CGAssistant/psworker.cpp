#include <QTimer>
#include "psworker.h"
#include "../CGALib/gameinterface.h"

#include "MINT.h"

extern CGA::CGAInterface *g_CGAInterface;

Q_DECLARE_METATYPE(CProcessItemList)

CRetryAttachProcessTimer::CRetryAttachProcessTimer(quint32 ProcessId, QObject *parent) : QTimer(parent)
{
    m_ProcessId = ProcessId;
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
    m_AutoAttachPID = 0;
    m_AutoAttachTID = 0;
    m_LastGameAnimTick = 0;
    m_AnimTickFreezeTime = 0;
    m_MaxFreezeTime = 15;
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
        if(WAIT_OBJECT_0 == WaitForSingleObject(hDataLock, 0))
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
            NotifyAttachProcessFailed(timer->m_ProcessId, -2, tr("Timeout for reading shared data."));
            return;
        }
        else
        {
            ++ timer->m_retry;
        }
    } else {
        timer->stop();
        timer->deleteLater();
        ConnectToServer(timer->m_ProcessId, port, hwnd);
    }
}

void CProcessWorker::ConnectToServer(quint32 ProcessId, int port, quint32 hwnd)
{
    if(!CreateAttachMutex(ProcessId))
        return;

    if(g_CGAInterface->Connect(port))
    {
        g_CGAInterface->GetNextAnimTickCount(m_LastGameAnimTick);
        g_CGAInterface->RegisterServerShutdownNotify(std::bind(&CProcessWorker::NotifyServerShutdown, this, std::placeholders::_1));
        NotifyAttachProcessOk(ProcessId, port, hwnd);
        QTimer::singleShot(100, this, SLOT(OnQueueQueryProcess()));
        m_AttachHwnd = (HWND)hwnd;
    }
    else
    {
        Disconnect();
        NotifyAttachProcessFailed(ProcessId, -3, tr("Could not connect to local RPC server."));
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
            NotifyAttachProcessFailed(ProcessId, errorCode, errorString);
            return;
        }
        CRetryAttachProcessTimer *timer = new CRetryAttachProcessTimer(ProcessId, this);
        connect(timer,SIGNAL(timeout()),this,SLOT(OnRetryAttachProcess()));
        timer->start( 500 );
    }
    else
    {
        //Already attached to game
        Disconnect();

        ConnectToServer(ProcessId, port, hwnd);
    }
}

bool CProcessWorker::CreateAttachMutex(quint32 ProcessId)
{
    WCHAR szMutex[32];
    wsprintfW(szMutex, L"CGAAttachMutex_%d", ProcessId);
    m_AttachMutex = CreateMutexW(NULL, TRUE, szMutex);
    if(m_AttachMutex == NULL && GetLastError() == ERROR_ALREADY_EXISTS)
    {
        NotifyAttachProcessFailed(ProcessId, -4, tr("Game already attached by other instance of CGAssistant."));
        return false;
    }
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

void CProcessWorker::OnCheckFreezeProcess()
{
    if(!IsWindow(m_AttachHwnd))
        return;

    DWORD pid, tid;
    tid = GetWindowThreadProcessId(m_AttachHwnd, &pid);
    if(!pid || !tid){
        m_AttachHwnd = NULL;
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
                m_AttachHwnd = NULL;
            }
            CloseHandle(ProcessHandle);
        }
    }
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
