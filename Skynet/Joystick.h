#pragma once

namespace Communications {
	ref class PlaneWatcher;
}

struct DIJOYCONFIG;
struct XINPUT_DEVICE_NODE;

struct XINPUT_DEVICE_NODE
{
    DWORD dwVidPid;
    XINPUT_DEVICE_NODE* pNext;
};

struct DI_ENUM_CONTEXT
{
    DIJOYCONFIG* pPreferredJoyCfg;
    bool bPreferredJoyCfgValid;
};

struct JOY_STATUS
{
	double stickAngle;
	double thresholds;

};

namespace Skynet { ref class SkynetController; }

public ref class Joystick
{
public:
	Joystick( Object ^ theParent );
	virtual ~Joystick(void);

	void setDelegate(Skynet::SkynetController ^ newDelegate) { theDelegate = newDelegate; }

	HRESULT UpdateInputState( HWND hDlg );
	HRESULT init(HWND hDlg);
	Communications::PlaneWatcher ^ theWatcher;

protected:
	HRESULT SetupForIsXInputDevice();

	//HRESULT UpdateInputState( HWND hDlg );
	array<int>^ tracker;
	int zoom_level;

	VOID FreeDirectInput();
	void CleanupForIsXInputDevice();

	bool manualMode;
	int manualModeCounter;

	Object ^ parent;

	System::Timers::Timer ^ zoomTimer;
	int imageCounter;
	Skynet::SkynetController ^ theDelegate;
};
