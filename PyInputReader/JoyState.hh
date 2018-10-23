#ifndef JOY_STATE_HH
#define JOY_STATE_HH

#include <dinput.h>

struct JoyState
{
	char connected;
	int axisCount;
	long axis[12];
	int buttonCount;
	char buttons[128];
	WCHAR name[MAX_PATH];
	unsigned char guid[16];
};

struct JoyStates
{
	int stateCount;
	JoyState* joyStates;
};
#endif // !JOY_STATE_HH
