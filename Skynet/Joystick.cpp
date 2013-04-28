#include "StdAfx.h"
#include "MasterHeader.h"

#include "SkynetController.h"

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

#include "Joystick.h"

using namespace System::Threading;



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


using namespace System;
using namespace System::Timers;

Joystick::Joystick( SkynetController^ controller )
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

	manualMode = false;
	manualModeCounter = 0;
	imageCounter = 0;
}

Joystick::~Joystick(void)
{
	FreeDirectInput();
}


HRESULT
Joystick::init(HWND hDlg)
{
	HRESULT hr;

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

	if (this == nullptr)
		return S_OK;

	if (theWatcher == nullptr)
		return S_OK;


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
	

	// Button 2 = X = save picture
	if( js.rgbButtons[BTN_X] & 0x80 ) { tracker[BTN_X]++; }
	else { tracker[BTN_X] = 0; }

	if( tracker[BTN_X] == 1 )
	{
		Thread ^thread = gcnew Thread( gcnew ThreadStart(theDelegate, &Skynet::SkynetController::saveCurrentFrameAsUnverified));
		thread->Name = "SkynetController saveFrameAUnverified thread";
		thread->Start();
		PRINT("save image pressed: " + ++imageCounter);	
	}

	if( js.rgbButtons[BTN_CIRCLE] & 0x80 ) { tracker[BTN_CIRCLE]++; }
	else { tracker[BTN_CIRCLE] = 0; }

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