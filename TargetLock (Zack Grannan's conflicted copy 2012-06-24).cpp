
#include "StdAfx.h"
#include "TargetLock.h"
#include <msclr/lock.h>
#include "Comms.h"
#include "PlaneWatcher.h"


using namespace std;
using namespace Communications;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace Threading;
using namespace msclr;

#define TARGET_LOCK_UPDATE_FREQUENCY	4 // number of updates per second (we could also send an update every time telem is updated)
#define ESTIMATE_TIME_TO_SEND_PACKET	50 //(ms)

TargetLock::TargetLock(Comms ^ theComms, PlaneWatcher ^ theWatcher)
{
	comms = theComms;
	planeWatcher = theWatcher;
	lat = 0.0f;
	lon = 0.0f;
	locked = false;

	targetLockThread = gcnew Thread(gcnew ThreadStart(this, &TargetLock::targetLockThreadFunction));
	targetLockThread->Name = "TargetLock Thread";
	targetLockThread->Start();
}

TargetLock::~TargetLock(void)
{
	if (targetLockThread != nullptr)
		targetLockThread->Abort();
	
	targetLockThread = nullptr;
}

void TargetLock::lockOntoGPS( float newLat, float newLon )
{
	if (lat > 180 || lon > 180)
		return;

	lat = newLat;
	lon = newLon;
	locked = true;

	comms->sendGimbalLatLon(lat, lon);


}

void TargetLock::endLock()
{
	locked = false;
}


void TargetLock::targetLockThreadFunction()
{
	//unsigned __int16 roll, pitch;

	try {
		while (true) 
		{
			if (!locked) {
				// sleep for short time
				Thread::Sleep(100);
			}

			else {

				// calculate new roll and pitch
				//planeWatcher->requiredRollPitchForGPS( roll, pitch, lat, lon);
				//System::Diagnostics::Trace::WriteLine("maintaining target lock with roll " + roll + " and pitch " + pitch + "lat " + lat + " lon " + lon);

				if (!locked)
					continue;

				// send command
				comms->sendGimbalLatLon(lat, lon);

				// sleep for long time
				Thread::Sleep(1000/TARGET_LOCK_UPDATE_FREQUENCY - ESTIMATE_TIME_TO_SEND_PACKET);

			}
		}
	}
	catch (ThreadAbortException ^) {}
	catch (Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR in TargetLock: " + e);
	}
}
