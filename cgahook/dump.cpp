extern "C"
{
#pragma warning (disable:4005)
#include <Windows.h>
}

#include <dbghelp.h>
#pragma comment(lib,"dbghelp.lib")

LONG WINAPI MinidumpCallback(EXCEPTION_POINTERS* pException)
{
	HANDLE hDumpFile = CreateFile(L"minidump.mdmp", GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDumpFile != INVALID_HANDLE_VALUE) {

		MINIDUMP_EXCEPTION_INFORMATION dumpInfo;
		dumpInfo.ExceptionPointers = pException;
		dumpInfo.ThreadId = GetCurrentThreadId();
		dumpInfo.ClientPointers = TRUE;

		MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hDumpFile, (MINIDUMP_TYPE)(MiniDumpNormal | MiniDumpWithFullMemory), &dumpInfo, NULL, NULL);
		CloseHandle(hDumpFile);
	}

	//MessageBox(NULL, L"A fatal error occured, sorry but we have to terminate this program.\nSee minidump for more information.", L"Fatal Error", MB_ICONWARNING);

	return EXCEPTION_EXECUTE_HANDLER;
}
