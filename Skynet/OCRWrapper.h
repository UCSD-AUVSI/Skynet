#pragma once
#include <cv.h>
#include <highgui.h>
#include "MasterHeader.h"

class Auvsi_Ocr;

namespace Vision
{
	#define SOFTWARE_DIRECTORY (DROPBOX_DIR + "AUVSI\\Technical\\Software\\")
	#define SOFTWARE_DIRECTORY "C:\\Users\\ucsd_auvsi\\Dropbox\\AUVSI\\Technical\\Software\\"
	#define TRAINING_DIRECTORY (SOFTWARE_DIRECTORY + "OCR training data\\")
	#define TESTING_DIRECTORY (SOFTWARE_DIRECTORY + "OCR testing data\\")

 	using namespace System;
 	using namespace System::Threading;
	using namespace Collections::Generic;

	ref class VisionController;
	ref class Dataset;
	ref class TargetRecognizer;
	public ref struct TargetResult
	{
		String ^ letter;
		String ^ shape;
		String ^ shapeColor;
		String ^ letterColor;

		/**
		 * Orientation of the target, counterclockwise from due east.
		 */
		double targetOrientationDegrees;

		TargetResult(String ^ letter, String ^ shape, String ^ shapeColor, String ^ letterColor, double targetOrientationDegrees): 
			letter(letter), shape(shape), shapeColor(shapeColor), letterColor(letterColor), targetOrientationDegrees(targetOrientationDegrees) {}

		String^ getHeadingString(){
			if (targetOrientationDegrees > 0 && targetOrientationDegrees < 22.5){
				return "E";
			} else if (targetOrientationDegrees < 67.5){
				return "NE";
			} else if (targetOrientationDegrees < 112.5){
				return "N";
			} else if (targetOrientationDegrees < 157.5){
				return "NW";
			} else if (targetOrientationDegrees < 202.5){
				return "W";
			} else if (targetOrientationDegrees < 247.5){
				return "SW";
			} else if (targetOrientationDegrees < 292.5){
				return "S";
			} else if (targetOrientationDegrees < 337.5){
				return "SE";
			} else if (targetOrientationDegrees < 360){
				return "E";
			} else {
				throw gcnew Exception("targetOrientation must be at least 0 and less than 360");
			}
		}
	};

 	ref struct OcrItem
 	{
 		array<float> ^ _homography;
 		double _heading;
 		String ^ _path;
 		System::Windows::Forms::DataGridViewRow ^ _row;
 	};

	public ref class OCRWrapper
	{
	public:
		OCRWrapper(VisionController ^ parent);
 		OCRWrapper( Object ^ parent, int nThreads );

 		void ocrThread( void );

 		void startProcessing(void);
 		void stopProcessing(void);

		TargetResult^ recognizeImage(cv::Mat image);
		TargetResult^ recognizeImage(cv::Mat image, String ^ filepath);

		// compute feature descriptor for shape or both?
		String ^ recognizeLetter(String^ const imageFilename);
		String ^ recognizeLetter(cv::Mat image);

		TargetRecognizer ^ targetRecognizer;
 	private:
		Dataset ^ shapeDataset;
		
 		void buildTrainingSet();
		void runTestingSet();
		void testArtificialLetters();
		void testArtificialShapes();
		void testRealImages();

 		static Mutex ^ mutex_Data = gcnew Mutex;
 		int _nThreads;
 		array<Thread ^> ^ _threads;
 		Object ^ _parent;
 	};
}
