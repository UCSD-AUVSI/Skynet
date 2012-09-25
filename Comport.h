#pragma once
#include <iostream>
#include <fstream>

using namespace std;
using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Communications
{
	union intfloat
	{
		__int32 i; 
		float f;
	};
	
	struct ComportUpstream
	{
		ComportUpstream();

		intfloat gps_lat_gimbal_x;
		intfloat gps_lon_gimbal_y;
		intfloat gps_alt;
		__int32 camera_zoom;
		char update_type;
	};

	struct ComportDownstream
	{
		__int32 time_offset;
		float gps_lat;
		float gps_lon;


		float gps_alt;
		float airplane_roll;
		float airplane_pitch;
		float airplane_heading;
		__int32 gimbal_roll;
		__int32 gimbal_pitch;
		__int32 camera_zoom;
		char error_code;
	};

	float decodeZoomFloat( __int32 zoomRaw );
	int decodeZoomInt( __int32 zoomRaw );

	delegate void comportHandlerUpdateDelegate( array<System::Byte> ^ inBuffer );
	delegate void comportNoDataDelegate( void );

	public ref class Comport
	{
	public:
		Comport(Object ^ parent);
		~Comport(void);

		bool connect();
		bool disconnect();

		array<String ^> ^ getPortNames();

		bool setPortName( String ^ portName );
		bool setBaudRate( int baudRate );

		void readThread(void);

		int readData(void);
		array<System::Byte> ^ readRawData(int timeout);

		array<System::Byte> ^ decodeData(array<System::Byte> ^ inBuffer);
		array<System::Byte> ^ stripAndChecksum(array<System::Byte> ^ inBuffer);

		void writeData( ComportUpstream * data );
		void writeRawData( array<System::Byte> ^ buffer );
		void writeEncodedData( array<System::Byte> ^ buffer );


		bool isConnected(void) { return _serialPort->IsOpen; }
		bool kill;
		int updateFrequency;
	private:
		int decodeByte(void);
		unsigned char encodeByte( unsigned char data );
		inline bool isSpecialByte( unsigned char data );
		__int16 calculateChecksum( array<System::Byte> ^data, int packetSize );
		
		comportHandlerUpdateDelegate ^ comHandlerDelegate;
		//comportUpdateDelegate ^ comDelegate;
		comportNoDataDelegate ^ comNoDataDelegate;

	protected:
		SerialPort ^ _serialPort;
		array<String ^> ^ data;
		array<String ^> ^ portNames;
		Object ^ parent;
		
	};
}
