#include "Joystick.hh"

#ifdef __cplusplus
extern "C" {
#endif
	extern const DIDATAFORMAT c_dfDIMouse;

#if(DIRECTINPUT_VERSION >= 0x0700)
	extern const DIDATAFORMAT c_dfDIMouse2;
#endif /* DIRECTINPUT_VERSION >= 0x0700 */

	extern const DIDATAFORMAT c_dfDIKeyboard;

#if(DIRECTINPUT_VERSION >= 0x0500)
	extern const DIDATAFORMAT c_dfDIJoystick;
	extern LPCDIDATAFORMAT WINAPI GetdfDIJoystick();

	extern const DIDATAFORMAT c_dfDIJoystick2;
#endif /* DIRECTINPUT_VERSION >= 0x0500 */

#ifdef __cplusplus
};
#endif


Joystick::Joystick(DIDEVICEINSTANCE pInstance, LPDIRECTINPUT8 pDInput, HWND hwnd) :
	mInst(pInstance),
	mDev(0),
	mInitialized(false),
	mSliderCount(0),
	mPOVCount(0),
	mXAxis(false),
	mYAxis(false),
	mZAxis(false),
	mRXAxis(false),
	mRYAxis(false),
	mRZAxis(false),
	mKeyCount(0),
	mCurState()
{
	HRESULT hr = pDInput->CreateDevice(pInstance.guidInstance, &mDev, nullptr);
	if (FAILED(hr))
	{
		return;
	}

	if (FAILED(hr = mDev->SetDataFormat(&c_dfDIJoystick2)))
	{
		return;
	}
	
	if (FAILED(hr = mDev->SetCooperativeLevel(hwnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE)))
	{
		return;
	}

	if (FAILED(hr = mDev->EnumObjects(EnumObjectsCallback, this, DIDFT_ALL)))
	{
		return;
	}
	memcpy(mCurState.name, mInst.tszInstanceName, MAX_PATH);

	mCurState.guid[0] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data1)[0];
	mCurState.guid[1] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data1)[1];
	mCurState.guid[2] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data1)[2];
	mCurState.guid[3] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data1)[3];

	mCurState.guid[4] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data2)[0];
	mCurState.guid[5] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data2)[1];

	mCurState.guid[6] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data3)[0];
	mCurState.guid[7] = reinterpret_cast<unsigned char*>(&mInst.guidInstance.Data3)[1];

	mCurState.guid[8] =  mInst.guidInstance.Data4[0];
	mCurState.guid[9] =  mInst.guidInstance.Data4[1];
	mCurState.guid[10] = mInst.guidInstance.Data4[2];
	mCurState.guid[11] = mInst.guidInstance.Data4[3];
	mCurState.guid[12] = mInst.guidInstance.Data4[4];
	mCurState.guid[13] = mInst.guidInstance.Data4[5];
	mCurState.guid[14] = mInst.guidInstance.Data4[6];
	mCurState.guid[15] = mInst.guidInstance.Data4[7];

	mInitialized = true;
}


Joystick::~Joystick()
{
	if (mDev != 0)
	{
		mDev->Unacquire();
		mDev->Release();
	}
}

bool Joystick::isInitialized() const
{
	return mDev != 0;
}

const JoyState Joystick::poll()
{
	HRESULT hr;
	DIJOYSTATE2 js;
	mCurState.connected = 0;

	hr = mDev->Poll();
	if(FAILED(hr))
	{
		hr = mDev->Acquire();
		if (FAILED(hr))
		{
			mCurState.connected = false;
			return mCurState;
		}
	}

	if (FAILED(hr = mDev->GetDeviceState(sizeof(js), &js)))
	{
		mCurState.connected = false;
		return mCurState;
	}

	int currentAxisIndex = 0;

	mCurState.connected = 1;

	SetAxis(mXAxis, mCurState.axis, currentAxisIndex, js.lX);
	SetAxis(mYAxis, mCurState.axis, currentAxisIndex, js.lY);
	SetAxis(mZAxis, mCurState.axis, currentAxisIndex, js.lZ);
	SetAxis(mRXAxis, mCurState.axis, currentAxisIndex, js.lRx);
	SetAxis(mRYAxis, mCurState.axis, currentAxisIndex, js.lRy);
	SetAxis(mRZAxis, mCurState.axis, currentAxisIndex, js.lRz);

	for (auto i = 0; i < mSliderCount; ++i)
	{
		mCurState.axis[currentAxisIndex++] = js.rglSlider[i];
	}

	for (auto i = 0; i < mPOVCount; ++i)
	{
		mCurState.axis[currentAxisIndex++] = js.rgdwPOV[i];
	}

	mCurState.axisCount = currentAxisIndex;
	for (auto i = 0; i < 128; ++i)
	{
		if (js.rgbButtons[i] && 0x80)
		{
			mCurState.buttons[i / 8] |= 1 << (i % 8);
		}
		else
		{
			mCurState.buttons[i / 8] &= ~(1 << (i % 8));
		}
	}
	mCurState.buttonCount = mKeyCount;

	return mCurState;
}

const JoyState Joystick::getState()
{
	return mCurState;
}

void Joystick::SetAxis(bool active, long* axisLst, int& index, long value)
{
	if (active)
	{
		axisLst[index++] = value;
	}
}



BOOL Joystick::EnumObjectsCallback(const DIDEVICEOBJECTINSTANCE * pdidoi, VOID * pContext)
{
	auto& joy = *reinterpret_cast<Joystick*>(pContext);

	//if (pdidoi->dwType & DIDFT_AXIS)
	//{
	//	DIPROPRANGE diprg;
	//	diprg.diph.dwSize = sizeof(DIPROPRANGE);
	//	diprg.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	//	diprg.diph.dwHow = DIPH_BYID;
	//	diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
	//	diprg.lMin = -1000;
	//	diprg.lMax = +1000;

	//	// Set the range for the axis
	//	if (FAILED(joy.mDev->SetProperty(DIPROP_RANGE, &diprg.diph)))
	//	{
	//		return DIENUM_STOP;
	//	}
	//}

	if (pdidoi->guidType == GUID_XAxis)
	{
		joy.mXAxis = true;
	}
	if (pdidoi->guidType == GUID_YAxis)
	{
		joy.mYAxis = true;
	}
	if (pdidoi->guidType == GUID_ZAxis)
	{
		joy.mZAxis = true;
	}
	if (pdidoi->guidType == GUID_RxAxis)
	{
		joy.mRXAxis = true;
	}
	if (pdidoi->guidType == GUID_RyAxis)
	{
		joy.mRYAxis = true;
	}
	if (pdidoi->guidType == GUID_RzAxis)
	{
		joy.mRZAxis = true;
	}
	if (pdidoi->guidType == GUID_Slider)
	{
		joy.mSliderCount += 1;
	}
	if (pdidoi->guidType == GUID_POV)
	{
		joy.mPOVCount += 1;
	}
	if (pdidoi->guidType == GUID_Button)
	{
		joy.mKeyCount += 1;
	}

	return DIENUM_CONTINUE;
}

