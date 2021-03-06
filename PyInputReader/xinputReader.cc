#include "dllmain.hh"
#include "XReaderThread.hh"

#include <sstream>
#include <string>
#include <algorithm>

#undef min

#ifdef __cplusplus    // If used by C++ code, 
extern "C" {          // we need to export the C interface
#endif

_declspec (dllexport) const JoyStates* poll(XReaderThread* xThread)
{
	auto states = xThread->getStates();

	return states;
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

_declspec (dllexport) XReaderThread* start()
{
	auto thread = new XReaderThread(getModule());
	return thread;
}

_declspec (dllexport) void stop(XReaderThread* xThread)
{
	delete xThread;
}

_declspec (dllexport) void rescan(XReaderThread* xThread)
{
	xThread->rescan();
}

_declspec (dllexport) void setFreq(XReaderThread* xThread, int pFreq)
{
	xThread->setFreq(pFreq);
}

#ifdef __cplusplus
}
#endif