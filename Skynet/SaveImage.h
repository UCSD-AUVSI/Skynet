#pragma once

#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)

#include <string>

namespace ImageUtil
{
	class SaveImage
	{
	public:
		SaveImage( int height, int width, int channels );
		~SaveImage(void);

		void setupVideo( std::string path );
		void stopVideo( void );
		void changeEncoding( std::string encoding );

		void saveFrame( float * frame, std::string path );
		void saveFrame( float * buffer, std::string path, std::string pathbase, float * homography, double heading  );
		void saveFrameNonInterlaced( float * buffer, std::string path  );
		void writeFrame( float * frame );

	protected:
		
		cv::Mat convertBuffer( float * frame, float scale, bool interlaced );
		cv::Mat convertBuffer( float * buffer, float scale );
	private:
		int _height;
		int _width;
		int _channels;
		double _fps;
		int _fourcc;
		
		cv::VideoWriter _writer;
	};
}
