#pragma once
#include <cv.h>
#include <highgui.h>

class Auvsi_Ocr;

namespace Vision
{
 	using namespace System;
 	using namespace System::Threading;

	ref class VisionController;
	ref class Dataset;
	ref class TessWrapper;

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
		ref struct Results
		{
			wchar_t letter;
			String^ shape;
			float letterConfidence;
			float shapeConfidence;
			Results(wchar_t letter, String^ shape, float letterConfidence, float shapeConfidence):
				letter(letter), shape(shape), letterConfidence(letterConfidence), shapeConfidence(shapeConfidence){}
		};
		OCRWrapper(VisionController ^ parent);
 		OCRWrapper( Object ^ parent, int nThreads );

 		void ocrThread( void );

 		void startProcessing(void);
 		void stopProcessing(void);

		// take image, calc letter and shape of target
		void recognizeImage(cv::Mat image, wchar_t & letter, String ^ &shape)
		{
			float a,b;
			recognizeImage(image, letter, shape, a, b);
		}
		void recognizeImage(cv::Mat image, wchar_t & letter, 
				String ^ &shape, float & letterConf, float & shapeConf)
		{
			recognizeImage(image, letter, shape, letterConf, shapeConf, "");
		}
		void recognizeImage(cv::Mat image, wchar_t & letter, 
				String ^ &shape, float & letterConf, float & shapeConf, String ^filename);
		Results^ recognizeImage(cv::Mat image);

		// compute feature descriptor for shape or both?
		void computeImageForShape(Auvsi_Ocr * chenOCR, cv::Mat src, float **shapeDescriptor, String ^ filepath);
		void computeImageForBoth(Auvsi_Ocr * chenOCR, cv::Mat src,  wchar_t & letterChar, float **shapeDescriptor, String ^ filepath);
		
		void computeImageForShape(Auvsi_Ocr * chenOCR, cv::Mat src, float **shapeDescriptor)
		{ computeImageForShape(chenOCR, src, shapeDescriptor, ""); }
		void computeImageForBoth(Auvsi_Ocr * chenOCR, cv::Mat src,  wchar_t & letterChar, float **shapeDescriptor)
		{ computeImageForBoth(chenOCR, src, letterChar, shapeDescriptor, nullptr);}


 	private:
		Dataset ^ shapeDataset;
		
 		void buildTrainingSet();
		void runTestingSet();
		void testArtificialLetters();
		void testArtificialShapes();
		void testRealImages();

		wchar_t recognizeLetter(String^ const imageFilename, float & conf);
		wchar_t recognizeLetter(cv::Mat image, float & conf);

		String^ analyzeImageForShape (String^ const imageFilename, float & conf);
		void analyzeImageForBoth(String^ const imageFilename, wchar_t & letter, 
				String ^ &shape, float & letterConf, float & shapeConf);

		

 		static Mutex ^ mutex_Data = gcnew Mutex;
 		int _nThreads;
 		array<Thread ^> ^ _threads;
 		Object ^ _parent;
		TessWrapper ^tessOCR;
 	};
}
