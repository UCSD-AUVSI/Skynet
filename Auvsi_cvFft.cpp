/*
 *  Auvsi_Fft.cpp
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 6/2/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#include "Auvsi_cvFft.h"


Auvsi_Fft::Auvsi_Fft()
{
	// dont need to do anything
}


// set input image (result of dualtree)
void 
Auvsi_Fft::setImage(cv::Mat input)
{
	inputImage = input;	// save input
}

// clear old results
void Auvsi_Fft::clearFeatureDescriptor()
{
	// loop through array
	for (int i = 0; i < LENGTH_FEATURE_DESCRIPTOR; i++) {
		featureDescriptor[i] = 0.0;
	}
}

// Get specified column from inputImage
Complex *
Auvsi_Fft::getColumn(int columnNumber)
{	
	typedef cv::Vec<float, 2> VT;
	
	// allocate column
	Complex * column = new Complex[inputImage.size().height];	

	VT * ptrBegin;

	// loop down the image
	for (int i = 0; i < inputImage.size().height; i++) {
		ptrBegin = inputImage.ptr<VT>(i);		// pointer to element
		Complex number;
		number.x = ptrBegin[columnNumber][0];	// make complex number
		number.y = ptrBegin[columnNumber][1];
		column[i] = number;						// add to array
	}	

	return column; // return the column
}

// Take FFT of given Complex array
Complex *
Auvsi_Fft::computeFFT(Complex *input, int length)
{
	typedef cv::Vec<float, 2> VT;

	cv::Mat src = cv::Mat::Mat(length, 1, CV_32FC2, (void *)input); // convert input to cv::Mat matrix
	cv::Mat dst = cv::Mat::Mat(length, 1, CV_32FC2);				// allocate result
	cv::dft(src, dst);												// compute forward fft;
	
	Complex  * result = new Complex[length];	// allocate result array
	VT * ptrBegin = dst.ptr<VT>(0);				// pointer to image data

	// loop through elements
	for (int i = 0; i < length; i++)
	{
		Complex number;
		number.x = ptrBegin[i][0];
		number.x = ptrBegin[i][1];
		result[i] = number;			// save Complex number in array
	}

	return result;	
}

// Add given FFT to feature descriptor in specified location
void
Auvsi_Fft::addToFeatureDescriptor(Complex *input, int lineNumber)
{
	float x, y, mag; 
	int j;
	
	// loop through elements to change
	for (int i = NUMBER_COEFFICIENTS_TO_KEEP*lineNumber, j = 0; 
					i < NUMBER_COEFFICIENTS_TO_KEEP*(lineNumber + 1); i++, j++) {
		x = input[j].x;
		y = input[j].y;
		mag = sqrt(x*x + y*y);			// abs(complexNumber)
		
		featureDescriptor[i] = mag ;	// save output
	}


}

// run ffts, keep certain results, combine into feature descriptor
void 
Auvsi_Fft::computeFeatureDescriptor()
{
	clearFeatureDescriptor();
	Complex * currentColumn;	// current values to compute
	Complex * currentResults;	// current FFT results
	
	if (!COMPUTE_ROW) {		// compute each column
		
		// loop through columns
		for (int i = 0; i < inputImage.size().width; i++) {
			currentColumn = getColumn(i);								// get column
			currentResults = computeFFT(currentColumn, HEIGHT_INPUT);	// take fft of column
			addToFeatureDescriptor(currentResults, i);					// add results to feature descriptor
		}
	}
	
	// we are done! 
	
}













