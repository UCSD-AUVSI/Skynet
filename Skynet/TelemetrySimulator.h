#pragma once

//#include "ComportHandler.h"
//#include "SimHandler.h"

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;

namespace Communications
{
	ref class RabbitComport;
	ref class AutopilotComport;
}

namespace Simulator
{
	ref class SimHandler;

	public ref class TelemetrySimulator{
	public:
		System::DateTime time;
		Communications::RabbitComport ^ theRabbitComport;
		Communications::AutopilotComport ^ theAutopilotComport;

		TelemetrySimulator(SimHandler ^ simHandler, Communications::RabbitComport ^ rabbitComport, Communications::AutopilotComport ^ autopilotComport)
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




		void setRabbit(Communications::RabbitComport ^ rabbitComport) { theRabbitComport = rabbitComport; }

		void setAutopilot(Communications::AutopilotComport ^ autopilotComport) { theAutopilotComport = autopilotComport; }

		void beginRecording()
		{
			time = System::DateTime::Now;
			record = true;
		}
		void write(int type, array<System::Byte> ^ byteArray);
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