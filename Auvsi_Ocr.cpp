/*
 *  Auvsi_Ocr.cpp
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 6/5/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#include "Auvsi_Ocr.h"

// constructor
Auvsi_Ocr::Auvsi_Ocr()
{
	//theRecognize = new Auvsi_Recognize();
	theRadon = new Auvsi_Radon();
	theDualTree = new Auvsi_DualTree();
	theFft = new Auvsi_Fft();
}

// destructor
Auvsi_Ocr::~Auvsi_Ocr()
{
	// free memory
	delete theRecognize;
	delete theRadon;
	delete theDualTree;
	delete theFft;
}


// process the given image, save results in given arrays
void 
Auvsi_Ocr::processImage(cv::Mat image, float **letterFeatDesc, float **shapeFeatDesc)
{
//	IplImage *imageI = cvGetImage(image);
	Auvsi_Recognize *recognize = new Auvsi_Recognize(image);
	//theRecognize->setImage(image);	// set image for segmentation
	recognize->runComputation();	// run segmentation
	// calculate feature descriptor for letter and shape
	*letterFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter)));
	*shapeFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_shape)));

	delete recognize;
}
