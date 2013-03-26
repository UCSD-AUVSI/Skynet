#include "StdAfx.h"
#include "ColorBlob.h"
#include "Util.h"
#include "VisionUtil.h"
#include <cmath>

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;



ColorBlob::ColorBlob(BlobId bId, cv::Size sizeIn)
{
	id = bId;
	size = new cv::Size();
	size->width = sizeIn.width;
	size->height = sizeIn.height;
	mask = new cv::Mat(*size, CV_8UC1);
	mask->setTo(0);
	blobColor = NULL;
	setBlobColor(blackPixelColor());

	stats = gcnew ColorBlobStatistics(this);
}

ColorBlob::!ColorBlob()
{
	delete size;
	delete mask;
	if (blobColor != NULL)
		delete blobColor;
}

bool 
ColorBlob::hasSameColor(ColorBlob ^ blob, cv::Mat img)
{
	//calculateBlobColor(img);
	//blob->calculateBlobColor(img);
	
	PixelColor thisColor = stats->getMostCommonColor(img, area());//mostCommonColor(img);
	PixelColor otherColor = blob->stats->getMostCommonColor(img, blob->area());//->mostCommonColor(img);

	float colorDistance = calcColorDistance(thisColor, otherColor);
	return colorDistance <= MaxColorDistanceForMerge;
}

PixelColor 
ColorBlob::mostCommonColor(cv::Mat img)
{
	bool blobIsVerySmall = area() < 5.1f;
	if (blobIsVerySmall)
	{
		return getARandomPixel(img);
	}
	else
	{
		cv::MatND hist = calcHistogram(img);
		return getHistogramPeak(hist);
	}
}

PixelColor 
ColorBlob::getARandomPixel(cv::Mat img)
{
	int maxIdx[2];
	cv::minMaxIdx(*mask, 0, 0, 0, maxIdx);
	PixelColor color = img.at<PixelColor>(maxIdx[0], maxIdx[1]);
	PixelColor equivalentHistogramLocation;
	for (int i = 0; i < 3; ++i)
		equivalentHistogramLocation[i] = (float)std::floor(color[i]*NUM_HISTOGRAM_BINS/MAX_RGB_COLOR_VALUE);
	
	return equivalentHistogramLocation;
}

cv::MatND 
ColorBlob::calcHistogram(cv::Mat img)
{
	return calcHistogramOfImage(img, NUM_HISTOGRAM_BINS, *mask);
}

PixelColor
ColorBlob::getHistogramPeak(cv::MatND hist)
{
	double maxVal;
	int maxIdx[3];
	cv::minMaxIdx(hist, 0, &maxVal, 0, maxIdx);
	PixelColor color;
	for (int i = 0; i < 3; ++i)
		color[i] = (float)maxIdx[i];
	
	return color;
}

void 
ColorBlob::expandToContainBlob(ColorBlob ^ blob, cv::Mat *blobIds)
{
	cv::bitwise_or(*mask, *(blob->mask), *mask);
	bool blobIdsIsValid = blobIds != NULL;//.size().width != 0;
	if (blobIdsIsValid)
	{
		BlobId newId = id;
		blobIds->setTo(newId, *(blob->mask));
	}

	stats->invalidate();
}

void 
ColorBlob::calculateBlobColor(cv::Mat img)
{
	cv::Scalar newBlobColor = cv::mean(img, *mask);
	(*blobColor)[0] = (float)newBlobColor[0];
	(*blobColor)[1] = (float)newBlobColor[1];
	(*blobColor)[2] = (float)newBlobColor[2];
}

void 
ColorBlob::drawIntoImg(cv::Mat img)
{
	img.setTo(*blobColor, *mask);
}

void 
ColorBlob::drawIntoFalseColorImg(cv::Mat img, FalseColor color)
{
	img.setTo(color, *mask);
}
void 
ColorBlob::drawFilledIntoFalseColorImg(cv::Mat img, unsigned char color)
{
	cv::Mat filled = mask->clone();
	cv::floodFill(filled, cv::Point(0,0), cv::Scalar(128));
	cv::Mat outsideShape = filled - *mask;
	img.setTo(color, outsideShape == 0);
}

void 
ColorBlob::addPoint(cv::Point pt, cv::Mat blobIds)
{
	mask->at<MaskElement>(pt.y, pt.x)[0] = 1;
	bool blobIdsIsValid = blobIds.size().width != 0;
	if (blobIdsIsValid)
	{
		blobIds.at<BlobIdElement>(pt.y, pt.x)[0] = id;
	}
	stats->invalidate();
}


float 
ColorBlob::area()
{
	return (float)cv::sum(*mask)[0];
}

void 
ColorBlob::setBlobColor(PixelColor color)
{
	if (blobColor != NULL)
		delete blobColor;

	blobColor = new PixelColor(color);
}

bool 
ColorBlob::isInterior()
{
	cv::Mat edgeMask = cv::Mat(mask->size(), CV_8UC1);
	edgeMask.setTo(0);
	edgeMask.col(0).setTo(1);
	edgeMask.row(0).setTo(1);
	edgeMask.col(mask->cols-1).setTo(1);
	edgeMask.row(mask->rows-1).setTo(1);
	cv::Mat valuesOnEdges;
	mask->copyTo(valuesOnEdges, edgeMask);
	int numPixelsOnBorder = cv::countNonZero(valuesOnEdges);
	bool edgesAreOff = numPixelsOnBorder == 0;
	return edgesAreOff;
}

unsigned char 
ColorBlob::maskValueAt(cv::Point pt)
{
	unsigned char maskValue = mask->at<MaskElement>(pt.y, pt.x)[0];
	return maskValue;
}

void 
ColorBlob::runTests()
{
	ColorBlob ^testFixture = gcnew ColorBlob(5, cv::Size(10,10));
	testFixture->testSameColor();

	testFixture = gcnew ColorBlob(5, cv::Size(10,10));
	testFixture->testCalcBlobColor();

	testFixture = gcnew ColorBlob(5, cv::Size(10,10));
	testFixture->testAddPoint();

	testFixture = gcnew ColorBlob(5, cv::Size(10,10));
	testFixture->testExpandToContainBlob();
}

void 
ColorBlob::testSameColor()
{
	cv::Size imgSize = cv::Size(10,10);
	cv::Mat img = cv::Mat::zeros(imgSize, CV_8UC3);

	/*ColorBlob ^secondFixture = gcnew ColorBlob(6, cv::Size(10,10));
	secondFixture->blobColor[0] = 2.0f;
	secondFixture->blobColor[1] = 0.5f;
	secondFixture->blobColor[2] = 1.0f;

	if (!hasSameColor(secondFixture))
		throw gcnew Exception("ERROR");
	
	blobColor[0] = 200.0f;
	
	if (hasSameColor(secondFixture))
		throw gcnew Exception("ERROR");*/
}

void 
ColorBlob::testExpandToContainBlob()
{
	// setup
	cv::Size imgSize = cv::Size(10,10);
	cv::Mat blobIds = cv::Mat::zeros(imgSize, CV_32SC1);
	ColorBlob ^secondFixture = gcnew ColorBlob(6, imgSize);
	secondFixture->addPoint(cv::Point(1,0), blobIds);
	addPoint(cv::Point(0,0), blobIds);
	
	expandToContainBlob(secondFixture, &blobIds);

	// check mask
	bool maskIsZero = mask->at<MaskElement>(0, 1)[0] == 0;
	if (maskIsZero)
		throw gcnew Exception("ERROR");

	maskIsZero = mask->at<MaskElement>(0, 0)[0] == 0;
	if (maskIsZero)
		throw gcnew Exception("ERROR");

	// check blobIds
	BlobId idAtSecondBlob = blobIds.at<BlobIdElement>(0, 1)[0];
	bool blobIdsIsCorrect = idAtSecondBlob == id;
	if (!blobIdsIsCorrect)
		throw gcnew Exception("ERROR");

	idAtSecondBlob = blobIds.at<BlobIdElement>(0, 0)[0];
	blobIdsIsCorrect = idAtSecondBlob == id;
	if (!blobIdsIsCorrect)
		throw gcnew Exception("ERROR");


}

void 
ColorBlob::testCalcBlobColor()
{
	cv::Mat img(*size, CV_32FC3);
	img.setTo(5.0f);

	addPoint(cv::Point(0,0));
	calculateBlobColor(img);

	PixelColor correctColor;
	correctColor[0] = 5.0f;
	correctColor[1] = 5.0f;
	correctColor[2] = 5.0f;

	bool blobColorIsIncorrect = calcColorDistance(*blobColor, correctColor) > 0.001f;
	if (blobColorIsIncorrect)
		throw gcnew Exception("ERROR");
}

void 
ColorBlob::testAddPoint()
{
	cv::Mat blobIds(*size, CV_32SC1);
	cv::Point pt(0,0);
	bool maskIsZero = mask->at<MaskElement>(pt.y, pt.x)[0] == 0;
	if (!maskIsZero)
		throw gcnew Exception("ERROR");

	addPoint(pt, blobIds);

	maskIsZero = mask->at<MaskElement>(pt.y, pt.x)[0] == 0;
	if (maskIsZero)
		throw gcnew Exception("ERROR");

	bool blobIdsAtProperValue = blobIds.at<BlobIdElement>(pt.y, pt.x)[0] == id;
	if (!blobIdsAtProperValue)
		throw gcnew Exception("ERROR");
}

PixelColor 
ColorBlobStatistics::getMostCommonColor(cv::Mat img, float area)
{
	float thresholdRatio = 0.03f; // 3 percent is a good number
	float changeInArea = abs(area-cachedArea);
	bool areaHasChangedSignificantly = cachedArea < 0.0f || changeInArea > cachedArea*thresholdRatio;
	if (areaHasChangedSignificantly)
	{
		cachedArea = area;
		mostCommonColor = gcnew MRef<PixelColor>( new PixelColor(parent->mostCommonColor(img)));
		setWasUpdated();
	}
		
	return *(mostCommonColor->obj);
}
