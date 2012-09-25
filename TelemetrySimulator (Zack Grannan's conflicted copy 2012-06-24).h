#pragma once

//#include "ComportHandler.h"
#include "SimHandler.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;

namespace Simulator
{
	public ref class TelemetrySimulator{
	public:
		System::DateTime time;
		Object ^ theRabbitComport;
		Object ^ theAutopilotComport;

		TelemetrySimulator(SimHandler ^ simHandler, Object ^ rabbitComport, Object ^ autopilotComport)
		{
			theRabbitComport = rabbitComport;
			theAutopilotComport = autopilotComport;
			theSimHandler = simHandler;
			record = false;
		}

		/* Run loop that gets telem file and analyzes it */
		
		void startTelSim(String ^ filename);
		void endRunLoop();
		bool isPaused() { return paused; }
		void resumePlayback();
		void pausePlayback();




		void setRabbit(Object ^ rabbitComport) { theRabbitComport = rabbitComport; }

		void setAutopilot(Object ^ autopilotComport) { theAutopilotComport = autopilotComport; }

		void beginRecording()
		{
			time = System::DateTime::Now;
			record = true;
		}
		void write(int type, array<System::Byte> ^ byteArray)
		{
			//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED "+record);
			if(record)
			{
				//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED FOR REALS");
				theSimHandler->writeTelemetry(System::DateTime::Now.Subtract(time), type, byteArray->Length, byteArray);
			}
			else{return;}
		}
		void stopRecording()
		{
			record=false;
		}
	protected:
	private:
		bool paused;
		String ^ pathname;
		bool breakLoop;
		void simRunLoop();
		Thread ^ simRunLoopThread;
		bool record;
		SimHandler ^ theSimHandler;
	};
}