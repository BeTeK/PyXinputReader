#include "JoyPoller.hh"

#ifndef STRICT
#define STRICT
#endif STRICT

#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_DEPRECATE
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <commctrl.h>
#include <basetsd.h>

#pragma warning(push)
#pragma warning(disable:6000 28251)
#include <dinput.h>
#pragma warning(pop)

#include <dinputd.h>
#include <assert.h>
#include <oleauto.h>
#include <shellapi.h>

#include <wbemidl.h>

#ifndef DIJ_RINGZERO

DEFINE_GUID(IID_IDirectInputEffectDriver, 0x02538130, 0x898F, 0x11D0, 0x9A, 0xD0, 0x00, 0xA0, 0xC9, 0xA0, 0x6E, 0x35);
DEFINE_GUID(IID_IDirectInputJoyConfig, 0x1DE12AB1, 0xC9F5, 0x11CF, 0xBF, 0xC7, 0x44, 0x45, 0x53, 0x54, 0x00, 0x00);
DEFINE_GUID(IID_IDirectInputPIDDriver, 0xEEC6993A, 0xB3FD, 0x11D2, 0xA9, 0x16, 0x00, 0xC0, 0x4F, 0xB9, 0x86, 0x38);

DEFINE_GUID(IID_IDirectInputJoyConfig8, 0xeb0d7dfa, 0x1990, 0x4f27, 0xb4, 0xd6, 0xed, 0xf2, 0xee, 0xc4, 0xa4, 0x4c);

#endif /* DIJ_RINGZERO */

JoyPoller::~JoyPoller()
{
	if (mDI != 0)
	{
		mDI->Release();
	}
}

JoyPoller::JoyPoller(WindowsHandler & pWinHanler) : mWinHandler(pWinHanler), mDI(0), mJoyLst(), mJoyStates()
{
	InitCommonControls();
	initialize(pWinHanler);
}

const std::vector<JoyState>& JoyPoller::poll()
{
	size_t curIndex = 0;
	for (auto joy : mJoyLst)
	{
		auto result = joy->poll();
		mJoyStates.at(curIndex) = result;
		curIndex += 1;
	}

	return getStates();
}

const std::vector<JoyState>& JoyPoller::getStates()
{
	return mJoyStates;
}

void JoyPoller::initialize(WindowsHandler & pWinHanler)
{
	HRESULT hr;
	if(FAILED(DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (VOID**)&mDI, nullptr)))
	{
		return;
	}

	DIJOYCONFIG PreferredJoyCfg = { 0 };
	IDirectInputJoyConfig8* pJoyConfig = nullptr;
	if (FAILED(hr = mDI->QueryInterface(IID_IDirectInputJoyConfig8, (void**)&pJoyConfig)))
	{
		return;
	}

	std::vector<DIDEVICEINSTANCE> instances;
	if (FAILED(hr = mDI->EnumDevices(DI8DEVCLASS_GAMECTRL,
		EnumJoysticksCallback,
		&instances, DIEDFL_ATTACHEDONLY)))
	{
		return;
	}

	for (auto inst : instances)
	{
		mJoyLst.push_back(Joystick::JoystickPtr(new Joystick(inst, mDI, pWinHanler.getHwnd())));
	}

	mJoyStates.resize(mJoyLst.size());
}



BOOL CALLBACK JoyPoller::EnumJoysticksCallback(const DIDEVICEINSTANCE * pdidInstance, VOID * pContext)
{
	auto& context = *reinterpret_cast<std::vector<DIDEVICEINSTANCE>*>(pContext);
	DIDEVICEINSTANCE tmp;
	std::memcpy(&tmp, pdidInstance, sizeof(tmp));
	context.push_back(tmp);

	return DIENUM_CONTINUE;
}

