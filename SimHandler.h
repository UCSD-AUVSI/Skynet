#pragma once

#include "VideoSimulator.h"
#include "DecklinkCallback.h"
#include <fstream>
#include <iostream>

using namespace std;
using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace System::Runtime::InteropServices;



#define ALL_FAILED			0
#define RECORDING_VIDEO		1
#define RECORDING_TELEM		2
#define RECORDING_ALL		3


#define RABBIT 0
#define AUTOPILOT 1
#define UNKNOWN 2
#define STARTVIDEO 3
#define ENDVIDEO 4


#define SPLIT_LENGTH 5*60 // (seconds) // make this 20min+ otherwise we get a million fucking video files

namespace OpenGLForm 
{
	ref class COpenGL;
}

namespace Simulator
{
	ref class TelemetrySimulator;
	public ref class SimHandler
	{
	public:
		int numframes;
		System::DateTime before;
		TelemetrySimulator ^ theTelSimulator; // telemetry
		Object ^ theComms;

		SimHandler(VideoSimulator ^ vidSim, Decklink::Callback * decklinkCallback, OpenGLForm::COpenGL ^ openGL, Vision::VisionController ^ visionCtrl);
		~SimHandler();

		void startPlayback(String ^ filename);
		void resumePlayback();
		void pausePlayback();
		void endPlayback();
		bool isPaused();

		void setRecordTelemetryBool(bool shouldRecordT) { recordTelemetry = shouldRecordT; }
		void setRecordVideoBool(bool shouldRecordV) { recordVideo = shouldRecordV; }
		int beginRecording();
		void startPlayingVideo(String ^ filename);
		void endRecording();
		void splitVideo(Object^ stateInfo);
		void tryToStartVideo(Object ^ arg);
		void writeTelemetry( System::TimeSpan time,  int type, int length, array<System::Byte>^ byteArray);
	private:
		String ^ makeVideoFilename();
		void stopVideo();
		bool firstPacket;
		StreamWriter ^ fileO;
		StreamReader ^ fileI;
		bool recordTelemetry;
		bool recordVideo;
		bool pleaseRecord;
		bool breakNow;
		Threading::Timer ^ splitTimer;

		Simulator::VideoSimulator ^ theVideoSimulator;
		OpenGLForm::COpenGL ^ openGLView;
		Vision::VisionController ^ visionController;
		Decklink::Callback * callback;
		Thread ^ videoAttemptThread;
		Thread ^ videoWriteThread;

		void endTelemetry();
		void beginTelemetry(String ^ filename);
		void writeVideo(); // video writing run loop


	protected:
		

		std::string ManagedToSTL(String ^ s) 
		{
		   using namespace Runtime::InteropServices;
		   const char* chars = 
		      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		   std::string retVal = chars;
		   Marshal::FreeHGlobal(IntPtr((void*)chars));

		   return retVal;
		}
	};
	
}


// coments...


		/**
		 * Should flush & close the stream writing to telemetry file.
		 */
		//True = Comport's calls will result in telemetry data being recorded.
		//False = calls will result in return.

		/*
		 * bool runVideofileO->
		 * bool runTelemetryfileO->
		 */

		/**
		 * Calls OpenGL's savevideo()
		 * and initializes fileStream for telemetry.
		 */
			//if recordVideo call OpenGL
			//if recordTelem call beginTelemetryfileO->
	
		/**
		 * Calls OpenGL's stopVideofileO->
		 * Call endTelemetryfileO->
		 */
		/**
		 * Begins feeding video AS IF its coming from the plane.
		 * Begins feeding telemetry data from file.
		 *	Needs to be done at correct intervals using a thread.
		 */
				//UNIMPLEMENTED
		//void feedVideofileO->
		/*
		 * IF recordTelemetry
			write to file;
		   else
			return;
		 */
		//get ofstream stuff from Comport

		//void write(System::TimeSpan theOffset, int type, int length, array<System::Byte> ^ byteArray); // TODO: implement
		/**
		 * Initializes Telemetry Stream and names it to video filename.
		 * append .telemetry to filename resulting in filename.telemetry
		 */

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
		//void pauseVideofileO->
		//void resumeVideofileO->