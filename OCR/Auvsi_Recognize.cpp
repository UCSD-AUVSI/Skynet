#include "Auvsi_Recognize.h"
#include "BlobResult.h"
#include <vector>
#include <algorithm>
#include <iostream>
#include "Auvsi_cvUtil.h"


// Whether we want to use just color channels or not
//#define TWO_CHANNEL 1

/**
 * Constructor for testing using an IplImage
 */
Auvsi_Recognize::Auvsi_Recognize( IplImage * img )
{
	// std::printf("begining recognize constructor\n");

	// Set equal to image
	cv::Mat _input =  img ;
	
	cv::Mat temp2( _input.size(), CV_32F );
	_image = cv::Mat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F );

	_input.convertTo( temp2, _image.type(), 1.0/255.0f );
	cv::imshow( "Input", _input );

	cv::resize(temp2, _image, _image.size() );
	cv::cvtColor( _image, _image, CV_RGB2Lab );
	//std::printf("converted color in constructor\n");

	// Only use A and B channels
	#ifdef TWO_CHANNEL
		std::vector<cv::Mat> splitData;
		cv::split( _image, splitData );
		splitData.erase( splitData.begin() );
		cv::merge( splitData, _image );
	#endif
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

	cv::resize(temp, _image, _image.size() );

	// Convert to LAB
	cv::cvtColor( _image, _image, CV_RGB2Lab );
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
	_image = cv::Mat( IMAGE_HEIGHT, IMAGE_WIDTH, CV_32F );

	_input.convertTo( temp2, _image.type(), 1.0/255.0f );

	cv::imshow( "Input", _input );

	cv::resize(temp2, _image, _image.size() );
	cv::cvtColor( _image, _image, CV_RGB2Lab );
	//std::printf("converted color in set image \n");

	// Only use A and B channels
	#ifdef TWO_CHANNEL
		std::vector<cv::Mat> splitData;
		cv::split( _image, splitData );
		splitData.erase( splitData.begin() );
		cv::merge( splitData, _image );
	#endif


}

void
Auvsi_Recognize::runComputation( void )
{


	//cv::imshow("ShapePre", _shape );
	//cv::imshow("LetterPre", _letter );


	

	//cvWaitKey(0);
	
	
	// GOOD code:

	extractShape<float>();
	extractLetter<float>();
	


	_shape = centerBinary(_shape);
	_letter = centerBinary(_letter);
	
	cv::imshow("ShapePre", _shape );
	cv::imshow("LetterPre", _letter );
	
	checkAll();
	
	//cv::imshow( "Shape", centerBinary( _shape ) );
	//cv::imshow( "Letter", centerBinary( _letter ) );
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
Auvsi_Recognize::doClustering( cv::Mat input, int numClusters, bool colored = true )
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
	cv::Mat centers( numClusters, 1, input.type() );

	// Perform the actual k-means clustering
	// POSSIBLE FLAGS: KMEANS_PP_CENTERS KMEANS_RANDOM_CENTERS
	cv::kmeans( kMeansData, numClusters, labels, cv::TermCriteria( CV_TERMCRIT_EPS+CV_TERMCRIT_ITER, 10, 1.0 ), NUMBER_OF_ATTEMPTS, cv::KMEANS_RANDOM_CENTERS, centers );

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
	sum = sum / input.size();

	return sum;
}

cv::Mat
Auvsi_Recognize::centerBinary( cv::Mat input )
{
	typedef cv::Vec<unsigned char, 1> VT_binary;

	cv::Mat buffered = cv::Mat( input.rows * 2, input.cols * 2, input.type() );
	cv::Mat retVal;

	int centerX, centerY;
	int minX, minY, maxX, maxY;
	int radiusX, radiusY;
	std::vector<int> xCoords;
	std::vector<int> yCoords;

	// Get centroid
	cv::Moments imMoments = cv::moments( input, true );

	centerX = (imMoments.m10 / imMoments.m00) - buffered.cols / 2;
	centerY = (imMoments.m01 / imMoments.m00) - buffered.rows / 2;

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
	radiusX = std::max( abs(maxX - centerX), abs(centerX - minX) );
	radiusY = std::max( abs(maxY - centerY), abs(centerY - minY) );

	// Center image in temporary buffered array
	buffered = cvScalar(0);

	std::vector<int>::iterator iterX = xCoords.begin();
	std::vector<int>::iterator endX = xCoords.end();
	std::vector<int>::iterator iterY = yCoords.begin();

	for( ; iterX != endX; ++iterX, ++iterY )
	{
		buffered.at<VT_binary>( *iterY, *iterX ) = VT_binary(255);
	}

	// Center image
	buffered = buffered.colRange( centerX - radiusX, centerX + radiusX + 1 );
	buffered = buffered.rowRange( centerY - radiusY, centerY + radiusY + 1 );

	// Add extra padding to make square
	int outH, outW;
	outH = buffered.rows;
	outW = buffered.cols;
	
	if( outH < outW ) // pad height
		cv::copyMakeBorder( buffered, retVal, (outW-outH)/2, (outW-outH)/2, 0, 0, cv::BORDER_CONSTANT, cvScalar(0) );
	else // pad width
		cv::copyMakeBorder( buffered, retVal, 0, 0, (outH-outW)/2, (outH-outW)/2, cv::BORDER_CONSTANT, cvScalar(0) );

	// Make sure output is desired width
	cv::resize( retVal, buffered, input.size(), 0, 0, cv::INTER_NEAREST );
	
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

	// Reduce input to two colors
	cv::Mat reducedColors = doClustering<T>( _image, 2 );	
	cv::Mat grayScaled, binary;

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
	#else
		typedef cv::Vec<T, 3> VT;
	#endif
	typedef cv::Vec<int, 1> IT;
	

	
	
	
	
	// Erode input slightly
	cv::Mat input;
	cv::erode( _shape, input, cv::Mat() );

	// Remove any small white blobs left over
	CBlobResult blobs;
	CBlob * currentBlob;
	CBlob biggestBlob;
	IplImage binaryIpl = input;
	
	blobs = CBlobResult( &binaryIpl, NULL, 0 );
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
	
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, biggestBlob.Area() );
	
	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
    	currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( &binaryIpl, cvScalar(0));
	}
	
	// Perform k-means on this region only
	int areaLetter = (int)biggestBlob.Area();
	cv::Mat kMeansInput = cv::Mat( areaLetter, 1, _image.type() );

	// Discard if we couldn't extract a letter
	if( areaLetter <= 0 )
	{
		_letter = cv::Mat( _shape );
		_letter = cv::Scalar(0);
		return;
	}

	cv::MatIterator_<VT_binary> binaryIterator = input.begin<VT_binary>();
	cv::MatIterator_<VT_binary> binaryEnd = input.end<VT_binary>();
	cv::MatIterator_<VT> kMeansIterator = kMeansInput.begin<VT>();

	for( ; binaryIterator != binaryEnd; ++binaryIterator )
	{
		if( (*binaryIterator)[0] > 0 )
		{
			(*kMeansIterator) = _image.at<VT>( binaryIterator.pos() );
			++kMeansIterator;
		}
	}

	// Get k-means labels
	cv::Mat labels = doClustering<T>( kMeansInput, 2, false );	
	int numZeros = areaLetter - cv::countNonZero( labels );
	bool useZeros = numZeros < cv::countNonZero( labels );

	// Reshape into original form
	_letter = cv::Mat( _shape.size(), _shape.type() );
	_letter = cv::Scalar(0);

	binaryIterator = input.begin<VT_binary>();
	binaryEnd = input.end<VT_binary>();
	cv::MatIterator_<IT> labelsIterator = labels.begin<IT>();

	for( int index = 0; binaryIterator != binaryEnd; ++binaryIterator )
	{
		if( (*binaryIterator)[0] > 0 )
		{
			// Whichever label was the minority, we make that value white and all other values black
			unsigned char value = (*labelsIterator)[0];

			if( useZeros )
				if( value )
					value = 0;
				else
					value = 255;
			else
				if( value )
					value = 255;
				else
					value = 0;

			_letter.at<VT_binary>( binaryIterator.pos() ) = VT_binary( value );
			++labelsIterator;
		}
	}
	
	// Attempt to deal with any spurious locations that are left
	//	If we can be fairly confident that one of the blobs left is not a letter, remove it
	double confidence = 0.50; // how much we trust that the biggest blob is the letter
	binaryIpl = _letter;
	
	blobs = CBlobResult( &binaryIpl, NULL, 0 );
	blobs.GetNthBlob( CBlobGetArea(), 0, biggestBlob );
	
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_GREATER_OR_EQUAL, biggestBlob.Area() * confidence );
	
	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
		currentBlob = blobs.GetBlob(i);
		currentBlob->FillBlob( &binaryIpl, cvScalar(0));
	}
}