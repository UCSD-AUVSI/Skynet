#include "StdAfx.h"
#include "VisionUtil.h"
#include "Util.h"
#include <Windows.h>

using namespace Vision;
using namespace Util;
using namespace System;


ManagedIPL::ManagedIPL(String ^ path, int options)
{
	img = cvLoadImage(Util::managedToSTL(path).c_str(), options);
}

ManagedIPL::!ManagedIPL()
{
	IplImage *localImg = img;
	cvReleaseImage(&localImg);
}

String ^ Vision::shapeFloatToString(float input)
{
	int in = (int)Math::Round(input);
	System::String ^ retVal;
	switch (in)
	{
	case 0:
		retVal = "Squa";
		break;
	case 1:
		retVal = "Squa";//"Diamond";
		break;
	case 2:
		retVal = "Rect";
		break;
	case 3:
		retVal = "Para";
		break;
	case 4:
		retVal = "Hexa";
		break;
	case 5:
		retVal = "Circ";
		break;
	case 6:
		retVal = "Pent";
		break;
	case 7:
		retVal = "Semi"; //semi circle
		break;
	case 8:
		retVal = "Star";
		break;
	case 9:
		retVal = "Trap";
		break;
	case 10:
		retVal = "Cros";
		break;
	case 11:
		retVal = "Tria";
		break;
	case 12:
		retVal = "Quar"; //quarter circle
		break;
	default:
		retVal = "Unknown";
		break;
	}

	return retVal->ToLower();
}



// return an int. 100 == unknown
// if you modify this, modify shapeFloatToString also
int Vision::shapeStrToInt(String ^ const shape)
{
	String ^ theShape = shape->ToLower();
	if (theShape->Equals("squa") || theShape->Equals("diam")) //
		return 1; // square == diamond
	if (theShape->Equals("rect")) //
		return 2;
	if (theShape->Equals("para")) //
		return 3;	
	if (theShape->Equals("hexa"))
		return 4;	
	if (theShape->Equals("circ")) //
		return 5;	
	if (theShape->Equals("pent"))
		return 6;	
	if (theShape->Equals("semi")) //
		return 7;	
	if (theShape->Equals("star"))
		return 8;	
	if (theShape->Equals("trap")) //
		return 9;	
	if (theShape->Equals("cros")) //
		return 10;	
	if (theShape->Equals("tria")) //
		return 11;	
	if (theShape->Equals("quar")) //
		return 12;	

	throw gcnew Exception("Unknown Shape: " + shape);
}


String ^ Vision::parseFilenameForLetter(String^ const filename)
{
	String ^ letter = filename->ToLower()->Substring(0,1);
	letter = disambiguateLetter(letter);
	return letter;
}

String^ Vision::parseFilenameForShape(String^ const filename)
{
	return filename->Substring(2,4)->ToLower();
}

String^ Vision::parseFilenameForSeparateShape(String^ const filename)
{
	return filename->Substring(0,4)->ToLower();
}

String ^ Vision::disambiguateLetter(String ^ input)
{
	if (input == nullptr)
		return nullptr;
	if (input->Equals("o"))
		input = "0";
	if (input->Equals("6"))
		input = "9";
	return input;
}

PixelColor Vision::blackPixelColor()
{
	PixelColor color;
	color[0] = 0;
	color[1] = 0;
	color[2] = 0;
	return color;
}

float Vision::calcColorDistance(cv::Vec<float, 3> color1, cv::Vec<float, 3> color2)
{
	float deltaSquaredSum = 0.0f;
	for (int i = 0; i < 3; ++i)
	{
		float delta = (color1[i] - color2[i]);
		deltaSquaredSum += delta*delta;
	}
	return sqrt(deltaSquaredSum);
}


void Vision::saveImage(cv::Mat img, String ^ filename)
{
	IplImage outputIpl = img;
	cvSaveImage(managedToSTL(filename).c_str(), &outputIpl);
}

cv::MatND Vision::calcHistogramOfImage(cv::Mat img)
{
	int defaultNumBins = 20;
	return calcHistogramOfImage(img, defaultNumBins);
}

cv::MatND Vision::calcHistogramOfImage(cv::Mat img, int numBins)
{
	return calcHistogramOfImage(img, numBins, cv::Mat());
}

cv::MatND Vision::calcHistogramOfImage(cv::Mat img, int numBins, cv::Mat mask)
{
	cv::MatND hist;
	int channels[] = {0, 1, 2};
	int histSize[] = {numBins, numBins, numBins};
    float channelRanges[] = { 0, MAX_RGB_COLOR_VALUE+1 };
    const float* ranges[] = { channelRanges, channelRanges, channelRanges };
	cv::calcHist(&img, 1, channels, mask, hist, 3, histSize, ranges);

	return hist;
}

cv::Mat Vision::magnitude(cv::Mat img)
{
	std::vector<cv::Mat> imgChannels;
	cv::split(img, imgChannels);

	int numChannels = 3;
	cv::Mat sumSquaredTotal = cv::Mat::zeros(img.size(), CV_32FC1);
	for (int i = 0; i < numChannels; ++i)
	{
		cv::Mat chn = imgChannels[i];
		cv::multiply(chn, chn, chn);
		cv::add(chn, sumSquaredTotal, sumSquaredTotal);
	}
	cv::Mat magnitude;
	cv::sqrt(sumSquaredTotal, magnitude);

	return magnitude;
}

cv::Mat Vision::outsideFill(cv::Mat input, unsigned char color)
{
	uchar differentColor = (color == 128 ? 1 : 128);
	cv::Mat filled = input.clone();
	cv::floodFill(filled, cv::Point(0,0), cv::Scalar(differentColor));
	cv::Mat changedPixels = filled - input;
	input.setTo(color, changedPixels == 0);
	return input;
}

cv::Mat Vision::centerFill(cv::Mat input, unsigned char color)
{
	cv::Mat filled = input.clone();
	cv::floodFill(filled, cv::Point(input.cols/2,input.rows/2), cv::Scalar(color));
	return filled;
}

float Vision::farthestPixelFromCenter(cv::Mat input, unsigned char color)
{
	cv::Point center = cv::Point(input.cols/2, input.rows/2);
	cv::Mat masked = input == color;
	float farthestDistance = 0.0f;
	for (int x = 0; x < input.cols; ++x)
	{
		for (int y = 0; y < input.rows; ++y)
		{
			bool pixelIsOn = masked.at<cv::Vec<uchar, 1>>(y,x)[0] > 0;
			if (pixelIsOn)
			{
				float thisDistance = pythagoreanDistance((float)x-center.x, (float)y-center.y);
				farthestDistance = std::max(farthestDistance, thisDistance);
			}
		}
	}
	return farthestDistance;
}

float Vision::radians(float deg) 
{ 
	return (float)(deg*PI/180.0f); 
}

float Vision::degrees(float rad) 
{ 
	return (float)(rad*180.0f/PI); 
}

float Vision::distanceBetweenAngles(float angleARadians, float angleBRadians)
{
	return distanceBetweenAngles(angleARadians, angleBRadians, (float)PI);
}

float Vision::distanceBetweenAngles(float angleARadians, float angleBRadians, float overflowLocationRadians)
{
	angleARadians = shiftAngleToPositive(angleARadians, overflowLocationRadians);
	angleBRadians = shiftAngleToPositive(angleBRadians, overflowLocationRadians);
	float angleDistance = abs(angleARadians - angleBRadians);
	if (angleDistance > overflowLocationRadians/2)
	{
		// try rotating along unit circle, since 0 degrees and 179 degrees should be close
		angleDistance = (float)abs(angleDistance - overflowLocationRadians);
	}
	return angleDistance;
}

float Vision::shiftAngleToPositive(float angleRadians, float overflowLocationRadians)
{
	if (angleRadians < VERY_LOW_NUMBER)
		return 0.0f;
	if (angleRadians > VERY_LARGE_NUMBER)
		return 0.0f;
	while (angleRadians < 0.0f)
		angleRadians += overflowLocationRadians;
	while (angleRadians >= overflowLocationRadians)
		angleRadians -= overflowLocationRadians;
	return angleRadians;
}

float Vision::pythagoreanDistance(float x, float y)
{
	return std::sqrt(std::pow(x,2) + std::pow(y,2));
}



float Vision::ratioOfBlobToWhole(cv::Mat img)
{
	int sizeOfImg = img.rows * img.cols;
	int sizeOfShape = cv::countNonZero(img);
	return sizeOfShape/(float)sizeOfImg;
}