#include "MINT.h"
#include "inject.h"

#define ULONG_TO_ULONG64(addr) ((ULONG64)addr & 0xFFFFFFFF)
#define PVOID_TO_ULONG64(addr) (sizeof(addr) == 4 ? ULONG_TO_ULONG64(addr) : (ULONG64)addr)


CInject::CInject()
{
	m_NtdllBase = NULL;
	m_pfnProtectVirtualMemory = NULL;
	m_pfnLdrLoadDll = NULL;
	m_pfnZwTestAlert = NULL;
}

void CInject::Initialize()
{
	m_NtdllBase = GetModuleHandleW(L"ntdll.dll");
	if (m_NtdllBase)
	{
		m_pfnProtectVirtualMemory = GetProcAddress((HMODULE)m_NtdllBase, "NtProtectVirtualMemory");
		m_pfnLdrLoadDll = GetProcAddress((HMODULE)m_NtdllBase, "LdrLoadDll");
		m_pfnZwTestAlert = GetProcAddress((HMODULE)m_NtdllBase, "ZwTestAlert");
	}

	SYSTEM_INFO si;
	GetNativeSystemInfo(&si);
}

const UCHAR HookCode[] =
{
	0x55,									// push        ebp
	0x8B, 0xEC,								// mov         ebp,esp
	0x83, 0xEC, 0x0C,						// sub         esp,0Ch
	0xA1, 0, 0, 0, 0,						// mov         eax,dword ptr[fnHookFunc] //offset +7
	0x89, 0x45, 0xF4,						// mov         dword ptr[ebp-0Ch],eax
	0x8D, 0x45, 0xFC,						// lea         eax,[ebp - 4]
	0x50,									// push        eax
	0x6A, 0x40,								// push        40h
	0x8D, 0x45, 0xF8,						// lea         eax,[ebp - 8]
	0xC7, 0x45, 0xF8, 5, 0, 0, 0,			// mov         dword ptr[ebp - 8],5
	0x50,									// push        eax
	0x8D, 0x45, 0xF4,						// lea         eax,[ebp - 0Ch]
	0x50,									// push        eax
	0x6A, 0xFF,								// push        0FFFFFFFFh
	0xE8, 0, 0, 0, 0,						// call        NtProtectVirtualMemory //offset +38
	0x8B, 0x0D, 0, 0, 0, 0,					// mov         ecx,dword ptr ds : [fnHookFunc] //offset + 44
	0xA1, 0, 0, 0, 0,						// mov         eax,dword ptr ds : [fnOriCode] //offset + 49
	0x89, 0x01,								// mov         dword ptr[ecx],eax
	0xA0, 0, 0, 0, 0,						// mov         al,byte ptr ds : [fnOriCode+4] //offset +56
	0x88, 0x41, 0x04,						// mov         byte ptr[ecx + 4],al
	0x8D, 0x45, 0xFC,						// lea         eax,[ebp-4]
	0x50,									// push        eax
	0xFF, 0x75, 0xFC,						// push        dword ptr[ebp-4]
	0x8D, 0x45, 0xF8,						// lea         eax,[ebp - 8]
	0x50,									// push        eax
	0x8D, 0x45, 0xF4,						// lea         eax,[ebp - 0Ch]
	0x50,									// push        eax
	0x6A, 0xFF,								// push        0FFFFFFFFh
	0xE8, 0, 0, 0, 0,                       // call        NtProtectVirtualMemory //offset +81
	0x68, 0, 0, 0, 0,                       // push        ModuleHandle           //offset +86
	0x68, 0, 0, 0, 0,                       // push        ModuleFileName         //offset +91
	0x6A, 0,                                // push        0  
	0x6A, 0,                                // push        0
	0xE8, 0, 0, 0, 0,                       // call        LdrLoadDll              //offset +100
	0x8B, 0xE5,								// mov         esp,ebp
	0x5D,									// pop         ebp
	0xE9, 0, 0, 0, 0,						// jmp								   //offset+108
	0xCC,									// padding
};

PVOID CInject::AllocateInjectMemory(IN HANDLE ProcessHandle, IN PVOID DesiredAddress, IN SIZE_T DesiredSize)
{
	MEMORY_BASIC_INFORMATION mbi;
	SIZE_T AllocateSize = DesiredSize;

	if ((ULONG_PTR)DesiredAddress >= 0x70000000 && (ULONG_PTR)DesiredAddress < 0x80000000)
		DesiredAddress = (PVOID)0x70000000;

	while (1)
	{
		if (!NT_SUCCESS(NtQueryVirtualMemory(ProcessHandle, DesiredAddress, MemoryBasicInformation, &mbi, sizeof(mbi), NULL)))
			return NULL;

		if (DesiredAddress != mbi.AllocationBase)
		{
			DesiredAddress = mbi.AllocationBase;
		}
		else
		{
			DesiredAddress = (PVOID)((ULONG_PTR)mbi.AllocationBase - 0x10000);
		}

		if (mbi.State == MEM_FREE)
		{
			if (NT_SUCCESS(NtAllocateVirtualMemory(ProcessHandle, &mbi.BaseAddress, 0, &AllocateSize, MEM_RESERVE, PAGE_EXECUTE_READWRITE)))
			{
				if (NT_SUCCESS(NtAllocateVirtualMemory(ProcessHandle, &mbi.BaseAddress, 0, &AllocateSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE)))
				{
					return mbi.BaseAddress;
				}
			}
		}
	}
	return NULL;
}

PINJECT_BUFFER CInject::GetInlineHookCode(IN HANDLE ProcessHandle, IN PUNICODE_STRING pDllPath)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	PINJECT_BUFFER pBuffer = NULL;
	INJECT_BUFFER Buffer = { 0 };

	//Try to allocate before ntdll.dll
	pBuffer = (PINJECT_BUFFER)AllocateInjectMemory(ProcessHandle, m_NtdllBase, 0x1000);
	if (pBuffer != NULL)
	{
		status = NtReadVirtualMemory(ProcessHandle, m_pfnZwTestAlert, Buffer.original_code, sizeof(Buffer.original_code), NULL);
		if (NT_SUCCESS(status))
		{
			// Fill data
			Buffer.path32.Length = min(pDllPath->Length, sizeof(Buffer.buffer));
			Buffer.path32.MaximumLength = min(pDllPath->MaximumLength, sizeof(Buffer.buffer));
			Buffer.path32.Buffer = (ULONG)pBuffer->buffer;
			Buffer.hook_func = PVOID_TO_ULONG64(m_pfnZwTestAlert);
			memcpy(Buffer.buffer, pDllPath->Buffer, Buffer.path32.Length);
			memcpy(Buffer.code, HookCode, sizeof(HookCode));

			// Fill code
			*(DWORD*)((PUCHAR)Buffer.code + 7) = (DWORD)&pBuffer->hook_func;
			*(DWORD*)((PUCHAR)Buffer.code + 38) = (DWORD)((DWORD)m_pfnProtectVirtualMemory - ((DWORD)pBuffer + 42));
			*(DWORD*)((PUCHAR)Buffer.code + 44) = (DWORD)&pBuffer->hook_func;
			*(DWORD*)((PUCHAR)Buffer.code + 49) = (DWORD)pBuffer->original_code;
			*(DWORD*)((PUCHAR)Buffer.code + 56) = (DWORD)pBuffer->original_code + 4;
			*(DWORD*)((PUCHAR)Buffer.code + 81) = (DWORD)((DWORD)m_pfnProtectVirtualMemory - ((DWORD)pBuffer + 85));
			*(DWORD*)((PUCHAR)Buffer.code + 86) = (DWORD)&pBuffer->module;
			*(DWORD*)((PUCHAR)Buffer.code + 91) = (DWORD)&pBuffer->path32;
			*(DWORD*)((PUCHAR)Buffer.code + 100) = (DWORD)((DWORD)m_pfnLdrLoadDll - ((DWORD)pBuffer + 104));
			*(DWORD*)((PUCHAR)Buffer.code + 108) = (DWORD)((DWORD)m_pfnZwTestAlert - ((DWORD)pBuffer + 112));

			// Copy all
			NtWriteVirtualMemory(ProcessHandle, pBuffer, &Buffer, sizeof(Buffer), NULL);

			return pBuffer;
		}
	}

	return NULL;
}

bool CInject::InjectByHookNtdll(HANDLE hProcess, const std::wstring &dllPath)
{
	UNICODE_STRING ustrDllPath;
	RtlInitUnicodeString(&ustrDllPath, (PWCHAR)dllPath.c_str());
	PINJECT_BUFFER pBuffer = GetInlineHookCode(hProcess, &ustrDllPath);
	if (pBuffer)
	{
		UCHAR trampo[] = { 0xE9, 0, 0, 0, 0 };
		ULONG ReturnLength, OldProtect = 0;
		PVOID ProtectAddress = m_pfnZwTestAlert;
		SIZE_T ProtectSize = sizeof(trampo);

		*(DWORD *)(trampo + 1) = (DWORD)((DWORD)pBuffer->code - ((DWORD)m_pfnZwTestAlert + 5));

		NTSTATUS status = NtProtectVirtualMemory(hProcess, &ProtectAddress, &ProtectSize, PAGE_EXECUTE_READWRITE, &OldProtect);
		if (NT_SUCCESS(status))
		{
			NtWriteVirtualMemory(hProcess, m_pfnZwTestAlert, trampo, sizeof(trampo), &ReturnLength);
			NtProtectVirtualMemory(hProcess, &ProtectAddress, &ProtectSize, OldProtect, &OldProtect);
			return true;
		}
	}
	return false;
}