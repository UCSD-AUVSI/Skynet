/*
 *  Auvsi_cvUtil.h
 *  OpenCV2Test
 *
 *  Created by Lewis Anderson on 5/26/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */
#pragma once
#include <cv.h>
#include <highgui.h>

#define rightWeight (1 - leftWeight)
#define topWeight (1 - bottomWeight)
#define IMAGE_WIDTH  64
#define IMAGE_HEIGHT 64

/*
	Auvsi_Radon * theRadon = new Auvsi_Radon();
	theRadon->setImage(recognize->_letter);
	theRadon->computeRadon();
	
	//imshow("input", recognize->_image);
	//imshow("shape", recognize->_shape);
	//imshow("letter", recognize->_letter);

*/

namespace util {

	cv::Vec<float, 1> getValuef(cv::Mat image, float x, float y);

	cv::Vec<float, 2> getValuec(cv::Mat image, float x, float y);
}