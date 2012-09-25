#include "StdAfx.h"
#include "VideoSimulator.h"

#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)


#include <string>

#include "MasterHeader.h"
#include "VisionController.h"

using namespace Simulator;
using namespace cv;
using namespace std;

// read frame frequency (in milliseconds) (for 30 fps)
#define WAIT_INTERVAL		0
#define SHORT_WAIT_INTERVAL	2

#define FRAME_FREQUENCY		1000 / VIDEO_FRAMERATE

VideoSimulator::VideoSimulator( Vision::VisionController ^ parent )
{
	receiver = parent;
	frameCount = -1;
	theReader = nullptr;
}

VideoSimulator::VideoSimulator( Vision::VisionController ^ parent, const char * path )
{
	receiver = parent;
	
	frameCount = -1;

	theReader = new VideoReader( path );

}


VideoSimulator::~VideoSimulator(void)
{
	stopVideo();
}

void 
VideoSimulator::loadVideo( const char * path ) 
{
	// setup video reader
	if (theReader)
		delete theReader;

	theReader = new VideoReader( path );
	frameCount = -1;
	paused = false;

}


void 
VideoSimulator::startVideo( void ) 
{
	if (theReader == nullptr)
		return;

	if (videoReadThread != nullptr)
		videoReadThread->Abort();

	// make thread that reads frame 30 times per second
	videoReadThread = gcnew Thread(gcnew ThreadStart(this, &VideoSimulator::readThread));
	videoReadThread->Name = "Simulator Video Read Thread";
	paused = false;

	videoReadThread->Start();

	System::Diagnostics::Trace::WriteLine("VideoSimulator::startVideo()");

}

void 
VideoSimulator::stopVideo( void )
{
	// stop read thread
	if (videoReadThread == nullptr)
		return;
	
	videoReadThread->Abort();
	videoReadThread = nullptr;
}

void 
VideoSimulator::pauseVideo( void )
{

	paused = true;
	//videoReadThread->Sleep( -1 );

}

void 
VideoSimulator::playVideo( void )
{
	paused = false;
	if (videoReadThread != nullptr)
		videoReadThread->Interrupt();

}

void 
VideoSimulator::readThread( void )
{
	try {
		System::DateTime last = System::DateTime::Now;
		System::DateTime first = System::DateTime::Now;
		int counter = 0;


		// main video reading loop
		while (true) {
			
			//System::Diagnostics::Trace::WriteLine("VideoSimulator::readThread() looping");

			try {

				// check the reader
				if (theReader == nullptr) {
					Thread::Sleep( 100 );
					continue;
				}


				// pause
				if (paused) {

		
					// sleep until interrupted
					try {

						Thread::Sleep( -1 );
					} catch ( ThreadInterruptedException ^ ) {}

				}
		
				// wait until 33 ms comes up
				while ((System::DateTime::Now.Subtract(last)).Milliseconds < FRAME_FREQUENCY) {
					Thread::Sleep( SHORT_WAIT_INTERVAL );
				}

				// save time that we are reading this frame
				last = last.AddMilliseconds( FRAME_FREQUENCY );

				// read frame, convert frame to "float *", and send to reciever
				
				float * frame = this->convertBuffer( theReader->retrieve() ); 
				/*

				A first chance exception of type 'System.AccessViolationException' occurred in Skynet.exe
				An unhandled exception of type 'System.AccessViolationException' occurred in Skynet.exe

				Additional information: Attempted to read or write protected memory. This is often an indication that other memory is corrupt.




				*/
		
				if (frame != 0 ) {
					receiver->receiveFrame( frame );
					
					
					// benchmark code
					/*
					counter++;
					int numSeconds = System::DateTime::Now.Subtract(first).Seconds;
					if (numSeconds == 0) numSeconds = 1;

					float frameRate = counter/numSeconds;
					System::Diagnostics::Trace::WriteLine( "framerate:" + frameRate + " counter:" + counter + " seconds:" + numSeconds);
					*/
					//delete frame;
				}

				// prepare next frame
				theReader->grab();
			} catch (ThreadInterruptedException ^) {}
		}
	} 
	catch (ThreadAbortException ^ ) {
	}
	catch (System::Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR in VideoSimulator::readThread: " + e);
	}
	
}


float * 
VideoSimulator::convertBuffer( cv::Mat inMat )
{
	typedef cv::Vec<unsigned char, 3> VT;

	// increment frame count
	frameCount++;

	// get width, height, numChannels, scale, rowSize
	int rows, cols, numChannels, width, height, channels2;
	height = rows = inMat.rows;
	width = cols = inMat.cols;
	channels2 = inMat.channels();
	numChannels = 4;


	float scale = 1.0f/255.0f;

	if ( inMat.channels() < 3 )
		return 0;

	// make iterator for going through image
	cv::MatIterator_<VT> matIter = inMat.begin<VT>();

	// alloc frame memory
	float * frame = new float[rows * cols * numChannels]; 	

	// pointer to data
	unsigned char *inputBuffer = &inMat.data[0];

	int rowSize = cols*numChannels;

	// loop through each pixel and copy it
	for( int row = 0; row < height; row++ )
	{
		// pointer to  row
		unsigned char *pixel = inputBuffer + (3*(width*row));

		for( int col = 0; col < width; col++ )
		{
			//VT pixel = *matIter; 

			// also convert from BGR to RGB
			frame[(row*cols + col)*numChannels + 0] = pixel[3*col + 2] / 255.0f; // blue -> red 
			frame[(row*cols + col)*numChannels + 1] = pixel[3*col + 1] / 255.0f; // green -> green, * scale;
			frame[(row*cols + col)*numChannels + 2] = pixel[3*col + 0] / 255.0f; // red -> blue, * scale;
			frame[(row*cols + col)*numChannels + 3] = 0;

				

			//++matIter;

		}
	}
	
	// setup view for display of video at this size
	if( frameCount < 2 )
	{
		System::Diagnostics::Trace::WriteLine( "setup callback");
		receiver->gotFirstFrame( width, height );
		//firstFrame = false;
	}

	

	return frame;



}

