#include "StdAfx.h"
#include "SaveImage.h"

#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)

#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>

#include "MasterHeader.h"

using namespace ImageUtil;
using namespace std;


SaveImage::SaveImage( int height, int width, int channels )
{
	_height = height;
	_width = width;
	_channels = channels;
	//_fourcc = 0;
	//_fourcc = -1;
	_fourcc = CV_FOURCC('M','J','P','G');		// SET A GOOD DEFAULT: mpeg
	//_fourcc = CV_FOURCC('f','f','d','s');
	_fps = VIDEO_FRAMERATE;
}


SaveImage::~SaveImage(void)
{
	stopVideo();
}

cv::Mat
SaveImage::convertBuffer( float * buffer, float scale )
{
	return convertBuffer(buffer, scale, true);
}

cv::Mat
SaveImage::convertBuffer( float * buffer, float scale, bool interlaced )
{
	cv::Mat retVal;
	typedef cv::Vec<char, 3> VT; // was float
	int fullWidth = _width * _channels;
	float total = 0;
	retVal = cv::Mat( _height, _width, /*CV_32FC3*/CV_8UC3 ); // 

	cv::MatIterator_<VT> matIter = retVal.begin<VT>();

	// interlaced so iterate over rows / 2
	for( int row = 0; row < _height / (interlaced ? 2 : 1); ++row )
	{
		// replicate each row twice
		for( int interLace = 0; interLace < (interlaced ? 2 : 1); ++interLace )
		{
			for( int col = 0; col < _width; ++col )
			{
				// B G R format
				float r = buffer[row * fullWidth + col * _channels + 0] * scale;
				float g = buffer[row * fullWidth + col * _channels + 1] * scale;
				float b = buffer[row * fullWidth + col * _channels + 2] * scale;
				total += r+g+b;
				*matIter = VT( (char)b, (char)g, (char)r );
				++matIter;
			}
		}
	}
	cout << "convertBuffer:" << total;
	//cv::imshow("write frame", retVal);
	return retVal;
}

void
SaveImage::saveFrame( float * buffer, std::string path  )
{
	cv::imwrite( path, convertBuffer( buffer, 255.0f ) );
}

void
SaveImage::saveFrameNonInterlaced( float * buffer, std::string path  )
{
	cv::imwrite( path, convertBuffer( buffer, 255.0f,  false ) );
}

void
SaveImage::saveFrame( float * buffer, std::string path, std::string pathbase, float * homography, double heading  )
{
	
	typedef cv::Vec<float, 1> VT;
	cv::Mat image = convertBuffer( buffer, 255.0f );

	cv::imwrite( path, image ); // save regular image


	cv::Mat warped, rotated;
	cv::Mat rotation = cv::getRotationMatrix2D( cv::Point2f( image.cols/2.0f, image.rows/2.0f ), heading, 1.0 );
	
	cv::Mat homo(3, 3, CV_32FC1 );
	homo.at<VT>( 0, 0 )[0] = homography[0];
	homo.at<VT>( 0, 1 )[0] = homography[1];
	homo.at<VT>( 0, 2 )[0] = homography[2];
	homo.at<VT>( 1, 0 )[0] = homography[3];
	homo.at<VT>( 1, 1 )[0] = homography[4];
	homo.at<VT>( 1, 2 )[0] = homography[5];
	homo.at<VT>( 2, 0 )[0] = homography[6];
	homo.at<VT>( 2, 1 )[0] = homography[7];
	homo.at<VT>( 2, 2 )[0] = homography[8];

	//cv::warpAffine( image, rotated, rotation, image.size() );
	//cv::imwrite( pathbase + "_rotated.jpg", rotated );

	//cv::warpPerspective( image, warped, homo.t(), image.size() );
	cv::warpAffine( image, rotated, rotation, image.size() );
	cv::imwrite( pathbase + "_rectified.jpg", rotated ); // save rectified image

	//cv::imwrite( path, image );
}

void
SaveImage::writeFrame( float * buffer )
{
	printf("SaveImage::writeFrame() WAS ACTUALLYH CALLED"); // DEBUG
	if( _writer.isOpened() )
	{
		cv::Mat imageToWrite = convertBuffer( buffer, 255.0f ); 
		_writer << imageToWrite;
		
		
		/*FILE *myfile;
		myfile = fopen("D:\\Skynet Files\\Flight Logs\\aaaaaaa error.txt","w");
		cv::Mat imageToWrite;
		try
		{
			imageToWrite = convertBuffer( buffer, 255.0f ); // was 1.0f 
			//cv::imwrite("D:\\Skynet Files\\Flight Logs\\aaaaaaa test image.jpg",imageToWrite); // DEBUG: remove this 
			_writer << imageToWrite; //convertBuffer( buffer, 1.0f ); // DEBUG: put this back in
		}
		catch(exception& e)
		{
			//myfile = new ofstream("D:\\Skynet Files\\Flight Logs\\aaaaaaa error.txt");
			//myfile.open;
			fprintf(myfile,"error:\n");
			fprintf(myfile,"%s\n%d\nRows:%d\nCols:%d", e.what(),imageToWrite.depth(),imageToWrite.rows,imageToWrite.cols);
			fclose(myfile);
		}

		printf((_writer.isOpened() ? "write frame: opened":"write frame: closed")); // DEBUG: premove this*/
	}
}

void 
SaveImage::setupVideo( std::string path )
{
	// stop video if necessary every time, or it doesnt work.
	
	//stopVideo();
	
	_writer = cv::VideoWriter( path, _fourcc, _fps, cv::Size( _width, _height ), true);
	//printf("setup video");
}

void 
SaveImage::stopVideo( void )
{

	if(_writer.isOpened() ) {
		_writer.~VideoWriter();
		printf("stop video");
	}
}

void
SaveImage::changeEncoding( std::string encoding )
{
	return; // dont run this function

	if( encoding == "None" )
	{
		//_fourcc = 0;
	}
	else if( encoding == "XviD" )
	{
		_fourcc = CV_FOURCC('x','v','i','d');
	}
	else if( encoding == "MJPG" )
	{
		_fourcc = CV_FOURCC('M','J','P','G');
	}
}