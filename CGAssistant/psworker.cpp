#include <QTimer>
#include "psworker.h"
#include "../CGALib/gameinterface.h"

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

    m_AttachMutex = NULL;
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
                    hWnd=data->hWnd;

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
        g_CGAInterface->RegisterServerShutdownNotify(std::bind(&CProcessWorker::NotifyServerShutdown, this, std::placeholders::_1));
        NotifyAttachProcessOk(ProcessId, port, hwnd);
        QTimer::singleShot(100, this, SLOT(OnQueueQueryProcess()));
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

void CProcessWorker::OnQueueAttachProcess(quint32 ProcessId, quint32 ThreadId, quint32 hWnd, QString dllPath)
{
    //Already attached to game
    Disconnect();

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

    const wchar_t szFindClass[] = { 39764, 21147, 23453, 36125, 0 };
    HWND hWnd = NULL;
    DWORD pid, tid;
    WCHAR szText[256];

    while ((hWnd = FindWindowExW(NULL, hWnd, szFindClass, NULL)) != NULL)
    {
        if((tid = GetWindowThreadProcessId(hWnd, (LPDWORD)&pid)) != 0 && pid != GetCurrentProcessId())
        {
            if(GetWindowTextW(hWnd, szText, 256))
            {
                CProcessItemPtr item(new CProcessItem((quint32)pid, (quint32)tid,(quint32)hWnd, szText, IsProcessAttached(pid)));
                list.append(item);
            }
        }
    }

    NotifyQueryProcess(list);
}
