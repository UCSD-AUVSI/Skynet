#pragma once


#include <cv.h>
#include <highgui.h>

#include "OpenGLForm.h"

#include <string>

#include "VideoReader.h"

using namespace Threading;

namespace Simulator
{
	public ref class VideoSimulator
	{
	public:
		VideoSimulator( OpenGLForm::COpenGL ^ parent );
		VideoSimulator( OpenGLForm::COpenGL ^ parent, const char * path );
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

		OpenGLForm::COpenGL ^ receiver;

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
