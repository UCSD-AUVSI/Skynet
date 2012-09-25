#pragma once

#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)

#include <string>


namespace Simulator {

	class VideoReader {
	public:
		VideoReader( void );
		VideoReader( const char * path);
		~VideoReader( void );
		void setPath( const char * path);
		void grab( void );
		cv::Mat retrieve( void );

	private:
		cv::VideoCapture _capture;
	};


}