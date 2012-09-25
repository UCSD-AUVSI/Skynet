#include "StdAfx.h"
#include "MasterHeader.h"

#include "SkynetControllerInterface.h"

//button bindings
#define BTN_SQUARE 0
#define BTN_X 1
#define BTN_CIRCLE 2
#define BTN_TRIANGLE 3
#define BTN_L1 4
#define BTN_R1 5
#define BTN_L2 6
#define BTN_R2 7
#define BTN_SELECT 8
#define BTN_START 9
#define BTN_L3 10 // LEFT STICK
#define BTN_R3 11


#define ZOOM_TIME				800
#define GIMBAL_FACTOR			4	// 1..8, 4 is best
#define EXPONENTIAL_FACTOR		4.0
#define ZOOM_NONE				-1
#define ZOOM_IN					0
#define ZOOM_OUT				1



#define INTENDED_POSITION_UPDATES_ENABLED	true

#define DIRECTINPUT_VERSION 0x0800
#define _CRT_SECURE_NO_DEPRECATE
#define JOYSTICK_PI 3.14159265358979323846264338327950288
#ifndef _WIN32_DCOM
#define _WIN32_DCOM
#endif

#pragma warning( disable : 4995 ) // disable deprecated warning 
#pragma warning( disable : 4996 ) // disable deprecated warning 
#include <windows.h>
#include <commctrl.h>
#include <dinput.h>
#include <dinputd.h>
#include <math.h>
#include <wbemidl.h>

#include <strsafe.h>

#include "Form1.h"
#include "Joystick.h"
#include "Comport.h"
#pragma warning( default : 4995 )
#pragma warning( default : 4996 )

#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }



// Function decls
BOOL CALLBACK EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext );
BOOL CALLBACK EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext );

bool IsXInputDevice( const GUID* pGuidProductFromDirectInput );


bool                    g_bFilterOutXinputDevices;
XINPUT_DEVICE_NODE*     g_pXInputDeviceList;

LPDIRECTINPUT8          g_pDI;
LPDIRECTINPUTDEVICE8    g_pJoystick;


static Communications::ComportUpstream * lastPacket;
using namespace System;
using namespace System::Timers;
//using namespace Skynet;

Joystick::Joystick( Object ^ theParent )
{
	g_bFilterOutXinputDevices = false;
	g_pXInputDeviceList = NULL;

	g_pDI = NULL;
	g_pJoystick = NULL;
	tracker = gcnew array<int>(12);
	for(int i = 0; i < 12; i++)
	{
		tracker[i] = 0;
	}
	zoom_level = 1;

	parent = theParent;

	manualMode = false;
	manualModeCounter = 0;
	lastPacket = NULL;
	imageCounter = 0;
}

Joystick::~Joystick(void)
{
	FreeDirectInput();
}

int
Joystick::getZoom(void)
{
	return zoom_level;
}

void
Joystick::setZoom( int level )
{
	if( level > 0 )
		zoom_level = level;
}



void Joystick::sendZoom( Object^ source, ElapsedEventArgs^ e )
{
	System::Diagnostics::Trace::WriteLine("sendZoom started");
	int newZoomLevel = theWatcher->zoomLevel;
	if (newZoomLevel == ZOOM_IN)
		newZoomLevel++;
	else if (newZoomLevel == ZOOM_OUT)
		newZoomLevel--;
	else
		return;

	if (newZoomLevel > MAX_ZOOM_LEVEL)
		newZoomLevel = MAX_ZOOM_LEVEL;
	if (newZoomLevel < MIN_ZOOM_LEVEL)
		newZoomLevel = MIN_ZOOM_LEVEL;
	theWatcher->zoomLevel = newZoomLevel;

	unsigned __int32 zoom = 0;
	switch( newZoomLevel )
	{
		case 1:
			zoom = 0x00000000;
			break;
		case 2:
			zoom = 0x00080000;
			break;
		case 3:
			zoom = 0x01000000;
			break;
		case 4:
			zoom = 0x01080000;
			break;
		case 5:
			zoom = 0x02000000;
			break;
		case 6:
			zoom = 0x02080000;
			break;
		case 7:
			zoom = 0x03000000;
			break;
		case 8:
			zoom = 0x03080000;
			break;
		case 9:
			zoom = 0x04000000;
			break;
		default:
			zoom = 0xAAAAAAAA; // no zoom update
			break;
	}
	comm->sendZoom(zoom);
	System::Diagnostics::Trace::WriteLine("sendZoom sent");
	
}

HRESULT
Joystick::init(HWND hDlg)
{
	HRESULT hr;

	
	zoomTimer = gcnew Timers::Timer(ZOOM_TIME/10);
	zoomTimer->Elapsed += gcnew ElapsedEventHandler( this, &Joystick::sendZoom );
	zoomTimer->Stop();
	zoomDirection = ZOOM_NONE;

    // Register with the DirectInput subsystem and get a pointer
    // to a IDirectInput interface we can use.
    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle( NULL ), DIRECTINPUT_VERSION,
                                         IID_IDirectInput8, ( VOID** )&g_pDI, NULL ) ) )
        return hr;

    if( g_bFilterOutXinputDevices )
        SetupForIsXInputDevice();

    DIJOYCONFIG PreferredJoyCfg = {0};
    DI_ENUM_CONTEXT enumContext;
    enumContext.pPreferredJoyCfg = &PreferredJoyCfg;
    enumContext.bPreferredJoyCfgValid = false;

    IDirectInputJoyConfig8* pJoyConfig = NULL;
    if( FAILED( hr = g_pDI->QueryInterface( IID_IDirectInputJoyConfig8, ( void** )&pJoyConfig ) ) )
        return hr;

    PreferredJoyCfg.dwSize = sizeof( PreferredJoyCfg );
    if( SUCCEEDED( pJoyConfig->GetConfig( 0, &PreferredJoyCfg, DIJC_GUIDINSTANCE ) ) ) // This function is expected to fail if no Joystick is attached
        enumContext.bPreferredJoyCfgValid = true;
    SAFE_RELEASE( pJoyConfig );

    // Look for a simple Joystick we can use for this sample program.
    if( FAILED( hr = g_pDI->EnumDevices( DI8DEVCLASS_GAMECTRL,
                                         EnumJoysticksCallback,
                                         &enumContext, DIEDFL_ATTACHEDONLY ) ) )
        return hr;

    if( g_bFilterOutXinputDevices )
        CleanupForIsXInputDevice();

    // Make sure we got a Joystick
    if( NULL == g_pJoystick )
    {
        // joystick not found
		
        return S_OK;
    }

    // Set the data format to "simple Joystick" - a predefined data format 
    //
    // A data format specifies which controls on a device we are interested in,
    // and how they should be reported. This tells DInput that we will be
    // passing a DIJOYSTATE2 structure to IDirectInputDevice::GetDeviceState().
	if( FAILED( hr = g_pJoystick->SetDataFormat( &c_dfDIJoystick2 ) ) )
	{
		System::Diagnostics::Trace::WriteLine("returned1");
        return hr;
	}

    // Set the cooperative level to let DInput know how this device should
    // interact with the system and with other DInput applications.
    /*if( FAILED( hr = g_pJoystick->SetCooperativeLevel( hDlg, DISCL_EXCLUSIVE |
                                                       DISCL_FOREGROUND ) ) )
	{
		System::Diagnostics::Trace::WriteLine("returned2");
        return hr;
	}*/

    // Enumerate the Joystick objects. The callback function enabled user
    // interface elements for objects that are found, and sets the min/max
    // values property for discovered axes.
    if( FAILED( hr = g_pJoystick->EnumObjects( EnumObjectsCallback, ( VOID* )hDlg, DIDFT_ALL ) ) )
	{
		System::Diagnostics::Trace::WriteLine("\nEnumCallback not called");	
        return hr;
	}

    return S_OK;
}

HRESULT 
Joystick::SetupForIsXInputDevice()
{
    IWbemServices* pIWbemServices = NULL;
    IEnumWbemClassObject* pEnumDevices = NULL;
    IWbemLocator* pIWbemLocator = NULL;
    IWbemClassObject* pDevices[20] = {0};
    BSTR bstrDeviceID = NULL;
    BSTR bstrClassName = NULL;
    BSTR bstrNamespace = NULL;
    DWORD uReturned = 0;
    bool bCleanupCOM = false;
    UINT iDevice = 0;
    VARIANT var;
    HRESULT hr;

    // CoInit if needed
    hr = CoInitialize( NULL );
    bCleanupCOM = SUCCEEDED( hr );

    // Create WMI
    hr = CoCreateInstance( __uuidof( WbemLocator ),
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           __uuidof( IWbemLocator ),
                           ( LPVOID* )&pIWbemLocator );
    if( FAILED( hr ) || pIWbemLocator == NULL )
        goto LCleanup;

    // Create BSTRs for WMI
    bstrNamespace = SysAllocString( L"\\\\.\\root\\cimv2" ); if( bstrNamespace == NULL ) goto LCleanup;
    bstrDeviceID = SysAllocString( L"DeviceID" );           if( bstrDeviceID == NULL )  goto LCleanup;
    bstrClassName = SysAllocString( L"Win32_PNPEntity" );    if( bstrClassName == NULL ) goto LCleanup;

    // Connect to WMI 
    hr = pIWbemLocator->ConnectServer( bstrNamespace, NULL, NULL, 0L,
                                       0L, NULL, NULL, &pIWbemServices );
    if( FAILED( hr ) || pIWbemServices == NULL )
        goto LCleanup;

    // Switch security level to IMPERSONATE
    CoSetProxyBlanket( pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
                       RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0 );

    // Get list of Win32_PNPEntity devices
    hr = pIWbemServices->CreateInstanceEnum( bstrClassName, 0, NULL, &pEnumDevices );
    if( FAILED( hr ) || pEnumDevices == NULL )
        goto LCleanup;

    // Loop over all devices
    for(; ; )
    {
        // Get 20 at a time
        hr = pEnumDevices->Next( 10000, 20, pDevices, &uReturned );
        if( FAILED( hr ) )
            goto LCleanup;
        if( uReturned == 0 )
            break;

        for( iDevice = 0; iDevice < uReturned; iDevice++ )
        {
            // For each device, get its device ID
            hr = pDevices[iDevice]->Get( bstrDeviceID, 0L, &var, NULL, NULL );
            if( SUCCEEDED( hr ) && var.vt == VT_BSTR && var.bstrVal != NULL )
            {
                // Check if the device ID contains "IG_".  If it does, then it’s an XInput device
                // Unfortunately this information can not be found by just using DirectInput 
                if( wcsstr( var.bstrVal, L"IG_" ) )
                {
                    // If it does, then get the VID/PID from var.bstrVal
                    DWORD dwPid = 0, dwVid = 0;
                    WCHAR* strVid = wcsstr( var.bstrVal, L"VID_" );
                    if( strVid && swscanf_s( strVid, L"VID_%4X", &dwVid ) != 1 )
                        dwVid = 0;
                    WCHAR* strPid = wcsstr( var.bstrVal, L"PID_" );
                    if( strPid && swscanf_s( strPid, L"PID_%4X", &dwPid ) != 1 )
                        dwPid = 0;

                    DWORD dwVidPid = MAKELONG( dwVid, dwPid );

                    // Add the VID/PID to a linked list
                    XINPUT_DEVICE_NODE* pNewNode = new XINPUT_DEVICE_NODE;
                    if( pNewNode )
                    {
                        pNewNode->dwVidPid = dwVidPid;
                        pNewNode->pNext = g_pXInputDeviceList;
                        g_pXInputDeviceList = pNewNode;
                    }
                }
            }
            SAFE_RELEASE( pDevices[iDevice] );
        }
    }

LCleanup:
    if( bstrNamespace )
        SysFreeString( bstrNamespace );
    if( bstrDeviceID )
        SysFreeString( bstrDeviceID );
    if( bstrClassName )
        SysFreeString( bstrClassName );
    for( iDevice = 0; iDevice < 20; iDevice++ )
    SAFE_RELEASE( pDevices[iDevice] );
    SAFE_RELEASE( pEnumDevices );
    SAFE_RELEASE( pIWbemLocator );
    SAFE_RELEASE( pIWbemServices );

    return hr;
}

//-----------------------------------------------------------------------------
// Returns true if the DirectInput device is also an XInput device.
// Call SetupForIsXInputDevice() before, and CleanupForIsXInputDevice() after
//-----------------------------------------------------------------------------
bool 
IsXInputDevice( const GUID* pGuidProductFromDirectInput )
{
    // Check each xinput device to see if this device's vid/pid matches
    XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
    while( pNode )
    {
        if( pNode->dwVidPid == pGuidProductFromDirectInput->Data1 )
            return true;
        pNode = pNode->pNext;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Name: EnumJoysticksCallback()
// Desc: Called once for each enumerated Joystick. If we find one, create a
//       device interface on it so we can play with it.
//-----------------------------------------------------------------------------
BOOL CALLBACK 
EnumJoysticksCallback( const DIDEVICEINSTANCE* pdidInstance,
                                     VOID* pContext )
{
    DI_ENUM_CONTEXT* pEnumContext = ( DI_ENUM_CONTEXT* )pContext;
    HRESULT hr;

    if( g_bFilterOutXinputDevices && IsXInputDevice( &pdidInstance->guidProduct ) )
        return DIENUM_CONTINUE;

    // Skip anything other than the perferred Joystick device as defined by the control panel.  
    // Instead you could store all the enumerated Joysticks and let the user pick.
    if( pEnumContext->bPreferredJoyCfgValid &&
        !IsEqualGUID( pdidInstance->guidInstance, pEnumContext->pPreferredJoyCfg->guidInstance ) )
        return DIENUM_CONTINUE;

    // Obtain an interface to the enumerated Joystick.
    hr = g_pDI->CreateDevice( pdidInstance->guidInstance, &g_pJoystick, NULL );

    // If it failed, then we can't use this Joystick. (Maybe the user unplugged
    // it while we were in the middle of enumerating it.)
    if( FAILED( hr ) )
        return DIENUM_CONTINUE;

    // Stop enumeration. Note: we're just taking the first Joystick we get. You
    // could store all the enumerated Joysticks and let the user pick.
    return DIENUM_STOP;
}

//-----------------------------------------------------------------------------
// Name: EnumObjectsCallback()
// Desc: Callback function for enumerating objects (axes, buttons, POVs) on a 
//       Joystick. This function enables user interface elements for objects
//       that are found to exist, and scales axes min/max values.
//-----------------------------------------------------------------------------
BOOL CALLBACK 
EnumObjectsCallback( const DIDEVICEOBJECTINSTANCE* pdidoi,
                                   VOID* pContext )
{
    HWND hDlg = ( HWND )pContext;

    static int nSliderCount = 0;  // Number of returned slider controls
    static int nPOVCount = 0;     // Number of returned POV controls

    // For axes that are returned, set the DIPROP_RANGE property for the
    // enumerated axis in order to scale min/max values.
    if( pdidoi->dwType & DIDFT_AXIS )
    {
        DIPROPRANGE diprg;
        diprg.diph.dwSize = sizeof( DIPROPRANGE );
        diprg.diph.dwHeaderSize = sizeof( DIPROPHEADER );
        diprg.diph.dwHow = DIPH_BYID;
        diprg.diph.dwObj = pdidoi->dwType; // Specify the enumerated axis
        diprg.lMin = -1000;
        diprg.lMax = +1000;

        // Set the range for the axis
        if( FAILED( g_pJoystick->SetProperty( DIPROP_RANGE, &diprg.diph ) ) )
            return DIENUM_STOP;
    }

	//System::Diagnostics::Trace::WriteLine("EnumObjectsCallback\n");
    return DIENUM_CONTINUE;
}

//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID 
Joystick::FreeDirectInput()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( g_pJoystick )
        g_pJoystick->Unacquire();

    // Release any DirectInput objects.
    SAFE_RELEASE( g_pJoystick );
    SAFE_RELEASE( g_pDI );
}


HRESULT 
Joystick::UpdateInputState( HWND hDlg )
{
	//PRINT("Start: " + System::DateTime::Now.Second + "." + System::DateTime::Now.Millisecond );
	//System::Diagnostics::Trace::WriteLine("UpdateInputState");
    HRESULT hr;
    TCHAR strText[512] = {0}; // Device state text
    DIJOYSTATE2 js;           // DInput Joystick state 
	JOY_STATUS joyie;
	bool change = false;			  // flag for any changes
	bool zoomChanged = false;

	unsigned __int16 newGimbalRoll = theWatcher->gimbalRoll;
	unsigned __int16 newGimbalPitch = theWatcher->gimbalPitch;
	int newZoomLevel = theWatcher->zoomLevel;

    if( NULL == g_pJoystick )
        return S_OK;

    // Poll the device to read the current state
    hr = g_pJoystick->Poll();
    if( FAILED( hr ) )
    {
        // DInput is telling us that the input stream has been
        // interrupted. We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done. We
        // just re-acquire and try again.
        hr = g_pJoystick->Acquire();
        while( hr == DIERR_INPUTLOST )
            hr = g_pJoystick->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK;
    }

    // Get the input's device state
    if( FAILED( hr = g_pJoystick->GetDeviceState( sizeof( DIJOYSTATE2 ), &js ) ) )
        return hr; // The device should have been acquired during the Poll()
	
	//Communications::ComportUpstream * packet = new Communications::ComportUpstream();




	//Calculate JOY_STATUS things.
	joyie.thresholds = 1;
	bool exponential = true;
	double localX, localY;

	// calculate joystick positions
	if(js.lY > 200  || js.lX > 200 || js.lY < -200 || js.lX < -200){
		manualMode = true;
		manualModeCounter = 0;	
		
		localX = js.lX;
		localY = js.lY;
		double thresholdRange = 1000.00/joyie.thresholds;
		localX = js.lX / thresholdRange;
		localY = js.lY / thresholdRange;

		change = true;

		
		if(exponential){
			if(localX < 0) {
				localX = -( pow(EXPONENTIAL_FACTOR, (-localX))); // output = (EXPONENTIAL_FACTOR^input - 1)/(EXPONENTIAL_FACTOR - 1)
				localX += 1;
				localX /= EXPONENTIAL_FACTOR - 1;
			}
			else{			
				localX =  pow(EXPONENTIAL_FACTOR, localX);
				localX -= 1;
				localX /= EXPONENTIAL_FACTOR - 1;
			}
			if(localY < 0) {
				localY = -( pow(EXPONENTIAL_FACTOR, (-localY)));
				localY += 1;
				localY /= EXPONENTIAL_FACTOR - 1;
			}
			else{
				localY = pow(EXPONENTIAL_FACTOR, localY);
				localY -= 1;
				localY /= EXPONENTIAL_FACTOR - 1;
			}

			
		}

		joyie.stickAngle = atan((float) (-js.lY)/js.lX)* 180.0 / JOYSTICK_PI;
		//ANGLE CALCULATIONS
		//1st quadrant conversion
		if(js.lY <= 0 && js.lX > 0)
		{
			joyie.stickAngle = 90.0 - joyie.stickAngle;
		}
		//2nd quadrant
		else if (js.lY < 0 && js.lX < 0)
		{
			joyie.stickAngle = 360 - (joyie.stickAngle + 90);
		}
		//3rd quad
		else if (js.lY >= 0 && js.lX < 0)
		{
			joyie.stickAngle = 270 -joyie.stickAngle;
		}
		//4th quad
		else if (js.lY > 0 && js.lX >= 0)
		{
			joyie.stickAngle = (-joyie.stickAngle) + 90;
		}
		else
			joyie.stickAngle = 0.0;
	}
	else
	{
		localX = 0.0;
		localY = 0.0;
		joyie.stickAngle = 0.0;
	}
	


	// Button 2 = X = save picture
	if( js.rgbButtons[BTN_X] & 0x80 ) { tracker[BTN_X]++; }
	else { tracker[BTN_X] = 0; }

	if( tracker[BTN_X] == 1 )
	{
		Thread ^thread = gcnew Thread( gcnew ThreadStart(theDelegate, &Skynet::SkynetController::saveCurrentFrameAsCandidate));
		thread->Start();
		//theDelegate->saveCurrentFrameAsCandidate();
		PRINT("save image pressed: " + ++imageCounter);	
	}

	// localX and Y contain the stick coordinates between -1 and 1
	newGimbalRoll -= (unsigned __int16)(40.0f*GIMBAL_FACTOR*localX);
	newGimbalPitch += (unsigned __int16)(40.0f*GIMBAL_FACTOR*localY);
		
	// Button 1 = SQUARE = Gimbal Stabilized Mode
	if( js.rgbButtons[BTN_SQUARE] & 0x80 )
	{ tracker[BTN_SQUARE]++; }
	else { tracker[BTN_SQUARE] = 0; }

	if( tracker[BTN_SQUARE] == 1 )
	{
		PRINT("Entering stabilized look-down mode");
		comm->commandGimbalStabilizedMode();
		theDelegate->printConsoleMessageInGreen("Set gimbal look-down mode");

	}


	if( tracker[BTN_SQUARE] == 1 ) { }

	if( js.rgbButtons[BTN_CIRCLE] & 0x80 ) { tracker[BTN_CIRCLE]++; }
	else { tracker[BTN_CIRCLE] = 0; }

	if( tracker[BTN_CIRCLE] == 1 ) { }
	

	// L1 button to zoom in
	if( js.rgbButtons[BTN_L1] & 0x80 ) { tracker[BTN_L1]++; }
	else { tracker[BTN_L1] = 0; }
	
	// R1 button to zoom out
	if( js.rgbButtons[BTN_R1] & 0x80 ) { tracker[BTN_R1]++; }
	else { tracker[BTN_R1] = 0; }


	// handle zoom buttons
	if(tracker[BTN_L1] == 1 && !(tracker[BTN_R1] == 1))
	{
		if(newZoomLevel > MIN_ZOOM_LEVEL)
		{
			newZoomLevel--;
		}
		zoomChanged = true;

		// start timer
		zoomDirection = ZOOM_OUT;
		//zoomTimer->Start();
		
	}
	else if(tracker[BTN_R1] == 1 && !(tracker[BTN_L1] == 1))
	{
		//send zoom packet
		if(newZoomLevel < MAX_ZOOM_LEVEL)
		{
			newZoomLevel++;
			//packet->update_type = 0x0A;
		}
		zoomChanged = true;
		

		// start timer
		zoomDirection = ZOOM_IN;
		//zoomTimer->Start();
		//System::Diagnostics::Trace::WriteLine("R1 press registered");
	}

	else {
		// stop timer
		zoomDirection = ZOOM_NONE;
		//zoomTimer->Stop();
		//System::Diagnostics::Trace::WriteLine("no press");
	}


	// center gimbal by pushing stick
	if( js.rgbButtons[BTN_L3] & 0x80 )
	{ tracker[BTN_L3]++; }
	else
	{ tracker[BTN_L3] = 0; }
	if( tracker[BTN_L3] == 1 )
	{
		newGimbalRoll = 3000;
		newGimbalPitch = 3000;
	}

	/*// Print status of each button
    for( int i = 0; i < 128; i++ )
    {
        if( js.rgbButtons[i] & 0x80 )
        {
			System::Diagnostics::Trace::WriteLine("Button " + i + " pressed.");
        }
    }*/

	// send commands to Comms
	if (newGimbalRoll != theWatcher->gimbalRoll || newGimbalPitch != theWatcher->gimbalPitch) {

		((Skynet::SkynetController ^)theDelegate)->stopTargetLock();

		if (newGimbalRoll > MAX_ROLL)
			newGimbalRoll = MAX_ROLL;
		if (newGimbalRoll < MIN_ROLL)
			newGimbalRoll = MIN_ROLL;
		
		if (newGimbalPitch > MAX_PITCH)
			newGimbalPitch = MAX_PITCH;
		if (newGimbalPitch < MIN_PITCH)
			newGimbalPitch = MIN_PITCH;
		
		theWatcher->gimbalRoll = newGimbalRoll;
		theWatcher->gimbalPitch = newGimbalPitch;

		comm->sendGimbalRollPitch(newGimbalRoll, newGimbalPitch);

		if (INTENDED_POSITION_UPDATES_ENABLED)
			((Skynet::SkynetController ^)theDelegate)->intendedGimbalPositionUpdated(theWatcher->rawToDegrees(newGimbalRoll), theWatcher->rawToDegrees(newGimbalPitch));

	}

	if (zoomChanged) { // newZoomLevel != theWatcher->zoomLevel) {
		if (newZoomLevel > MAX_ZOOM_LEVEL)
			newZoomLevel = MAX_ZOOM_LEVEL;
		if (newZoomLevel < MIN_ZOOM_LEVEL)
			newZoomLevel = MIN_ZOOM_LEVEL;
		theWatcher->zoomLevel = newZoomLevel;

		unsigned __int32 zoom = 0;
		switch( newZoomLevel )
		{
			case 1:
				zoom = 0x00000000;
				break;
			case 2:
				zoom = 0x00080000;
				break;
			case 3:
				zoom = 0x01000000;
				break;
			case 4:
				zoom = 0x01080000;
				break;
			case 5:
				zoom = 0x02000000;
				break;
			case 6:
				zoom = 0x02080000;
				break;
			case 7:
				zoom = 0x03000000;
				break;
			case 8:
				zoom = 0x03080000;
				break;
			case 9:
				zoom = 0x04000000;
				break;
			default:
				zoom = 0xAAAAAAAA; // no zoom update
				break;
		}
		comm->sendZoom(zoom);
		if (INTENDED_POSITION_UPDATES_ENABLED)
			((Skynet::SkynetController ^)theDelegate)->intendedCameraZoomUpdated(theWatcher->rawZoomToFloat(zoom));
	}
	
	//PRINT("End: " + System::DateTime::Now.Second + "." + System::DateTime::Now.Millisecond );
    return S_OK;
}

void 
Joystick::CleanupForIsXInputDevice()
{
    // Cleanup linked list
    XINPUT_DEVICE_NODE* pNode = g_pXInputDeviceList;
    while( pNode )
    {
        XINPUT_DEVICE_NODE* pDelete = pNode;
        pNode = pNode->pNext;
        SAFE_DELETE( pDelete );
    }
}