#pragma once

#include "Auvsi_FFT.h"
/******************************************************************************
 * Important Constants
 *****************************************************************************/
// Saliency Related - weights should sum to 1.0
#define SALIENCY_WEIGHT_COLOR_L (1.0f/3.0f)
#define SALIENCY_WEIGHT_COLOR_A (1.0f/6.0f)
#define SALIENCY_WEIGHT_COLOR_B (1.0f/6.0f)
#define SALIENCY_WEIGHT_GABOR   (1.0f/3.0f)

// Number of filters
#define NUM_GAUSSIAN_FILTERS 8
#define NUM_GABOR_FILTERS    8
#define MAX_FILTER_W 311
#define MAX_FILTER_H 311

/**
 * Notes:
 *	The following FULL_WIDTH values for various kernels were pre computed
 *	in order to speed up computation.
 *
 *	Sigma	|	FULL_WIDTH	|	NOTES
 *	2			13				inhibition filter
 *	25			151				exhibition filter
 *	4			25				4 through 32*1.6 are for DoG filters
 *	4*1.6		39
 *	8			49
 *	8*1.6		77
 *	16			97
 *	16*1.6		155
 *	32			195
 *	32*1.6		311
 *  2*PI		39				smaller gabor
 *	4*PI		77				larger gabor
 */

class Auvsi_Matrix;
class Auvsi_Image;
class Auvsi_Filter;

class __declspec(dllexport) Auvsi_Saliency
{
public:
	Auvsi_Saliency( void );
	~Auvsi_Saliency( void );

	void setInformation( int imageW, int imageH );

	void deleteFilters( void );
	void deleteBuffers( void );

	void prepareFilters( void );

	int doNothing(void) { return 1; }
	
	void loadImage(const char * file);
	void loadImage(int width, int height, void * data);
	void getResult(void);

	void saveImages(void);
	void prepareForSaliency(void);

	void computeSaliency(float threshold);
	void labelConnectedComponents();
	void getFinalData(float * buffer, box * &boundingBoxes, int &numBoxes);

	void *finalImageBuffer;

	int testFunc();
	
protected:
	// Utility functions - should probably get moved into Util later
	void minMaxValue( Auvsi_Matrix * src, float & minValue, float & maxValue );
	void normalize( Auvsi_Matrix * src, float lowerBound, float upperBound );
	void writeToFile(const char * input);
	// Saliency functions
	void accumulateSaliencyChannels(	
		Auvsi_Matrix * dst, 
		Auvsi_Matrix * colorL, 
		Auvsi_Matrix * colorA, 
		Auvsi_Matrix * colorB, 
		Auvsi_Matrix * colorG,
		float threshold
		);

	void colorConvert( 
		Auvsi_Matrix * src, 
		Auvsi_Matrix * colorL, 
		Auvsi_Matrix * colorA, 
		Auvsi_Matrix * colorB, 
		Auvsi_Matrix * colorG,
		Auvsi_Filter * filter
    );
	
	void buildDoGFilter( void );

	void processDoG( Auvsi_Matrix * src );
	void processDoGUsingFFT( Auvsi_Matrix * src );
	void processGabor( Auvsi_Matrix * src );
	void processGaborUsingFFT( Auvsi_Matrix * src );


	// Filter prep functions
	

private:
	// temporary matrices for holding split channels
	Auvsi_Matrix * _colorL;
	Auvsi_Matrix * _colorA;
	Auvsi_Matrix * _colorB;
	Auvsi_Matrix * _colorG;

	// filters
	int _maxFilterWidth;
	int _maxFilterHeight;
	Auvsi_Filter * _mainGaussianFilter;
	Auvsi_Filter * _gaussianFilters[NUM_GAUSSIAN_FILTERS];
	Auvsi_Filter * _gaborFilters[NUM_GABOR_FILTERS];

	// images
	int _imageW;
	int _imageH;
	int _fftWidth;
	int _fftHeight;
	int _imagePitch;	

	Auvsi_Image * _inputImage;
	Auvsi_Matrix * _finalImage;	

	// Bounding boxes
	box * _boundingBoxes;
	int _numBoxes;
};
