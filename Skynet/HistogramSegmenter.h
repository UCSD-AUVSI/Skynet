#pragma once

#include <cv.h>
#include <highgui.h>
#include "VisionUtil.h"
#include "Segmenter.h"

namespace Vision {

	#define COLOR_DISTANCE_THRESHOLD 100.0f
	#define MERGE_COLOR_DISTANCE 30.0f // approx one bin
	#define NUM_VALID_COLORS 8
	#define NUM_BINS 10 // was 20 - 98% ugly
	
	using namespace System;
	using namespace Collections;
	using namespace Collections::Generic;

	template<typename T> ref class MRef;
	ref class ColorBlob;

	ref class HistogramSegmenter : Segmenter
	{
	public:
		HistogramSegmenter(void);

		virtual List<ColorBlob ^> ^ findBlobs(cv::Mat colorImg, String ^ filename) override;
		virtual cv::Mat secondSegmentation(PixelColor color) override;
	private:
		cv::Mat reduceColorsWithBinning();
		List<ColorBlob ^> ^ findBlobWithHistogram(String ^ filename);
		void setImageWithPreprocessing(cv::Mat colorImg, String ^ filename);
		
		List<MRef<PixelColor> ^> ^ biggestColorsInHistogram(cv::MatND hist, int numColors);
		PixelColor getAndRemoveBiggestColorInHistogram(cv::MatND hist);
		void getBiggestIndexInHist(cv::MatND hist, int idxOfMax[3]);
		void zeroOutBinAtIndex(cv::MatND hist, int index[3]);
		List<MRef<PixelColor> ^> ^ mergeCloseColors(List<MRef<PixelColor> ^> ^ colors);
		PixelColor convertBinToColor(int idxOfBiggest[3]);
		cv::Mat redrawImageWithColors(cv::Mat input, List<MRef<PixelColor> ^> ^ validColors);
		PixelColor convertToValidColor(PixelColor inputColor, List<MRef<PixelColor> ^> ^ validColors);
		List<ColorBlob ^> ^ segmentImageIntoBlobsWithColors(cv::Mat input, List<MRef<PixelColor> ^> ^ validColors);
		List<ColorBlob ^> ^ makeBlobsWithColors(List<MRef<PixelColor> ^> ^ colors);
		void drawImageIntoBlobs(cv::Mat input, List<ColorBlob ^> ^ blobs);
		void drawPixelIntoBlobs(cv::Point pt, PixelColor pixelColor, List<ColorBlob ^> ^ blobs);

		MRef<cv::Mat> ^ mImg;
		int numBins;
		float binsToRGBRatio;
	};

}
