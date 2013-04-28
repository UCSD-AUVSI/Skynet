/*
 *  Auvsi_Ocr.cpp
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 6/5/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#include "Auvsi_OCR.h"
#include "Auvsi_cvUtil.h"
#include <cv.h>

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
	//delete theRecognize;
	delete theRadon;
	delete theDualTree;
	delete theFft;
}

void 
Auvsi_Ocr::segmentImageIntoShapeAndLetter(cv::Mat colorImg, cv::Mat & letter, cv::Mat & shape, std::string filepath)
{
	throw new std::exception("dont use this anymore. use ShapeFinder");
	Auvsi_Recognize *recognize = new Auvsi_Recognize(colorImg);
	recognize->runComputation(filepath);
	shape = recognize->_shape;
	if (recognize->hasGoodLetter())
		letter = recognize->_letter;
	else
		letter = cv::Mat();
	delete recognize;
}

void 
Auvsi_Ocr::computeFeatureDescriptor(cv::Mat blackWhiteImg, float ** featDesc)
{
	*featDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(blackWhiteImg)));
}



/*
// process the given image, save results in given arrays
void 
Auvsi_Ocr::processImage(cv::Mat image, float **letterFeatDesc, float **shapeFeatDesc, std::string filepath)
{
//	IplImage *imageI = cvGetImage(image);
	Auvsi_Recognize *recognize = new Auvsi_Recognize(image);
	//theRecognize->setImage(image);	// set image for segmentation
	recognize->runComputation(filepath);	// run segmentation
	// calculate feature descriptor for letter and shape
	if (recognize->hasGoodLetter())
		*letterFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter)));
	else
		*letterFeatDesc = 0;
	*shapeFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_shape)));

	delete recognize;
}*/

/**
	Input: 1-Channel Grayscale Image
	Output: 1-Channel 8UC1 Inverted Binary Image
**/
cv::Mat Auvsi_Ocr::prepareTrainingImage(cv::Mat image, bool isShape, std::string imgPath)
{
	cv::Mat toProcess(image.size(),CV_8UC1);
	toProcess = image < 128;
	toProcess = Auvsi_Recognize::ProcessShape(toProcess);

	bool pathIsValid = imgPath.length() > 0;
	if (pathIsValid)
	{
		cv::Mat aClone = toProcess.clone();
		cv::circle(aClone, cv::Point(31,31), 32, cv::Scalar(128));
		IplImage imageIpl = aClone;
		
		cvSaveImage((imgPath + "_debug.jpg").c_str(), &imageIpl);
	}

	//cv::imshow("img",toProcess);
	//cv::waitKey(10);

	if (!Auvsi_Recognize::checkImage(toProcess))
		printf("WARNING: bad image in Auvsi_Ocr.cpp"); // this may not show up in Skynet

	return toProcess;
}

// process the given image, save results in given arrays
void 
Auvsi_Ocr::processTrainingImage(cv::Mat image, float **featDesc, bool isShape, std::string imgPath)
{
	cv::Mat thresholdedImage = prepareTrainingImage(image, isShape, imgPath);

	computeFeatureDescriptor(thresholdedImage, featDesc);
}