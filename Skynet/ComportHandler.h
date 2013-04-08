#pragma once

#include "Comport.h"
#include "TelemetrySimulator.h"

using namespace System::IO::Ports;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

using namespace Simulator;

namespace Communications
{
	ref class Comms;

	public ref class ComportHandler {
	public:
		ComportHandler(TelemetrySimulator ^ telSimulator, Comms ^ newDelegate, String ^ comType);

		bool connect(String ^ portName);
		virtual void disconnect();

		//void beginReading();
		void writeData( array<System::Byte> ^ inBuffer ); 
		void receiveData( array<System::Byte> ^ inBuffer );
		virtual void analyzeData( array<System::Byte> ^ inBuffer );
		void beginReading(String ^ comportType);
		virtual void afterBeginReading();
		void updateComportStatus(int status);

	protected:
		Comms ^ theDelegate;
		Comport ^ thePort;
		Thread ^ comReadThread;
		String ^ type;
		int intType;

	private:
		TelemetrySimulator ^ theTelSimulator;
	};


}