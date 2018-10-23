
#ifndef DLLMAIN_H
#define DLLMAIN_H

#include "JoyPoller.hh"

#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved);
HMODULE getModule();

#endif // !DLLMAIN_H



