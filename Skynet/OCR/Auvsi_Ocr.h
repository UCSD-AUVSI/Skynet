/*
 *  Auvsi_OCRWrapper.h
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 6/5/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */
 
#pragma once
#include <cv.h>
//#include <highgui.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "Auvsi_DualTree.h"
#include "Auvsi_Fft.h"
#include "Auvsi_Radon.h"
#include "Auvsi_Recognize.h"

class Auvsi_Ocr {
public:
	Auvsi_Ocr();	// constructor
	~Auvsi_Ocr();	// destructor

	void segmentImageIntoShapeAndLetter(cv::Mat colorImg, cv::Mat & letter, cv::Mat & shape, std::string filepath="");
	static cv::Mat prepareTrainingImage(cv::Mat image,bool isShape=false, std::string imgPath="");
	void computeFeatureDescriptor(cv::Mat blackWhiteImg, float ** featDesc);

	//void processImage(cv::Mat image, float **letterFeatDesc, float **shapeFeatDesc, std::string filepath="");	// process the image
	//void processTrainingImageForLetter(cv::Mat image, float **letterFeatDesc, std::string imgPath="")
		//{ processTrainingImage(image, letterFeatDesc, false, imgPath); }
	void processTrainingImageForShape(cv::Mat image, float **shapeFeatDesc, std::string imgPath="")
		{ processTrainingImage(image, shapeFeatDesc, true, imgPath); }
private:	// objects used for processing
	Auvsi_DualTree *theDualTree;
	Auvsi_Radon *theRadon;
	Auvsi_Fft *theFft;
	Auvsi_Recognize *theRecognize;
	
	void processTrainingImage(cv::Mat image, float **featDesc, bool isShape, std::string imgPath="");
};