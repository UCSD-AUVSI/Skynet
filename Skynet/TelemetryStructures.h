#pragma once

using namespace System;

namespace Communications {

	public ref struct PlaneGPSPacket
	{
		float gpsVelocity;					//longitude of UAV in decimal degrees
		double gpsAltitude;					//the gps computed altitude, in meters
		double gpsHeading;					//the gps computed heading, in radians (0 -> 2 pi)
		double gpsLatitude;					//latitude of UAV is decimal degrees
		double gpsLongitude;					//longitude of UAV in decimal degrees
		float gpsHomePositionLatitude;      //latitude of the home position in degrees
		float gpsHomePositionLongitude;		//longitude of the home position in degrees
		float gpsHomePositionAltitude;		//altitdude of the home position, MSL, in meters

		unsigned char currentCommand;       //current command of flight script (0 = first waypoint, 2 = third waypoint, etc)

		float desiredLatitude;              //the desired latitude of the UAV in degrees
		float desiredLongitude;				//the desired longitude of the UAV in degrees
		float timeToTarget;					//the estimalted time to get to the next targ
		float distanceToTarget;				//distance from UAV to desired target, meters
		float headingToTarget;              //heading to desired UAV target, radians (0 to 2 pi)

		unsigned char UTCyear;				//years since 1900
		unsigned char UTCmonth;				//UTC month (1 - 12)
		unsigned char UTCday;				//UTC day of month (1 - 31)
		unsigned char UTChour;				//UTC hour of dat (0 - 23)
		unsigned char UTCmin;				//UTC minute of hour (0 - 59)
		unsigned __int16 UTCmillisecond;	//UTC millisecond of minute (0 - 59,999)
		
		String ^ stringVal()
		{
			String ^ retVal = "";
			retVal += "\tgps_lat:" + gpsLatitude + ", \n";
			retVal += "\tgps_lon:" + gpsLongitude + ", \n";
			//retVal += "\talt:" + gpsAltitude + ", ";
			return retVal;
		}
	};

	public ref struct PlaneTelemPacket
	{
		double altitudeHAL;            //meters above launch
		float velocity;               //meters per second
		double roll;                   //current roll angle, radians (-pi to pi)
		double pitch;				  //current pitch angle, radians (-pi to pi)
		double heading;				  //current heading angle, radians (0 -> 2 pi), true psi
		float turnRate;				  //radians per second
		float batteryVoltage ;        //how many volts are being input to Kestrel
		unsigned char gpsNumSats;     //how many satellites are being tracked
		float desiredAltitude;        //the desired altitude of the UAV in meters.  Unsure whether this is MSL or HAL
		float desiredVelocity;		  //the desired velocity of the UAV in meters per second
		float desiredRoll;            //desired roll, radians
		float desiredPitch;			  //desired pitch, radians
		float desiredHeading;		  //desired heading, radians (0 - 2 pi)
		float desiredTurnRate;		  //desired turn rate, radians per second
		float desiredClimbRate;       //desired climb rate, meters per second
		float airbornTimer;			  //amount of time the autopilot has been airborn, in seconds
		float rollRate;               //rotational rate around x, radians per second
		float pitchRate;			  //rotational rate around y, radians per second
		float yawRate;				  //rotational rate around z, radians per second
		float altitudeMSL;            //mean sea level
		float climbRate;              //meters per second
		unsigned char UTCyear;				//years since 1900
		unsigned char UTCmonth;				//UTC month (1 - 12)
		unsigned char UTCday;				//UTC day of month (1 - 31)
		unsigned char UTChour;				//UTC hour of dat (0 - 23)
		unsigned char UTCmin;				//UTC minute of hour (0 - 59)
		unsigned __int16 UTCmillisecond;	//UTC millisecond of minute (0 - 59,999)

		
		String ^ stringVal()
		{
			String ^ retVal = "";
			retVal += "\tplane_roll:" + roll + ", \n";
			retVal += "\tplane_pitch:" + pitch + ", \n";
			retVal += "\tplane_heading:" + heading + ", \n";
			retVal += "\taltitude_hal:" + altitudeHAL + ", \n";
			return retVal;
		}
	};

	
	public ref struct GimbalInfo
	{
		GimbalInfo(){ roll = 0; pitch = 0; zoom = 0; }

		unsigned __int16 roll;
		unsigned __int16 pitch;
		__int8			 zoom;
		
		
		String ^ stringVal()
		{
			String ^ retVal = "";
			retVal += "\tgimbal_roll:" + (roll - 3000.0f)/20.0f + ", \n";
			retVal += "\tgimbal_pitch:" + (pitch - 3000.0f)/20.0f + ", \n";
			retVal += "\tgimbal_zoom:" + zoom + ", ";
			return retVal;
		}

		unsigned char UTCyear;				//years since 1900
		unsigned char UTCmonth;				//UTC month (1 - 12)
		unsigned char UTCday;				//UTC day of month (1 - 31)
		unsigned char UTChour;				//UTC hour of dat (0 - 23)
		unsigned char UTCmin;				//UTC minute of hour (0 - 59)
		unsigned __int16 UTCmillisecond;	//UTC millisecond of minute (0 - 59,999)
	};

	public ref struct PlaneState
	{
		PlaneState() { isDefault = false; gpsData = gcnew PlaneGPSPacket(); telemData = gcnew PlaneTelemPacket(); gimbalInfo = gcnew GimbalInfo(); }
		PlaneGPSPacket		^ gpsData;
		PlaneTelemPacket 	^ telemData;
		GimbalInfo			^ gimbalInfo;	
		float 				blurFactor;
		bool				isDefault;
		String ^ stringVal()
		{
			String ^ retVal = "PlaneState:\n";
			retVal += "\t\tgps:\n" + gpsData->stringVal() + "";
			retVal += "\t\ttelem:\n" + telemData->stringVal() + "";
			retVal += "\t\tgimbal:\n" + gimbalInfo->stringVal() + "";
			retVal += "\tblurFactor:" + blurFactor + "";
			return retVal;
		}
	};

}