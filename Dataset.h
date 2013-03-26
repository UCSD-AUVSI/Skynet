#pragma once

#include <cv.h>
#include <highgui.h>
#include <ml.h>

namespace Vision
{
	using namespace System;

	ref class Dataset
	{
	public:
		static Dataset ^LoadFrom(cv::FileStorage fs, String ^prefix);
		static cv::Mat arrayToMat(float *arraySample, int length);

		Dataset(int numSamples);
		Dataset(cv::Mat trainDataIn, cv::Mat trainClassesIn);
		!Dataset();
		~Dataset() { this->!Dataset(); }

		void allocAndInitialize(cv::Mat trainDataIn, cv::Mat trainClassesIn);

		void trainOn(cv::Mat sample, float classValue);
		float testOn(cv::Mat sample);

		void trainOn(float *sample, int length, float classValue)
		{
			cv::Mat matSample = arrayToMat(sample, length);
			trainOn(matSample, classValue);
		}

		float testOn(float *sample, int length)
		{
			cv::Mat matSample = arrayToMat(sample, length);
			float result = testOn(matSample);
			return result;
		}

		void saveTo(cv::FileStorage fs, String ^prefix);
		
	private:
		void rebuildKNN();
		void resetDataset();
		void confirmSpaceForTrainingData();
		void copySampleIntoDataAtIndex(cv::Mat sample, int index);
		void copyValueIntoClassAtIndex(float value, int index);

		CvKNearest *trainedKNN;
		cv::Mat *trainData;
		cv::Mat *trainClasses;
		int nextSampleIndex;

	};
}
