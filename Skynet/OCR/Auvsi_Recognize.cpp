#include "Auvsi_Recognize.h"
#include "BlobResult.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include <string>
#include "Auvsi_cvUtil.h"


// Whether we want to use just color channels or not
#define TWO_CHANNEL 1


cv::Mat
Auvsi_Recognize::SegmentLetter(cv::Mat colorImg, cv::Mat binaryShapeImg)
{
	Auvsi_Recognize *recognize = new Auvsi_Recognize(colorImg);
	
	recognize->_shape = recognize->resizeAndPadImage(binaryShapeImg);
	recognize->extractLetter<float>();
	cv::Mat letter = recognize->_letter;
	letter = recognize->centerBinary(letter);
	cv::resize(letter, letter, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));
	letter = letter > 128;

	delete recognize;

	return letter;
}

cv::Mat 
Auvsi_Recognize::ProcessShape(cv::Mat shape)
{
	shape = Auvsi_Recognize::centerBinary(shape);
	cv::resize(shape, shape, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));
	shape = shape > 128; // threshold
	return shape;
}

/**
 * Constructor for testing using a cv::Mat
 */
Auvsi_Recognize::Auvsi_Recognize( cv::Mat img )
{

	setImage(img);
	// std::printf("begining recognize constructor\n");

	/*// Set equal to image
	cv::Mat _input =  img ;
	
	cv::Mat temp2( _input.size(), CV_32F );
	_image = cv::Mat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F );

	_input.convertTo( temp2, _image.type(), 1.0/255.0f );
	//cv::imshow( "Input", _input ); // DEBUG

	cv::resize(temp2, _image, _image.size() );
	//cv::imshow( "_image", _image ); // DEBUG
	//cv::waitKey(10); // DEBUG
	cv::cvtColor( _image, _image, CV_RGB2Lab );
	//std::printf("converted color in constructor\n");


	// Only use A and B channels
	#ifdef TWO_CHANNEL
		std::vector<cv::Mat> splitData;
		cv::split( _image, splitData );
		splitData.erase( splitData.begin() );
		cv::merge( splitData, _image );
	#endif*/
}

/**
 * Constructor for using a data buffer
 *
 * Input:
 *	height	-	image height
 *	width	-	image width
 *	type	-	OpenCV image type (typically for us this will be CV_32FC3)
 *	data	-	Pointer to data
 */
Auvsi_Recognize::Auvsi_Recognize( int height, int width, int type, void * data )
{
	// Load image and resize
	cv::Mat temp( height, width, type, data );
	_image = cv::Mat( IMAGE_HEIGHT, IMAGE_WIDTH, type );

	setImage(_image);
	//cv::resize(temp, _image, _image.size() );

	// Convert to LAB
	//cv::cvtColor( _image, _image, CV_RGB2Lab );
}
// default constructor. Before starting compuation, user must call setImage
Auvsi_Recognize::Auvsi_Recognize()
{

}


bool Auvsi_Recognize::checkAll()
{
	bool result = true;
	if (checkImage(_shape))
		;//printf("Shape is good");
	else {
		result = false;
		printf("Shape is bad");
	}
	
	if (checkImage(_letter))
		;//printf("Letter is good\n");
	else  {
		result = false;
		printf("Letter is bad\n");
	}
	
	return result;
}


bool Auvsi_Recognize::checkImage( cv::Mat img )
{
	typedef cv::Vec<unsigned char, 1> VT;
	bool status = true;
	unsigned char value;
	
	for (int r = 0; r < img.rows; r++)
		for (int c = 0; c < img.cols; c++) {
			VT pixel = img.at<VT>(r, c);
			value = pixel[0];
			//for (int p = 0; p < 3; p++)
				if (value != 255 && value != 0) {
					printf("pixel:%d", value);
					return false;
				}
		}
	
	return status;
	
}

// set image so that we can start computation
void
Auvsi_Recognize::setImage( cv::Mat img )
{
	
	// Set equal to image
	cv::Mat _input( img );
	
	cv::Mat temp2( _input.size(), CV_32F );

	if (_input.type() != CV_32FC3)
		_input.convertTo( temp2, _image.type());//, 1.0/255.0f );
	else
		temp2 = _input.clone();

	_image = resizeAndPadImage(temp2);

	// do color conversion
	//cv::cvtColor( _image, _image, CV_RGB2Lab ); // DEBUG 
	
	/*cv::imshow( "Input", img ); // DEBUG
	cv::imshow( "temp2", temp2 ); // DEBUG
	cv::imshow( "_image", _image ); // DEBUG
	cv::waitKey(10);*/

	//std::printf("converted color in set image \n");

	// Only use A and B channels
	#ifdef TWO_CHANNEL
		std::vector<cv::Mat> splitData;
		cv::split( _image, splitData );
		splitData.erase( splitData.begin() );
		cv::merge( splitData, _image );
	#endif
}

cv::Mat 
Auvsi_Recognize::resizeAndPadImage(cv::Mat input)
{
	cv::Mat result = cv::Mat( IMAGE_HEIGHT, IMAGE_WIDTH, input.type() );
	
	// pad image to square
	int outH, outW;
	outH = input.rows;
	outW = input.cols;
	cv::Mat padded;
	
	if( outH < outW ) // pad height
		cv::copyMakeBorder( input, padded, (outW-outH)/2, (outW-outH)/2, 0, 0, cv::BORDER_REPLICATE);//, cvScalar(0) );
	else // pad width
		cv::copyMakeBorder( input, padded, 0, 0, (outH-outW)/2, (outH-outW)/2, cv::BORDER_REPLICATE);//, cvScalar(0) );

	// Make sure output is desired width
	//cv::resize( retVal, buffered, input.size(), 0, 0, cv::INTER_NEAREST );

	// resize image if it is too big for proper computation
	if (padded.size().width > MAX_IMAGE_SIZE || padded.size().height > MAX_IMAGE_SIZE )
		cv::resize(padded, result, cv::Size(MAX_IMAGE_SIZE, MAX_IMAGE_SIZE) );
	else
		result = padded;
	
	//cv::imshow("result", result); // DEBUG

	return result;
}


template<typename T>
bool
tEquals(T a, T b)
{
	return abs(a - b) < 0.001;
}

void
Auvsi_Recognize::runComputation( std::string imgPath )
{
	bool pathIsValid = imgPath.length() > 0;

	// extract shape and letter from _image (large, LAB)
	extractShape<float>();
	extractLetter<float>();

	//erodeLetter<float>(); // this doesn't improve accuracy, so don't do it

	// center _shape and _letter (both large, binary)
	_shape = centerBinary(_shape);
	_letter = centerBinary(_letter);
	
	if (pathIsValid)
	{
		IplImage letterIpl = _letter;
		IplImage shapeIpl = _shape;
		cvSaveImage((imgPath + "_letter_acenter_debug.jpg").c_str(), &letterIpl);
		cvSaveImage((imgPath + "_shape_acenter_pre_debug.jpg").c_str(), &shapeIpl);
	}

	// resize _shape and _letter to proper size for OCR
	cv::resize(_letter, _letter, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));
	cv::resize(_shape, _shape, cv::Size(IMAGE_WIDTH, IMAGE_HEIGHT));

	_letter = _letter > 128;

	if (pathIsValid)
	{
		IplImage letterIpl = _letter;
		IplImage shapeIpl = _shape;
		cvSaveImage((imgPath + "_letter_bpre_debug.jpg").c_str(), &letterIpl);
		cvSaveImage((imgPath + "_shape_bpre_debug.jpg").c_str(), &shapeIpl);
	}

	
	// convert shape to an outline
	_shape = takeImageGradient(_shape);
	//_letter = takeImageGradient(_letter);

	//cv::imshow("ShapePre", _shape ); // DEBUG
	//cv::imshow("LetterPre", _letter ); // DEBUG
	//cv::waitKey(10); // DEBUG

	if (pathIsValid)
	{
		IplImage letterIpl = _letter;
		IplImage shapeIpl = _shape;
		cvSaveImage((imgPath + "_letter_cgrad_debug.jpg").c_str(), &letterIpl);
		cvSaveImage((imgPath + "_shape_cgrad_debug.jpg").c_str(), &shapeIpl);
	}
	
	checkAll();
}

/**
 * doClustering
 *	Performs k-means clustering on a two channel input, clustering to numClusters.
 *	Can either return the newly colored image or just the labels.
 *
 * Template:
 *	T			-	The type of data our matrices hold (int, float, etc)
 *
 * Input:
 *	input		-	Two channel matrix to cluster.
 *	numClusters	-	Choice of k.
 *	colored		-	Whether we want the colored image (true) or the labels (false)
 *
 * Output:
 *	Either the colored input or the labels.  Colored input has same size and type as the input.
 *	Label output is a (input_size) column vector of CV_32S containing labels.
 */
template <typename T>
cv::Mat
Auvsi_Recognize::doClustering( cv::Mat input, int numClusters, bool colored = true, cv::Mat & centers = cv::Mat() )
{
	#ifdef TWO_CHANNEL
		typedef cv::Vec<T, 2> VT;
	#else
		typedef cv::Vec<T, 3> VT;
	#endif
	
	typedef cv::Vec<int, 1> IT;
	
	const int NUMBER_OF_ATTEMPTS = 5;
	int inputSize = input.rows*input.cols;

	// Create destination image
	cv::Mat retVal( input.size(), input.type() );

	// Format input to k-means
	cv::Mat kMeansData( input );
	kMeansData = kMeansData.reshape( input.channels(), inputSize );

	// For the output of k-means
	cv::Mat labels( inputSize, 1, CV_32S );
	centers = cv::Mat( numClusters, 1, input.type() );

	// Perform the actual k-means clustering
	// POSSIBLE FLAGS: KMEANS_PP_CENTERS KMEANS_RANDOM_CENTERS
	auto criteria = cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 );
	cv::kmeans( kMeansData, numClusters, labels, criteria , NUMBER_OF_ATTEMPTS, cv::KMEANS_RANDOM_CENTERS /*KMEANS_RANDOM_CENTERS*/, centers );

	// Label the image according to the clustering results
	cv::MatIterator_<VT> retIterator = retVal.begin<VT>();
	cv::MatIterator_<VT> retEnd = retVal.end<VT>();

	cv::MatIterator_<IT> labelIterator = labels.begin<IT>();

	for( ; retIterator != retEnd; ++retIterator, ++labelIterator )
	{
		VT data = centers.at<VT>( cv::saturate_cast<int>((*labelIterator)[0]), 0);

		#ifdef TWO_CHANNEL
			*retIterator = VT( cv::saturate_cast<T>(data[0]), cv::saturate_cast<T>(data[1]) );//, cv::saturate_cast<T>(data[2]) );
		#else
			*retIterator = VT( cv::saturate_cast<T>(data[0]), cv::saturate_cast<T>(data[1]), cv::saturate_cast<T>(data[2]) );
		#endif
	}

	if( colored )
		return retVal;
	else
		return labels;
}

Auvsi_Recognize::~Auvsi_Recognize(void)
{
}

/**
 * convertToGray
 *	Converts a two channel matrix to some grayscale version that is one channel
 *
 * Input:
 *	input		-	Two channel matrix to convert.
 *
 * Output:
 *	A single channel matrix of the same type as the input that has been "grayscaled"
 *	Note that this will not look visually correct; it is just for processing purposes.
 */
cv::Mat
Auvsi_Recognize::convertToGray( cv::Mat input )
{
	cv::Mat retVal;
	cv::Mat zeros( input.rows, input.cols, CV_32F );

	std::vector<cv::Mat> splitData;
	cv::split( input, splitData );
	splitData.push_back( zeros );
	cv::merge( splitData, zeros );

	cv::cvtColor( zeros, retVal, CV_RGB2GRAY );

	return retVal;
}

template <typename T>
T 
Auvsi_Recognize::getVectorMean( std::vector<T> input )
{
	T sum = 0;
	
	for( typename std::vector<T>::iterator it = input.begin(); it != input.end(); ++it )
	{
		sum += *it;
	}
	sum = sum / (int)input.size();

	return sum;
}

cv::Mat 
Auvsi_Recognize::padImageToSquare(cv::Mat img)
{
	int maxDimension = (int)(std::max(img.rows, img.cols)*1.1);
	
	cv::Mat padded;//(cv::Size(maxDimension, maxDimension), img.type());
	int vertPadding = (maxDimension-img.rows)/2;
	int horizPadding = (maxDimension-img.cols)/2;
	cv::copyMakeBorder( img, padded, vertPadding, vertPadding, horizPadding, horizPadding, cv::BORDER_CONSTANT, cvScalar(0) );

	return padded;
}

cv::Mat
Auvsi_Recognize::centerBinary( cv::Mat input )
{
	typedef cv::Vec<unsigned char, 1> VT_binary;

	input = padImageToSquare(input);
	

	cv::Mat buffered = cv::Mat( input.rows * 2, input.cols * 2, input.type() );
	cv::Mat retVal;

	int centerX, centerY;
	int minX, minY, maxX, maxY;
	//int radiusX, radiusY;
	std::vector<int> xCoords;
	std::vector<int> yCoords;

	// Get centroid
	cv::Moments imMoments = cv::moments( input, true );

	centerX = (int) (imMoments.m10 / imMoments.m00) - buffered.cols / 2;
	centerY = (int) (imMoments.m01 / imMoments.m00) - buffered.rows / 2;

	// Get centered x and y coordinates
	cv::MatIterator_<VT_binary> inputIter = input.begin<VT_binary>();
	cv::MatIterator_<VT_binary> inputEnd = input.end<VT_binary>();

	for( ; inputIter != inputEnd; ++inputIter )
	{
		unsigned char value = (*inputIter)[0];
		if( value )
		{
			xCoords.push_back( inputIter.pos().x - centerX );
			yCoords.push_back( inputIter.pos().y - centerY );
		}
	}

	if( xCoords.size() <= 0 || yCoords.size() <= 0 ) // nothing in image
	{
		return input;
	}

	// Get min and max x and y coords (centered)
	minX = *std::min_element( xCoords.begin(), xCoords.end() );
	minY = *std::min_element( yCoords.begin(), yCoords.end() );
	maxX = *std::max_element( xCoords.begin(), xCoords.end() );
	maxY = *std::max_element( yCoords.begin(), yCoords.end() );	

	// Get new centroids
	centerX = getVectorMean<int>( xCoords );
	centerY = getVectorMean<int>( yCoords );



	// Get radius from center in each direction
	//radiusX = std::max( abs(maxX - centerX), abs(centerX - minX) );
	//radiusY = std::max( abs(maxY - centerY), abs(centerY - minY) );
	

	// Center image in temporary buffered array
	buffered = cvScalar(0);

	std::vector<int>::iterator iterX = xCoords.begin();
	std::vector<int>::iterator endX = xCoords.end();
	std::vector<int>::iterator iterY = yCoords.begin();

	int radius = 0;
	for( ; iterX != endX; ++iterX, ++iterY )
	{
		int x = *iterX, y = *iterY;

		// update radius
		int dist = (int) sqrt((float)(x - centerX)*(x - centerX) + (y - centerY)*(y - centerY));
		radius = std::max(radius, dist);

		// store pixels into image
		buffered.at<VT_binary>( y, x ) = VT_binary(255);
	}

	// Center image
	const int border = (int) (radius*.2);
	buffered = buffered.colRange( centerX - radius - border, centerX + radius + border );
	buffered = buffered.rowRange( centerY - radius - border, centerY + radius + border );

	// Add extra padding to make square
	int outH, outW;
	outH = buffered.rows;
	outW = buffered.cols;
	
	if( outH < outW ) // pad height
		cv::copyMakeBorder( buffered, retVal, (outW-outH)/2, (outW-outH)/2, 0, 0, cv::BORDER_CONSTANT, cvScalar(0) );
	else // pad width
		cv::copyMakeBorder( buffered, retVal, 0, 0, (outH-outW)/2, (outH-outW)/2, cv::BORDER_CONSTANT, cvScalar(0) );

	// resize to same as input only if input was square! otherwise this will distort shapes
	//cv::resize( retVal, buffered, input.size(), 0, 0, cv::INTER_NEAREST );
	
	return buffered;
}

/**
 * extractShape
 *	Extracts a binary image containing just the shape.
 *
 * Template:
 *	T			-	The type of data our matrices hold (int, float, etc)
 *
 * Input:
 *	void
 *
 * Output:
 *	void
 */
template <typename T>
void
Auvsi_Recognize::extractShape( void )
{
	typedef cv::Vec<T, 1> VT;
	#ifdef TWO_CHANNEL
		typedef cv::Vec<T, 2> VTTT;
		const int numChannels = 2;
	#else
		typedef cv::Vec<T, 3> VTTT;
		const int numChannels = 3;
	#endif

	// Reduce input to two colors
	cv::Mat shapeColors;
	cv::Mat reducedColors = doClustering<T>( _image, 2, true, shapeColors );
	cv::Mat grayScaled, binary;

	//cv::imshow("_image",_image);
	//cv::imshow("imgMed",imgMed);
	//cv::waitKey(10);

	// Make output grayscale
	grayScaled = convertToGray( reducedColors );
	//cv::cvtColor( reducedColors, grayScaled, CV_RGB2GRAY );

	// Make binary
	double min, max;
	cv::minMaxLoc( grayScaled, &min, &max );
	cv::threshold( grayScaled, binary, min, 1.0, cv::THRESH_BINARY );	

	// ensure that background is black, image white
	if( binary.at<VT>(0, 0)[0] > 0.0f )
		cv::threshold( grayScaled, binary, min, 1.0, cv::THRESH_BINARY_INV );

	binary.convertTo( binary, CV_8U, 255.0f );

	// Fill in all black regions smaller than largest black region with white
	CBlobResult blobs;
	CBlob * currentBlob;
	IplImage binaryIpl = binary;
	blobs = CBlobResult( &binaryIpl, NULL, 255 );
	
	// Get area of biggest blob
	CBlob biggestBlob;
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );

	// Remove all blobs of smaller area
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, biggestBlob.Area() );

	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
    	currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( &binaryIpl, cvScalar(255));
	}
	


	// Fill in all small white regions black 
	blobs = CBlobResult( &binaryIpl, NULL, 0 );
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );

	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, biggestBlob.Area() );

	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
    	currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( &binaryIpl, cvScalar(0));
	}

	binary = cv::Scalar(0);
	biggestBlob.FillBlob( &binaryIpl, cvScalar(255));

	_shape = binary;
}

cv::Mat
Auvsi_Recognize::takeImageGradient(cv::Mat img)
{

	cv::Mat thresholdedImage = img > 0;

	cv::Mat imgFloat;
	thresholdedImage.convertTo(imgFloat, CV_32F);

	// take gradient of image using Sobel operator
	cv::Mat gradX;
	cv::Mat gradY;

	cv::Mat derivativeKernel = cv::Mat::ones(2,1,CV_32FC1);
	derivativeKernel.at<cv::Vec<float, 1>>(1,0)[0] = -1;
	cv::Point anchor = cv::Point(0,0);
	
	cv::filter2D(imgFloat, gradX, -1, derivativeKernel.t(), anchor);
	cv::filter2D(imgFloat, gradY, -1, derivativeKernel, anchor);

	// take abs of gradient to get edges
	cv::Mat grad = cv::abs(gradX) + cv::abs(gradY); // DEBUG
	
	//grad = grad > 0.1f;
	
	// save result
	grad.convertTo(img, CV_8U);

	return img;
}

template <typename T> 
cv::Mat
Auvsi_Recognize::customErode(cv::Mat input)
{
	typedef cv::Vec<T, 1> VT;

	
	// erode + top hat
	cv::Mat kern = cv::Mat(cv::Size(3,3), CV_8U);
	kern.setTo(255);
	kern.at<VT>(0,0)[0] = 0;
	kern.at<VT>(0,2)[0] = 0;
	kern.at<VT>(2,0)[0] = 0;
	kern.at<VT>(2,2)[0] = 0;

	//cv::dilate(input, input, kern);
	//cv::dilate(input, input, kern);
	for (int i=0; i < 1; ++i)
	{
		cv::Mat eroded;
		cv::erode(input, eroded, kern);

		cv::Mat topHat;
		cv::morphologyEx(input, topHat, cv::MORPH_TOPHAT, kern);

		cv::bitwise_or(eroded, topHat, input);
		//cv::imshow("_letter", _letter);
		//cv::waitKey(500);
	}

	/*for (int x = 0; x < input.cols; ++x)
	{
		for (int y = 0; y < input.rows; ++y)
		{
			bool thisPixel = pixelIsValid<T>(input,x,y);
			bool rightNeighbor = pixelIsValid<T>(input,x+1,y);
			bool bottomNeighbor = pixelIsValid<T>(input,x,y+1);
			bool leftNeighbor = pixelIsValid<T>(input,x-1,y);
			//if (thisPixel && rightNeighbor && bottomNeighbor)
			bool isSkinnyLine = !leftNeighbor && !rightNeighbor;
			if (bottomNeighbor && !isSkinnyLine)
				input.at<VT>(y,x)[0] = 0;
		}
	}*/
	return input;
}

template <typename T> 
bool 
Auvsi_Recognize::pixelIsValid(cv::Mat img, int x, int y)
{
	typedef cv::Vec<T, 1> VT;

	bool pointIsWithinImage = ((x >= 0) && (y >= 0) && (x < img.cols) && (y < img.rows));
	if (pointIsWithinImage)
		if (img.at<VT>(y,x)[0] > 0)
			return true;
	return false;
}

template <typename T> 
void 
Auvsi_Recognize::takeShapeBorder( void )
{
	// this takes _shape, extracts the border, and stores it back in _shape
	_shape = takeImageGradient(_shape);

}

bool 
Auvsi_Recognize::hasGoodLetter()
{
	return cv::sum(_letter)[0]/255.0f > 10.0f;
}

template <typename T> 
void 
Auvsi_Recognize::erodeLetter( void )
{
	typedef cv::Vec<unsigned char, 1> VT;

	// NOTE: this isn't used. it was a failed experiment

	// erode _letter such that every line is one pixel thick
	// assume every letter is no more than 20 pixels thick

	cv::imshow("orig", _letter.clone());

	// erode + top hat
	cv::Mat kern = cv::Mat(cv::Size(3,3), CV_8U);
	kern.setTo(255);
	kern.at<VT>(0,0)[0] = 0;
	kern.at<VT>(0,2)[0] = 0;
	kern.at<VT>(2,0)[0] = 0;
	kern.at<VT>(2,2)[0] = 0;

	cv::dilate(_letter, _letter, kern);
	cv::dilate(_letter, _letter, kern);
	for (int i=0; i < 7; ++i)
	{
		cv::Mat eroded;
		cv::erode(_letter, eroded, kern);

		cv::Mat topHat;
		cv::morphologyEx(_letter, topHat, cv::MORPH_TOPHAT, kern);

		cv::bitwise_or(eroded, topHat, _letter);
		cv::imshow("_letter", _letter);
		cv::waitKey(500);
	}
}

/**
 * extractLetter
 *	Extracts a binary image containing just the letter.  Must be run after extractShape.
 *
 * Template:
 *	T			-	The type of data our matrices hold (int, float, etc)
 *
 * Input:
 *	void
 *
 * Output:
 *	void
 */
template <typename T>
void
Auvsi_Recognize::extractLetter( void )
{
	typedef cv::Vec<unsigned char, 1> VT_binary;
	#ifdef TWO_CHANNEL
		typedef cv::Vec<T, 2> VT;
		const int numChannels = 2;
	#else
		typedef cv::Vec<T, 3> VT;
		const int numChannels = 3;
	#endif
	typedef cv::Vec<int, 1> IT;

	if (_image.type() != CV_32FC3)
		_image.convertTo(_image, CV_32FC3);

	// make shape extended - color image where bg is same color as shape
	cv::Scalar shapeColorScalar = cv::mean(_image, _shape);
	VT shapeColor((float)shapeColorScalar[0], (float)shapeColorScalar[1], (float)shapeColorScalar[2]);
	_shapeExtended = cv::Mat( _image.size(), _image.type() );
	_shapeExtended.setTo(shapeColor);
	_image.copyTo(_shapeExtended,_shape);
	//_letter = _image; // DEBUG


	// do blob detection on shapeExtended
	//if (false) // DEBUG
	{
		// subtract shape color
		cv::add(_shapeExtended, -shapeColor, _shapeExtended);

		// abs
		_shapeExtended = cv::abs(_shapeExtended);

		// split channels
		std::vector<cv::Mat> shapeChannels;
		cv::split(_shapeExtended, shapeChannels);

		// calc distance of each pixel from 0 (0 is shape color) ... 

		// get sum**2 channels
		cv::Mat total = cv::Mat(_shapeExtended.size(), CV_32FC1);
		for (int i = 0; i < numChannels; ++i)
		{
			cv::Mat chn = shapeChannels[i];

			// chn = chn**2
			cv::multiply(chn, chn, chn);

			// total += chn
			cv::add(chn, total, total);
		}

		// total = sqrt(channels**2)
		cv::sqrt(total, total);

		// threshold
		double minDist, maxDist;
		cv::minMaxIdx(total, &minDist, &maxDist);

		maxDist = std::max(maxDist, 5.0);

		float threshold = (float)(maxDist/3.0); 

		cv::threshold(total, total, threshold, 255.0, cv::THRESH_BINARY);

		total.convertTo(_letter, CV_8U);
	
	}


	// get blob
	//if (false) // DEBUG
	{
		// Remove any small white blobs left over
		CBlobResult blobs;
		//CBlob * currentBlob;
		CBlob biggestBlob;
		IplImage binaryIpl = _letter;
	
		blobs = CBlobResult( &binaryIpl, NULL, 0 );
		blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
	
		//blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, biggestBlob.Area() );
	
		//for (int i = 0; i < blobs.GetNumBlobs(); i++ )
		//{
    	//	currentBlob = blobs.GetBlob(i);
		//	currentBlob->FillBlob( &binaryIpl, cvScalar(0));
		//}

		// set everything outside the letter to 0
		cv::Mat letterMask = cv::Mat(_letter.size(), _letter.type());
		letterMask.setTo(255);
		binaryIpl = letterMask;
		biggestBlob.FillBlob(&binaryIpl, cvScalar(0));

		_letter.setTo(0.0, letterMask);
	}
}
