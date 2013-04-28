#pragma once
#include <opencv2/opencv.hpp>
//#include <cv.h>
//#include <highgui.h>

class Auvsi_Recognize
{
public:
	static cv::Mat ProcessShape(cv::Mat shape);
	static cv::Mat SegmentLetter(cv::Mat colorImg, cv::Mat binaryShapeImg);

	Auvsi_Recognize( cv::Mat img );
	Auvsi_Recognize( int height, int width, int type, void * data );
	Auvsi_Recognize();
	~Auvsi_Recognize(void);
	void setImage( cv::Mat  img );
	
	void runComputation( std::string imgPath = "" );

	bool hasGoodLetter();

	bool checkAll();
	static bool checkImage( cv::Mat img );
	cv::Mat getLetter() { return _letter; }
	static cv::Mat centerBinary( cv::Mat input );
	static cv::Mat takeImageGradient(cv::Mat img);
private:
	cv::Mat resizeAndPadImage(cv::Mat input);
	template <typename T> void extractShape( void );
	template <typename T> void extractLetter( void );
	template <typename T> void takeShapeBorder( void );
	template <typename T> void erodeLetter( void );
	template <typename T> static cv::Mat customErode(cv::Mat input);
	template <typename T> static bool pixelIsValid(cv::Mat img, int x, int y);
	template <typename T> cv::Mat doClustering( cv::Mat input, int numClusters, bool colored, cv::Mat & centers );
	cv::Mat convertToGray( cv::Mat input );

	// Utility functions - should probably put in a different namespace
	template <typename T> static T getVectorMean( std::vector<T> input );
	static cv::Mat padImageToSquare(cv::Mat img);
	
public:
	cv::Mat _input;		// original input
	cv::Mat _image;		// resized color converted input
	cv::Mat _shape;		// resized binary shape
	cv::Mat _letter;	// resized binary letter
	cv::Mat _shapeExtended; // resized color image (bg replaced with shape color)
};

