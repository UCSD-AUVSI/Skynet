#pragma once

#include <cv.h>
#include <highgui.h>
#include "VisionUtil.h"

namespace Vision {

	using namespace System;
	using namespace Collections;
	using namespace Collections::Generic;

	ref class ColorBlob;

	ref class Segmenter
	{
	public:
		Segmenter(void)
		{
		}

		virtual List<ColorBlob ^> ^ findBlobs(cv::Mat colorImg, String ^ filename)
		{
			throw gcnew Exception("Tried to use Abstract Class - Use subclass instead");
			return nullptr;
		}

		List<ColorBlob ^> ^ findBlobs(cv::Mat colorImg) 
		{ 
			return findBlobs(colorImg, nullptr); 
		}

		virtual cv::Mat secondSegmentation(PixelColor color)
		{
			throw gcnew Exception("Tried to use Abstract Class - Use subclass instead");
			return cv::Mat();
		}

		
	};

}
