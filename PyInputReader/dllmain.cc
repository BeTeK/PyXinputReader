// dllmain.cpp : Defines the entry point for the DLL application.
#include "dllmain.hh"
#include "WindowsHandler.hh"
#include "JoyPoller.hh"

unsigned int processCount = 0;
WindowsHandler* activeWindow = NULL;
JoyPoller* poller = NULL;
HMODULE hInst;


BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	{
		processCount += 1;
		break;
	}
	case DLL_THREAD_ATTACH:
	{
		break;
	}
	case DLL_THREAD_DETACH:
	{
		break;
	}
	case DLL_PROCESS_DETACH:
	{
		processCount -= 1;
		break;
	}
	}
	if (processCount == 1)
	{
		hInst = hModule;
	}

	if (processCount == 0)
	{
	}

	return TRUE;
}

HMODULE getModule()
{
	return hInst;
}
