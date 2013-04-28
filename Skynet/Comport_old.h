#pragma once

using namespace System;

struct IComPort;

ref class Comport
{
	public:
		Comport(void);
		virtual ~Comport(void);
		
		bool init();
		bool disc();
		bool setPort(String ^ port);
		bool setBaudRate(int baudrate);
		bool readBytes( array<String ^> ^ % data );

	private:
		String ^ parseRecdBytes(int dataType, BSTR data, int startIndex);

	protected:
		String ^ m_comport;
		int m_baudrate;
		IComPort * m_port;

};
