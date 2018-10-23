#ifndef JOYSTICK_HH
#define JOYSTICK_HH

#ifndef STRICT
#define STRICT
#endif STRICT

#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_DEPRECATE
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include "JoyState.hh"

#include <dinput.h>
#include <memory>

class Joystick
{
public:
	typedef std::shared_ptr<Joystick> JoystickPtr;

	Joystick(DIDEVICEINSTANCE pInstance, LPDIRECTINPUT8 pDInput, HWND hwnd);
	Joystick(const Joystick& pJoy) = delete;
	virtual ~Joystick();
	Joystick& operator= (const Joystick& pOne) = delete;

	bool isInitialized() const;
	const JoyState poll();
	const JoyState getState();
private:
	static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pContext);
	static void SetAxis(bool active, long* axisLst, int& index, long value);

	int mSliderCount;
	int mPOVCount;
	int mKeyCount;
	bool mXAxis;
	bool mYAxis;
	bool mZAxis;
	bool mRXAxis;
	bool mRYAxis;
	bool mRZAxis;

	DIDEVICEINSTANCE mInst;
	LPDIRECTINPUTDEVICE8 mDev;
	bool mInitialized;
	JoyState mCurState;
};

#endif // !JOYSTICK_HH
