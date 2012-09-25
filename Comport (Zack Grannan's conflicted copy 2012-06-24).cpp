#include "StdAfx.h"
#include "Form1.h"
#include "Comport.h"
#include <time.h>
#include "SimHandler.h"
#include "ComportHandler.h"

/*
 * Serial use bytes
 * We use the same method as defined on page 15 of the Kestrel
 * communications manual
 */
 
#define START_BYTE 		0xFF
#define END_BYTE		0xFE
#define SPECIAL_BYTE 	0xFD
#define FUTURE_BYTE		0xCC

#define MAP_TO_START	0x02
#define MAP_TO_END		0x01
#define MAP_TO_SPECIAL	0x00
#define MAP_TO_FUTURE	0x03

#define BAUD_RATE		57600

// Comport read timeout in miliseconds
#define TIMEOUT_MS		1000

// Comport update frequency (in miliseconds)
#define DEFAULT_UPDATE_FREQ		250

// buffer size in bytes to read in to
#define BUFFER_SIZE		512

#define PACKET_LOSS		YELLOW_STATUS
#define PACKET_RECEIVED	GREEN_STATUS

using namespace Communications;
using namespace System::Threading;

//static ComportUpstream * lastPacket;


ComportUpstream::ComportUpstream()
{
	gps_lat_gimbal_x.i = 0;
	gps_lon_gimbal_y.i = 0;
	gps_alt.i = 0;
	camera_zoom = 0xAAAAAAAA;
	update_type = 0;
}

Comport::Comport(Object ^ theParent)
{
	_serialPort = gcnew SerialPort();
	_serialPort->ReadTimeout = TIMEOUT_MS;
	_serialPort->BaudRate = BAUD_RATE;

	portNames = SerialPort::GetPortNames();

	parent = theParent;

	updateFrequency = DEFAULT_UPDATE_FREQ;

	if (theParent != nullptr)
		comHandlerDelegate = gcnew comportHandlerUpdateDelegate(((ComportHandler ^)theParent), &ComportHandler::receiveData );
}

Comport::~Comport(void)
{
	disconnect();
}

array<String ^> ^ Comport::getPortNames()
{
	return portNames;
}

bool Comport::connect()
{
	bool success = false;

	if (_serialPort->IsOpen)
		_serialPort->Close();

	if( !_serialPort->IsOpen ) 
	{
		// try ... catch, and check to see if comport exists first
		try 
		{
			_serialPort->Open();
			success = true;
		}
		catch( Exception ^ )
		{
			System::Diagnostics::Trace::WriteLine("Exception ... could not connect");
			success = false;
		}
	} 
	else 
	{
		System::Diagnostics::Trace::WriteLine("Serial port already open");
	}

	return success;

}

bool Comport::disconnect(void)
{
	bool success = false;
	if( _serialPort->IsOpen ) 
	{
		_serialPort->Close();
		success = true;
	}

	return success;
}

bool Comport::setPortName( String ^ portName )
{
	bool success = false;
	if( _serialPort->IsOpen )
	{

	}
	else
	{
		_serialPort->PortName = portName;
		success = true;
	}
	return success;
}

bool Comport::setBaudRate( int baudRate )
{
	bool success = false;
	if( _serialPort->IsOpen )
	{

	}
	else
	{
		_serialPort->BaudRate = baudRate;
		success = true;
	}
	return success;
}

int Comport::decodeByte(void)
{
	int dataIn = -1;

	try
	{
		dataIn = _serialPort->ReadByte();

		if( dataIn >= 0 )
		{
			// Correct the byte here			
			switch( Convert::ToByte( dataIn ) )
			{
			case MAP_TO_START:
				dataIn = START_BYTE;
				break;
			case MAP_TO_END:
				dataIn = END_BYTE;
				break;
			case MAP_TO_SPECIAL:
				dataIn = SPECIAL_BYTE;
				break;
			case MAP_TO_FUTURE:
				dataIn = FUTURE_BYTE;
				break;
			default:
				break;
			}
		}
	}
	catch( Exception ^ )
	{
		dataIn = -1;
		System::Diagnostics::Trace::WriteLine("catch in comport");
	}

	return dataIn;
}

unsigned char Comport::encodeByte( unsigned char data )
{
	// FF = 0
	// FE = 1
	// FD = 2
	// CC = 3
	switch( data )
	{
		case 0xFF: return 0x0;
		case 0xFE: return 0x1;
		case 0xFD: return 0x2;
		case 0xCC: return 0x3;
		default : return 0x4; // should never reach
	}
}

void Comport::readThread(void)
{
	System::Diagnostics::Trace::WriteLine("Comport::readThread() began");
	int lastResult = GREEN_STATUS;
	try {
		while( true )
		{

			// only read data if we are connected
			if (isConnected()) {
				if (lastResult != readData()) {

					// alert ui about change in status
					if (lastResult == GREEN_STATUS) lastResult = YELLOW_STATUS;
					else if (lastResult == YELLOW_STATUS) lastResult = GREEN_STATUS;

					((ComportHandler ^)parent)->updateComportStatus(lastResult);

				}
					

			}

			Thread::Sleep( updateFrequency );
		}
	}
	catch (Exception ^) 
	{
		System::Diagnostics::Trace::WriteLine("Comport::readThread in Comport ending.");
	}
}

int Comport::readData(void)
{
	const int NUM_ELEMENTS = 11;
	int dataIn = -1;
	int bufLen = 0;
	byte tempByte;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	//ComportDownstream * packet = new ComportDownstream();
	
	//System::Diagnostics::Trace::WriteLine("begin read");
	try
	{
		// Read until we get a start byte
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				System::Diagnostics::Trace::WriteLine("Comprt::readData(): bad byte");
				return PACKET_LOSS;
			}
			else if( tempByte == START_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}
		}
		//System::Diagnostics::Trace::WriteLine("found beginning");

		// Read in the message string, decoding as necessary
		while( true )
		{
			//System::Diagnostics::Trace::WriteLine("start iteration: " + bufLen);
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			//System::Diagnostics::Trace::WriteLine("look for end byte");

			// error / end of data check
			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				System::Diagnostics::Trace::WriteLine("Comport::readData(): bad byte");
				return PACKET_LOSS;
			}
			else if( tempByte == END_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				//System::Diagnostics::Trace::WriteLine("FOUND END BYTE");
				break;
			}
			
			//System::Diagnostics::Trace::WriteLine("look for special byte 0x" + Convert::ToString(tempByte, 16));

			// process actual byte
			// if this is a special byte, decode it
			if( tempByte == SPECIAL_BYTE )
			{
				dataIn = decodeByte();

				if( dataIn < 0 )
				{
					_serialPort->DiscardInBuffer();
					System::Diagnostics::Trace::WriteLine("Comprt::readData(): bad byte");
					return PACKET_LOSS;
				}
				else
					tempByte = Convert::ToByte( dataIn );

				
			}
			//System::Diagnostics::Trace::WriteLine("save byte (" + bufLen + ") = 0x" + Convert::ToString(tempByte, 16) + " vs 0x" + Convert::ToString(END_BYTE, 16) + " " + tempByte + " vs " + 0xFE);
			//if (tempByte == 0xFE)
			//	System::Diagnostics::Trace::WriteLine("fuck you");

			buffer[bufLen] = tempByte;
			bufLen++;


		}
		
		//System::Diagnostics::Trace::WriteLine("Calc checksum");
		__int16 checksum = calculateChecksum( buffer, bufLen - 3);

		//System::Diagnostics::Trace::WriteLine("Checksum: " + String::Format("{0:X}", checksum));

		if (!( ((unsigned char *)&checksum)[1] == buffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == buffer[bufLen - 2] ))
		{
			//System::Diagnostics::Trace::WriteLine("Checksum: FAILED");
			_serialPort->DiscardInBuffer();
			System::Diagnostics::Trace::WriteLine("Comport::readData(): bad checksum. bufLen: " + (bufLen - 4));
			System::Diagnostics::Trace::WriteLine("Comport::readData(): bad checksum. packettype:" + Convert::ToString(buffer[1], 10));
			System::Diagnostics::Trace::WriteLine("Comport::readData(): bad checksum. mine: " + Convert::ToString(((unsigned char *)&checksum)[1], 16) + " " + Convert::ToString(((unsigned char *)&checksum)[0], 16) + " his:" +
																								Convert::ToString(buffer[bufLen - 3], 16) + " " + Convert::ToString(buffer[bufLen - 2], 16));
			String ^ bufferString = "0x" + Convert::ToString(buffer[0], 16);
			for (int i = 1; i < bufLen; i++) {
				bufferString = bufferString + " 0x" + Convert::ToString(buffer[i], 16);
			}
		
			System::Diagnostics::Trace::WriteLine("Packet ^^. bufLen:" + bufLen + " buffer:" + bufferString + ".\n");
			
			return PACKET_RECEIVED;
 		}

		/*ComportDownstream * packet = new ComportDownstream();

		char * dataPtr = (char *)packet;

		for (int j = 0; j < NUM_ELEMENTS; j++) {
			for (int i = 3, k = 0; i > -1; i--, k++) {
				int letterPosition = j*4 + k;
				dataPtr[letterPosition] = buffer[j*4 + i + 1];

			}			
		}*/

		//save time that packet came in

		//write to files
		//packet->gps_lat;

		//read file

		/*packet->error_code = buffer[bufLen - 4];

		if (false && packet->error_code) {
			System::Diagnostics::Trace::WriteLine("autopilot timeout \t     " + (bool)(packet->error_code & 0x01));
			System::Diagnostics::Trace::WriteLine("autopilot invalid packet " + (bool)(packet->error_code & 0x02));
			System::Diagnostics::Trace::WriteLine("ground station timeout   " + (bool)(packet->error_code & 0x04));
			System::Diagnostics::Trace::WriteLine("uplink timeout           " + (bool)(packet->error_code & 0x08));
			System::Diagnostics::Trace::WriteLine("uplink invalid packet    " + (bool)(packet->error_code & 0x10));
			System::Diagnostics::Trace::WriteLine("gimbal timeout           " + (bool)(packet->error_code & 0x20));
			System::Diagnostics::Trace::WriteLine("gimbal invalid mode      " + (bool)(packet->error_code & 0x40));
			System::Diagnostics::Trace::WriteLine("reserved                 " + (bool)(packet->error_code & 0x80));
			System::Diagnostics::Trace::WriteLine("\n");
		}
		*/
		// Verify checksum
		

		
	//	((Simulator::SimHandler ^)theSimHandler)->writeTelemetry(packet);

		// Callback to Form1 (stays in this offshoot thread)
		//System::Diagnostics::Trace::WriteLine("about to copy to outbuffer");

		// copy packet into new packet
		array<System::Byte> ^ outBuffer = gcnew array<System::Byte>(bufLen - 4); // ignore ff, checksum*2, fe
		int j = 0;
		for (int i = 1; i < bufLen - 3; i++) { // ignore last three bytes (checksum checksum fe)
			outBuffer[j] = buffer[i];
			j++;
		}
		//System::Diagnostics::Trace::WriteLine("bufLen: " + bufLen);
		//System::Diagnostics::Trace::WriteLine("send to delegate");
		// send data to delegate
		//System::Diagnostics::Trace::WriteLine("pass it up");
		try {
			comHandlerDelegate( outBuffer );
		}
		catch( ThreadAbortException ^ theException)
		{
			throw theException; // rethrow if its a threadAbortException
		}
		catch( Exception ^ e)
		{
			System::Diagnostics::Trace::WriteLine( "Exception in comHandlerDelegate: " + e);

		}
		//JK..WRITE HERE!!

		//gcroot<ComportDownstream *> bla(packet);
		//((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->comportUpdateDelegate, bla );
		// In this case packet is deleted by the main thread

		// clear the rest of the buffer
		_serialPort->DiscardInBuffer();
		return PACKET_RECEIVED;
	}
	catch( ThreadAbortException ^ theException)
	{
		throw theException; // rethrow if its a threadAbortException
	}
	catch( TimeoutException ^ )
	{
		

		//delete packet;
		// TODO object disposed exception
		//((Skynet::Form1 ^)parent)->Invoke( ((Skynet::Form1 ^)parent)->comportErrorDelegate );
		/*String ^ bufferString = "0x" + Convert::ToString(buffer[0], 16);
		for (int i = 1; i < buffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(buffer[i], 16);
		}
		
		System::Diagnostics::Trace::WriteLine("Catch in comport. bufLen:" + bufLen + " buffer:" + bufferString);*/
		//comNoDataDelegate();
	}
	catch( System::IO::IOException ^ e)
	{
		System::Diagnostics::Trace::WriteLine("IOException in comport");
		e = nullptr;
	}
	catch( Exception ^ e )
	{
		System::Diagnostics::Trace::WriteLine("WARNING: Unexpected catch in comport: " + e);
		String ^ bufferString = "0x" + Convert::ToString(buffer[0], 16);
		for (int i = 1; i < buffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(buffer[i], 16);
		}
		System::Diagnostics::Trace::WriteLine("Catch in comport. bufLen:" + bufLen + " buffer:" + bufferString);
	}

	return PACKET_RECEIVED;
}

array<System::Byte> ^ Comport::stripAndChecksum(array<System::Byte> ^ inBuffer)
{
	int bufLen = inBuffer->Length;


	__int16 checksum = calculateChecksum( inBuffer, bufLen - 3);

	//System::Diagnostics::Trace::WriteLine("Checksum: " + String::Format("{0:X}", checksum));

	if (!( ((unsigned char *)&checksum)[1] == inBuffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == inBuffer[bufLen - 2] ))
	{
		//System::Diagnostics::Trace::WriteLine("Comport::stripAndChecksum(): bad checksum. bufLen: " + (bufLen - 4));
		//System::Diagnostics::Trace::WriteLine("Comport::stripAndChecksum(): bad checksum. mine: " + Convert::ToString(((unsigned char *)&checksum)[1], 16) + Convert::ToString(((unsigned char *)&checksum)[0], 16) + " his:" +
		//																					Convert::ToString(inBuffer[bufLen - 3], 16) + Convert::ToString(inBuffer[bufLen - 2], 16));
			
		String ^ bufferString = "";
		for (int i = 0; i < inBuffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(inBuffer[i], 16);
		}
		//System::Diagnostics::Trace::WriteLine("Comport::stripAndChecksum():" + bufLen + " buffer:" + bufferString);
		return nullptr;
 	}


	// copy packet into new packet
	array<System::Byte> ^ outBuffer = gcnew array<System::Byte>(bufLen - 4);
	for (int i = 1; i < bufLen - 3; i++) { // ignore ff, checksum, fe
		outBuffer[i - 1] = inBuffer[i];
	}

	return outBuffer;
}

array<System::Byte> ^ Comport::decodeData(array<System::Byte> ^ inBuffer)
{
	array<System::Byte> ^ buffer = gcnew array<System::Byte>(inBuffer->Length);
	int bufLen = 0;
	int packetIndex = 0;
	
	byte tempByte;
	byte tempByte2;

	// Read until we get a start byte
	while( true )
	{
		if (packetIndex >= inBuffer->Length) {
			System::Diagnostics::Trace::WriteLine("Comport::decodeData(): didn't find start byte: " + packetIndex);
			return nullptr;
		}

		tempByte = inBuffer[packetIndex];
		packetIndex++;

		if( tempByte == START_BYTE )
		{
			buffer[bufLen] = tempByte;
			bufLen++;
			//System::Diagnostics::Trace::WriteLine("Comport::decodeData(): start packetIndex: " + packetIndex);
			break;
		}

	}

	// Read in the message string, decoding as necessary
	while( true )
	{
		if (packetIndex >= inBuffer->Length) {
			System::Diagnostics::Trace::WriteLine("Comport::decodeData(): did not find end byte: " + packetIndex + " last byte: " + Convert::ToString(tempByte, 16));
			return nullptr;
		}

		tempByte = inBuffer[packetIndex];

		if( packetIndex + 1 == inBuffer->Length ) // tempByte == END_BYTE )
		{
			buffer[bufLen] = tempByte;
			bufLen++;
		//	System::Diagnostics::Trace::WriteLine("Comport::decodeData(): end packetIndex: " + packetIndex);
			break;
		}

		// process actual byte
		// if this is a special byte, decode it
		if( tempByte == SPECIAL_BYTE )
		{
			
			packetIndex++;
			tempByte2 = inBuffer[packetIndex];

			// Correct the byte here			
			switch( tempByte2 )
			{
				case MAP_TO_START:
					tempByte = START_BYTE;
					break;
				case MAP_TO_END:
					tempByte = END_BYTE;
					break;
				case MAP_TO_SPECIAL:
					tempByte = SPECIAL_BYTE;
					break;
				case MAP_TO_FUTURE:
					tempByte = FUTURE_BYTE;
					break;
				default:
					break;
			}
		}

		buffer[bufLen] = tempByte;
		bufLen++;
		
		packetIndex++;

	}


	__int16 checksum = calculateChecksum( buffer, bufLen - 3);

	//System::Diagnostics::Trace::WriteLine("Checksum: " + String::Format("{0:X}", checksum));

	if (!( ((unsigned char *)&checksum)[1] == buffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == buffer[bufLen - 2] ))
	{
		System::Diagnostics::Trace::WriteLine("Comport::decodeData(): bad checksum. bufLen: " + (bufLen - 4));
		System::Diagnostics::Trace::WriteLine("Comport::decodeData(): bad checksum. mine: " + Convert::ToString(((unsigned char *)&checksum)[1], 16) + Convert::ToString(((unsigned char *)&checksum)[0], 16) + " his:" +
																							Convert::ToString(buffer[bufLen - 3], 16) + Convert::ToString(buffer[bufLen - 2], 16));
			
		String ^ bufferString = "0x" + Convert::ToString(buffer[0], 16);
		for (int i = 1; i < buffer->Length; i++) {
			bufferString = bufferString + " 0x" + Convert::ToString(buffer[i], 16);
		}
		System::Diagnostics::Trace::WriteLine("Comport::decodeData():" + bufLen + " buffer:" + bufferString);
		return nullptr;
 	}


	// copy packet into new packet
	array<System::Byte> ^ outBuffer = gcnew array<System::Byte>(bufLen - 4);
	for (int i = 1; i < bufLen - 3; i++) { // ignore ff, checksum, fe
		outBuffer[i - 1] = buffer[i];
	}

	return outBuffer;
}

__int16 Comport::calculateChecksum( array<System::Byte> ^data, int packetSize ) 
{
	const int startIndex = 1;
	int endIndex = packetSize;
	int realPacketSize = packetSize - 1;
	
	//__int16 CheckValue = 0; 
	unsigned char checkHigh = 0;
	unsigned char checkLow = 0;

	for (int i = startIndex; i < endIndex - 1; i+= 2)
	{
		checkHigh ^= data[i];
		checkLow ^= data[i + 1];
		//CheckValue ^= data[i];
		//CheckValue ^= data[i+1]<<8;
	}
	
	if ((realPacketSize)%2 == 1) // odd
		checkHigh ^= data[packetSize - 1];

	__int16 retval;
	unsigned char *ptr = (unsigned char *)&retval;
	*ptr = checkLow; ptr++;
	*ptr = checkHigh;

	return retval;

}

array<System::Byte> ^ Comport::readRawData( int timeout ) {

	// set new timeout
	//int oldtimeout = _serialPort->ReadTimeout;
	//_serialPort->ReadTimeout = timeout;



	const int NUM_ELEMENTS = 11;
	int dataIn = -1;
	int bufLen = 0;
	byte tempByte;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);

	try
	{
		// Read until we get a start byte
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return nullptr;
			}
			else if( tempByte == START_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}
		}

		// Read in the message string, decoding as necessary
		while( true )
		{
			dataIn = _serialPort->ReadByte();
			tempByte = Convert::ToByte( dataIn );

			// error / end of data check
			if( dataIn < 0 )
			{
				_serialPort->DiscardInBuffer();
				return nullptr;
			}
			else if( tempByte == END_BYTE )
			{
				buffer[bufLen] = tempByte;
				bufLen++;
				break;
			}

			// process actual byte
			// if this is a special byte, decode it
			if( tempByte == SPECIAL_BYTE )
			{
				dataIn = decodeByte();

				if( dataIn < 0 )
				{
					_serialPort->DiscardInBuffer();
					return nullptr;
				}
				else
					tempByte = Convert::ToByte( dataIn );
			}

			buffer[bufLen] = tempByte;
			bufLen++;


		}

		
		// calculate checksum
		__int16 checksum = calculateChecksum( buffer, bufLen - 3);

		/*if (!( ((unsigned char *)&checksum)[1] == buffer[bufLen - 3] && ((unsigned char *)&checksum)[0] == buffer[bufLen - 2] ))
		{
			//System::Diagnostics::Trace::WriteLine("Checksum: FAILED");
			_serialPort->DiscardInBuffer();
			return;
 		}*/

		// Verify checksum
		


		// clear the rest of the buffer
		_serialPort->DiscardInBuffer();
	}
	catch( Exception ^ )
	{
	}




	// restore old timeout
	//_serialPort->ReadTimeout = oldtimeout;


	
	array<System::Byte> ^ retbuffer = gcnew array<System::Byte>(bufLen);

	for (int i = 0; i < bufLen; i++)
		retbuffer[i] = buffer[i];

	return retbuffer;
}

void Comport::writeEncodedData( array<System::Byte> ^ inBuffer ) 
{
	if (!isConnected())
		return;


	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	array<System::Byte> ^ bufferNoSpecial = gcnew array<System::Byte>(BUFFER_SIZE);
	int bufLen = 0;
	int bufNoSpecialLen = 0;
	char outByte;

	try
	{
		// start byte
		buffer[bufLen] = START_BYTE;
		bufferNoSpecial[bufNoSpecialLen] = START_BYTE;
		bufNoSpecialLen++;
		bufLen++;

		
		// encode packet
		for (int j = 0; j < inBuffer->Length; j ++) {
			outByte = inBuffer[j];

			bufferNoSpecial[bufNoSpecialLen] = outByte;
			bufNoSpecialLen++;

			if (isSpecialByte(outByte)) {
				buffer[bufLen] = SPECIAL_BYTE;
				bufLen++;
				buffer[bufLen] = encodeByte(outByte);
				bufLen++;
			} else {
				buffer[bufLen] = outByte;
				bufLen++;
			}
		}		


		// calculate checksum
		__int16 checksum = calculateChecksum( bufferNoSpecial, bufNoSpecialLen );
		
		outByte = ((char *)&checksum)[1];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		outByte = ((char *)&checksum)[0];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		// end byte
		buffer[bufLen] = END_BYTE;
		bufLen++;
		

		// send it
		_serialPort->Write( buffer, 0, bufLen );		
	}
	catch( Exception ^ )
	{
		// no valid data, no big deal
		System::Diagnostics::Trace::WriteLine("catch in Comport::writeEncodedData( array<System::Byte> ^ buffer ) ");

	}

}


void Comport::writeRawData( array<System::Byte> ^ buffer )
{
	if (!isConnected())
		return;

	
	System::Diagnostics::Trace::WriteLine( "Comport: writeRawData" );
	_serialPort->Write( buffer, 0, buffer->Length );	
}


// SEND IN BIG-ENDIAN (x86 stores in little-Endian)
void Comport::writeData( ComportUpstream * data )
{
	//System::Diagnostics::Trace::WriteLine("DATA TO WRITE TO COMPORT : ");
	//for( int i=0; i<21; i++){
	//	System::Diagnostics::Trace::WriteLine(data[i] + " ");
	//}
	//System::Diagnostics::Trace::WriteLine("\n");
	
	//lastPacket

	if (!isConnected())
		return;

	array<System::Byte> ^ buffer = gcnew array<System::Byte>(BUFFER_SIZE);
	array<System::Byte> ^ bufferNoSpecial = gcnew array<System::Byte>(BUFFER_SIZE);
	int bufLen = 0;
	int bufNoSpecialLen = 0;
	char outByte;

	try
	{
		// start byte
		buffer[bufLen] = START_BYTE;
		bufferNoSpecial[bufNoSpecialLen] = START_BYTE;
		bufNoSpecialLen++;
		bufLen++;

		// encode packet 
		//outByte = &(char *)data[0];
		
		char * thePointer = (char *)&data->gps_lat_gimbal_x;
		for (int j = 0; j < 4; j ++) {
			for (int i = 3; i >= 0; i --) {
				outByte = thePointer[i + j*4];

				bufferNoSpecial[bufNoSpecialLen] = outByte;
				bufNoSpecialLen++;

				if (isSpecialByte(outByte)) {
					buffer[bufLen] = SPECIAL_BYTE;
					bufLen++;
					buffer[bufLen] = encodeByte(outByte);
					bufLen++;
				} else {
					buffer[bufLen] = outByte;
					bufLen++;
				}
			}
		}		
		
		// Handle non word sized data to send
		outByte = data->update_type;

		bufferNoSpecial[bufNoSpecialLen] = outByte;
		bufNoSpecialLen++;

		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}		

		// calculate checksum
		__int16 checksum = calculateChecksum( bufferNoSpecial, bufNoSpecialLen );
		
		outByte = ((char *)&checksum)[1];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}

		outByte = ((char *)&checksum)[0];
		if (isSpecialByte(outByte)) {
			buffer[bufLen] = SPECIAL_BYTE;
			bufLen++;
			buffer[bufLen] = encodeByte(outByte);
			bufLen++;
		} else {
			buffer[bufLen] = outByte;
			bufLen++;
		}
		
		/*
		buffer[bufLen] = data->update_type;
		bufLen++;

		buffer[bufLen] = 0x12;
		bufLen++;

		buffer[bufLen] = 0x13;
		bufLen++;
		*/

		// end byte
		buffer[bufLen] = END_BYTE;
		bufLen++;
		
		//for(int i=0;i<bufNoSpecialLen;i++){
		//	System::Diagnostics::Trace::WriteLine("predec " + i + " : " + String::Format("{0:X}", bufferNoSpecial[i]));
		//}

		//System::Diagnostics::Trace::WriteLine("\nchecksum : " + String::Format("{0:X}", checksum) + "\n");

		//for(int i=0;i<bufLen;i++){
		//	System::Diagnostics::Trace::WriteLine("Buff " + i + " : " + String::Format("{0:X}", buffer[i]) + "\n");
		//}
		

		// send it
		_serialPort->Write( buffer, 0, bufLen );		
	}
	catch( Exception ^ )
	{
		// no valid data, no big deal
		//System::Diagnostics::Trace::WriteLine("catch in comport");

	}

	// Delete input data
	//delete data;
}

bool Comport::isSpecialByte( unsigned char data )
{
	/*System::Diagnostics::Trace::Write(" ");
	if ((data ^ 0xff) == 0)
		System::Diagnostics::Trace::Write(" ");
	else
		System::Diagnostics::Trace::Write(" ");

	if ((data ^ 0xfe) == 0)
		System::Diagnostics::Trace::Write(" ");
	else
		System::Diagnostics::Trace::Write(" ");*/
	
	return (data == START_BYTE || data == END_BYTE || data == SPECIAL_BYTE || data == FUTURE_BYTE );
}

float
Communications::decodeZoomFloat( __int32 zoomRaw )
{
	float zoom = 1;

	switch( zoomRaw )
	{
		case 0x00000000:
			zoom = 1.0f;
			break;
		case 0x00080000:
			zoom = 1.2f;
			break;
		case 0x01000000:
			zoom = 1.5f;
			break;
		case 0x01080000:
			zoom = 1.9f;
			break;
		case 0x02000000:
			zoom = 2.5f;
			break;
		case 0x02080000:
			zoom = 3.4f;
			break;
		case 0x03000000:
			zoom = 4.8f;
			break;
		case 0x03080000:
			zoom = 6.8f;
			break;
		case 0x04000000:
			zoom = 10.1f;
			break;
		default:
			zoom = -1.0f;
			break;
	}

	return zoom;
}

int
Communications::decodeZoomInt( __int32 zoomRaw )
{
	int zoom = 1;

	switch( zoomRaw )
	{
		case 0x00000000:
			zoom = 1;
			break;
		case 0x00080000:
			zoom = 2;
			break;
		case 0x01000000:
			zoom = 3;
			break;
		case 0x01080000:
			zoom = 4;
			break;
		case 0x02000000:
			zoom = 5;
			break;
		case 0x02080000:
			zoom = 6;
			break;
		case 0x03000000:
			zoom = 7;
			break;
		case 0x03080000:
			zoom = 8;
			break;
		case 0x04000000:
			zoom = 9;
			break;
		default:
			zoom = -1;
			break;
	}

	return zoom;
}