#include "StdAfx.h"
#include "TelemetrySimulator.h"
#include "AutopilotComport.h"
#include "SimHandler.h"

using namespace std;
using namespace Simulator;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace Communications;

void TelemetrySimulator::startTelSim(String ^ filename)
{
	pathname = filename;

	//start the run loop
	//if (false) // DEBUG: this should be false
	//{
	//	theSimHandler->startPlayingVideo(filename);
	//}
	//else
	{
		simRunLoopThread = gcnew Thread(gcnew ThreadStart(this, &TelemetrySimulator::simRunLoop));
		simRunLoopThread->Name = "SimRunLoopThread";
		simRunLoopThread->Start();
	}
}

void TelemetrySimulator::simRunLoop()
{
	System::Diagnostics::Trace::WriteLine("HEY WE ARE IN THE SIM LOOP YO");
	breakLoop = false;
	StreamReader ^ sr = (gcnew FileInfo(pathname))->OpenText();
	System::DateTime startTime = System::DateTime::Now;
	String ^ temp = "";
	array<System::Byte> ^ packet;
	try
	{
		while(!breakLoop)
		{
			if(paused)
			{
				Thread::Sleep(250);
				startTime = startTime.AddMilliseconds(250);
				continue;
			}
			if(sr->Peek() == -1)
			{
				break;
			}
			
			float offset = 0.0f;
			int type = 0;
			int size = 0;
			temp = "";
			while(1)
			{
				int fChar = sr->Read();
				if((char) fChar != ',')
				{
					char tchar = (char) fChar;
					temp += gcnew String(tchar, 1);
				}
				else
				{
					//read the space afterwards
					sr->Read();
					break;
				}
			}
			try {
				offset = Convert::ToSingle(temp);
			}
			catch(Exception ^) {}
			temp = "";
			while(1)
			{
				int fChar = sr->Read();
				if(fChar != ',')
				{
					char tchar = (char) fChar;
					temp += gcnew String(tchar, 1);
				}
				else
				{
					//read the space afterwards
					sr->Read();
					break;
				}
			}
			try {
				type = Convert::ToInt32(temp);
			}
			catch(Exception ^) {}
			temp = "";
			while(1)
			{
				int fChar = sr->Read();
				if(fChar != ',')
				{
					char tchar = (char) fChar;
					temp += gcnew String(tchar, 1);
				}
				else
				{
					//read the space afterwards
					sr->Read();
					break;
				}
			}
			try {
				size = Convert::ToInt32(temp);
			}
			catch(Exception ^) {}
			temp = "";
			
			packet = gcnew array<System::Byte>(size);
			for(int i = 0; i < size; i++)
			{
				Int32 tempInt = sr->Read();
				packet[i] = (System::Byte) tempInt;
			}

			//read two newlines
			sr->Read();
			sr->Read();

			DateTime ptime = startTime.AddSeconds(offset);
			TimeSpan timeOffset = ptime.Subtract( System::DateTime::Now );
			int timespan = (int) timeOffset.TotalMilliseconds;

			if (timespan >= 1)
			{
				Thread::Sleep( timespan );
			}

			//System::Diagnostics::Trace::WriteLine("HEY WE GOT A PACKET!!! T:"+type+" S:"+size+" O:"+offset);

			if(type == AUTOPILOT)
			{
				((AutopilotComport ^) theAutopilotComport) -> receiveData( packet );
				continue;
			}
			if(type == STARTVIDEO)
			{
				Encoding^ u8 = Encoding::UTF8;
				theSimHandler->startPlayingVideo(u8->GetString(packet));
				continue;
			}
		}

	}
	catch (Exception ^ e)
	{
		e = nullptr;
	}
	
	sr->Close();
}

void TelemetrySimulator::pausePlayback()
{
	paused = true;

}

void TelemetrySimulator::resumePlayback()
{
	paused = false;
}

void TelemetrySimulator::endRunLoop()
{
	breakLoop = true;
	Thread::Sleep( 60 );
	if ( simRunLoopThread != nullptr ){
		simRunLoopThread->Abort();
		simRunLoopThread = nullptr;
	}
}
void TelemetrySimulator::write(int type, array<System::Byte> ^ byteArray)
{
	//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED "+record);
	if(record)
	{
		//System::Diagnostics::Trace::WriteLine("WRITE IN TELEMETRYSIM IS CALLED FOR REALS");
		theSimHandler->writeTelemetry(System::DateTime::Now.Subtract(time), type, byteArray->Length, byteArray);
	}
	else{return;}
}
