/*
 *  Auvsi_Radon.h
 *  OpenCV2Test
 *
 *  Created by Lewis Anderson on 5/26/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */
#pragma once
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <highgui.h>
#pragma warning(pop)
#include "Auvsi_Recognize.h"
#include "Auvsi_cvUtil.h"

class Auvsi_Radon {
public:
	int numLines;			// 64 * 2
	cv::Mat radon;			// 64*2 lines, 64 pts/line  holds results of radon transform (each row a line)
private:
	int numPointsOnLine;	// 64
	float * lines;			// size = [numLines], holds angle of each line
	cv::Mat image;			// image to be computed
public:
	Auvsi_Radon();			// no argument constructor
	~Auvsi_Radon();			// destructor
	cv::Mat getResults() { return radon;}
	void computeRadon();

	void setImage(cv::Mat inputImage);
	
	cv::Mat computeRadon(cv::Mat inputImage)	// overloaded, does everything
		{setImage(inputImage); computeRadon(); return getResults();}
private:
	void clearImage();
	void squashPixel(float position, int line);
	void projectPixel(int x, int y, int line);
	void computePixel(int x, int y);

	
};
