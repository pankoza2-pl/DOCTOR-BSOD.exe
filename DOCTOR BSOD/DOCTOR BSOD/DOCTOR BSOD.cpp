// DOCTOR BSOD.cpp : This Doctor will cause a BSOD with a random NTSTATUS (error) CODE!!!
// credits to YT-AleX-1337 for the true NRG
// please use the release x86 configuration
// version 2.0 - Added 2 Warnings and BSOD codes up to c000038e (last one available on 32-bit XP)
#include <windows.h>
//#include <tlhelp32.h>
typedef NTSTATUS(NTAPI* NRHEdef)(NTSTATUS, ULONG, ULONG, PULONG, ULONG, PULONG);
typedef NTSTATUS(NTAPI* RAPdef)(ULONG, BOOLEAN, BOOLEAN, PBOOLEAN);
HCRYPTPROV hProv;
int Random()
{
	if (!hProv)
		CryptAcquireContextA(&hProv, 0, 0, PROV_RSA_FULL, CRYPT_SILENT | CRYPT_VERIFYCONTEXT);

	int out = 0;
	CryptGenRandom(hProv, sizeof(out), (BYTE*)(&out)); //Generate random number
	return out & 0x7FFFFFFF;
}
int DRBSOD() {
	BOOLEAN bl;
	DWORD response;
	NRHEdef NtRaiseHardError = (NRHEdef)GetProcAddress(LoadLibraryW(L"ntdll"), "NtRaiseHardError");
	RAPdef RtlAdjustPrivilege = (RAPdef)GetProcAddress(LoadLibraryW(L"ntdll"), "RtlAdjustPrivilege");
	RtlAdjustPrivilege(19, 1, 0, &bl);
	NtRaiseHardError(0xC0000001 + Random() % 0x38e, 0, 0, 0, 6, &response); //Trigger BSoD

	// If the computer is still running, do it the normal way
	HANDLE token;
	TOKEN_PRIVILEGES privileges;

	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token);

	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &privileges.Privileges[0].Luid);
	privileges.PrivilegeCount = 1;
	privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	AdjustTokenPrivileges(token, FALSE, &privileges, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	// The actual restart
	ExitWindowsEx(EWX_REBOOT | EWX_FORCE, SHTDN_REASON_MAJOR_HARDWARE | SHTDN_REASON_MINOR_DISK);
	return 0;
}
int CALLBACK WinMain(
	HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine, int       nCmdShow
) {
	if (MessageBoxW(NULL, L"Run DOCTOR BSOD?", L"DOCTOR BSOD 2.0 by pankoza", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
	{
		ExitProcess(0);
	}
	else
	{
		if (MessageBoxW(NULL, L"Are you sure? It will crash your computer with a random error code, continue?", L"Last Warning - DOCTOR BSOD 2.0 by pankoza", MB_YESNO | MB_ICONEXCLAMATION) == IDNO)
		{
			ExitProcess(0);
		}
		else
		{
			SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS); //Max priority class
			DRBSOD();
		}
	}
}
