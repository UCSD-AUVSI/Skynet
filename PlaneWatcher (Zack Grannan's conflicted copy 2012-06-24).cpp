#include "StdAfx.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include <msclr/lock.h>
#include "GeoReference.h"
#include "AutopilotComport.h"

using namespace Communications;
using namespace msclr;
using namespace System::Runtime::InteropServices;

#define MAX_INT_32 2147483647

PlaneWatcher::PlaneWatcher(Object ^ theParent, Autosearch ^ autoS)
{ 
	autosearch = autoS;
	parent = theParent; 

	gimbalRoll = 3000;
	gimbalPitch = 3000;
	zoomLevel = 1;

    gimbalInfo = gcnew array<GimbalInfo ^>(NUM_GIMBAL_DATA);  //create a new array with num entities
	autopilotGPSInfo = gcnew array<PlaneGPSPacket ^>(NUM_GPS_DATA);
	autopilotTelemInfo = gcnew array<PlaneTelemPacket ^>(NUM_TELEM_DATA);

	gimbalInfoIndex  = 0;  //point it to the front of the array
	autopilotGPSInfoIndex = 0;
	autopilotTelemInfoIndex = 0;

	for(int a = 0; a < NUM_GIMBAL_DATA; a++){
		gimbalInfo[a] = nullptr;
	}

	for(int a = 0; a < NUM_GPS_DATA; a++){
		autopilotGPSInfo[a] = nullptr;
	}

	for(int a = 0; a < NUM_TELEM_DATA; a++){
		autopilotTelemInfo[a] = nullptr;
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
	((Skynet::Form1 ^)parent)->reloadTable();
	autosearch->updateState();
}

PlaneState ^ PlaneWatcher::stateOfCurrentImage()
{
	return predictLocationAtTime( -((float)CAMERA_LATENCY)/1000.0f );
}

PlaneState ^ PlaneWatcher::predictLocationAtTime( float timeOffset )
{
	PlaneState ^ retval = gcnew PlaneState();

	try {
		System::DateTime targetTime = System::DateTime::Now.AddSeconds(timeOffset);

		__int32 target = targetTime.Millisecond + 1000*targetTime.Second + 60000*targetTime.Minute + 3600000*targetTime.Hour;  //time in __int32 which we want to compare to
	
		/*
		 * cases: 
		 * (1) empty list: set defaults
		 * (2) list of size 1: do 1 point prediction
		 * (3) before the most recent GPS location
		 * (4) within the list of points
		 * (5) after the oldest GPS location
		 *
		 * remember to discount any array pointer values of 0, as these have not been instantiated 
		 */
	
	//	lock l(this); 

		//we know that we can get prediction data for both GPS and telem
		//do GPS prediction
	
		//////////////////// GPS info: lat, lon, alt /////////////////////////

		//for now, just returning the two closest values
		__int32 smallestDistance = 3600000;  //arbitrarily large
		int smallestIndex = -1;
		int numValidGPSPackets = 0;
		PlaneGPSPacket ^ olderGPSPacket;
		PlaneGPSPacket ^ bestGPSPacket;
		PlaneGPSPacket ^ newerGPSPacket;
		for(int i = 0; i < NUM_GPS_DATA; i++) {
	  
			lock l(autopilotGPSInfo); //lock it so as to avoid it changing while we access it

			// get packet at i-1,i,i+1
			PlaneGPSPacket ^ tempOlderGPSPacket = autopilotGPSInfo[(i > 0 ? i - 1 : NUM_GPS_DATA - 1)];
			PlaneGPSPacket ^ tempBestGPSPacket = autopilotGPSInfo[i];
			PlaneGPSPacket ^ tempNewerGPSPacket = autopilotGPSInfo[(i > NUM_GPS_DATA - 1 ? i + 1 : 0)];

			l.release();

			if(autopilotGPSInfo[i] != nullptr){
				numValidGPSPackets ++;
				__int32 dist = abs(target - getTimeUTC(autopilotGPSInfo[i]));
				//System::Diagnostics::Trace::WriteLine("PlaneWatcher::getClosestAutopilotState() dist:" + dist + " target:" + target);
				if(dist < smallestDistance){
					smallestDistance = dist;
					smallestIndex = i;
					olderGPSPacket = tempOlderGPSPacket;
					bestGPSPacket = tempBestGPSPacket;
					newerGPSPacket = tempNewerGPSPacket;
				}
			}
		}

		// if no packet found
		if (smallestIndex == -1) 
		{
			retval->gpsData->gpsVelocity = 20.0f;
			retval->gpsData->gpsAltitude = 37;
			retval->gpsData->gpsHeading = 0.0f;
			retval->gpsData->gpsLatitude = 0.0f;
			retval->gpsData->gpsLongitude = 0.0f;
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): No packets");
		}

		// if packet found, but only one
		else //if(numValidGPSPackets == 1)
		{
			retval->gpsData->gpsVelocity = bestGPSPacket->gpsVelocity;
			retval->gpsData->gpsAltitude = bestGPSPacket->gpsAltitude;
			retval->gpsData->gpsHeading = bestGPSPacket->gpsHeading;
			retval->gpsData->gpsLatitude = bestGPSPacket->gpsLatitude;
			retval->gpsData->gpsLongitude = bestGPSPacket->gpsLongitude;
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): One packet");
		}

		//if we have at least two packets to chose from
		/*else
		{ 
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): Lots of packets");

			//if we have another packet, check to see which one is closer, the one before this or the one after

			__int32 distAfter = (newerGPSPacket != nullptr ? abs(target - getTimeUTC(newerGPSPacket)) : MAX_INT_32);
			__int32 distBefore = (olderGPSPacket != nullptr ? abs(target - getTimeUTC(olderGPSPacket)) : MAX_INT_32);

			if(distAfter > distBefore)  //use the olderPacket
			{  
				newerGPSPacket = bestGPSPacket;
			}
			else  //use the newerPacket
			{
				olderGPSPacket = bestGPSPacket;
			}

			//now use the two packets and do a linear math thingy

			__int32 olderTime =  getTimeUTC(olderGPSPacket);
			__int32 newerTime =  getTimeUTC(newerGPSPacket);

			retval->gpsData->gpsVelocity  = linearInterpolation(olderGPSPacket->gpsVelocity, newerGPSPacket->gpsVelocity, olderTime, newerTime, target);
			retval->gpsData->gpsAltitude  = linearInterpolation(olderGPSPacket->gpsAltitude, newerGPSPacket->gpsAltitude, olderTime, newerTime, target);
			retval->gpsData->gpsHeading   = linearInterpolation(olderGPSPacket->gpsHeading, newerGPSPacket->gpsHeading, olderTime, newerTime, target);
			retval->gpsData->gpsLatitude  = linearInterpolation(olderGPSPacket->gpsLatitude, newerGPSPacket->gpsLatitude, olderTime, newerTime, target);
			retval->gpsData->gpsLongitude = linearInterpolation(olderGPSPacket->gpsLongitude, newerGPSPacket->gpsLongitude, olderTime, newerTime, target);
		}*/
	
		//////////////////// Telemetry: roll, pitch, heading, etc /////////////////////////
		smallestDistance = 3600000;  //arbitrarily large
		smallestIndex = -1;
		int numValidTelemPackets = 0;
		PlaneTelemPacket ^ olderTelemPacket;
		PlaneTelemPacket ^ bestTelemPacket;
		PlaneTelemPacket ^ newerTelemPacket;
		for(int i = 0; i < NUM_TELEM_DATA; i++){

			lock l(autopilotTelemInfo); //lock it so as to avoid it changing while we access it

			PlaneTelemPacket ^ tempOlderTelemPacket = autopilotTelemInfo[(i > 0 ? i - 1 : NUM_TELEM_DATA - 1)];
			PlaneTelemPacket ^ tempBestTelemPacket = autopilotTelemInfo[i];
			PlaneTelemPacket ^ tempNewerTelemPacket = autopilotTelemInfo[(i > NUM_TELEM_DATA - 1 ? i + 1 : 0)];

			l.release();  //release it back into the wild

			if(autopilotTelemInfo[i] != nullptr){
				numValidTelemPackets ++;
				__int32 dist = abs(target - getTimeUTC(autopilotTelemInfo[i]));
				if(dist < smallestDistance){
					smallestDistance = dist;
					smallestIndex = i;

					olderTelemPacket = tempOlderTelemPacket;
					bestTelemPacket  = tempBestTelemPacket;
					newerTelemPacket = tempNewerTelemPacket;
				}
			}
		}

		// if no packet found
		if (smallestIndex == -1) 
		{
			retval->telemData->roll = 0;
			retval->telemData->pitch = 0;
			retval->telemData->heading = 0;
			retval->telemData->altitudeHAL = 0;
		}

		// if packet found, but only one
		else // if(numValidTelemPackets == 1)
		{
			retval->telemData->roll        = bestTelemPacket->roll;
			retval->telemData->pitch       = bestTelemPacket->pitch;
			retval->telemData->heading     = bestTelemPacket->heading;
			retval->telemData->altitudeHAL = bestTelemPacket->altitudeHAL;
		}
		/*
		//if we have at least two packets to chose from
		else
		{ 
			//if we have another packet, check to see which one is closer, the one before this or the one after

			__int32 distAfter =  (newerTelemPacket != nullptr ? abs(target - getTimeUTC(newerTelemPacket)) : MAX_INT_32);
			__int32 distBefore = (olderTelemPacket != nullptr ? abs(target - getTimeUTC(olderTelemPacket)) : MAX_INT_32);

			if(distAfter > distBefore)  //use the olderPacket
			{  
				newerTelemPacket = bestTelemPacket;
			}
			else  //use the newerPacket
			{
				olderTelemPacket = bestTelemPacket;
			}

			//now use the two packets and do a linear math thingy

			__int32 olderTime =  getTimeUTC(olderTelemPacket);
			__int32 newerTime =  getTimeUTC(newerTelemPacket);

			retval->telemData->roll        = linearInterpolation(olderTelemPacket->roll, newerTelemPacket->roll, olderTime, newerTime, target);
			retval->telemData->pitch       = linearInterpolation(olderTelemPacket->pitch, newerTelemPacket->pitch, olderTime, newerTime, target);
			retval->telemData->heading     = linearInterpolation(olderTelemPacket->heading, newerTelemPacket->heading, olderTime, newerTime, target);
			retval->telemData->altitudeHAL = linearInterpolation(olderTelemPacket->altitudeHAL, newerTelemPacket->altitudeHAL, olderTime, newerTime, target);
		}*/

	
		//////////////////// Gimbal Info /////////////////////////
		smallestDistance = 3600000;  //arbitrarily large
		smallestIndex = -1;
		int numValidGimbalPackets = 0;
		GimbalInfo ^ olderGimbalPacket;
		GimbalInfo ^ bestGimbalPacket;
		GimbalInfo ^ newerGimbalPacket;
		for(int i = 0; i < NUM_TELEM_DATA; i++){

			lock l(gimbalInfo); //lock it so as to avoid it changing while we access it

			GimbalInfo ^ tempOlderGimbalPacket = gimbalInfo[(i > 0 ? i - 1 : NUM_GPS_DATA - 1)];
			GimbalInfo ^ tempBestGimbalPacket  = gimbalInfo[i];
			GimbalInfo ^ tempNewerGimbalPacket = gimbalInfo[(i > NUM_GPS_DATA - 1 ? i + 1 : 0)];

			l.release(); //let it go free! :)

			if(gimbalInfo[i] != nullptr){
				numValidGimbalPackets ++;
				__int32 dist = abs(target - getTimeUTC(gimbalInfo[i]));
				if(dist < smallestDistance){
					smallestDistance = dist;
					smallestIndex = i;
					olderGimbalPacket = tempOlderGimbalPacket;
					bestGimbalPacket  = tempBestGimbalPacket;
					newerGimbalPacket = tempNewerGimbalPacket;
				}
			}
		}

		// if no packet found
		if (smallestIndex == -1) 
		{
			retval->gimbalInfo->zoom = 1;
			retval->gimbalInfo->pitch = 3000;
			retval->gimbalInfo->roll = 3000;
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): No rabbit packets");
		}
		// if packet found, but only one
		else
		{

			retval->gimbalInfo->roll    = bestGimbalPacket->roll;
			retval->gimbalInfo->pitch   = bestGimbalPacket->pitch;
			retval->gimbalInfo->zoom	= bestGimbalPacket->zoom;
			//System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): Rabbit one packet");

		}
		/*
		//if we have at least two packets to chose from
		else
		{ 
			//if we have another packet, check to see which one is closer, the one before this or the one after

			__int32 distAfter =  (newerGimbalPacket != nullptr ? abs(target - getTimeUTC(newerGimbalPacket)) : MAX_INT_32);
			__int32 distBefore = (olderGimbalPacket != nullptr ? abs(target - getTimeUTC(olderGimbalPacket)) : MAX_INT_32);

			if(distAfter > distBefore)  //use the olderPacket
			{  
				newerGimbalPacket = bestGimbalPacket;
			}
			else  //use the newerPacket (change the older one)
			{
				olderGimbalPacket = bestGimbalPacket;
			}

			//now use the two packets and do a linear math thingy

			__int32 olderTime =  getTimeUTC(olderGimbalPacket);
			__int32 newerTime =  getTimeUTC(newerGimbalPacket);

			retval->gimbalInfo->roll    = linearInterpolation(olderGimbalPacket->roll, newerGimbalPacket->roll, olderTime, newerTime, target); // TODO: nullreferenceexception HERE
			retval->gimbalInfo->pitch   = linearInterpolation(olderGimbalPacket->pitch, newerGimbalPacket->pitch, olderTime, newerTime, target);
			System::Diagnostics::Trace::WriteLine("PlaneWatcher::predictLocationAtTime(): Rabbit lots of packets");
		}*/
	}
	catch(Exception ^ e) 
	{
		
		retval->telemData->roll = 0;
		retval->telemData->pitch = 0;
		retval->telemData->heading = 0;
		retval->telemData->altitudeHAL = 0;

		retval->gpsData->gpsVelocity = 20.0f;
		retval->gpsData->gpsAltitude = 37;
		retval->gpsData->gpsHeading = 0.0f;
		retval->gpsData->gpsLatitude = 0.0f;
		retval->gpsData->gpsLongitude = 0.0f;
			
		retval->gimbalInfo->zoom = 1;
		retval->gimbalInfo->pitch = 3000;
		retval->gimbalInfo->roll = 3000;

		e = nullptr;
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

unsigned __int16 PlaneWatcher::gimbalDegreesToRaw(float input)
{
	return (unsigned __int16)(input*20.0f + 3000.0f);
}

float PlaneWatcher::rawToDegrees(unsigned __int16 input)
{
	return (((float)input) - 3000.0f)/20.0f;
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
	gimbalInfoIndex ++;  //increment
	if(gimbalInfoIndex >= NUM_GIMBAL_DATA){
		gimbalInfoIndex = 0;  //reset to zero
	}
}

void PlaneWatcher::incrementGPSInfoIndex()
{
	autopilotGPSInfoIndex ++;  //increment
	if(autopilotGPSInfoIndex >= NUM_GPS_DATA){
		autopilotGPSInfoIndex = 0;  //reset to zero
	}
}

void PlaneWatcher::incrementTelemInfoIndex()
{
	autopilotTelemInfoIndex ++;  //increment
	if(autopilotTelemInfoIndex >= NUM_TELEM_DATA){
		autopilotTelemInfoIndex = 0;  //reset to zero
	}
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

float PlaneWatcher::linearInterpolation(float A, float B, __int32 timeA, __int32 timeB, __int32 timeActual)
{
	// DOESNT ACTUALLY DO INTERPOLATION
	if (timeA < timeB)
		return A;
	else 
		return B;

	return A + (B - A) * (timeActual - timeA) / (timeB - timeA);
}