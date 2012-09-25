/*
 *  Auvsi_Fft.h
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 6/2/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#pragma once
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)
#include "Auvsi_DualTree.h"

#define WIDTH_INPUT 16
#define HEIGHT_INPUT 128
#define COMPUTE_ROW false
#define NUMBER_COEFFICIENTS_TO_KEEP 20
#define LENGTH_FEATURE_DESCRIPTOR NUMBER_COEFFICIENTS_TO_KEEP*WIDTH_INPUT


class Auvsi_Fft {
private:
	cv::Mat inputImage;									// 128*16 (h,w) (Complex)
	float featureDescriptor[LENGTH_FEATURE_DESCRIPTOR]; // holds results of computatio
	
public:
	Auvsi_Fft();						// constructor
	void setImage(cv::Mat input);		// set input image (result of dualtree)
	void computeFeatureDescriptor();	// run ffts, keep certain results, combine into feature descriptor	
	float * getFeatureDescriptor() { return featureDescriptor;}	// access results
	
	float * computeFeatureDescriptor(cv::Mat image) // overloaded, does all at once
		{ setImage(image); computeFeatureDescriptor(); return getFeatureDescriptor(); }
private:
	void clearFeatureDescriptor();
	Complex * getColumn(int columnNumber);
	Complex * computeFFT(Complex *input, int length);
	void addToFeatureDescriptor(Complex *input, int lineNumber);

};