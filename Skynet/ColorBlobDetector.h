#pragma once

#include <cv.h>
#include <highgui.h>
#include "ColorBlob.h"
#include "Segmenter.h"

namespace Vision {

#define BlobColorBoundaryThreshold 3.0f // 5.0f was good, but didn't segment some stuff

	using namespace System;
	using namespace Collections;
	using namespace Collections::Generic;

	ref class MCvPoint
	{
	public:
		MCvPoint(cv::Point p)
		{
			pt = new cv::Point();
			pt->x = p.x;
			pt->y = p.y;
		}
		!MCvPoint()
		{
			delete pt;
		}
		~MCvPoint() { this->!MCvPoint(); }
		cv::Point *pt;
	};

	ref class ColorBlobDetector : Segmenter
	{
	public:
		ColorBlobDetector(void);

		virtual List<ColorBlob ^> ^ findBlobs(cv::Mat colorImg, String ^ filename) override;
		List<ColorBlob ^> ^ findBlobs(cv::Mat colorImg) { return findBlobs(colorImg, nullptr); }
	
		static void saveBlobsToFalseColorImage(List<ColorBlob ^> ^ blobList, String ^ filename, cv::Size size);


	private:
		!ColorBlobDetector(void);
		~ColorBlobDetector() { this->!ColorBlobDetector(); }

		void setImageWithPreprocessing(cv::Mat colorImg, String ^ filename);
		void doCannyThreshAndSave(cv::Mat channel, String ^ filename);
		void resetBlobDetection();
		List<ColorBlob ^> ^ adaptiveBlobDetection();
		void processPixel(Dictionary<int, ColorBlob^>^blobs, cv::Mat *blobIds, cv::Point pixel);
		BlobId getBlobId(cv::Point pt);
		PixelColor getImgColor(cv::Point pt);
		List<ColorBlob ^> ^ listFromDictionary(Dictionary<BlobId, ColorBlob^>^ dict);
		ColorBlob ^ retrieveBlob(Dictionary<int, ColorBlob^>^blobs, int blobId, cv::Point pixel);
		ColorBlob ^ makeNewBlob(cv::Point pixel);
		List<MCvPoint ^> ^ getNeighborPixels(cv::Point pixel);
		List<ColorBlob ^> ^ combineSameColorBlobs(List<ColorBlob ^> ^ blobList);
		void addBlobToListWithColorMatching(ColorBlob ^ blob, List<ColorBlob ^> ^ blobList);
		Dictionary<BlobId, int> ^ getDictionaryOfIds(List<ColorBlob ^> ^ blobs);
		void saveBlobs(String ^ filename, Dictionary<BlobId, int> ^ blobIdIndexes);
		
		static FalseColor hashIndexToFalseColor(int index);
		static void hashIndexToBGR(int index, unsigned char & b, unsigned char & g, unsigned char & r);
		void saveBlobsWithColor(String ^ filename, List<ColorBlob ^> ^ mergedBlobs);
		
		int lastBlobId;
		cv::Mat *img;
		cv::Mat *blobIds;
	};

}
