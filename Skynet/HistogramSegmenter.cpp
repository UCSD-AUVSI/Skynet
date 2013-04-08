#include "StdAfx.h"
#include "HistogramSegmenter.h"
#include "VisionUtil.h"
#include "Util.h"
#include "ColorBlobDetector.h"
#include "ColorBlob.h"

using namespace Vision;
using namespace Util;
using namespace System;
using namespace Collections;
using namespace Collections::Generic;

HistogramSegmenter::HistogramSegmenter(void)
{
	numBins = NUM_BINS;
	binsToRGBRatio = (float) (numBins / MAX_RGB_COLOR_VALUE);
}


List<ColorBlob ^> ^ 
HistogramSegmenter::findBlobs(cv::Mat colorImg, String ^ filename)
{
	setImageWithPreprocessing(colorImg, filename);
	List<ColorBlob ^> ^ blobList = findBlobWithHistogram(filename);
	return blobList;
}

void
HistogramSegmenter::setImageWithPreprocessing(cv::Mat colorImg, String ^ filename)
{
	if (colorImg.type() != CV_32FC3)
		colorImg.convertTo(colorImg, CV_32FC3);
	
	cv::normalize(colorImg, colorImg, 0.0, MAX_RGB_COLOR_VALUE, cv::NORM_MINMAX);
	//cv::GaussianBlur(colorImg, colorImg, cv::Size(3,3), 0);


	mImg = gcnew MRef<cv::Mat>(new cv::Mat(colorImg));
}


cv::Mat 
HistogramSegmenter::reduceColorsWithBinning()
{
	throw gcnew Exception("Don't use this! I just made it to prototype something");
	cv::Mat binnedImage = (*(mImg->obj)) * binsToRGBRatio;
	binnedImage.convertTo(binnedImage, CV_32SC3);
	binnedImage.convertTo(binnedImage, CV_32FC3);
	binnedImage /= binsToRGBRatio;

	return binnedImage;
}


List<ColorBlob ^> ^
HistogramSegmenter::findBlobWithHistogram(String ^ filename)
{
	cv::Mat input = *(mImg->obj);
	
	cv::MatND hist = calcHistogramOfImage(input, numBins); 
	List<MRef<PixelColor> ^> ^ validColors = biggestColorsInHistogram(hist, NUM_VALID_COLORS);
	List<ColorBlob ^> ^ blobList = segmentImageIntoBlobsWithColors(input, validColors);

	if (filename != nullptr)
	{
		cv::Mat output = redrawImageWithColors(input, validColors);
		saveImage(output, filename + "_a_binned.bmp");
	}
	
	return blobList;
}
List<MRef<PixelColor> ^> ^ 
HistogramSegmenter::biggestColorsInHistogram(cv::MatND hist, int numColors)
{
	List<MRef<PixelColor> ^> ^ colors = gcnew List<MRef<PixelColor> ^>(numColors);
	for (int i = 0; i < numColors; ++i)
	{
		PixelColor biggestRemainingColor = getAndRemoveBiggestColorInHistogram(hist);
		colors->Add(gcnew MRef<PixelColor>(biggestRemainingColor));
	}
	colors = mergeCloseColors(colors);
	return colors;
}


PixelColor 
HistogramSegmenter::getAndRemoveBiggestColorInHistogram(cv::MatND hist)
{
	int idxOfBiggest[3];
	getBiggestIndexInHist(hist, idxOfBiggest);
	zeroOutBinAtIndex(hist, idxOfBiggest);
	PixelColor colorOfBin = convertBinToColor(idxOfBiggest);
	return colorOfBin;
}

void
HistogramSegmenter::getBiggestIndexInHist(cv::MatND hist, int idxOfMax[3])
{
	double maxVal;
	cv::minMaxIdx(hist, 0, &maxVal, 0, idxOfMax);
}

void 
HistogramSegmenter::zeroOutBinAtIndex(cv::MatND hist, int index[3])
{
	hist.at<float>(index[0], index[1], index[2]) = 0.0f;
}

List<MRef<PixelColor> ^> ^ 
HistogramSegmenter::mergeCloseColors(List<MRef<PixelColor> ^> ^ colors)
{
	List<MRef<PixelColor> ^> ^ newColors = gcnew List<MRef<PixelColor> ^>();
	for each (MRef<PixelColor> ^ color in colors)
	{
		bool foundMatch = false;
		for each (MRef<PixelColor> ^ candidate in newColors)
			if (calcColorDistance(color->o(), candidate->o()) < MERGE_COLOR_DISTANCE)
				foundMatch = true;
		if (!foundMatch)
			newColors->Add(color);
	}
	return newColors;
}

PixelColor 
HistogramSegmenter::convertBinToColor(int idxOfBiggest[3])
{
	PixelColor color;
	for (int i = 0; i < 3; ++i)
		color[i] = idxOfBiggest[i]/binsToRGBRatio;
	return color;
}

cv::Mat 
HistogramSegmenter::redrawImageWithColors(cv::Mat input, List<MRef<PixelColor> ^> ^ validColors)
{
	for (cv::MatIterator_<PixelColor> it = input.begin<PixelColor>(); 
		 it != input.end<PixelColor>(); ++it) 
	{
		PixelColor inputColor = *it;
		PixelColor convertedColor = convertToValidColor(inputColor, validColors);
		//("clr: " + convertedColor[0] + "," + convertedColor[1] + "," + convertedColor[2]);

		*it = convertedColor;
	}

	return input;
}

PixelColor 
HistogramSegmenter::convertToValidColor(PixelColor inputColor, List<MRef<PixelColor> ^> ^ validColors)
{
	PixelColor closestColor;
	float bestDistance = COLOR_DISTANCE_THRESHOLD + 10.0f;
	for each (MRef<PixelColor> ^ mCandidate in validColors)
	{
		PixelColor candidate = *(mCandidate->obj);
		
		//PRINT("candidate: " + candidate[0] + "," + candidate[1] + "," + candidate[2]);

		float distance = calcColorDistance(inputColor, candidate);
		bool candidateIsBetter = distance < bestDistance;
		if (candidateIsBetter)
		{
			closestColor = candidate;
			bestDistance = distance;
		}
	}
	bool colorIsValid = bestDistance <= COLOR_DISTANCE_THRESHOLD;
	if (colorIsValid)
		return closestColor;
	else
		return blackPixelColor();
}

List<ColorBlob ^> ^ 
HistogramSegmenter::segmentImageIntoBlobsWithColors(cv::Mat input, List<MRef<PixelColor> ^> ^ validColors)
{
	List<ColorBlob ^> ^ blobList = makeBlobsWithColors(validColors);
	drawImageIntoBlobs(input, blobList);

	return blobList;
}

List<ColorBlob ^> ^ 
HistogramSegmenter::makeBlobsWithColors(List<MRef<PixelColor> ^> ^ colors)
{
	List<ColorBlob ^> ^ blobs = gcnew List<ColorBlob ^>(colors->Count);
	int index = 1;
	for each (MRef<PixelColor> ^ mColor in colors)
	{
		PixelColor color = mColor->o();
		ColorBlob ^ blob = gcnew ColorBlob(index++, mImg->o().size());
		blob->setBlobColor(color);
		blobs->Add(blob);
	}
	return blobs;
}

void 
HistogramSegmenter::drawImageIntoBlobs(cv::Mat input, List<ColorBlob ^> ^ blobs)
{
	for (int x = 0; x < input.cols; ++x)
		for (int y = 0; y < input.rows; ++y)
		{
			PixelColor color = input.at<PixelColor>(y,x);
			drawPixelIntoBlobs(cv::Point(x,y), color, blobs);
		}
}

void 
HistogramSegmenter::drawPixelIntoBlobs(cv::Point pt, PixelColor pixelColor, List<ColorBlob ^> ^ blobs)
{
	ColorBlob ^ blob;
	float bestDistance = COLOR_DISTANCE_THRESHOLD + 10.0f;
	for each (ColorBlob ^ candidate in blobs)
	{
		float distance = calcColorDistance(pixelColor, candidate->getBlobColor());
		bool candidateIsBetter = distance < bestDistance;
		if (candidateIsBetter)
		{
			blob = candidate;
			bestDistance = distance;
		}
	}
	bool blobIsValid = bestDistance <= COLOR_DISTANCE_THRESHOLD;
	if (blobIsValid)
		blob->addPoint(pt);
}

cv::Mat 
HistogramSegmenter::secondSegmentation(PixelColor color) 
{
	cv::Mat input = *(mImg->obj);
	cv::Mat distanceFromShape;
	cv::add(input, -color, distanceFromShape);
	distanceFromShape = magnitude(distanceFromShape);

	cv::Mat shape;
	cv::threshold(distanceFromShape, shape, COLOR_DISTANCE_THRESHOLD, 255.0, cv::THRESH_BINARY_INV);
	shape = outsideFill(shape, 255);
	return shape;
}