// xinputReader.cpp : Defines the exported functions for the DLL application.
//

#include "dllmain.hh"
#include "JoyState.hh"
#include "JoyPoller.hh"
#include "XReaderThread.hh"

#include <sstream>
#include <string>
#include <algorithm>

#undef min

XReaderThread* xThread = 0;
JoyStates joyStates;

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

_declspec (dllexport) JoyStates* poll()
{
	if (xThread != 0)
	{
		auto states = xThread->getStates();

		if (states.size() != joyStates.stateCount && joyStates.joyStates != 0)
		{
			delete[] joyStates.joyStates;
			joyStates.joyStates = 0;
		}

		if (joyStates.joyStates == 0 && states.size() != 0)
		{
			joyStates.joyStates = new JoyState[states.size()];
			joyStates.stateCount = (int)states.size();
		}

		if (states.size() != 0)
		{
			memcpy(joyStates.joyStates, &states[0], sizeof(states[0]) * states.size());
		}
	}

	return &joyStates;
}

_declspec (dllexport) const char* version()
{
	static char strBuffer[255];

	std::stringstream stream;
	stream << "xinputReader version 0.1";
	std::string str = stream.str();
	
	size_t maxLen = std::strlen(str.c_str());
	
	size_t size = std::min(maxLen, sizeof(strBuffer) / sizeof(strBuffer[0]) - 1);
	std::memcpy(strBuffer, str.c_str(), size);

	return strBuffer;
}

_declspec (dllexport) void start()
{
	if (xThread == 0)
	{
		joyStates.stateCount = 0;
		xThread = new XReaderThread(getModule());
	}
}

_declspec (dllexport) void stop()
{
	if (xThread != 0)
	{
		delete xThread;
		xThread = 0;
		joyStates.stateCount = 0;
		if (joyStates.joyStates != 0)
		{
			delete[] joyStates.joyStates;
			joyStates.joyStates = 0;
		}
	}
}

_declspec (dllexport) void rescan()
{
	if (xThread != 0)
	{
		xThread->rescan();
	}
}

#ifdef __cplusplus
}
#endif