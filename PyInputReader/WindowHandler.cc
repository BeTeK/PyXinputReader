#include "WindowsHandler.hh"
#include "dllmain.hh"
#include "JoyPoller.hh"
#include <string>
#include <thread>

std::wstring FormatMsg(DWORD errNo);
const wchar_t* WindowsHandler::mClassName = L"xinputHandler";

WindowsHandler::WindowsHandler(HINSTANCE pInst) : mHwnd(NULL), mAtom(0), mStopRequested(false), mTimer(0)
{
	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = pInst;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = mClassName;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	mAtom = RegisterClassEx(&wc);
	if (mAtom == 0)
	{
		return;
	}

	//mHwnd = CreateWindowEx(WS_EX_TRANSPARENT,
	//	mClassName,
	//	L"captureWindow",
	//	WS_EX_CLIENTEDGE,
	//	1,
	//	1,
	//	100,
	//	100,
	//	NULL,
	//	NULL,
	//	pInst,
	//	NULL);
	mHwnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		mClassName,
		L"The title of my window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 240, 120,
		NULL, NULL, pInst, NULL);

	if (mHwnd == 0)
	{
		return;
	}

	mTimer = SetTimer(mHwnd, 0, 1000 / 200, nullptr);

}


std::wstring FormatMsg(DWORD errNo)
{

	LPVOID lpMsgBuf;
	DWORD dw = errNo;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	std::wstring ret((wchar_t*)lpMsgBuf);

	LocalFree(lpMsgBuf);
	return ret;
}

WindowsHandler::~WindowsHandler()
{
	if (mHwnd != 0 && mTimer != 0)
	{
		KillTimer(mHwnd, mTimer);
	}

	if (mHwnd != 0)
	{
		DestroyWindow(mHwnd);
	}

	if (mAtom != 0)
	{
		UnregisterClass(mClassName, 0);
	}
}

HWND WindowsHandler::getHwnd() const
{
	return mHwnd;
}

bool WindowsHandler::isInitialized() const
{
	return mHwnd != 0 && mAtom != 0;
}

LRESULT CALLBACK WindowsHandler::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_INITDIALOG:
		return TRUE;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;
	case WM_TIMER:
		return TRUE;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
}

bool WindowsHandler::processMessages()
{
	MSG Msg;
	while (PeekMessage(&Msg, mHwnd, 0, 0, PM_REMOVE) != 0)
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
		if (Msg.message == WM_TIMER)
		{
			return true;
		}
	}
	return false;
}

void WindowsHandler::waitMessages()
{
	WaitMessage();
}

void WindowsHandler::setTimerFreq(unsigned int pFreq)
{
	if (mHwnd != 0 && mTimer != 0)
	{
		KillTimer(mHwnd, mTimer);
	}

	auto freq = pFreq;
	if (freq > 1000)
	{
		freq = 1000;
	}
	if (freq == 0)
	{
		freq = 1;
	}

	mTimer = SetTimer(mHwnd, 0, 1000 / pFreq, nullptr);
}
