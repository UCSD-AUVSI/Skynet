#pragma once

#include <cv.h>
#include <highgui.h>
#include "VisionUtil.h"

namespace Vision {

	typedef int BlobId;
	typedef cv::Vec<unsigned char, 1> MaskElement;
	typedef cv::Vec<BlobId, 1> BlobIdElement;
	typedef cv::Vec<unsigned char, 3> HistColor;
	typedef cv::Vec<unsigned char, 3> FalseColor;

	#define MaxColorDistanceForMerge 7.5f; // 5 = 85% pretty, 7.5=90% pretty, histogram bins are in range 0-NUM_HISTOGRAM_BINS
	#define NUM_HISTOGRAM_BINS 20 // 25 works well
	
	ref class ColorBlob;

	ref class ColorBlobStatistics : CachedStatistics
	{
	public:
		ColorBlobStatistics(ColorBlob ^ p) : CachedStatistics() { parent = p; cachedArea = -1.0f; }

		PixelColor getMostCommonColor(cv::Mat img, float area);
	private:
		ColorBlob ^ parent;
		MRef<PixelColor> ^ mostCommonColor;
		float cachedArea;

		void updateStatistics() { throw gcnew Exception("Not implemented"); }
	};

	ref class ColorBlob
	{
	public:
		ColorBlob(BlobId bId, cv::Size sizeIn);
		!ColorBlob();
		~ColorBlob() { this->!ColorBlob(); }
		bool hasSameColor(ColorBlob ^ blob, cv::Mat img);
		void expandToContainBlob(ColorBlob ^ blob, cv::Mat *blobIds);
		void expandToContainBlob(ColorBlob ^ blob) {expandToContainBlob(blob, NULL);}
		void calculateBlobColor(cv::Mat img);
		void drawIntoImg(cv::Mat img);
		void drawIntoFalseColorImg(cv::Mat img, FalseColor color);
		void drawFilledIntoFalseColorImg(cv::Mat img, unsigned char color);
		PixelColor mostCommonColor(cv::Mat img);

		float area();
		bool isInterior();
		PixelColor getBlobColor() { return *blobColor; }
		void setBlobColor(PixelColor color);

		void addPoint(cv::Point pt, cv::Mat blobIds);
		void addPoint(cv::Point pt) {addPoint(pt, cv::Mat());}
		
		static void runTests();
		
		BlobId id;
		PixelColor * blobColor;
	private:
		cv::Size *size;
		cv::Mat *mask;
		ColorBlobStatistics ^ stats;

		PixelColor getARandomPixel(cv::Mat img);
		cv::MatND calcHistogram(cv::Mat img);
		PixelColor getHistogramPeak(cv::MatND hist);
		unsigned char maskValueAt(cv::Point pt);
		
		void testSameColor();
		void testExpandToContainBlob();
		void testCalcBlobColor();
		void testAddPoint();
	};

}