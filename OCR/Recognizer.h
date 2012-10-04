#pragma once

#include "../MasterHeader.h"

#include <cv.h>

using namespace std;
using namespace System;


namespace Vision
{

	public ref struct ImageData {
	public:
		String ^ toString(){
			return "Shape: " + shape + ", Letter: " + letter;
		};
		ImageData(String ^s, String ^l)
		{
			shape = s;
			letter = l;
		};
	private:
		String ^ shape;
		String ^ letter;

		
	};

	public ref class Recognizer {
	public:
		Recognizer();

		void trainRecognizer();

		ImageData ^ recognizeImage(cv::Mat input);

	private:
		void loadData();

		void train(String ^ directory, String ^ outputFilename);
		void trainLetters(Object ^ directory);
		void trainShapes(Object ^ directory);

		void saveDatabase(cv::Mat data, cv::Mat classes, String ^ filename);

		int objectStrToInt(String ^ object);
		String ^ letterIntToStr(int input);
		int shapeStrToInt(String ^ shape);
		String ^ shapeIntToStr(int input);

		bool	isReady;
	};

}