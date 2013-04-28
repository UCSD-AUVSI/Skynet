#pragma once


#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)


#include <string>

#include "VideoReader.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace Threading;

namespace Vision
{
	ref class VisionController;
}

namespace Simulator
{
	public ref class VideoSimulator
	{
	public:
		VideoSimulator( Vision::VisionController ^ parent );
		VideoSimulator( Vision::VisionController ^ parent, const char * path );
		~VideoSimulator(void);

		void loadVideo( const char * path );
		void startVideo( void );
		void stopVideo( void );
		void pauseVideo( void );
		void playVideo( void);
		bool isPaused() { return paused; }

	protected:
		
		float * convertBuffer( cv::Mat );
		bool paused;
		int frameCount;
		
		Thread ^ videoReadThread;

		Vision::VisionController ^ receiver;

	private:
		/*int _height;
		int _width;
		int _channels;
		double _fps;
		int _fourcc;*/
		//CvCapture *_cvReader;
		//cv::VideoCapture *_reader;

		VideoReader *theReader;

		void readThread( void );
	};
}
