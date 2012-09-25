/*
 *  Auvsi_cvUtil.cpp
 *  OpenCV2Test
 *
 *  Created by Lewis Anderson on 5/26/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#include "Auvsi_cvUtil.h"
#include <vector>
#include <iostream>




using namespace cv;
using namespace util;

cv::Vec<float, 1> getValuef(cv::Mat image, float x, float y) 
{
	// the following assumes the origin is (0,0), and the first pixel is at (0.5,0.5)
	typedef Vec<float, 1> VT;
	
	float minx = x - .5;
	float miny = x - .5;
	
	float xThresh = (int)x; // line dividing left half of pixels from right half
	float yThresh = (int)y; // line dividing top half from bottom half
	
	float leftWeight = xThresh - minx;
	float bottomWeight = yThresh - miny;
	
	// get the four pixels around it: 
	float botLeft = image.at<VT>(xThresh - 1, yThresh - 1)[0];
	float botRight = image.at<VT>(xThresh, yThresh - 1)[0];
	float topLeft = image.at<VT>(xThresh - 1, yThresh)[0];
	float topRight = image.at<VT>(xThresh, yThresh)[0];	

	float result = botLeft*leftWeight*bottomWeight + botRight*rightWeight*bottomWeight + 
			topLeft*leftWeight*topWeight + topRight*rightWeight*topWeight;
	return result;
}

Vec<float, 2> getValuec(Mat image, float x, float y)
{
	// the following assumes the origin is (0,0), and the first pixel is at (0.5,0.5)
	typedef Vec<float, 2> VT;
	
	float minx = x - .5;
	float miny = x - .5;
	
	float xThresh = (int)x; // line dividing left half of pixels from right half
	float yThresh = (int)y; // line dividing top half from bottom half
	
	float leftWeight = xThresh - minx;
	float bottomWeight = yThresh - miny;
	
	// get the four pixels around it: 
	float botLeft = image.at<VT>(xThresh - 1, yThresh - 1)[0];
	float botRight = image.at<VT>(xThresh, yThresh - 1)[0];
	float topLeft = image.at<VT>(xThresh - 1, yThresh)[0];
	float topRight = image.at<VT>(xThresh, yThresh)[0];
	
	float resultX = botLeft*leftWeight*bottomWeight + botRight*rightWeight*bottomWeight + 
			topLeft*leftWeight*topWeight + topRight*rightWeight*topWeight;
	
	botLeft = image.at<VT>(xThresh - 1, yThresh - 1)[1];
	botRight = image.at<VT>(xThresh, yThresh - 1)[1];
	topLeft = image.at<VT>(xThresh - 1, yThresh)[1];
	topRight = image.at<VT>(xThresh, yThresh)[1];
	
	float resultY = botLeft*leftWeight*bottomWeight + botRight*rightWeight*bottomWeight + 
			topLeft*leftWeight*topWeight + topRight*rightWeight*topWeight;
	
	VT result;
	result[0] = resultX;
	result[1] = resultY;
	return result;

}