#include "StdAfx.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include <msclr/lock.h>
#include "GeoReference.h"
#include "AutopilotComport.h"
#include "..\ImageDownloader\ImageDownloader\ImageReceiver.h"

using namespace Communications;
using namespace msclr;
using namespace System::Runtime::InteropServices;

#define MAX_INT_32 2147483647

PlaneWatcher::PlaneWatcher(Object ^ theParent)
{ 
	parent = theParent; 
}
		

void PlaneWatcher::updateInfo (ImageWithPlaneData ^ data) {
	if ( data != nullptr && parent != nullptr){
		state = data;
		((Skynet::Form1 ^)parent)->reloadTable(data);
	}
}

void PlaneWatcher::updateGimbalInfo( GimbalInfo ^ data)
{
	

	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::updateGimbalInfo()");

	incrementGimbalInfoIndex();

	lock l(gimbalInfo);
	gimbalInfo[gimbalInfoIndex] = data;
	l.release();

	planeStateUpdated();
}

void PlaneWatcher::updatePlaneGPSInfo( PlaneGPSPacket ^ data)
{
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::updatePlaneGPSInfo()");
	theController->gotGPS();
	incrementGPSInfoIndex();

	lock l(autopilotGPSInfo);
	autopilotGPSInfo[autopilotGPSInfoIndex] = data;
	l.release();

	planeStateUpdated();
}

void PlaneWatcher::updatePlaneTelemInfo( PlaneTelemPacket ^ data)
{

	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::updatePlaneTelemInfo()");

	incrementTelemInfoIndex();

	lock l(autopilotTelemInfo);
	autopilotTelemInfo[autopilotTelemInfoIndex] = data;
	l.release();

	planeStateUpdated();
	
}

void PlaneWatcher::planeStateUpdated()
{
//	((Skynet::Form1 ^)parent)->reloadTable();
}

PlaneState ^ PlaneWatcher::stateOfCurrentImage()
{
	return predictLocationAtTime( -((float)CAMERA_LATENCY)/1000.0f );
}

PlaneState ^ PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	PlaneState ^ retval = gcnew PlaneState();

	try {
		System::DateTime targetDateTime = System::DateTime::Now.AddSeconds(timeOffset);

		__int32 targetTime = targetDateTime.Millisecond + 1000*targetDateTime.Second + 60000*targetDateTime.Minute + 3600000*targetDateTime.Hour; 
	
		// get closest packets

		/////// GPS ///////
		__int32 bestOlderTime = -10000000;
		__int32 bestNewerTime = 10000000;
		PlaneGPSPacket ^ olderGPSPacket = nullptr;
		PlaneGPSPacket ^ newerGPSPacket = nullptr;

		// loop through packets
		for(int i = 0; i < NUM_GPS_DATA; i++) {

			lock l(autopilotGPSInfo);
			PlaneGPSPacket ^ tempPacket = autopilotGPSInfo[i];
			l.release();

			if (tempPacket != nullptr)
			{
				// check if this packet is best newer or older
				__int32 tempTime = getTimeUTC(tempPacket);

				if (tempTime < targetTime && tempTime > bestOlderTime)
				{
					bestOlderTime = tempTime;
					olderGPSPacket = tempPacket;
				}
				else if (tempTime >= targetTime && tempTime < bestNewerTime)
				{
					bestNewerTime = tempTime;
					newerGPSPacket = tempPacket;
				}
			}
		}

		/////// TELEM ///////
		bestOlderTime = -10000000;
		bestNewerTime = 10000000;
		PlaneTelemPacket ^ olderTelemPacket = nullptr;
		PlaneTelemPacket ^ newerTelemPacket = nullptr;

		// loop through packets
		for(int i = 0; i < NUM_TELEM_DATA; i++) {

			lock l(autopilotTelemInfo);
			PlaneTelemPacket ^ tempPacket = autopilotTelemInfo[i];
			l.release();

			if (tempPacket != nullptr)
			{
				// check if this packet is best newer or older
				__int32 tempTime = getTimeUTC(tempPacket);

				if (tempTime < targetTime && tempTime > bestOlderTime)
				{
					bestOlderTime = tempTime;
					olderTelemPacket = tempPacket;
				}
				else if (tempTime >= targetTime && tempTime < bestNewerTime)
				{
					bestNewerTime = tempTime;
					newerTelemPacket = tempPacket;
				}
			}
		}

		/////// GIMBAL ///////
		bestOlderTime = -10000000;
		bestNewerTime = 10000000;
		GimbalInfo ^ olderGimbalPacket = nullptr;
		GimbalInfo ^ newerGimbalPacket = nullptr;

		// loop through packets
		for(int i = 0; i < NUM_GIMBAL_DATA; i++) {

			lock l(gimbalInfo);
			GimbalInfo ^ tempPacket = gimbalInfo[i];
			l.release();

			if (tempPacket != nullptr)
			{
				// check if this packet is best newer or older
				__int32 tempTime = getTimeUTC(tempPacket);

				if (tempTime < targetTime && tempTime > bestOlderTime)
				{
					bestOlderTime = tempTime;
					olderGimbalPacket = tempPacket;
				}
				else if (tempTime >= targetTime && tempTime < bestNewerTime)
				{
					bestNewerTime = tempTime;
					newerGimbalPacket = tempPacket;
				}
			}
		}

		// build final packet using interpolation + default values

		/////// GPS ///////
		// if no packet found
		if (newerGPSPacket == nullptr && olderGPSPacket == nullptr) 
		{
			retval->gpsData->gpsVelocity = 20.0f;
			retval->gpsData->gpsAltitude = 37;
			retval->gpsData->gpsHeading = 0.0f;
			retval->gpsData->gpsLatitude = 0.0f;
			retval->gpsData->gpsLongitude = 0.0f;
		}

		// if only one packet found
		else if (newerGPSPacket != nullptr || olderGPSPacket != nullptr)
		{
			PlaneGPSPacket ^ bestGPSPacket;
			if (newerGPSPacket != nullptr)
				bestGPSPacket = newerGPSPacket;
			else
				bestGPSPacket = olderGPSPacket;

			retval->gpsData->gpsVelocity = bestGPSPacket->gpsVelocity;
			retval->gpsData->gpsAltitude = bestGPSPacket->gpsAltitude;
			retval->gpsData->gpsHeading = bestGPSPacket->gpsHeading;
			retval->gpsData->gpsLatitude = bestGPSPacket->gpsLatitude;
			retval->gpsData->gpsLongitude = bestGPSPacket->gpsLongitude;
		}
		else
		{
			double dist = (double)(targetTime - bestOlderTime)/(bestNewerTime - bestOlderTime);

			retval->gpsData->gpsVelocity  = (float)linearInterpolation(olderGPSPacket->gpsVelocity, newerGPSPacket->gpsVelocity, dist);
			retval->gpsData->gpsAltitude  = (float)linearInterpolation(olderGPSPacket->gpsAltitude, newerGPSPacket->gpsAltitude, dist);
			retval->gpsData->gpsHeading   = (float)linearInterpolation(olderGPSPacket->gpsHeading, newerGPSPacket->gpsHeading, dist);
			retval->gpsData->gpsLatitude  = (float)linearInterpolation(olderGPSPacket->gpsLatitude, newerGPSPacket->gpsLatitude, dist);
			retval->gpsData->gpsLongitude = (float)linearInterpolation(olderGPSPacket->gpsLongitude, newerGPSPacket->gpsLongitude, dist);
		}

		

		/////// TELEM ///////
		float blur = 4.0f; // approximates the most drastic movement possible

		// if no packet found
		if (newerTelemPacket == nullptr && olderTelemPacket == nullptr) 
		{
			retval->telemData->roll = 0;
			retval->telemData->pitch = 0;
			retval->telemData->heading = 0;
			retval->telemData->altitudeHAL = 0;
			retval->blurFactor = blur;
		}

		// use closest packet (no interpolation on telem)
		else 
		{
			PlaneTelemPacket ^ bestTelemPacket;
			if (olderTelemPacket == nullptr || abs(getTimeUTC(olderTelemPacket) - targetTime) > abs(getTimeUTC(newerTelemPacket) - targetTime))
				bestTelemPacket = newerTelemPacket;
			else
				bestTelemPacket = olderTelemPacket;

			retval->telemData->roll        = bestTelemPacket->roll;
			retval->telemData->pitch       = bestTelemPacket->pitch;
			retval->telemData->heading     = bestTelemPacket->heading;
			retval->telemData->altitudeHAL = bestTelemPacket->altitudeHAL;

			if (newerTelemPacket != nullptr && olderTelemPacket != nullptr) 
			{
				// if found two packets, calculate blur factor
				blur = 0.0f;
				blur += (float)(newerTelemPacket->roll - olderTelemPacket->roll);
				blur += (float)(newerTelemPacket->pitch - olderTelemPacket->pitch);
				blur += (float)(newerTelemPacket->heading - olderTelemPacket->heading);
			}
			
			retval->blurFactor = blur;
		}


		

		/////// GIMBAL ///////
		// if no packet found
		if (newerGimbalPacket == nullptr && olderGimbalPacket == nullptr) 
		{
			retval->gimbalInfo->zoom = 1;
			retval->gimbalInfo->pitch = gimbalDegreesToRaw((float)(-retval->telemData->pitch*RAD_TO_DEG)); // 3000
			retval->gimbalInfo->roll = gimbalDegreesToRaw((float)(-retval->telemData->roll*RAD_TO_DEG)); // 3000		
		}

		// use closest packet (no interpolation on gimbal)
		else
		{
			GimbalInfo ^ bestGimbalPacket;
			if (olderGimbalPacket == nullptr || abs(getTimeUTC(olderGimbalPacket) - targetTime) > abs(getTimeUTC(newerGimbalPacket) - targetTime))
				bestGimbalPacket = newerGimbalPacket;
			else
				bestGimbalPacket = olderGimbalPacket;

			retval->gimbalInfo->roll    = bestGimbalPacket->roll;
			retval->gimbalInfo->pitch = gimbalDegreesToRaw((float)(-retval->telemData->pitch*RAD_TO_DEG)); // 3000
			retval->gimbalInfo->roll = gimbalDegreesToRaw((float)(-retval->telemData->roll*RAD_TO_DEG)); // 3000		
		}

	}
	catch(Exception ^ e) 
	{

		PRINT("PlaneWatcher::predictLocationAtTime() Exception: " + e);

		retval->gpsData->gpsVelocity = 20.0f;
		retval->gpsData->gpsAltitude = 37;
		retval->gpsData->gpsHeading = 0.0f;
		retval->gpsData->gpsLatitude = 0.0f;
		retval->gpsData->gpsLongitude = 0.0f;
		
		retval->telemData->roll = 0;
		retval->telemData->pitch = 0;
		retval->telemData->heading = 0;
		retval->telemData->altitudeHAL = 0;
			
		retval->gimbalInfo->zoom = 1;
		retval->gimbalInfo->pitch = 3000;
		retval->gimbalInfo->roll = 3000;

		retval->isDefault = true;
	}

	return retval;
}

void PlaneWatcher::requiredRollPitchForGPS( unsigned __int16 & roll, unsigned __int16 & pitch, float lat, float lon)
{
	// get current state
	PlaneState ^ state = currentState();

	// calculate reverse georeference
	double rollFloat, pitchFloat;
	Vision::GeoReference::reverseGeoreference(state->gpsData->gpsLatitude, state->gpsData->gpsLongitude, state->telemData->altitudeHAL, state->telemData->roll*180.0/PI, 
				state->telemData->pitch*180.0/PI, state->telemData->heading*180.0/PI, 
				lat, lon, 0.0, rollFloat, pitchFloat );
	System::Diagnostics::Trace::WriteLine("PlaneWatcher::requiredRollPitchForGPS(): rollFloat: " + rollFloat + " pitchFloat + " + pitchFloat);

	// convert rollFloat and pitchFloat into roll and pitch
	roll = gimbalDegreesToRaw((float)rollFloat);
	pitch = gimbalDegreesToRaw((float)pitchFloat);
}

unsigned __int16 PlaneWatcher::gimbalDegreesToRaw(float degrees)
{
	return (unsigned __int16)(degrees*20.0f + 3000.0f);
}

float PlaneWatcher::rawToDegrees(unsigned __int16 raw)
{
	return (((float)raw) - 3000.0f)/20.0f;
}

float PlaneWatcher::rawZoomToFloat(int theZoom)
{
	float retval = -1.0f;
	switch( theZoom )
	{
		case 0x00000000:
			retval = 1.0f;
			break;
		case 0x00080000:
			retval = 2.0f;
			break;
		case 0x01000000:
			retval = 3.0f;
			break;
		case 0x01080000:
			retval = 4.0f;
			break;
		case 0x02000000:
			retval = 5.0f;
			break;
		case 0x02080000:
			retval = 6.0f;
			break;
		case 0x03000000:
			retval = 7.0f;
			break;
		case 0x03080000:
			retval = 8.0f;
			break;
		case 0x04000000:
			retval = 9.0f;
			break;
		default:
			retval = -1.0f; // no zoom update
			break;
	}

	return retval;
}

float PlaneWatcher::gimbalRollInDegrees()
{
	// TODO: store time of GimbalInfo packets, and access it when needed. include in PlaneState.
	
	if (gimbalInfo[gimbalInfoIndex] == nullptr)
		return 0;

	return (((float)gimbalInfo[gimbalInfoIndex]->roll) - 3000.0f)/20.0f;
}


float PlaneWatcher::gimbalPitchInDegrees()
{
	// TODO: store time of GimbalInfo packets, and access it when needed. include in PlaneState.
	
	if (gimbalInfo[gimbalInfoIndex] == nullptr)
		return 0;

	return (((float)gimbalInfo[gimbalInfoIndex]->pitch) - 3000.0f)/20.0f;

}

void PlaneWatcher::incrementGimbalInfoIndex()
{
	gimbalInfoIndex = (gimbalInfoIndex+1)%NUM_GIMBAL_DATA;  //increment
}

void PlaneWatcher::incrementGPSInfoIndex()
{
	autopilotGPSInfoIndex = (autopilotGPSInfoIndex+1)%NUM_GPS_DATA;  //increment
}

void PlaneWatcher::incrementTelemInfoIndex()
{
	autopilotTelemInfoIndex = (autopilotTelemInfoIndex+1)%NUM_TELEM_DATA;  //increment
}

__int32 PlaneWatcher::getTimeUTC(PlaneGPSPacket ^ state)
{
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getTimeUTC() time:" + time);
			

	return time;
}

__int32 PlaneWatcher::getTimeUTC(PlaneTelemPacket ^ state)
{
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getTimeUTC() time:" + time + " mill:" + state->UTCmillisecond + " min:" + state->UTCmin + " hr:" + state->UTChour);

	return time;
}

__int32 PlaneWatcher::getTimeUTC(GimbalInfo ^state){
	if (state == nullptr)
		return 0;

	__int32 time = 0;
	time += (__int32)state->UTCmillisecond;
	time += (__int32)(state->UTCmin) * 60000;
	time += (__int32)(state->UTChour) * 3600000;
	
	return time;
}

double PlaneWatcher::linearInterpolation(double A, double B, double distFromAToB)
{
	// DOESNT do extrapolation
	if (distFromAToB < 0.0)
		return A;
	else if (distFromAToB > 1.0)
		return B;

	return A*(1.0-distFromAToB) + B*distFromAToB;
}