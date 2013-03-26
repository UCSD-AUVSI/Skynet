#pragma once

#include "Comport.h"
#include "ComportHandler.h"
#include "TelemetryStructures.h"

#define AUTOPILOT_LATENCY	0.125 // (ms)

using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{

	public ref struct PacketHeader
	{
		__int32 type;
		__int32 size;

	};

	public ref struct CommPacket5
	{
		unsigned char numBytes;
		array<System::Byte> ^ arr;
	};

	public ref struct AutopilotVCPacket10
	{
		__int16 destAddr;
		unsigned char kestrelPacketType;
		array<System::Byte> ^ arr;  //data
	};

	public ref struct UAVCommand
	{
		virtual String ^ ToString() override {
			return "("+lat+","+lon+")";
		}
		unsigned __int16	destAddr			;
		unsigned char		commandType			;
		float				lat					;
		float				lon					;
		float				speed				;
		float				altitude			;
		unsigned __int16	gotoIndex			;
		unsigned __int16	time				;
		float 				radius				;
		float 				flareSpeed			;
		float 				flareAltitude		;
		float 				breakAltitude		;
		float				descentRate			;
		float				approachAltitude	;
		float				approachLongitude	;
		__int32				unusedOne;
		float				unusedTwo;
	};



	public ref struct AutopilotVCPacket20
	{
		unsigned char packetID;
		unsigned char onOff;
	};

	public ref struct AutopilotVCPacket30
	{
		__int16 destAddr;
	};

	public ref struct AutopilotVCPacket32
	{
		__int16				destAddr;
		array<UAVCommand ^> ^ instructions;
	};

	


	/*
	 * All of the values associated with an autopilot (194 bytes inc header)
	 */
	/*
	struct AutopilotState
	{
		__int16 address;  //the id of this agent
		
		float roll;          //current roll angle, radians (-pi to pi)
		float pitch;         //current pitch angle, radians (-pi to pi)
		float heading;       //current heading angle, radians (0 -> 2 pi), true psi
		float turnRate;      //radians per second
		float groundTrack;   //radians 0 to 2 pi
		float altitudeHAL;   //meters above launch
		float altitudeMSL;   //mean sea level
		float airspeed;      //meters per second
		float velocity;      //meters per second
		float climbRate;     //meters per second

		float accelX;        //x acceleration, meters per second^2 (out of nose, assuming the autopilot is correctly oriented inside the plane)
		float accelY;        //y acceleration, meters per second^2 (out of right wing)
		float accelZ;        //z acceleration, meters per second^2 (down)
		
		float rollRate;      //rotational rate around x, radians per second
		float pitchRate;     //rotational rate around y, radians per second
		float yawRate;       //rotational rate around z, radians per second

		float velocityNorth; //gps velocity in the north direction, meters per second
		float velocityEast;  //gps velocity in the east direction, meters per second
		float velocityDown;  //gps velocity in the down direction, meters per second

		float gpsLatitude;   //latitude of UAV is decimal degrees
		float gpsLongitude;  //longitude of UAV in decimal degrees
		float gpsVelocity;   //the gps computed velocity, meters per second
		float gpsAltitude;   //the gps computed altitude, in meters
		float gpsHeading;    //the gps computed heading, in radians (0 -> 2 pi)
		unsigned char gpsNumSats;  //how many satellites are being tracked

		float gpsHomePositionLatitude;  //latitude of the home position in degrees
		float gpsHomePositionLongitude; //longitude of the home position in degrees
		float gpsHomePositionAltitude;  //altitdude of the home position, MSL, in meters

		unsigned char currentCommand;  //current command of flight script (0 = first waypoint, 2 = third waypoint, etc)

		bool synchStatus;    //whether or not utc time is in synch

		unsigned char UTCyear;           //years since 1900
		unsigned char UTCmonth;          //UTC month (1 - 12)
		unsigned char UTCday;            //UTC day of month (1 - 31)
		unsigned char UTChour;           //UTC hour of dat (0 - 23)
		unsigned char UTCmin;            //UTC minute of hour (0 - 59)
		unsigned __int16 UTCmillisecond; //UTC millisecond of minute (0 - 59,999)

		float desiredLatitude;   //the desired latitude of the UAV in degrees
		float desiredLongitude;  //the desired longitude of the UAV in degrees
	    float desiredAltitude;   //the desired altitude of the UAV in meters.  Unsure whether this is MSL or HAL
		float desiredVelocity;   //the desired velocity of the UAV in meters per second
		float desiredRoll;       //desired roll, radians
		float desiredPitch;      //desired pitch, radians
		float desiredHeading;    //desired heading, radians (0 - 2 pi)
		float desiredTurnRate;   //desired turn rate, radians per second
		float desiredClimbRate;  //desired climb rate, meters per second

		float timeToTarget;      //the estimalted time to get to the next target, or in the case of loiter, the time remaining in loiter (255 = infinite loiter), in seconds
		float distanceToTarget;  //distance from UAV to desired target, meters
		float headingToTarget;   //heading to desired UAV target, radians (0 to 2 pi)
		
		float batteryVoltage;    //how many volts are being input to Kestrel
		float airbornTimer;      //amount of time the autopilot has been airborn, in seconds
	};
	*/

	public ref class AutopilotComport : public ComportHandler {
	public:
		AutopilotComport(TelemetrySimulator ^ telSimulator, Comms ^ newDelegate) : ComportHandler(telSimulator, newDelegate, "Autopilot") {
			planeAddress = 2418;
		};

		void writeData( array<System::Byte> ^ inBuffer ); 
		virtual void analyzeData( array<System::Byte> ^ inBuffer ) override;
		virtual void disconnect() override;
		void receivePassthrough( array<System::Byte> ^ inBuffer );
		
		void gotoLatLon(float lat, float lon);
		void requestPacketForwarding();  //turn on packet forwarding from VC
		virtual void afterBeginReading() override;
		void requestAgents();
		void sendWaypoints(Object ^ waypointsObject);
		void sendWaypointsInThread(array<Waypoint ^>^ commands);
		void writeDataInThread( Object ^ buffer);

		void sendPassthroughPacket( array<System::Byte> ^ inBuffer );
		bool connect(String ^ portName);

		static UAVCommand ^ AutopilotComport::commandFromWaypoint(Waypoint ^ waypoint);
		array<System::Byte> ^ getVCPacket32(array<UAVCommand ^>^ commands);
		
		
		Object ^ rabbit;

	private:

		array<System::Byte> ^ getCommPacket5Data(array<System::Byte> ^ dataForRabbit);
		array<array<System::Byte> ^>^ AutopilotComport::getVCPackets11(array<Waypoint ^>^ waypoints);
		array<System::Byte> ^ getVCPacket10(unsigned char kestrelPacketType, array<System::Byte> ^ arr);
		array<System::Byte> ^ getVCPacket20(unsigned char packetID, unsigned char onOff);
		array<System::Byte> ^ getVCPacket30();
		
		array<System::Byte> ^ getVCPacket61();

		__int16 getInt16FromBytes(array<System::Byte> ^ arr, int startIndex);
		unsigned __int16 getUInt16FromBytes(array<System::Byte> ^ arr, int startIndex);
		__int32 getInt32FromBytes(array<System::Byte> ^ arr, int startIndex);
		unsigned char getUCharFromBytes(array<System::Byte> ^ arr, int startIndex);
		char getCharFromBytes(array<System::Byte> ^ arr, int startIndex);
		float getFloatFromBytes(array<System::Byte> ^ arr, int startIndex);
		__int16 planeAddress;  

		String ^ passthroughBuffer;
	};

}