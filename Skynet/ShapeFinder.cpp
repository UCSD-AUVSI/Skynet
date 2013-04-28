#include "StdAfx.h"
#include "ShapeFinder.h"
#include "Segmenter.h"
#include "ColorBlobDetector.h"
#include "HistogramSegmenter.h"
#include "Util.h"
#include "VisionUtil.h"

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;

ShapeFinder::ShapeFinder(void)
{
	segmenter = gcnew HistogramSegmenter();
}


cv::Mat 
ShapeFinder::findShape(cv::Mat colorImg, String ^ filename)
{
	List<ColorBlob ^> ^ blobList = segmenter->findBlobs(colorImg);
	List<ColorBlob ^> ^ largeBlobs = getLargeBlobs(blobList, colorImg.size());
	List<ColorBlob ^> ^ interiorBlobs = getInteriorBlobs(largeBlobs);
	ColorBlob ^ shapeBlob = getBiggestBlob(interiorBlobs);
	cv::Mat shape = getShapeFromBlob(shapeBlob, colorImg.size());
	
	if (!shape.empty())
		saveImage(shape, filename + "_f_shape.jpg");

	
	PRINT("Num large blobs:" + largeBlobs->Count);
	PRINT("Num interior blobs:" + interiorBlobs->Count);
	
	/*if (filename != nullptr)
		ColorBlobDetector::saveBlobsToFalseColorImage(largeBlobs, filename + "_d_big.jpg", colorImg.size());
	if (filename != nullptr)
		ColorBlobDetector::saveBlobsToFalseColorImage(interiorBlobs, filename + "_e_interior.jpg", colorImg.size());
	*/

	return shape;
}

List<ColorBlob ^> ^ 
ShapeFinder::getLargeBlobs(List<ColorBlob ^> ^ blobList, cv::Size imgSize)
{
	float imgArea = (float)imgSize.width * imgSize.height;
	float minimumBlobSize = (float)(imgArea*sizeThresholdRatio);

	List<ColorBlob ^> ^ bigBlobs = gcnew List<ColorBlob ^>();
	for each (ColorBlob ^ blob in blobList)
		if (blob->area() > minimumBlobSize)
			bigBlobs->Add(blob);
	return bigBlobs;
}


List<ColorBlob ^> ^ 
ShapeFinder::getInteriorBlobs(List<ColorBlob ^> ^ blobList)
{
	List<ColorBlob ^> ^ interiorBlobs = gcnew List<ColorBlob ^>();
	for each (ColorBlob ^ blob in blobList)
		if (blob->isInterior())
			interiorBlobs->Add(blob);
	return interiorBlobs;
}

ColorBlob ^ 
ShapeFinder::getBiggestBlob(List<ColorBlob ^> ^ blobList)
{
	if (blobList->Count == 0)
		return nullptr;

	ColorBlob ^ blob = blobList[0];
	for each (ColorBlob ^ candidate in blobList)
		if (candidate->area() > blob->area())
			blob = candidate;
	return blob;
}

cv::Mat 
ShapeFinder::getShapeFromBlob(ColorBlob ^ blob, cv::Size imgSize)
{
	cv::Mat shape;
	if (blob != nullptr)
	{
		shape = cv::Mat::zeros(imgSize, CV_8UC1);

		blob->drawFilledIntoFalseColorImg(shape, 255);
	
		shape.convertTo(shape, CV_8UC1);
		shape = shape > 0;
	}
	else
		shape = cv::Mat();
		
	return shape;
}
