#pragma once

#include "MINT.h"
#include "string"

typedef struct _INJECT_BUFFER
{
	UCHAR code[0x200];
	UCHAR original_code[8];
	DWORD64 hook_func;
	union
	{
		UNICODE_STRING64 path64;
		UNICODE_STRING32 path32;
	};

	WCHAR buffer[488];
	DWORD64 module;
} INJECT_BUFFER, *PINJECT_BUFFER;

class CInject
{
public:
	CInject();
	void Initialize();
	bool InjectByHookNtdll(HANDLE hProcess, const std::wstring &dllPath);
private:
	PVOID m_NtdllBase;
	PVOID m_pfnProtectVirtualMemory;
	PVOID m_pfnLdrLoadDll;
	PVOID m_pfnZwTestAlert;

private:

	PVOID AllocateInjectMemory(IN HANDLE ProcessHandle, IN PVOID DesiredAddress, IN SIZE_T DesiredSize);
	PINJECT_BUFFER GetInlineHookCode(IN HANDLE ProcessHandle, IN PUNICODE_STRING pDllPath);
};