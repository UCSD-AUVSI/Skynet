#include "StdAfx.h"

#include "RabbitComport.h"
#include "Comms.h"
#include "AutopilotComport.h"
#include "MasterHeader.h"


#define GIMBAL_STABILIZED_PACKET	0x03
#define GIMBAL_TARGET_LOCK_PACKET	0x02
#define GIMBAL_PACKET				0x01
#define CAMERA_PACKET				0x00
#define HELLO_PACKET				0x33

using namespace Communications;

/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void RabbitComport::afterBeginReading()
{
	// better update frequency
	thePort->updateFrequency = 5;
	zoomLevel = 1;
}

/*
void RabbitComport::connectToRabbit()
{
	
	rabbitState = RABBIT_CONNECTING;
	connectionThread = gcnew Thread(gcnew ThreadStart(this, &Comms::attemptToConnect));
	connectionThread->Name = "Rabbit Connection Thread";
	connectionThread->Start(thePortName);

}

void RabbitComport::attemptToConnect()
{
	try {
		while (rabbitState == RABBIT_CONNECTING)
		{
			// send hello packet through autopilot
			autopilot->sendPassthroughPacket( {0x33, 0x01 } );

			// sleep 1 second
			Thread::Sleep( 1000 );
		}

		
	} catch (Exception ^)
	{
		// kill when we get a response or disconnect from autopilot
	}
	
	connectionThread = nullptr;

}


void RabbitComport::stopConnecting( bool connected )
{
	if (connected)
		rabbitState = RABBIT_CONNECTED;
	else 
		rabbitState = RABBIT_DISCONNECTED;

	Thread::Sleep( 100 );
	if (connectionThread != nullptr) {
		connectionThread->Abort();
		connectionThread = nullptr;
	}

}*/

void RabbitComport::disconnect()
{
	rabbitState = false;

}

void RabbitComport::sendBytesToRabbit( array<System::Byte> ^ packet )
{
	((AutopilotComport ^)autopilotComport)->sendPassthroughPacket( packet );
}

void RabbitComport::sendGimbalTarget(float lat, float lon)
{
	const int packetLength = 21;
	unsigned char packetType = GIMBAL_TARGET_LOCK_PACKET;
	char * dataPtr;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = *(char *)&packetType;
	dataPtr = (char*)&lat; // lat
	packet[1] = *dataPtr++;
	packet[2] = *dataPtr++;
	packet[3] = *dataPtr++;
	packet[4] = *dataPtr++;
	dataPtr = (char*)&lon; // lon
	packet[5] = *dataPtr++;
	packet[6] = *dataPtr++;
	packet[7] = *dataPtr++;
	packet[8] = *dataPtr++;

	float zero = 0.0f; // alt
	dataPtr = (char*)&zero;
	packet[9] = *dataPtr++;
	packet[10] = *dataPtr++;
	packet[11] = *dataPtr++;
	packet[12] = *dataPtr++;

	zero = 0.0f; // ground lat
	dataPtr = (char*)&zero;
	packet[13] = *dataPtr++;
	packet[14] = *dataPtr++;
	packet[15] = *dataPtr++;
	packet[16] = *dataPtr++;

	zero = 0.0f; // ground lon
	dataPtr = (char*)&zero;
	packet[17] = *dataPtr++;
	packet[18] = *dataPtr++;
	packet[19] = *dataPtr++;
	packet[20] = *dataPtr++;


	System::Diagnostics::Trace::WriteLine("SENDING gimbal command type 2. lat: " + lat + " lon: " + lon);
		

	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::enterStabilizedMode()
{
	const int packetLength = 1;
	unsigned char packetType = GIMBAL_STABILIZED_PACKET;
	
//	char * dataPtr;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = *(char *)&packetType;

	System::Diagnostics::Trace::WriteLine("SENDING command to gimbal: enter stabilized mode");	

	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::sendGimbalCommand( unsigned __int16 roll, unsigned __int16 pitch )
{
//	System::Diagnostics::Trace::WriteLine("RabbitComport::sendGimbalCommand() roll:" + roll + " pitch:" + pitch);
	
	const int packetLength = 5;
	unsigned char packetType = GIMBAL_PACKET;
	char * dataPtr;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = *(char *)&packetType;
	dataPtr = (char*)&roll;
	packet[1] = *dataPtr++;   //NOTE: Tim Reversed thee bytes 1&2, 3&4
	packet[2] = *dataPtr++;
	dataPtr = (char*)&pitch;
	packet[3] = *dataPtr++;
	packet[4] = *dataPtr++;

	System::Diagnostics::Trace::WriteLine("SENDING gimbal command type "+packetType+". roll: " + roll + " pitch: " + pitch);
		

	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::sendHello()
{
	const int packetLength = 2;
	unsigned char packetType = HELLO_PACKET;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = packetType;
	packet[1] = 0x01;  //because its comming from the ground station computer

	// send 
	sendBytesToRabbit( packet );
}

void RabbitComport::sendCameraZoom( unsigned __int32 zoom )
{
	const int packetLength = 5;
	unsigned char packetType = CAMERA_PACKET;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength);
	packet[0] = *(char *)&packetType;
	char * dataPtr = (char *)&zoom;
	packet[4] = *dataPtr++;
	packet[3] = *dataPtr++;
	packet[2] = *dataPtr++;
	packet[1] = *dataPtr++;

	
	System::Diagnostics::Trace::WriteLine("SENDING zoom: " + zoom);

	// send 
	sendBytesToRabbit( packet );


}

void RabbitComport::analyzeData( array<System::Byte> ^ inBuffer )
{
	if (!rabbitState) {
		rabbitState = true;
		((Comms ^)theDelegate)->updateUIAboutCommsStatus(GREEN_STATUS, "Rabbit");
	}

	//System::Diagnostics::Trace::WriteLine("RabbitComport::receiveData()");
	
	// check input
	if (inBuffer->Length < 1)
		return;

	const int NUM_ELEMENTS = 11;
	const int BYTES_PER_FLOAT = 4;


	unsigned char packetType = inBuffer[0];

	// gimbal info
	if (packetType == GIMBAL_PACKET) {
		GimbalInfo ^ packet = gcnew GimbalInfo();
		const int bytesPerElement = 2;
		const int packetStart = 1;
		const int numElements = 2;

		pin_ptr<unsigned __int16> data = &(packet->roll);
		char * dataPtr = (char *)data;
		
		*dataPtr++ = inBuffer[1];
		*dataPtr++ = inBuffer[2];
		*dataPtr++ = inBuffer[3];
		*dataPtr++ = inBuffer[4];

		/*for (int j = 0; j < numElements; j++) {


			for (int i = 1; i >= 0; i--) {
				*dataPtr =  inBuffer[j*bytesPerElement + i + packetStart]; // 1 to ignore packetType byte
				dataPtr++;

			}			
		}*/

		// add last zoom level
		packet->zoom = zoomLevel;
		lastRoll = packet->roll;
		lastPitch = packet->pitch;

		// calculate packet time (ideally, set this on rabbit and send down with packet, but we will assume a 125ms delay)
		System::DateTime packetTime = System::DateTime::Now.AddMilliseconds(-RABBIT_DELAY);
		packet->UTCyear = packetTime.Year;
		packet->UTCmonth = packetTime.Month;
		packet->UTCday = packetTime.Day;
		packet->UTChour = packetTime.Hour;
		packet->UTCmin = packetTime.Minute;
		packet->UTCmillisecond = packetTime.Second*1000 + packetTime.Millisecond;
		System::Diagnostics::Trace::WriteLine("packet type 0. roll: " + packet->roll + " pitch: " + packet->pitch);
		
		((Comms ^)theDelegate)->receiveRabbitPacket(packet);
		
	}

	else if (packetType == CAMERA_PACKET)
	{
		GimbalInfo ^ packet = gcnew GimbalInfo();
		const int bytesPerElement = 2;
		const int packetStart = 1;
		const int numElements = 2;

		unsigned __int32 newZoom;
		char * dataPtr = (char *)&newZoom;
		*dataPtr++ = inBuffer[4];
		*dataPtr++ = inBuffer[3];
		*dataPtr++ = inBuffer[2];
		*dataPtr++ = inBuffer[1];

		switch( newZoom )
		{
			case 0x00000000:
				zoomLevel = 1;
				break;
			case 0x00080000:
				zoomLevel = 2;
				break;
			case 0x01000000:
				zoomLevel = 3;
				break;
			case 0x01080000:
				zoomLevel = 4;
				break;
			case 0x02000000:
				zoomLevel = 5;
				break;
			case 0x02080000:
				zoomLevel = 6;
				break;
			case 0x03000000:
				zoomLevel = 7;
				break;
			case 0x03080000:
				zoomLevel = 8;
				break;
			case 0x04000000:
				zoomLevel = 9;
				break;
			default:
				zoomLevel = 80; // no zoom update
				break;
		}

		packet->zoom = zoomLevel;
		packet->roll = lastRoll;
		packet->pitch = lastPitch;
		
		// calculate packet time (ideally, set this on rabbit and send down with packet, but we will assume a 125ms delay)
		System::DateTime packetTime = System::DateTime::Now.AddMilliseconds(-RABBIT_DELAY);
		packet->UTCyear = packetTime.Year;
		packet->UTCmonth = packetTime.Month;
		packet->UTCday = packetTime.Day;
		packet->UTChour = packetTime.Hour;
		packet->UTCmin = packetTime.Minute;
		packet->UTCmillisecond = packetTime.Second*1000 + packetTime.Millisecond;
		
		
		String ^ bufferString = "";
		for (int i = 0; i < inBuffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
		}
		
		//System::Diagnostics::Trace::WriteLine("RabbitComport::analyzeData(). bufLen:" + inBuffer->Length + " buffer:" + bufferString);
		System::Diagnostics::Trace::WriteLine("packet type 1. roll: " + packet->roll + " pitch: " + packet->pitch);
		((Comms ^)theDelegate)->receiveRabbitPacket(packet);
	}
	
	else if (packetType == HELLO_PACKET)
	{
		((Comms ^)theDelegate)->sendGimbalRollPitch(3000, 3000);
		((Comms ^)theDelegate)->sendZoom(0);
	}

}