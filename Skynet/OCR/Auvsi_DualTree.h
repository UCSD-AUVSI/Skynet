/*
 *  Auvsi_DualTree.h
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 5/29/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */
#pragma once
//#include <cv.h>
//#include <highgui.h>
#include <opencv2/opencv.hpp>
#include "Auvsi_Recognize.h"
#include "Auvsi_cvUtil.h"

struct Complex {
	float x;
	float y;
	};

class Auvsi_DualTree {
private:
	cv::Mat outputImage;	// 128*16 (h,w) (Complex)
	cv::Mat inputImage;		// 128*64 (height, width) computing the 64
	int stages;				// 4 stages
	int currentLine;		// which line to process in inputImage
	bool isRow;				// whether to compute row or column
	float	*inputVector;	// line being computed
	Complex passOne[32];		// results of various passes
	Complex passTwo[16];
	Complex passThree[8];
	Complex passFour[4];
	Complex passFive[4];
	float filterFafLo[2][10];	// see FSfarras.m
	float filterFafHi[2][10];	// see FSfarras.m
	float filterafLo[2][10];	// see dualfilt1.m
	float filterafHi[2][10];	// see dualfilt1.m

public:
	Auvsi_DualTree();	// constructor
	void setImage(cv::Mat image);	// set image for processing
	void setWhetherRow(bool isItRow) {isRow = isItRow;}	// set line to process
	void runDualTree();
	cv::Mat getResults() { return outputImage;}			// return results
	
	cv::Mat runDualTree(cv::Mat image, bool isItRow = false)	// overloaded - does all at once
		{ setImage(image); runDualTree(); return getResults();}

private:
	void fillInputVector(int lineNumber);
	void computeLine();
	void addToResults(int lineNumber);
	void clearResults();
	float * afb(float * input, float filterLo[10], float filterHi[10],  int pass, int tree);
	float * cshift(float * input, int length, int shift);
	float * runUpfirdn(float *input, int inLength, float *filter, int filterLength);
};