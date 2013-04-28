#include "stdafx.h"

#include <comdef.h>

#include "AComportConstants.h"
#include "AComport.h"
#include "AComport_i.c"

#include "Comport.h"

#include <sstream>
#include <iostream>

Comport::Comport(void)
{
	//m_comport.Empty();
	m_comport = "COM4";
	m_baudrate = 57600;
	m_port = NULL;
}

Comport::~Comport(void)
{
}

bool 
Comport::init()
{
	// Make sure comport and baudrate have been set
	if( m_comport->Length == 0 || m_baudrate < 0 )
		return false;

	// If connected, do nothign
	if( m_port != NULL )
		return false;

	// const TCHAR * COMPORT = m_comport;
	const char * COMPORT = "COM4"; // temporary
	//const TCHAR *COMPORT = "COM4");
	enum { BAUDRATE = 57600 };

	void ** temp = NULL;

	CoCreateInstance(CLSID_ComPort, NULL, CLSCTX_INPROC_SERVER, IID_IComPort, temp );

	m_port = *(IComPort**)temp;

	///////////Port Settings////////////////////////////////////////////////
	m_port->put_Device ( _bstr_t ( COMPORT ) );
	// m_port->put_BaudRate ( m_baudrate );
	m_port->put_BaudRate ( BAUDRATE );
	HRESULT hr = m_port->Open();
	
	if ( 0 == hr )
	{
		return true;
	}
	else{
		m_port = NULL;
		return false;
	}
}

bool 
Comport::disc()
{
	// If disconnected, do nothing
	if( m_port == NULL )
		return false;

	// actually disconnect here

	m_port = NULL;

	return true;
}

bool 
Comport::setPort(String ^ port)
{
	// If connected, do nothing
	if( m_port != NULL )
		return false;

	m_comport = port;

	return true;
}

bool 
Comport::setBaudRate(int baudrate)
{
	// If connected, do nothing
	if( m_port != NULL )
		return false;

	m_baudrate = baudrate;

	return true;
}

bool
Comport::readBytes( array<String ^> ^ % data )
{
	// check if initialized
	if( m_port == NULL )
		return false;

	BSTR byteAsBstr = NULL;

	String ^ messageOut = "";

	bool bReceived = FALSE;
	INT m_dwBytesWritten = 0;
	INT byteNum = 0, dataType = 0, startIndex=-1;
	bool foundStartByte = FALSE;

	// READ IN DATA
	m_port->ReadBytes ( &byteAsBstr );
	// GET NUM BYTES IN DATA
	INT nSourceBytes = ::SysStringLen ( byteAsBstr );
	
	// ONLY CONTINUE IF STREAM HAS BEEN READ
	if ( nSourceBytes )
	{
		bReceived = TRUE;		

		// CYCLE THROUGH BYTES RECD
		for ( int n = 0; n < nSourceBytes ; n++ )
		{			
			// MUST FIND START BYTE TO BEGIN PARSING DATA
			if(!foundStartByte){
				if(byteAsBstr [ n ]==0xff){ // looking for start byte 0xFF
					startIndex = n;
					foundStartByte=TRUE;
				}
				else
					continue;
			}		

			// PARSE DATA
			String ^ ret = parseRecdBytes( dataType, byteAsBstr, startIndex );
			data[dataType] = ret;
			//data->SetAt( dataType, ret );

			// INCR/RESET dataType
			if(dataType==16){
				// DONE PARSING & FORMATTING DATA
				// PRINT DATA TO GUI
				//out = (LPCTSTR) messageOut;
				//m_recv_text.SetWindowTextW( out );				
				// UpdateData ( FALSE );

				// RESET VARS
				dataType=0;
				messageOut = "";
			}
			else
				dataType++;

		}	

	}
	
	if( nSourceBytes <= 0 )
	{
		
		if ( bReceived )
		{
			//nTimeout++;

			//if ( nTimeout > 20 )
			//{
				m_port->Close ();
				// UpdateData ( FALSE );
				bReceived	= FALSE;
				//nTimeout	= 0;
				// KillTimer ( 2 );
			//}

		}
	}

	return true;
}
/*
bool
Comport::sendData()
{

}
*/
String ^ 
Comport::parseRecdBytes(int dataType, BSTR data, int startIndex){

	String ^ str1 = "";
	char arry [30];
	int curByte=0, byteSum=0;

	switch(dataType){

		/* ** SYSTEM ** */
		// START BYTE
		case 0:
			return "";

		// ERROR CODE 
		case 1:
				   
			  if(data[startIndex + 1]==0x0)
				  return "0";
			  else if(data[startIndex + 1]==0x1)
				  return "1";
			  else if(data[startIndex + 1]==0x2)
				  return "2";
			  else if(data[startIndex + 1]==0x3)
				  return "3";
			  else
				  return "unknown error code";

		// MODE
		case 2:
			  if(data[startIndex + 2]==0x0)
				  return "0";
			  else if(data[startIndex + 2]==0x1)
				  return "1";
			  else if(data[startIndex + 2]==0x2)
				  return "2";			  
			  else
				  return "unknown code";

	    /* ** AIRCRAFT */
	    // ALTITUDE
		case 3:

			// FORMAT DATA
			for(int i=0;i<4;i++){
				curByte = (int) data[startIndex + 3 + i];
				byteSum = (byteSum + curByte);
				if(i<3)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1; 

		// GPS LAT
		case 4:

			// FORMAT DATA
			for(int i=0;i<4;i++){
				curByte = (int) data[startIndex + 7 + i];
				byteSum = (byteSum + curByte);
				if(i<3)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// GPS LON
		case 5:

			// FORMAT DATA
			for(int i=0;i<4;i++){
				curByte = (int) data[startIndex + 11 + i];
				byteSum = (byteSum + curByte);
				if(i<3)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// HEADING
		case 6:

			// FORMAT DATA
			for(int i=0;i<2;i++){
				curByte = (int) data[startIndex + 15 + i];
				byteSum = (byteSum + curByte);
				if(i<1)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// PITCH ANGLE
		case 7:

			// FORMAT DATA
			for(int i=0;i<2;i++){
				curByte = (int) data[startIndex + 17 + i];
				byteSum = (byteSum + curByte);
				if(i<1)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// ROLL ANGLE
		case 8:

			// FORMAT DATA
			for(int i=0;i<2;i++){
				curByte = (int) data[startIndex + 19 + i];
				byteSum = (byteSum + curByte);
				if(i<1)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// PAN ANGLE
		case 9:

			// FORMAT DATA
			for(int i=0;i<2;i++){
				curByte = (int) data[startIndex + 21 + i];
				byteSum = (byteSum + curByte);
				if(i<1)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// TILT ANGLE
		case 10:

			// FORMAT DATA
			for(int i=0;i<2;i++){
				curByte = (int) data[startIndex + 23 + i];
				byteSum = (byteSum + curByte);
				if(i<1)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// ZOOM POSITION
		case 11:

			// FORMAT DATA
			byteSum = (int) data[startIndex + 25];
		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// FOV LAT
		case 12:

			// FORMAT DATA
			for(int i=0;i<4;i++){
				curByte = (int) data[startIndex + 26 + i];
				byteSum = (byteSum + curByte);
				if(i<3)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// FOV LON
		case 13:

			// FORMAT DATA
			for(int i=0;i<4;i++){
				curByte = (int) data[startIndex + 30 + i];
				byteSum = (byteSum + curByte);
				if(i<3)
					byteSum = byteSum << 8;
			}		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// CHECKSUM 1
		case 14:

			// FORMAT DATA
			byteSum = (int) data[startIndex + 34];
		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// CHECKSUM 2
		case 15:

			// FORMAT DATA
			byteSum = (int) data[startIndex + 35];
		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		// END BYTE
		case 16:

			// FORMAT DATA
			byteSum = (int) data[startIndex + 36];
		
			_itoa_s(byteSum,arry,10);
			str1 = gcnew String(arry);	
			return str1;

		default:
			break;
	}

	return "ERROR - cant parse byte";
}