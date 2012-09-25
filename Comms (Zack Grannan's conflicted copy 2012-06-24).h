#pragma once

#include "Comport.h"
#include "AutopilotComport.h"
#include "RabbitComport.h"
#include "ColorRef.h"

#define BOTH_CONNECTED			3
#define RABBIT_CONNECTED		1
#define AUTOPILOT_CONNECTED		2
#define BOTH_FAILED				0

#define GREEN_STATUS	1
#define YELLOW_STATUS	2
#define RED_STATUS		3

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Drawing;

namespace Communications
{

	
	delegate void rabbitUpdateDelegate( GimbalInfo ^ data );
	delegate void planeGPSDelegate( PlaneGPSPacket ^ data );
	delegate void planeTelemDelegate( PlaneTelemPacket ^ data );
	delegate void tellGUIAboutConnection( array<Int32> ^ retArr );
	delegate void guiConsoleDelegate( array<Object ^> ^ retArr );

	public ref class Comms {
	public:
		Comms(Object ^ telSimulator, Object ^ newDelegate);

		// connecting
		void connectAll();
		void disconnectAll();
		//void stopConnectingToRabbit();
		//void connectToRabbit();
		void attemptConnectionOnPort( Object ^ port );
		bool connectAutopilot();
		bool connectRabbit();
		void disconnectAutopilot();
		void disconnectRabbit();
		void rabbitJustConnected();

		// sending
		void gotoLatLon(float lat, float lon);
		void sendHelloToRabbit();
		void sendGimbalRollPitch(unsigned __int16 roll, unsigned __int16 pitch); 
		void sendGimbalLatLon(float lat, float lon);
		void sendZoom(unsigned __int32 zoom);
		void commandGimbalStabilizedMode();

		// receiving
		void receiveRabbitPacket(GimbalInfo ^ packet);
		void receivePlaneGPS(PlaneGPSPacket ^ packet);
		void receivePlaneTelem(PlaneTelemPacket ^ packet);

		// tell user stuff
		void printToConsole( String ^ message, Color col );
		void updateUIAboutCommsStatus(int status, String ^ type);

		// instance variables
		AutopilotComport ^ autopilot;
		RabbitComport ^ rabbit;


	private:

		TelemetrySimulator ^ theTelSimulator;
		Object ^ theDelegate;
		rabbitUpdateDelegate ^ rabbitDelegate;
		planeGPSDelegate ^ planeGPS;
		planeTelemDelegate ^ planeTelem;
		guiConsoleDelegate ^ consoleDelegate;

		Thread ^ rabbitConnectionThread;

		String ^ autopilotPortname;
		String ^ rabbitPortname;
		bool autopilotConnected;
		bool rabbitConnected;
 	};

}