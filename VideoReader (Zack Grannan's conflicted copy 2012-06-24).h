#pragma once

#include <cv.h>
#include <highgui.h>

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