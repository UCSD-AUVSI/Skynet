#pragma once
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)

class Auvsi_Recognize
{
public:
	Auvsi_Recognize(  cv::Mat img );
	Auvsi_Recognize( int height, int width, int type, void * data );
	Auvsi_Recognize();
	~Auvsi_Recognize(void);
	void setImage( cv::Mat  img );
	
	void runComputation( void );	

private:
	template <typename T> void extractShape( void );
	template <typename T> void extractLetter( void );
	template <typename T> cv::Mat doClustering( cv::Mat input, int numClusters, bool colored );
	cv::Mat convertToGray( cv::Mat input );
	cv::Mat centerBinary( cv::Mat input );

	// Utility functions - should probably put in a different namespace
	template <typename T> T getVectorMean( std::vector<T> input );
	
public:
	cv::Mat _input;		// original input
	cv::Mat _image;		// 64x64 color converted input
	cv::Mat _shape;		// 64x64 binary shape
	cv::Mat _letter;	// 64x64 binary letter
};

