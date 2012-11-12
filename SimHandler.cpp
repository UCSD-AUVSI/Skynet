


#include "StdAfx.h"
#include "SimHandler.h"
#include "TelemetrySimulator.h"
#include <msclr/lock.h>
#include "OpenGLForm.h"

#include "Comport.h"
#include <time.h>
#include <iomanip>    // Needed for stream modifiers fixed and setprecision
#include <string>

#include "MasterHeader.h"
#include "Comms.h"

using namespace std;
using namespace Simulator;
using namespace Communications;
using namespace System;
using namespace System::Text;
using namespace System::Windows::Forms;
using namespace System::IO;
using namespace msclr;


// video write frame frequency (in milliseconds) (for 30 fps)
#define WAIT_INTERVAL		0
#define SHORT_WAIT_INTERVAL	2

#define FRAME_FREQUENCY		1000 / VIDEO_FRAMERATE


SimHandler::SimHandler(VideoSimulator ^ vidSim, OpenGLForm::COpenGL ^ openGL, Vision::VisionController ^ visionCtrl)
{
	recordTelemetry = false;
	recordVideo = false;
	theVideoSimulator = vidSim;
	visionController = visionCtrl;
	openGLView = openGL;
	
	splitTimer = nullptr;

	theTelSimulator = gcnew TelemetrySimulator(this, nullptr, nullptr);
}

SimHandler::~SimHandler()
{
	fileO = nullptr;
}

void SimHandler::startPlayback(String ^ filename)
{
	((TelemetrySimulator ^) theTelSimulator)->startTelSim(filename);
}

void SimHandler::pausePlayback()
{
	((TelemetrySimulator ^) theTelSimulator)->pausePlayback();
	theVideoSimulator->pauseVideo();
}

void SimHandler::resumePlayback()
{
	theVideoSimulator->playVideo();
	((TelemetrySimulator ^) theTelSimulator)->resumePlayback();
}

bool SimHandler::isPaused()
{
	return ((TelemetrySimulator ^) theTelSimulator)->isPaused();
}

void SimHandler::endPlayback()
{
	theVideoSimulator->stopVideo();
	((TelemetrySimulator ^) theTelSimulator)->endRunLoop();
}



void SimHandler::startPlayingVideo(String ^ filename)
{
	theVideoSimulator->stopVideo();
	theVideoSimulator->loadVideo( (const char*)(Marshal::StringToHGlobalAnsi(filename)).ToPointer());
	theVideoSimulator->startVideo();
}

int SimHandler::beginRecording()
{
	int retStatus = ALL_FAILED;

	String ^ videoFilename = makeVideoFilename();

	recordVideo = false;

	firstPacket = true;
	pleaseRecord = true;
	beginTelemetry(videoFilename);
	retStatus += RECORDING_TELEM;
	


	if (videoWriteThread != nullptr) {
		// stop, then wait
		System::Diagnostics::Trace::WriteLine("saveVideoFrame called from SimHandler: " + numframes + " in " + (System::DateTime::Now.Subtract(before)).Milliseconds);

		breakNow = true;
		Thread::Sleep( 30 ); // ms

		videoWriteThread->Abort();
	}

	
	breakNow = false;
	videoAttemptThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &SimHandler::tryToStartVideo));
	videoAttemptThread->Name = "SimHandler tryToStartVideo Thread";
	videoAttemptThread->Start(videoFilename);

	// alert user that video started
	((Comms ^)theComms)->printToConsole("Start Video", Color::Green);
	System::Diagnostics::Trace::WriteLine("Video start written to Telemetry");

	return retStatus;
}

void SimHandler::tryToStartVideo(Object ^ arg)
{
	return;
}

void SimHandler::stopVideo()
{
	return;
		/* TODO: deal with this 
	A first chance exception of type 'System.NullReferenceException' occurred in Skynet.exe
System.Transactions Critical: 0 : <TraceRecord xmlns="http://schemas.microsoft.com/2004/10/E2ETraceEvent/TraceRecord" Severity="Critical"><TraceIdentifier>http://msdn.microsoft.com/TraceCodes/System/ActivityTracing/2004/07/Reliability/Exception/Unhandled</TraceIdentifier><Description>Unhandled exception</Description><AppDomain>Skynet.exe</AppDomain><Exception><ExceptionType>System.NullReferenceException, mscorlib, Version=4.0.0.0, Culture=neutral, PublicKeyToken=b77a5c561934e089</ExceptionType><Message>Object reference not set to an instance of an object.</Message><StackTrace>   at Simulator.SimHandler.stopVideo() in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\simhandler.cpp:line 204
   at Simulator.SimHandler.endRecording() in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\simhandler.cpp:line 216
   at Skynet.Form1.startRecordButton_Click(Object sender, EventArgs e) in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\form1.h:line 2249
   at System.Windows.Forms.Button.OnMouseUp(MouseEventArgs mevent)
   at System.Windows.Forms.Control.WmMouseUp(Message&amp;amp; m, MouseButtons button, Int32 clicks)
   at System.Windows.Forms.Control.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.ButtonBase.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.Button.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.NativeWindow.DebuggableCallback(IntPtr hWnd, Int32 msg, IntPtr wparam, IntPtr lparam)
   at System.Windows.Forms.UnsafeNativeMethods.DispatchMessageW(MSG&amp;amp; msg)
   at System.Windows.Forms.Application.ComponentManager.System.Windows.Forms.UnsafeNativeMethods.IMsoComponentManager.FPushMessageLoop(IntPtr dwComponentID, Int32 reason, Int32 pvLoopData)
   at System.Windows.Forms.Application.ThreadContext.RunMessageLoopInner(Int32 reason, ApplicationContext context)
   at System.Windows.Forms.Application.ThreadContext.RunMessageLoop(Int32 reason, ApplicationContext context)
   at main(String[] args) in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\skynet.cpp:line 16</StackTrace><ExceptionString>System.NullReferenceException: Object reference not set to an instance of an object.
   at Simulator.SimHandler.stopVideo() in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\simhandler.cpp:line 204
   at Simulator.SimHandler.endRecording() in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\simhandler.cpp:line 216
   at Skynet.Form1.startRecordButton_Click(Object sender, EventArgs e) in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\form1.h:line 2249
   at System.Windows.Forms.Button.OnMouseUp(MouseEventArgs mevent)
   at System.Windows.Forms.Control.WmMouseUp(Message&amp;amp; m, MouseButtons button, Int32 clicks)
   at System.Windows.Forms.Control.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.ButtonBase.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.Button.WndProc(Message&amp;amp; m)
   at System.Windows.Forms.NativeWindow.DebuggableCallback(IntPtr hWnd, Int32 msg, IntPtr wparam, IntPtr lparam)
   at System.Windows.Forms.UnsafeNativeMethods.DispatchMessageW(MSG&amp;amp; msg)
   at System.Windows.Forms.Application.ComponentManager.System.Windows.Forms.UnsafeNativeMethods.IMsoComponentManager.FPushMessageLoop(IntPtr dwComponentID, Int32 reason, Int32 pvLoopData)
   at System.Windows.Forms.Application.ThreadContext.RunMessageLoopInner(Int32 reason, ApplicationContext context)
   at System.Windows.Forms.Application.ThreadContext.RunMessageLoop(Int32 reason, ApplicationContext context)
   at main(String[] args) in c:\users\ucsd\documents\visual studio 2010\projects\skynet\skynet\skynet.cpp:line 16</ExceptionString></Exception></TraceRecord>
An unhandled exception of type 'System.NullReferenceException' occurred in Skynet.exe

Additional information: Object reference not set to an instance of an object.
*/
}

void SimHandler::endRecording()
{
	if(recordVideo)
	{
		if (splitTimer != nullptr) {
			splitTimer->~Timer();
			splitTimer = nullptr;
		}

		stopVideo();	

		System::Diagnostics::Trace::WriteLine("endRecording in SimHandler");
	}

	else 
	{
		// kills the "attempt to start video" thread
		breakNow = true;
		videoAttemptThread->Abort();

	}


	if(recordTelemetry)
	{
		endTelemetry();
		pleaseRecord=false;
	}
	//if recordTelem call endTelem()
}

	/**
	 * Calls OpenGL's stopVideo();
	 * Call endTelemetry();
	 */
	/*void endVideo();
	{
		//if recordvideo call end in OpenGL
		recordVideo = false;
		//if recordTelem call endTelem()
		recordTelemetry = false;
	}*/
	/**
	 * Begins feeding video AS IF its coming from the plane.
	 * Begins feeding telemetry data from file.
	 *	Needs to be done at correct intervals using a thread.
	 */
			//UNIMPLEMENTED
	//void feedVideo();

void
SimHandler::writeVideo() {
	// saveVideoFrame()

	numframes = 0;
	before = System::DateTime::Now;

	try {
		System::DateTime last = System::DateTime::Now;
		System::DateTime first = System::DateTime::Now;
		int counter = 0;

		
		System::Diagnostics::Trace::WriteLine("writeVideo() in SimHandler");

		// main video reading loop
		while (true) {


			try {
		
				// check to see if we should stop
				if (breakNow)
					break;

				// wait until 33 ms comes up
				while ((System::DateTime::Now.Subtract(last)).Milliseconds < FRAME_FREQUENCY) {
					Thread::Sleep( SHORT_WAIT_INTERVAL );
				}

				// save time that we are reading this frame
				last = last.AddMilliseconds( FRAME_FREQUENCY );
		
				// check to see if we should stop
				if (breakNow)
					break;

				// save video frame
				numframes++;

			} 
			catch (ThreadInterruptedException ^ e) 
			{
				throw e;
			}
			catch (Exception ^ e) 
			{
				System::Diagnostics::Trace::WriteLine("SimHandler::writeVideo() ERROR writing video frame: \n" + e + "\n" + e->StackTrace);
				PRINT("DAMNIT");
			}
		}
	} catch (Exception ^ ) {
		System::Diagnostics::Trace::WriteLine("SimHandler::writeVideo() shutting down");
		

	}
}

void SimHandler::splitVideo(Object^ stateInfo)
{
	
	stopVideo();

	String ^ videoFilename = makeVideoFilename();
	tryToStartVideo(videoFilename);

	// alert user that video started
	((Comms ^)theComms)->printToConsole("Split Video", Color::Green);
	System::Diagnostics::Trace::WriteLine("Video split written to Telemetry");

}

String ^ SimHandler::makeVideoFilename(){
	DateTime time = DateTime::Now;
	//time = time.AddSeconds( -Convert::ToInt32( SPLIT_LENGTH ) );
	String ^ filename = "D:\\Skynet Files\\Flight Logs\\video_" + time.ToString("o")->Replace(":", "-");
	return filename;
}

void SimHandler::writeTelemetry( System::TimeSpan time,  int type, int length, array<System::Byte>^ byteArray )
{
	//System::Diagnostics::Trace::WriteLine("write telem");
	if(pleaseRecord)
	{
		//System::Diagnostics::Trace::WriteLine("actually writing telem");
		lock l(this);
		fileO->Write("" + (float) time.TotalSeconds);
		fileO->Write(", ");
		fileO->Write("" + (int)type);
		fileO->Write(", ");
		fileO->Write("" + (int)length); 
		fileO->Write(", ");
		int x;
		for(x=0; x<length;x++)
		{
			fileO->Write(byteArray[x]);
			firstPacket = false;
		}
		fileO->WriteLine();
		fileO->WriteLine();
		
		fileO->Flush();
	}
	//System::Diagnostics::Trace::WriteLine("done with telem");
}
	//get ofstream stuff from Comport

/*private:
	ofstream fileO;
	ifstream fileI;
	bool recordTelemetry;
	bool recordVideo;*/
	/**
	 * Should flush & close the stream writing to telemetry file.
	 */
void SimHandler::endTelemetry()
{
	if (fileO != nullptr)
	{
			fileO->Close();
	}
	pleaseRecord=false;
	recordTelemetry = false;
	((TelemetrySimulator ^) theTelSimulator)->stopRecording();
}
/**
	* Initializes Telemetry Stream and names it to video filename.
	* append .telemetry to filename resulting in filename.telemetry
	*/
void SimHandler::beginTelemetry(String ^ filename)
{
	if (fileO != nullptr)
	{
		fileO->Close();
	}
	FileStream^ fs = gcnew FileStream( filename+".telemetry.txt", FileMode::CreateNew);
	fileO = gcnew StreamWriter( fs, Encoding::UTF8 );
	((TelemetrySimulator ^) theTelSimulator)->beginRecording();
	pleaseRecord=true;
	recordTelemetry = true;
}
/**
 * UNIMPLEMENTED/FUTURE USE
 */
/**
 * public:
 */
	/**
	 * Should PAUSE/UNPAUSE video recording in OpenGL.
	 */
	//Unimplemented.
	//void pauseVideo();
	//void resumeVideo();

//void SimHandler::write(System::TimeSpan theOffset, int type, int length, array<System::Byte> ^ byteArray) {} // TODO: finish implementing