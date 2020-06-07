/*
	---- FireFest v4 by NtKernelMC
	Becoming God!
*/
#include <Windows.h>
#include <stdio.h>
#include <thread>
#include "FireFest.h"
using namespace std;
void __stdcall OwnThread()
{
	if (!GetModuleHandleA("samp.dll"))
	{
		while (!GetModuleHandleA("client.dll")) { Sleep(10); }
	}
	FireFest::InitHacks();
}
__forceinline void RtlThread(void) { thread Asyncer(OwnThread); Asyncer.detach(); }
int __stdcall DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH) RtlThread();
	return 1;
}