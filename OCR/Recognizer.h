#pragma once

#include "../MasterHeader.h"

#include <cv.h>

using namespace std;
using namespace System;


namespace Vision
{

	public ref struct ImageData {
		String ^ shape;
		String ^ letter;

		ImageData(String ^s, String ^l)
		{
			shape = s;
			letter = l;
		}
	};

	public ref class Recognizer {
	public:
		Recognizer();

		void trainRecognizer();

		ImageData ^ recognizeImage(cv::Mat input);

	private:
		void loadData();

		void trainLetters(Object ^ dirArg);
		void trainShapes(Object ^ dirArg);

		void saveDatabase(cv::Mat data, cv::Mat classes, string filename);

		float letterStrToInt(String ^ letter);
		String ^ letterIntToStr(float input);
		float shapeStrToInt(String ^ shape);
		String ^ shapeIntToStr(float input);

		bool	isReady;
	};

}