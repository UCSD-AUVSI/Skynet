#include "StdAfx.h"
#include "ColorBlobDetector.h"
#include "Util.h"
#include "VisionUtil.h"

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;


ColorBlobDetector::ColorBlobDetector(void)
{
	lastBlobId = 0;
	img = NULL;
	blobIds = NULL;
}

ColorBlobDetector::!ColorBlobDetector(void)
{
	resetBlobDetection();
}

List<ColorBlob ^> ^
ColorBlobDetector::findBlobs(cv::Mat colorImg, String ^ filename)
{
	resetBlobDetection();
	setImageWithPreprocessing(colorImg, filename);
	List<ColorBlob ^> ^ rawBlobList = adaptiveBlobDetection();
	List<ColorBlob ^> ^ mergedBlobList = combineSameColorBlobs(rawBlobList);


	PRINT("Num Raw Blobs:" + rawBlobList->Count);
	//if (filename != nullptr)
		//saveBlobs(filename + "_a_blobspre.bmp", getDictionaryOfIds(rawBlobList));

	PRINT("Num Merged Blobs:" + mergedBlobList->Count);
	//if (filename != nullptr)
		//saveBlobs(filename+ "_b_blobspost.bmp", getDictionaryOfIds(mergedBlobList));
	//if (filename != nullptr)
		//saveBlobsWithColor(filename+ "_c_blobsclr.bmp", mergedBlobList);

	return mergedBlobList;
}

void
ColorBlobDetector::setImageWithPreprocessing(cv::Mat colorImg, String ^ filename)
{
	if (colorImg.type() != CV_32FC3)
		colorImg.convertTo(colorImg, CV_32FC3);
	
	cv::normalize(colorImg, colorImg, 0.0, MAX_RGB_COLOR_VALUE, cv::NORM_MINMAX);
	img = new cv::Mat(colorImg);
}


void 
ColorBlobDetector::doCannyThreshAndSave(cv::Mat channel, String ^ filename)
{
	cv::Mat chan8bit, edges;
	channel.convertTo(chan8bit, CV_8UC1);
	double lowThreshold = 50.0;
	double ratio = 3.0;
	cv::Canny(chan8bit, edges, lowThreshold, lowThreshold*ratio);
	IplImage outputIpl = edges;
	cvSaveImage(managedToSTL(filename).c_str(), &outputIpl);

	outputIpl = chan8bit;
	cvSaveImage(managedToSTL(filename + "_clr.jpg").c_str(), &outputIpl);
}

void 
ColorBlobDetector::resetBlobDetection()
{
	if (img != NULL)
	{
		delete img;
		img = NULL;
	}

	if (blobIds != NULL)
	{
		delete blobIds;
		blobIds = NULL;
	}
	lastBlobId = 0;
}

List<ColorBlob ^> ^
ColorBlobDetector::adaptiveBlobDetection()
{
	Dictionary<BlobId, ColorBlob^> ^ blobs = gcnew Dictionary<BlobId, ColorBlob^>();
	blobIds = new cv::Mat(cv::Mat::zeros(img->size(), CV_32SC1));
	for (int x = 0; x < img->size().width; ++x)
		for (int y = 0; y < img->size().height; ++y)
			processPixel(blobs, blobIds, cv::Point(x, y));

	return listFromDictionary(blobs);
}


void
ColorBlobDetector::processPixel(Dictionary<BlobId, ColorBlob^>^blobs, cv::Mat *blobIds, cv::Point pixel)
{
	BlobId blobId = getBlobId(pixel);
	ColorBlob ^ blob = retrieveBlob(blobs, blobId, pixel);
	blobId = blob->id;
	for each( MCvPoint ^mNeighbor in getNeighborPixels(pixel))
	{
		cv::Point neighbor = *(mNeighbor->pt);
		BlobId neighborId = getBlobId(neighbor);
		
		bool isSameBlob = neighborId == blobId;
		if (isSameBlob)
			continue;

		// grab pixel if it is unassigned and close
		PixelColor pixelColor = getImgColor(pixel);
		PixelColor neighborColor = getImgColor(neighbor);
		float colorDistance = calcColorDistance(pixelColor, neighborColor);
		bool neighborColorIsClose = colorDistance < BlobColorBoundaryThreshold;
		bool neighborHasNoBlob = neighborId == 0;
		if (neighborHasNoBlob && neighborColorIsClose)
		{
			blob->addPoint(neighbor, *blobIds);
			continue;
		}

		// merge blobs if it is assigned and close
		bool neighborInDifferentBlob = !isSameBlob && !neighborHasNoBlob;
		if (neighborInDifferentBlob && neighborColorIsClose)
		{
			ColorBlob ^ neighborBlob = retrieveBlob(blobs, neighborId, neighbor);
			blob->expandToContainBlob(neighborBlob, blobIds);
			blobs->Remove(neighborId);
			continue;
		}
	}
}

BlobId
ColorBlobDetector::getBlobId( cv::Point pt)
{
	return blobIds->at<BlobIdElement>(pt.y, pt.x)[0];
}

PixelColor
ColorBlobDetector::getImgColor(cv::Point pt)
{
	return img->at<PixelColor>(pt.y, pt.x);

}

ref class BlobComparer: Generic::IComparer<ColorBlob ^>
{
public:
    virtual int Compare(ColorBlob^ x, ColorBlob^ y)
    {
        if (x == nullptr)
        {
            if (y == nullptr) return 0;
            else return -1;
        }
        else
        {
            if (y == nullptr) return 1;
            else return x->id - y->id;
        }
    }
};

List<ColorBlob ^> ^
ColorBlobDetector::listFromDictionary(Dictionary<BlobId, ColorBlob^>^ dict)
{
	List<ColorBlob ^> ^ theList = gcnew List<ColorBlob ^>(dict->Count);
	for each (ColorBlob ^ obj in dict->Values)
		theList->Add(obj);
	theList->Sort(gcnew BlobComparer());
	return theList;
}

ColorBlob ^ 
ColorBlobDetector::retrieveBlob(Dictionary<int, ColorBlob^> ^ blobs, int blobId, cv::Point pixel)
{
	if (blobId == 0)
	{
		ColorBlob ^newBlob = makeNewBlob(pixel);
		blobId = newBlob->id;
		blobs->Add(blobId, newBlob);
	}
	return blobs[blobId];
}
ColorBlob ^ 
ColorBlobDetector::makeNewBlob(cv::Point pixel)
{
	lastBlobId += 1;
	ColorBlob ^ newBlob = gcnew ColorBlob(lastBlobId, img->size());
	newBlob->addPoint(pixel, *blobIds);
	return newBlob;
}

List<MCvPoint ^> ^
ColorBlobDetector::getNeighborPixels(cv::Point pixel)
{
	List<MCvPoint ^> ^pts = gcnew List<MCvPoint ^>(8);
	for (int y = pixel.y - 1; y < pixel.y + 2; ++y)
		for (int x = pixel.x - 1; x < pixel.x + 2; ++x)
		{
			bool isSamePixel = x == pixel.x && y == pixel.y;
			bool isInvalidLocation = std::min(x,y) < 0 || x >= img->size().width || y >= img->size().height;
			if (isSamePixel || isInvalidLocation)
				continue;
			pts->Add(gcnew MCvPoint(cv::Point(x,y)));
		}
	return pts;
}

List<ColorBlob ^> ^ 
ColorBlobDetector::combineSameColorBlobs(List<ColorBlob ^> ^ blobList)
{
	List<ColorBlob ^> ^ newBlobs = gcnew List<ColorBlob ^>(blobList->Count);
	for each (ColorBlob ^ blob in blobList)
		addBlobToListWithColorMatching(blob, newBlobs);
	return newBlobs;
}

void 
ColorBlobDetector::addBlobToListWithColorMatching(ColorBlob ^ blob, List<ColorBlob ^> ^ blobList)
{
	bool foundMatchingBlob = false;
	for each (ColorBlob ^ candidateBlob in blobList)
	{
		if (candidateBlob->hasSameColor(blob, *img))
		{
			candidateBlob->expandToContainBlob(blob, blobIds);
			foundMatchingBlob = true;
			break;
		}
	}
	if (!foundMatchingBlob)
		blobList->Add(blob);
}

void 
ColorBlobDetector::saveBlobsToFalseColorImage(List<ColorBlob ^> ^ blobList, String ^ filename, cv::Size size)
{
	cv::Mat outputImg = cv::Mat(size, CV_8UC3);
	outputImg.setTo(0);
	int index = 0;
	for each (ColorBlob ^ blob in blobList)
	{
		FalseColor blobColor = hashIndexToFalseColor(index++);
		blob->drawIntoFalseColorImg(outputImg, blobColor);
	}
	
	saveImage(outputImg, filename);
}

Dictionary<BlobId, int> ^
ColorBlobDetector::getDictionaryOfIds(List<ColorBlob ^> ^ blobs)
{
	Dictionary<BlobId, int> ^ blobIdIndexes = gcnew Dictionary<BlobId, int>();
	int blobIdIndex = 0;
	for each (ColorBlob ^ blob in blobs)
	{
		blobIdIndexes->Add(blob->id, blobIdIndex++);
	}
	return blobIdIndexes;
}

void 
ColorBlobDetector::saveBlobs(String ^ filename, Dictionary<BlobId, int> ^ blobIdIndexes)
{
	typedef cv::Vec<unsigned char, 3> VT;
	cv::Mat outputImg = cv::Mat(blobIds->size(), CV_8UC3);
	cv::MatIterator_<VT> outputIt = outputImg.begin<VT>();
	for (cv::MatIterator_<BlobIdElement> blobIdIt = blobIds->begin<BlobIdElement>(); 
		 blobIdIt != blobIds->end<BlobIdElement>(); ++blobIdIt) 
	{
		BlobId theId = (*blobIdIt)[0];
		int index;
		if (theId == 0)
			index = 0;
		else
			index = blobIdIndexes[theId];

		unsigned char r, g, b;

		hashIndexToBGR(index, b, g, r);

		(*outputIt)[0] = b;
		(*outputIt)[1] = g;
		(*outputIt)[2] = r;
		++outputIt;
	}
	
	saveImage(outputImg, filename);
}

FalseColor 
ColorBlobDetector::hashIndexToFalseColor(int index)
{
	unsigned char r, g, b;
	hashIndexToBGR(index, b, g, r);
	FalseColor color;
	color[0] = b;
	color[1] = g;
	color[2] = r;
	return color;
}


void 
ColorBlobDetector::hashIndexToBGR(int index, unsigned char & b, unsigned char & g, unsigned char & r)
{
	// generally picks a unique color for each blob, collisions possible
	//		if index is above 63
	r = 255 - (index)%4 * 64;
	g = 255 - (index >> 2)%4 * 64;
	b = 255 - (index >> 4)%4 * 64;
}

void 
ColorBlobDetector::saveBlobsWithColor(String ^ filename, List<ColorBlob ^> ^ mergedBlobs)
{
	cv::Mat outputImg = cv::Mat(blobIds->size(), CV_8UC3);
	outputImg.setTo(0);
	for each (ColorBlob ^ blob in mergedBlobs)
		blob->drawIntoImg(outputImg);
	
	saveImage(outputImg, filename);
}
