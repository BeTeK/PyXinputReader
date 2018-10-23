#ifndef JOY_POLLER_HH
#define JOY_POLLER_HH

#include "WindowsHandler.hh"
#include "Joystick.hh"
#include "JoyState.hh"

#include <dinput.h>
#include <vector>

class JoyPoller
{
public:
	virtual ~JoyPoller();
	JoyPoller() = delete;
	JoyPoller(WindowsHandler& pWinHanler);
	JoyPoller& operator=(const JoyPoller& pRight) = delete;
	JoyPoller(const JoyPoller& pRight) = delete;

	const std::vector<JoyState>& poll();
	const std::vector<JoyState>& getStates();

	void rescan();
private:
	WindowsHandler& mWinHandler;
	static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);

	std::vector<Joystick::JoystickPtr> mJoyLst;
	std::vector<JoyState> mJoyStates;

	LPDIRECTINPUT8 mDI;
};

#endif // !JOY_POLLER_HH

