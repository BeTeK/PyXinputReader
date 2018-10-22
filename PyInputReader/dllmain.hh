
#ifndef DLLMAIN_H
#define DLLMAIN_H

#include "JoyPoller.hh"

#include <windows.h>

BOOL DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
HWND getWindow();
JoyPoller* getJoyPoller();
HMODULE getModule();

#endif // !DLLMAIN_H



