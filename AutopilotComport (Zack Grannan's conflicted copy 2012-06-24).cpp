#include "StdAfx.h"

#include "AutopilotComport.h"
#include "RabbitComport.h"
#include "Comms.h"
#include "MasterHeader.h"

using namespace Communications;
using namespace System;
using namespace System::Drawing;
using namespace System::IO;


void AutopilotComport::disconnect() 
{
	System::Diagnostics::Trace::WriteLine("disconnecting AutopilotComport::request termination of packet forwarding()");

	array<System::Byte> ^ packet = getVCPacket20(0, 0);

	writeData(packet);
	
	//try {
	thePort->disconnect();
	Thread::Sleep(10);
	if (comReadThread != nullptr) {
		comReadThread->Abort();	
		comReadThread = nullptr;
	}
	//}
	//catch (Exception ^) {}
}

void AutopilotComport::writeData( array<System::Byte> ^ inBuffer )
{
	thePort->writeEncodedData( inBuffer );
}

String ^ bufferToString( array<System::Byte> ^ inBuffer )
{
	String ^ bufferString = "0x" + Convert::ToString(inBuffer[0], 16);
	for (int i = 1; i < inBuffer->Length; i++) {
		bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
	}

	return bufferString;
}

void AutopilotComport::analyzeData( array<System::Byte> ^ inBuffer )
{
	if (inBuffer->Length < 8){ 
		System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(): inbuffer too short");
		return;
	}
	//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData()");

	// handle data
	PacketHeader ^theHeader = gcnew PacketHeader();

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		dataPtr[j] = inBuffer[packetIndex];
		packetIndex++;
	}

	/* 	char * dataPtr = (char *)theHeader;
	int packetIndex = 0;
	
	theHeader->type = getInt32FromBytes(inBuffer, 0);
	theHeader->size = getInt32FromBytes(inBuffer, 4); */
	String ^ bufferString = "";
	for (int i = 0; i < inBuffer->Length; i++) {
		bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
	}
		
	//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(). bufLen:" + inBuffer->Length + " buffer:" + bufferString);
	//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() began: packetType: " + theHeader->type + " length: " + inBuffer->Length);
	
	if(theHeader->type == 10){  //pass through autopilot packet which we need to decode, passthrough non-guaranteed packet {

	//	System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData() before:" + inBuffer->Length + " buffer:" + bufferToString(inBuffer));

		array<System::Byte> ^ newPacket = gcnew array<System::Byte>(inBuffer->Length - 8); // ignore header
		int j = 0;
		for (int i = 8; i < inBuffer->Length; i++) { // ignore last three bytes (checksum checksum fe)
			newPacket[j] = inBuffer[i];
			j++;
		}
	//	System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData() after:" + newPacket->Length + " buffer:" + bufferToString(newPacket));

		newPacket = thePort->stripAndChecksum(newPacket);

		//System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData(): done decoding: " + newPacket);

		if(newPacket != nullptr){  //if the decoded packet's xor worked out
			unsigned char kestrelPacketType = getUCharFromBytes(newPacket, 0);  //the packet type, in kestrel terms rather than VC terms (ie, chapter 4 of kestrel comms guide)
			__int16 sourceAddress = getInt16FromBytes(newPacket, 1);  //get the source address of the packet, it should be from agentAddress
			
			/*if(kestrelPacketType == 1){ // acknowledgement packet
				unsigned char packetType = getUCharFromBytes(newPacket, 3);  //the packet type being acknowledged.  XXX we don't do anything with it, but we can if we need to
			}*/
			//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(): kestrelPacketType: " + kestrelPacketType);
			if(kestrelPacketType == 5)
			{
				int dataSize = newPacket[5];
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(): RABBIT PACKET! 5 We Has Serial E Passthrough:" + dataSize);
				// start at byte 5 of newPacket
				// byte 5 = num bytes
				// byte 6,7,8,9 = avionics time
				// byte 10-n = raw packet data

				array<System::Byte> ^ rabbitPacket = gcnew array<System::Byte>(dataSize);
				int packetIndex = 10;
				for (int i = 0; i < dataSize; i++)
					rabbitPacket[i] = newPacket[packetIndex++];

				((RabbitComport ^)rabbit)->analyzeData(rabbitPacket);

				//int packetIndex = 10;
				//String ^ bufferString = "0x" + Convert::ToString(newPacket[packetIndex++], 16);
				//for (int i = 1; i < dataSize; i++) {
				//	bufferString = bufferString + " 0x" + Convert::ToString(newPacket[packetIndex++], 16);
				//}
				//System::Diagnostics::Trace::WriteLine(bufferString + "\n... Done");
			}
			/*if(kestrelPacketType == 29)  //mixed telemetry response
			{
				planeState->roll = (float)(getInt16FromBytes(newPacket, 5)) / 1000.0f;
				planeState->pitch = (float)(getInt16FromBytes(newPacket, 7)) / 1000.0f;
				planeState->heading = (float)(getUInt16FromBytes(newPacket, 9)) / 1000.0f;
				planeState->groundTrack = (float)(getUInt16FromBytes(newPacket, 11)) / 1000.0f;
				planeState->altitudeHAL = (float)(getUInt16FromBytes(newPacket, 13)) / 6.0f - 1000.0f;
				planeState->gpsLatitude = getFloatFromBytes(newPacket, 15);
				planeState->gpsLongitude = getFloatFromBytes(newPacket, 19);
				planeState->UTCyear = getUCharFromBytes(newPacket, 25);
				planeState->UTCmonth = getUCharFromBytes(newPacket, 26);
				planeState->UTCday = getUCharFromBytes(newPacket, 27);
				planeState->UTChour = getUCharFromBytes(newPacket, 28);
				planeState->UTCmin = getUCharFromBytes(newPacket, 29);
				planeState->UTCmillisecond = getUInt16FromBytes(newPacket, 30);
				planeState->airspeed = (float)(getUInt16FromBytes(newPacket, 32)) / 20.0f - 10.0f;
				System::Diagnostics::Trace::WriteLine("TESTING: Latitude " + planeState->gpsLatitude + "  Longitude: " + planeState->gpsLongitude);
			} */
			else if(kestrelPacketType == 248)  //navigation packet
			{
				PlaneGPSPacket ^planeState = gcnew PlaneGPSPacket();
				planeState->gpsVelocity = (float)(getUInt16FromBytes(newPacket, 5)) / 20.0f - 10.0f;
				planeState->gpsAltitude = (float)(getUInt16FromBytes(newPacket, 7)) / 6.0f - 1000.0f;
				planeState->gpsHeading = ((float)(getUInt16FromBytes(newPacket, 9)) / 1000.0f)*(float)(180.0/PI);
				planeState->gpsLatitude = getFloatFromBytes(newPacket, 13);
				planeState->gpsLongitude = getFloatFromBytes(newPacket, 19);
				planeState->gpsHomePositionLatitude = getFloatFromBytes(newPacket, 23);
				planeState->gpsHomePositionLongitude = getFloatFromBytes(newPacket, 27);
				planeState->currentCommand = getUCharFromBytes(newPacket, 31);
				planeState->desiredLatitude = getFloatFromBytes(newPacket, 33);
				planeState->desiredLongitude = getFloatFromBytes(newPacket, 37);
				planeState->timeToTarget = getFloatFromBytes(newPacket, 41);
				planeState->distanceToTarget = getFloatFromBytes(newPacket, 45);
				planeState->headingToTarget = (float)(getUInt16FromBytes(newPacket, 49)) / 1000.0f;
				//planeState->UTCyear = getUCharFromBytes(newPacket, 58);
				//planeState->UTCmonth = getUCharFromBytes(newPacket, 59);
				//planeState->UTCday = getUCharFromBytes(newPacket, 60);
				//planeState->UTChour = getUCharFromBytes(newPacket, 61);
				//planeState->UTCmin = getUCharFromBytes(newPacket, 62);
				//planeState->UTCmillisecond = getUInt16FromBytes(newPacket, 63);
				planeState->gpsHomePositionAltitude = (float)(getUInt16FromBytes(newPacket, 64) / 6.0f - 1000.0f);

				System::DateTime targetTime = System::DateTime::Now.AddSeconds(-AUTOPILOT_LATENCY);
				planeState->UTCyear = targetTime.Year;
				planeState->UTCmonth = targetTime.Month;
				planeState->UTCday = targetTime.Day;
				planeState->UTChour = targetTime.Hour;
				planeState->UTCmin = targetTime.Minute;
				planeState->UTCmillisecond = targetTime.Second*1000 + targetTime.Millisecond;


				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() min:" + (targetTime.Minute - planeState->UTCmin) + " sec:" + (targetTime.Second - planeState->UTCmillisecond/1000));

				//System::Diagnostics::Trace::WriteLine("TESTING: alt:" + planeState->gpsAltitude + " vel:" + planeState->gpsVelocity + "  Latitude " + planeState->gpsLatitude + "  Longitude: " + planeState->gpsLongitude); 
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() mill:" + planeState->UTCmillisecond + " min:" + planeState->UTCmin + " hr:" + planeState->UTChour);
				String ^ bufferString = "";
				for (int i = 0; i < 64; i++) {
					bufferString = bufferString + " 0x" + Convert::ToString(newPacket[i], 16);
				}
		
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::planeState(). bufLen:" + 64 + " buffer:" + bufferString);
	
				
				((Comms ^)theDelegate)->receivePlaneGPS(planeState);
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(): receivePlaneGPS");

			}
			else if(kestrelPacketType == 249) //telemetry packet
			{
				PlaneTelemPacket ^planeState = gcnew PlaneTelemPacket();
				planeState->altitudeHAL = (float)(getUInt16FromBytes(newPacket, 5)) / 6.0f - 1000.0f;
				planeState->velocity = (float)(getUInt16FromBytes(newPacket, 7)) / 20.0f - 10.0f;
				planeState->roll = (float)(getInt16FromBytes(newPacket, 9)) / 1000.0f;
				planeState->pitch = (float)(getInt16FromBytes(newPacket, 11)) / 1000.0f;
				planeState->heading = ((float)(getUInt16FromBytes(newPacket, 13)) / 1000.0f);
				planeState->turnRate = (float)(getInt16FromBytes(newPacket, 15)) / 1000.0f;
				planeState->batteryVoltage = (float)(getUCharFromBytes(newPacket, 20)) / 5.0f;
				planeState->gpsNumSats = getUCharFromBytes(newPacket, 23);
				planeState->desiredAltitude = (float)(getUInt16FromBytes(newPacket, 25)) / 6.0f - 1000.0f;
				planeState->desiredVelocity = (float)(getUCharFromBytes(newPacket, 27)) / 2.0f - 10.0f;
				planeState->desiredRoll = (float)(getInt16FromBytes(newPacket, 28)) / 1000.0f;
				planeState->desiredPitch = (float)(getInt16FromBytes(newPacket, 30)) / 1000.0f;
				planeState->desiredHeading = (float)(getUInt16FromBytes(newPacket, 32)) / 1000.0f;
				planeState->desiredTurnRate = (float)(getInt16FromBytes(newPacket, 34)) / 1000.0f;
				planeState->airbornTimer = getFloatFromBytes(newPacket, 45);
				//planeState->UTCyear = getUCharFromBytes(newPacket, 49);
				//planeState->UTCmonth = getUCharFromBytes(newPacket, 50);
				//planeState->UTCday = getUCharFromBytes(newPacket, 51);
				//planeState->UTChour = getUCharFromBytes(newPacket, 52);
				//planeState->UTCmin = getUCharFromBytes(newPacket, 75);
				//planeState->UTCmillisecond = getUInt16FromBytes(newPacket, 76);
				planeState->rollRate = ((float)(getCharFromBytes(newPacket, 59)) - 128.0f) / 80.0f;
				planeState->pitchRate = ((float)(getCharFromBytes(newPacket, 60)) - 128.0f) / 80.0f;
				planeState->yawRate = ((float)(getCharFromBytes(newPacket, 61)) - 128.0f) / 80.0f;
				planeState->altitudeMSL = (float)(getUInt16FromBytes(newPacket, 62)) / 6.0f - 1000.0f;
				planeState->desiredClimbRate = (float)(getInt16FromBytes(newPacket, 79)) / 300.0f;
				planeState->climbRate = (float)(getInt16FromBytes(newPacket, 79)) / 300.0f;
				

				System::DateTime targetTime = System::DateTime::Now.AddSeconds(-AUTOPILOT_LATENCY);
				planeState->UTCyear = targetTime.Year;
				planeState->UTCmonth = targetTime.Month;
				planeState->UTCday = targetTime.Day;
				planeState->UTChour = targetTime.Hour;
				planeState->UTCmin = targetTime.Minute;
				planeState->UTCmillisecond = targetTime.Second*1000 + targetTime.Millisecond;


				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() altHAL:" + planeState->altitudeHAL + " roll:" + planeState->roll + " pitch:" + planeState->pitch + " heading:" + planeState->heading);
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() mill:" + planeState->UTCmillisecond + " min:" + planeState->UTCmin + " hr:" + planeState->UTChour);

				((Comms ^)theDelegate)->receivePlaneTelem(planeState);
				//System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData(): receivePlaneState");
			} 
			//else 
			//	System::Diagnostics::Trace::WriteLine("AutopilotComport::analyzeData() ignored type " + kestrelPacketType );
		}

	} else if(theHeader->type == 61) {  // get agent list response
		System::Diagnostics::Trace::WriteLine("Got agent list response");

		String ^ bufferString = "0x" + Convert::ToString(inBuffer[0], 16);
		for (int i = 1; i < inBuffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
		}
		
		//System::Diagnostics::Trace::WriteLine("AutopilotComport::receiveData bufLen:" + inBuffer->Length + " buffer:" + bufferString);

		unsigned char numAgents = getUCharFromBytes(inBuffer, 8);  //get the number of agents in the list.  We typically expect 1
		if(numAgents > 0){
			planeAddress = getInt16FromBytes(inBuffer, 9);  //get the agent address
			System::Diagnostics::Trace::WriteLine("Set Autopilot Address to " + planeAddress);
			//((Comms ^)theDelegate)->printToConsole("Set Autopilot Address to " + planeState->address, gcnew ColorRef( Color::Green ));
		}



	}
	// TODO: if rabbit packet, get rabbit data and do ((RabbitComport ^)rabbit)->analyzeData( rabbitData );
}
		
/*
 * Right after comms are set up, ask for packet forwarding and for the agent id
 */
void AutopilotComport::afterBeginReading()
{
	// better update frequency
	thePort->updateFrequency = 5;


	System::Diagnostics::Trace::WriteLine("running AutopilotComport::afterBeginReading()");
	Thread::Sleep( 250 );
	requestAgents();  //ask for all agents
	Thread::Sleep( 250 );
	requestPacketForwarding();  //ask VC to pass on data
}

void AutopilotComport::gotoLatLon(float lat, float lon) 
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::gotoLatLon()");

	array<System::Byte> ^ packet = getVCPacket32(0, lat, lon, 14.0f, 100.0f, 0, 0, 0, 5.0f, 5.0f, 15.0f, 3.0f, 0.0f, 0.0f);

	// copy all data into packet

	writeData( packet ); // set waypoint command on VC

	packet = getVCPacket30();  //get the upload commands packet

	writeData( packet );  //upload waypoint from VC to Autopilot
}

void AutopilotComport::sendPassthroughPacket( array<System::Byte> ^ inBuffer )
{
	
	//System::Diagnostics::Trace::WriteLine("AutopilotComport::sendPassthroughPacket()");

	writeData( getVCPacket10(5, getCommPacket5Data(inBuffer)) );  //on liner - zing!
}

/*
 * asks VC to forward all packets
 */
void AutopilotComport::requestPacketForwarding()
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::requestPacketForwarding()");

	array<System::Byte> ^ packet = getVCPacket20(0, 1);

	writeData( packet );
}

/*
 * asks VC to give a list of all agents
 */
void AutopilotComport::requestAgents()
{
	System::Diagnostics::Trace::WriteLine("running AutopilotComport::requestAgents()");

	array<System::Byte> ^ packet = getVCPacket61();

	writeData( packet );
}

/*
 * Serial E Passthrough Data
 */
array<System::Byte> ^ AutopilotComport::getCommPacket5Data(array<System::Byte> ^ dataForRabbit)
{
	array<System::Byte> ^ packet = gcnew array<System::Byte>(1 + dataForRabbit->Length);

	CommPacket5 ^ theStruct = gcnew CommPacket5();

	// copy data into struct	
	theStruct->numBytes = dataForRabbit->Length;
	theStruct->arr = dataForRabbit;

	// copy packet over
	packet[0] = theStruct->numBytes;
	for (int j = 0; j < theStruct->arr->Length; j++) {
		packet[j + 1] = theStruct->arr[j];
	}

	return packet;
}

/*
 * Passthrough non-guaranteed packet
 */
array<System::Byte> ^ AutopilotComport::getVCPacket10(unsigned char kestrelPacketType, array<System::Byte> ^ arr)
{
	int packetLength = 3 + arr->Length;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader ^theHeader = gcnew PacketHeader();
	theHeader->type = 10;
	theHeader->size = packetLength;

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket10 ^ theStruct = gcnew AutopilotVCPacket10();

	// copy data into struct
	theStruct->destAddr = planeAddress;	
	theStruct->kestrelPacketType = kestrelPacketType;
	theStruct->arr = arr;  ///TODO: fix array copying

	// copy packet over
	pin_ptr<__int16> data2 = &(theStruct->destAddr);
	dataPtr = (char *)data2;
	for (int j = 0; j < 3; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}
	for (int j = 0; j < theStruct->arr->Length; j++){
		packet[packetIndex] = theStruct->arr[j];
		packetIndex ++;
	}

	return packet;
}

/*
 * Packet Forwarding Setup
 * packetID = the kestrel packet id to forward, 0 means all packets will be forwarded
 * onOff = set to 1 to turn on forwarding, 0 turns off forwarding
 */
array<System::Byte> ^ AutopilotComport::getVCPacket20(unsigned char packetID, unsigned char onOff)
{
	int packetLength = 2;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader ^theHeader = gcnew PacketHeader();
	theHeader->type = 20;
	theHeader->size = packetLength;

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket20 ^ theStruct = gcnew AutopilotVCPacket20();

	// copy data into struct
	theStruct->packetID = packetID;	
	theStruct->onOff = onOff;	

	// copy packet over
	pin_ptr<unsigned char> data2 = &(theStruct->packetID);
	dataPtr = (char *)data2;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	return packet;
}

/*
 * Upload command list from VC to autopilot
 */
array<System::Byte> ^ AutopilotComport::getVCPacket30()
{
	int packetLength = 2;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader ^theHeader = gcnew PacketHeader();
	theHeader->type = 30;
	theHeader->size = packetLength;

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket30 ^ theStruct = gcnew AutopilotVCPacket30();

	// copy data into struct
	theStruct->destAddr = planeAddress;	

	// copy packet over
	pin_ptr<__int16> data2 = &(theStruct->destAddr);
	dataPtr = (char *)data2;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	return packet;
}

/*
 * Set Command List on VC
 */
array<System::Byte> ^  AutopilotComport::getVCPacket32(unsigned char commandType, float lat, float lon, float speed, float altitude, 
											unsigned __int16 gotoIndex, unsigned __int16 time, float radius, float flareSpeed, float flareAltitude, 
											float breakAltitude, float descentRate, float approachAltitude, float approachLongitude) 
{
	// 66 bytes total, including 8 bytes of unused at the end
	const int headerSize = 8;
	int packetLength = 59;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + headerSize);

	PacketHeader ^theHeader = gcnew PacketHeader();
	theHeader->type = 32;
	theHeader->size = packetLength;

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < headerSize; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	AutopilotVCPacket32 ^ theStruct = gcnew AutopilotVCPacket32();

	// copy data into struct
	theStruct->destAddr			 = planeAddress;	
	theStruct->commandType		 = commandType			;
	theStruct->lat				 = lat					;
	theStruct->lon				 = lon					;
	theStruct->speed			 = speed				;
	theStruct->altitude			 = altitude				;
	theStruct->gotoIndex		 = gotoIndex			;
	theStruct->time				 = time					;
	theStruct->radius			 = radius				;
	theStruct->flareSpeed		 = flareSpeed			;
	theStruct->flareAltitude	 = flareAltitude		;
	theStruct->breakAltitude	 = breakAltitude		;
	theStruct->descentRate		 = descentRate			;
	theStruct->approachAltitude	 = approachAltitude		;
	theStruct->approachLongitude = approachLongitude	;
	theStruct->unusedOne         = 0;
	theStruct->unusedTwo         = 0;




	// copy packet over
	pin_ptr<__int16> data2 = &(theStruct->destAddr);
	dataPtr = (char *)data2;
	for (int j = 0; j < packetLength; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}

	return packet;
}

/*
 * Request List of Agents
 */
array<System::Byte> ^ AutopilotComport::getVCPacket61()
{
	int packetLength = 0;
	array<System::Byte> ^ packet = gcnew array<System::Byte>(packetLength + 8);

	PacketHeader ^theHeader = gcnew PacketHeader();
	theHeader->type = 61;
	theHeader->size = packetLength;

	// copy packet over
	pin_ptr<__int32> data = &(theHeader->type);
	char * dataPtr = (char *)data;
	int packetIndex = 0;
	for (int j = 0; j < 8; j++) {
		packet[packetIndex] = dataPtr[j];
		packetIndex++;
	}
	
	return packet;
}

/*
 * extracts a 16 bit integer from a byte array
 */
__int16 AutopilotComport::getInt16FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 2){
		return 0;
	}

	__int16 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex; i < startIndex + 2; i++){
		* pointerToVal = arr[i];
		pointerToVal ++;
		//System::Diagnostics::Trace::WriteLine("AutopilotComport::getInt16FromBytes(): byte being converted: " + Convert::ToString(arr[i], 16));
	}

	return retVal;
}

/*
 * extracts an unsigned 16 bit integer from a byte array
 */
unsigned __int16 AutopilotComport::getUInt16FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 2){
		return 0;
	}

	unsigned __int16 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex; i < startIndex + 2; i++){
		* pointerToVal = arr[i];
		pointerToVal ++;
	}
	
	return retVal;
}

/*
 * extracts a 32 bit integer from a byte array
 */
__int32 AutopilotComport::getInt32FromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 4){
		return 0;
	}

	__int32 retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex; i < startIndex + 4; i++){
		* pointerToVal = arr[i];
		pointerToVal ++;
	}
	
	return retVal;
}

/*
 * extracts an 8 bit unsigned char from a byte array
 */
unsigned char AutopilotComport::getUCharFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 1){
		return 0;
	}
	return arr[startIndex];
}

/*
 * extracts an 8 bit char from a byte array
 */
char AutopilotComport::getCharFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 1){
		return 0;
	}
	return arr[startIndex];
}

/*
 * extracts a 32 bit float from a byte array
 */
float AutopilotComport::getFloatFromBytes(array<System::Byte> ^ arr, int startIndex)
{
	if(arr->Length < startIndex + 4){
		return 0;
	}

	float retVal;
	unsigned char * pointerToVal = (unsigned char *)(&retVal);
	for(int i = startIndex; i < startIndex + 4; i++){
		* pointerToVal = arr[i];
		pointerToVal ++;
	}
	
	return retVal;
}

//XXX
//TODO:
// change all non-unsigned get functions to use the float pointer method
// finish packet interpretation, packet 29 was where you left off
