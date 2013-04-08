#include "StdAfx.h"
#include "Dataset.h"
#include <cv.h>
#include "Util.h"

#define SIZE_OF_FEATURE_VECTOR 320
#define NUMBER_K_NEAREST_NEIGHBORS 2

using namespace System;
using namespace Vision;
using namespace Util;

Dataset ^ Dataset::LoadFrom(cv::FileStorage fs, String ^prefix)
{
	cv::Mat trainData, trainClasses;
	fs[managedToSTL(prefix + "trainData")] >> trainData;
	fs[managedToSTL(prefix + "trainClasses")] >> trainClasses;
		
	return gcnew Dataset(trainData, trainClasses);
}

cv::Mat Dataset::arrayToMat(float *arraySample, int length)
{
	typedef cv::Vec<float, 1> VT;
	VT *rowPointer;

	cv::Mat matSample = cv::Mat(1, length, CV_32FC1);
	rowPointer = matSample.ptr<VT>(0);		// save feature vector
	for (int j = 0; j < length; j++)
		rowPointer[j] = arraySample[j];

	return matSample;
}

Dataset::Dataset(int numSamples)
{
	bool invalidNumberOfSamples = numSamples == 0;
	if (invalidNumberOfSamples)
		throw gcnew Exception("Invalid number of samples for Vision::Dataset constructor");
	
	cv::Mat data = cv::Mat(numSamples, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	cv::Mat classes = cv::Mat(numSamples, 1, CV_32FC1);
	
	allocAndInitialize(data, classes);
	resetDataset();
}

Dataset::Dataset(cv::Mat trainDataIn, cv::Mat trainClassesIn)
{
	allocAndInitialize(trainDataIn, trainClassesIn);
}

void Dataset::allocAndInitialize(cv::Mat trainDataIn, cv::Mat trainClassesIn)
{
	trainData = new cv::Mat(trainDataIn);
	trainClasses = new cv::Mat(trainClassesIn);
	trainedKNN = NULL;
	nextSampleIndex = trainDataIn.rows;

	rebuildKNN();

}

void Dataset::resetDataset()
{
	nextSampleIndex = 0;
	trainData->setTo(0.0);
	trainClasses->setTo(0.0);

	rebuildKNN();
}

void Dataset::rebuildKNN()
{
	if (trainedKNN != NULL)
		delete trainedKNN;
	trainedKNN = new CvKNearest(*trainData, *trainClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);
}

void Dataset::trainOn(cv::Mat sample, float classValue)
{
	confirmSpaceForTrainingData();

	copySampleIntoDataAtIndex(sample, nextSampleIndex);
	copyValueIntoClassAtIndex(classValue, nextSampleIndex);
	
	nextSampleIndex += 1;
	rebuildKNN();
}

void Dataset::confirmSpaceForTrainingData()
{
	int maxSamples = trainData->rows;
	if (nextSampleIndex >= maxSamples)
		throw gcnew ArgumentOutOfRangeException();
}

void Dataset::copySampleIntoDataAtIndex(cv::Mat sample, int index)
{
	int leftSide = 0;
	const cv::Rect dstROI = cv::Rect(leftSide, index, sample.cols, sample.rows);
	cv::Mat trainDataSubset = (*trainData)(dstROI);
	sample.copyTo(trainDataSubset);
}

void Dataset::copyValueIntoClassAtIndex(float value, int index)
{
	typedef cv::Vec<float, 1> VT;
	VT *classesPtr = trainClasses->ptr<VT>(index);		
	classesPtr[0] = value;
}

float Dataset::testOn(cv::Mat sample)
{	
	cv::Mat sampleDist;
	float response = trainedKNN->find_nearest(sample, 1, cv::Mat(), cv::Mat(), sampleDist);
	float conf = sampleDist.at<cv::Vec<float, 1>>(0,0)[0];

	return response;
}

void Dataset::saveTo(cv::FileStorage fs, String ^prefix)
{
	fs << managedToSTL(prefix + "trainData") << *trainData;
	fs << managedToSTL(prefix + "trainClasses") << *trainClasses;
}

Dataset::!Dataset()
{
	delete trainData;
	delete trainClasses;
	delete trainedKNN;
}
