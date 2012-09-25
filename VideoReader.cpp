#include "VideoReader.h"

using namespace Simulator;

VideoReader::VideoReader( void ) 
{

}

VideoReader::VideoReader( const char * path)
{
	_capture = cv::VideoCapture( path );
}

VideoReader::~VideoReader( void )
{
	// kill reader 
}


void 
VideoReader::setPath( const char * path)
{
	_capture = cv::VideoCapture( path );
}

void 
VideoReader::grab( void )
{
	_capture.grab();
}

cv::Mat 
VideoReader::retrieve( void )
{
	cv::Mat retVal;

	_capture.retrieve( retVal );
	
	//retVal *= 1./255;
	//cv::cvtColor(retVal, retVal, CV_BGR2RGB);

	//cv::namedWindow( "Source", CV_WINDOW_AUTOSIZE);
//	cv::imshow( "Source", retVal);

	return retVal;

}

