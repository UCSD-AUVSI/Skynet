#pragma once

#include <cv.h>
#include <highgui.h>

namespace tessnet2 {
	ref class Tesseract;
}

namespace Vision {
	using namespace System;

	public ref class TessCharNotMatchedException : Exception
	{
	public:
		TessCharNotMatchedException(String^ message) 
			: Exception(message) {}

	};

	ref class TessOCRAttempt
	{
	public:
		String ^str;
		float conf;
		float rotationDegrees;
		
		TessOCRAttempt(String ^ str, float conf, float rotationDegrees): 
			str(str), conf(conf), rotationDegrees(rotationDegrees)
		{
		}

		/**
		 * TODO: This should return a pointer to the better attempt
		 */
		void replaceIfBetter(TessOCRAttempt ^ other)
		{
			if (this->conf < 0.0f || this-> < other->conf)
			{
				this->conf = other->conf;
				this->str = other->str;
				this->rotationDegrees = other->rotationDegrees;
			}
		}

	};

	ref class TessWrapper
	{
	public:
		TessWrapper(void);
		
		Tuple2<String^, double>^ computeImage(cv::Mat blackWhiteImg);

	private:
		tessnet2::Tesseract ^ ocr;

		TessOCRAttempt ^attemptOCRForRotation(cv::Mat img, float rotation);
		cv::Mat rotateImage(const cv::Mat& source, double angle);
		void sortResultsByConfidence(Collections::Generic::List<tessnet2::Word ^>^ words);
		Drawing::Bitmap ^matToBitmap(cv::Mat img);
		cv::Mat thresholdImage(cv::Mat input);
		void setGrayscalePalette(Drawing::Bitmap ^ bmp);
		String ^getBestWord(Collections::Generic::List<tessnet2::Word ^>^ words);
	};
}
