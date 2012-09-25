/*
 *  Auvsi_Radon.cpp
 *  OpenCV2Test
 *
 *  Created by Lewis Anderson on 5/26/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

// TODO: shrink image inside, or extend lines outward. Maintain equal length
//		of each projection line, but include all pixels. so stuff doesnt get
//		cut off.

#include "Auvsi_Radon.h"
#include <cxcore.hpp>

#define ORIGIN_X ((float)IMAGE_WIDTH)/2.0	// in the image 
#define ORIGIN_Y ((float)IMAGE_WIDTH)/2.0
#define ORIGIN_LINE ((float)IMAGE_WIDTH)/2.0 // on each projection line
#define PI 3.141592653589793

// constructor
Auvsi_Radon::Auvsi_Radon()
{
	numLines = IMAGE_WIDTH*2;
	numPointsOnLine = IMAGE_WIDTH;
	
	// allocate lines, and set angle of each one (0:180 in numLines steps)
	lines = new float[numLines];
	for (int i = 0; i < numLines; i++) {
		lines[i] = PI*((float)i)/((float)numLines);
		//std::printf("%f\t", lines[i]);

	}
	
	// init radon to all zeros
	radon = cv::Mat::zeros(numLines, numPointsOnLine, CV_32F);
}

// kill dat shit
Auvsi_Radon::~Auvsi_Radon()
{
	delete lines;
}

// clear old radon transform in preparation for new one
void
Auvsi_Radon::clearImage()
{
	// zero out radon
	radon = cv::Mat::zeros(numLines, numPointsOnLine, CV_32F);	
}

// sets a new image for taking radon transform
void 
Auvsi_Radon::setImage(cv::Mat inputImage)
{
	image = inputImage;
}

/* pixels usually fall over mulitple discrete segments on the line;
 *	we need to chop it up proportionally, this function does that
 *  function saves into the array
 *
 * ONE issue: we assume that pixels are squared, that are parallel to each line. very tiny loss in precision...
 *		see Bobby Anguelov's blog for further description
 */
void
Auvsi_Radon::squashPixel(float position, int line)
{
	typedef cv::Vec<float, 1> VT;
	// assume that pixel value is 1.0
	
	// figure out how much of that bitch is hanging over to the left side
	float min = position - 0.5;
	float leftWeight = (float)((int)position) - min;
	
	// increment correct positions by the proportion that is hanging over
	// left side
	VT pixelValue = radon.at<VT>(line, (int)position - 1);
	pixelValue[0] += leftWeight;// multiply by image pixel value to support non-binary images
	radon.at<VT>(line, (int)position - 1) = pixelValue;
	
	// right side
	pixelValue = radon.at<VT>(line, (int)position);
	pixelValue[0] += rightWeight;// multiply by image pixel value to support non-binary images
	radon.at<VT>(line, (int)position) = pixelValue;
	if (line == 0) {
		//printf("%d\t", (int)position);
	
	}
}

// project one pixel onto one line
void
Auvsi_Radon::projectPixel(int x, int y, int line)
{
	// get angle
	float theta = lines[line];

	// convert pixel number into image coordinates (origin at center)
	float xLoc = (float)x - ORIGIN_X;
	float yLoc = (float)y - ORIGIN_Y;
	
	// assemble vector describing line
	float xVecLine = cos(theta);
	float yVecLine = sin(theta);

	// compute cross product
	float result = xLoc*xVecLine + yLoc*yVecLine;
	float position = result + ORIGIN_LINE; // position of projected pixel along line
	
	// squash the pixel down onto the line in discrete segments
	squashPixel(position, line);
}

// add chosen pixel to each line

void 
Auvsi_Radon::computePixel(int x, int y)
{
	for (int i = 0; i < numLines; i++) 
		projectPixel(x, y, i);
}

// compute the radon transform
//	it clears out the old radon transform, then iterates through each pixel
//	projecting each one onto each line
void 
Auvsi_Radon::computeRadon()
{
	typedef cv::Vec<float, 1> VT;
	
	clearImage(); // zero out radon because of 
	
	// iterate through each pixel
	for (int i = 0; i < image.size().width; i++)
		for (int j = 0; j < image.size().height; j++)
			if (sqrt((double)(((float)i - ORIGIN_X)*((float)i - ORIGIN_X) + ((float)j - ORIGIN_Y)*((float)j - ORIGIN_Y)))
						< (float)IMAGE_WIDTH + 1.0)
				if (image.at<VT>(j, i)[0] > 0.0f)
					computePixel(i, j); // computes one pixel over each lines
}
