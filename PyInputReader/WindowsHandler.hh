#ifndef WINDOWS_HANDLER_HH
#define WINDOWS_HANDLER_HH

#include "windows.h"


class WindowsHandler
{
public:
	WindowsHandler() = delete;
	WindowsHandler(HINSTANCE pInst);
	virtual ~WindowsHandler();
	WindowsHandler(const WindowsHandler& pOne) = delete;
	WindowsHandler& operator = (const WindowsHandler& pOne) = delete;

	HWND getHwnd() const;
	bool isInitialized() const;
	bool processMessages();
	void waitMessages();
	void setTimerFreq(unsigned int pFreq);
private:
	static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static const wchar_t* mClassName;
	HWND mHwnd;
	ATOM mAtom;
	bool mStopRequested;
	UINT_PTR mTimer;
};

#endif // !WINDOWS_HANDLER_HH


