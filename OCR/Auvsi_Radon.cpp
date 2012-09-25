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
#include <opencv2/opencv.hpp>
//#include <cxcore.hpp>

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
	// a*s -> angle*distance from origin
}

// sets a new image for taking radon transform
void 
Auvsi_Radon::setImage(cv::Mat inputImage)
{
	cv::Mat rInFloat( inputImage.size(), CV_32F );
	inputImage.convertTo( rInFloat, rInFloat.type(), 1.0/255.0f );

	image = rInFloat;
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
	float dt = 0.5;
	for (int a = 0; a < radon.size().height; a++)
		for (float s = 0; s < radon.size().width; s++)
		{
			float sum = 0.0f;
			for (float t = -IMAGE_WIDTH; t < IMAGE_WIDTH; t += dt)
			{
				float a_val = lines[a];
				float s_val = s-radon.size().width/2;
				s_val *= 1.0;

				int x = t*sin(a_val) + s_val*cos(a_val);
				int y = -t*cos(a_val) + s_val*sin(a_val);

				// printf("%.1d,%.1f",x,y);

				int y_idx = y+IMAGE_WIDTH/2;
				int x_idx = x+IMAGE_WIDTH/2;

				if (y_idx >= 0 && x_idx >= 0 && y_idx < IMAGE_WIDTH && x_idx < IMAGE_WIDTH)
				{
					// printf ("%d,%d\n",x_idx,y_idx);
					float value = image.at<VT>(y_idx, x_idx)[0];
					// printf("%.1f,%.1f,%d,%d\n",sum,value,y_idx, x_idx);
					sum += value*dt;
				}
			}

			radon.at<VT>(a,s) = radon.at<VT>(a,s)[0] + sum;
		}
}
