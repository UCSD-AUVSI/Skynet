#pragma once

#include <cv.h>
#include <highgui.h>

namespace Vision {

#define sizeThresholdRatio .05 // .15 is probably best for actual images

	using namespace System;
	using namespace Collections;
	using namespace Collections::Generic;

	ref class ColorBlob;
	ref class Segmenter;

	ref class ShapeFinder
	{
	public:
		ShapeFinder(void);

		cv::Mat findShape(cv::Mat colorImg, String ^ filename);
		
	private:
		!ShapeFinder(void) {}
		~ShapeFinder() { this->!ShapeFinder(); }

		List<ColorBlob ^> ^ getLargeBlobs(List<ColorBlob ^> ^ blobList, cv::Size imgSize);
		List<ColorBlob ^> ^ getInteriorBlobs(List<ColorBlob ^> ^ blobList);
		ColorBlob ^ getBiggestBlob(List<ColorBlob ^> ^ blobList);
		cv::Mat getShapeFromBlob(ColorBlob ^ blob, cv::Size imgSize);
		Segmenter ^ segmenter;
	};
}

