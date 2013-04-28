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
		
		TessOCRAttempt(String ^ s, float c)
		{
			str = s;
			conf = c;
		}

		void replaceIfBetter(TessOCRAttempt ^ candidate)
		{
			replaceIfBetter(candidate->str, candidate->conf);
		}

		void replaceIfBetter(String ^ s, float c)
		{
			if (conf < 0.0f || c < conf)
			{
				conf = c;
				str = s;
			}
		}
	};

	ref class TessWrapper
	{
	public:
		TessWrapper(void);
		
		String ^computeImage(cv::Mat blackWhiteImg);

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
