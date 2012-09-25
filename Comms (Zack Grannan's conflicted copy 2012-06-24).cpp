#include "StdAfx.h"
#include "Comms.h"
#include "Comport.h"
#include "Form1.h"

#include "MasterHeader.h"

using namespace Communications;
using namespace System::Threading;


Comms::Comms(Object ^ telSimulator, Object ^ newDelegate) {

	theDelegate = newDelegate;
	autopilotPortname = nullptr;
	rabbitPortname = nullptr;
	autopilot = gcnew AutopilotComport((TelemetrySimulator ^)telSimulator, this);
	rabbit = gcnew RabbitComport((TelemetrySimulator ^)telSimulator, this, autopilot);
	autopilot->rabbit = rabbit;

	theTelSimulator = (TelemetrySimulator ^)telSimulator;

	autopilotConnected = false;
	rabbitConnected = false;
	
	rabbitDelegate = gcnew rabbitUpdateDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updateGimbalInfo );
	planeGPS = gcnew planeGPSDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updatePlaneGPSInfo );
	planeTelem = gcnew planeTelemDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::updatePlaneTelemInfo );
	consoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^ )theDelegate), &Skynet::Form1::printToConsole );

}

void Comms::connectAll() {

	Comport ^ thePort = gcnew Comport(nullptr);

	array<String ^> ^ portNames = thePort->getPortNames();
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };


	for (int i = 0; i < portNames->Length; i++) {
		
		String ^thePortName = portNames[i];

		Thread ^ connectThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Comms::attemptConnectionOnPort));
		connectThread->Name = "Comms Connection Thread for port " + thePortName;
		connectThread->Start(thePortName);
			
		/*// set port name
		//System::Diagnostics::Trace::WriteLine("Connecting to port " + portNames[i] );
		thePort->setPortName(portNames[i]);

		// connect to port
		thePort->connect();

		// send "hello" packet
		thePort->writeEncodedData( helloPacket );

		// wait for response
		array<System::Byte> ^ response = thePort->readRawData( 500 );

		// test response
		int responseType = -1; // 3 for autopilot, 2 for rabbit
		if (response->Length >= 4) {
			System::Diagnostics::Trace::WriteLine("4 or more bytes in response");
			if (response[0] == 0xFF && response[1] == 0x33 && response[5] == 0xFE) {
					responseType = response[2];
			}

		}


		// close comport
		thePort->disconnect();

		// save port name if valid
		if (responseType == 2) {
			rabbitPortname = portNames[i];
			System::Diagnostics::Trace::WriteLine("FOUND RABBIT");

		}
		else if (responseType == 3) {
			autopilotPortname = portNames[i];
			System::Diagnostics::Trace::WriteLine("FOUND AUTOPILOT");
		}*/
	}
	Thread::Sleep( 850 );

	//autopilotPortname = "COM1";
	//rabbitPortname = "COM1";

	// connect autopilot and rabbit
	int retval = BOTH_FAILED;
	if (this->connectAutopilot())
		retval += AUTOPILOT_CONNECTED;

//	if (this->connectRabbit())
//		retval += RABBIT_CONNECTED;

	
	tellGUIAboutConnection ^ tellGUIDelegate = gcnew tellGUIAboutConnection(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::handleConnectionResult );
	array<Int32> ^ retArr = {(Int32)retval};
	((Skynet::Form1 ^ )theDelegate)->Invoke( tellGUIDelegate, retArr );

	//printToConsole("I'm sorry, I'm afraid I can't let you do that.", Color::Orange);
	

	// attempt to connect to the rabbit
	/*if (retVal > BOTH_FAILED) {
		rabbit->connectToRabbit();
	}*/

}


void Comms::attemptConnectionOnPort( Object ^ port )
{
	String ^ portName = (String ^) port;
	System::Diagnostics::Trace::WriteLine("Comms: attempting to connect on port " + portName);
	
	Comport ^ thePort = gcnew Comport(nullptr);
	array<System::Byte> ^ helloPacket = {0x33, 0x01 };

	thePort->setPortName(portName);

	// connect to port
	thePort->connect();

	// send "hello" packet
	thePort->writeEncodedData( helloPacket );

	// wait for response
	array<System::Byte> ^ response = thePort->readRawData( 500 );

	// test response
	int responseType = -1; // 3 for autopilot, 2 for rabbit
	if (response->Length >= 4) {
		System::Diagnostics::Trace::WriteLine("4 or more bytes in response");
		if (response[0] == 0xFF && response[1] == 0x33 && response[5] == 0xFE) {
				responseType = response[2];
		}

	}


	// close comport
	thePort->disconnect();

	// save port name if valid
	if (responseType == 2) {
		rabbitPortname = portName;
		//this->connectRabbit();

	}
	else if (responseType == 3) {
		autopilotPortname = portName;
		//this->connectAutopilot();
	}

	
	System::Diagnostics::Trace::WriteLine("Comms: Finished attempting to connect on port " + portName);

}

void Comms::disconnectAll() {

	disconnectAutopilot();
	disconnectRabbit();

}

void testLatency( String ^ portName )
{
	// Crystal: test latency
	System::DateTime start;
	System::DateTime end;
	double totalDiff = 0;

	Comport ^ thePort = gcnew Comport(nullptr);
	array<System::Byte> ^ helloPacket = {0x02, 0x33, 0x01 }; // TODO: tim, what packet should we use for this?

	thePort->setPortName(portName);

	// connect to port
	thePort->connect();

	for ( int i = 0; i < 1000; i++)
	{
		start = System::DateTime::Now;

		// send "hello" packet
	    thePort->writeEncodedData( helloPacket );

	    // wait for response
	    array<System::Byte> ^ response = thePort->readRawData( 500 );

	    // test response
	    int responseType = -1; // 3 for autopilot, 2 for rabbit
	    if (response->Length >= 13) {
		    
		    if (response[0] == 0xFF && response[1] == 0x33 && response[5] == 0xFE) {
				   responseType = response[2];
		    }

	    }

		end = System::DateTime::Now;
		System::TimeSpan diff = end.Subtract(start);
		totalDiff += diff.TotalSeconds;
	}
	thePort->disconnect();
	System::Diagnostics::Trace::WriteLine(" Total Time: " +  totalDiff);

 
}


bool Comms::connectAutopilot() {
	if (autopilotPortname == nullptr)
		return false;
	
	bool success = false;

	System::Diagnostics::Trace::WriteLine("Connecting to AUTOPILOT");

	
	//testLatency(autopilotPortname);



	success = autopilot->connect(autopilotPortname);

	// try to connect to rabbit
	if (success) {
		autopilotConnected = true;
		autopilot->beginReading("Autopilot");
	}
	
	return success;
}


bool Comms::connectRabbit() {

	// only connect if autopilot is connected
	//if (autopilotConnected)
	//	rabbit->connectToRabbit();
	return true;
}



		
void Comms::disconnectAutopilot() {
	if (autopilotConnected) {
		autopilot->disconnect();
		autopilotConnected = false;
	}
}



void Comms::disconnectRabbit() {
	//rabbit->stopConnecting();
	rabbitConnected = false;
}


void Comms::rabbitJustConnected()
{
	printToConsole("Connected to Rabbit", Color::Green);
	updateUIAboutCommsStatus( GREEN_STATUS, "Rabbit" );

}

void Comms::gotoLatLon(float lat, float lon) 
{
	autopilot->gotoLatLon(lat, lon);
}

void Comms::sendHelloToRabbit(){
	rabbit->sendHello();
	System::Diagnostics::Trace::WriteLine("Comms::sendHelloToRabbit()");
}


void Comms::commandGimbalStabilizedMode()
{
	rabbit->enterStabilizedMode();
}

void Comms::sendGimbalLatLon(float lat, float lon)
{
	rabbit->sendGimbalTarget(lat, lon);

}

void Comms::sendGimbalRollPitch(unsigned __int16 roll, unsigned __int16 pitch)
{
	if (roll < MIN_ROLL || roll > MAX_ROLL || pitch < MIN_PITCH || pitch > MAX_PITCH) {
		System::Diagnostics::Trace::WriteLine("Comms::sendGimbalRollPitch() ERROR: roll " + roll + " or pitch " + pitch + " out of bounds: ");
		return;
	}

	rabbit->sendGimbalCommand(roll, pitch);

}

void Comms::sendZoom(unsigned __int32 zoom)
{
	rabbit->sendCameraZoom(zoom);
}

void Comms::printToConsole( String ^ message, Color col )
{

	guiConsoleDelegate ^ newconsoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::printToConsole );

	array<Object ^> ^ retArr = gcnew array< Object^ >{message, col};
	
	((Skynet::Form1 ^ )theDelegate)->Invoke( newconsoleDelegate, gcnew array<Object ^>{retArr});

	
}

void Comms::updateUIAboutCommsStatus(int status, String ^ type)
{
	guiConsoleDelegate ^ newconsoleDelegate = gcnew guiConsoleDelegate(((Skynet::Form1 ^)theDelegate), &Skynet::Form1::updateCommsStatus );

	Int32 value = status;
	array<Object ^> ^ retArr = gcnew array< Object^ >{type, value};
	
	((Skynet::Form1 ^ )theDelegate)->Invoke( newconsoleDelegate, gcnew array<Object ^>{retArr});
}

void Comms::receiveRabbitPacket(GimbalInfo ^ packet) 
{	
	rabbitDelegate(packet);
}


void Comms::receivePlaneGPS(PlaneGPSPacket ^ packet)
{
	planeGPS(packet);
	//System::Diagnostics::Trace::WriteLine("received PlaneGPSPacket in Comms");
}

void Comms::receivePlaneTelem(PlaneTelemPacket ^ packet)
{
	planeTelem(packet);
}