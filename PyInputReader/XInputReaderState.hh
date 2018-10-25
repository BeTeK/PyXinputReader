#ifndef XINPUT_READER_STATE_HH
#define XINPUT_READER_STATE_HH

#include "JoyState.hh"
#include "XReaderThread.hh"

struct XInputReaderState
{
	JoyStates joyStates;
	XReaderThread* xThread
};

#endif // !XINPUT_READER_STATE_HH
