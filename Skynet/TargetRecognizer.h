#pragma once

#include <cv.h>
#include <highgui.h>
#include "VisionUtil.h"
#include "Color_Util.h"

namespace Vision {
	
	using namespace System;
	using namespace System::Collections::Generic;
	
	ref struct TargetResult;

	ref class TessWrapper;

	ref class TargetNotFound : System::Exception {};

	ref class TargetRecognizer
	{
	public:
		static TargetRecognizer() { tessOCR = nullptr; }
		TargetRecognizer(void);
		
		TargetResult ^ recognizeTarget(String ^ testShapeFilename);
		TargetResult ^ recognizeTarget(cv::Mat img);
		TargetResult ^ recognizeTarget(cv::Mat img, String ^ filepath);

		String ^ recognizeLetter(cv::Mat img);
	private:
		TargetResult ^ doRecognition();
		void segmentShape();
		bool shapeIsValid();
		String ^ recognizeShape();
		void segmentLetter();
		String ^ recognizeLetter();
		bool letterIsValid();
        String ^ recognizeColor(cv::Mat colorImg, cv::Mat img);
		double calculateTargetOrientationDegrees();

		String ^ filename;
		static TessWrapper ^tessOCR;
		MRef<cv::Mat> ^ mColorImg;
		MRef<cv::Mat> ^ mShapeImg;
		MRef<cv::Mat> ^ mLetterImg;
	};

}
