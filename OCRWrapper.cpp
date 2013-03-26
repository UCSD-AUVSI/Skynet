#include "StdAfx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "Form1.h"
#include "OCRWrapper.h"
#include "OCRTrainer.h"
#include "Dataset.h"
#include "Auvsi_OCR.h"
#include "TessWrapper.h"
#include "ColorBlob.h"
#include "ColorBlobDetector.h"
#include "ShapeFinder.h"
#include "HistogramSegmenter.h"
#include <ml.h>
#include <cmath>
#include "Util.h"
#include "VisionUtil.h"
#include "ShapeRecognizer.h"

#define OCR_THREAD_INTERVAL 5

#define NUMBER_K_NEAREST_NEIGHBORS 2


#define LETTER_TEST_DIRECTORY (TESTING_DIRECTORY + "LetterTrainSet")
//#define SHAPE_TEST_DIRECTORY (TESTING_DIRECTORY + "SingleShape")
#define SHAPE_TEST_DIRECTORY (TESTING_DIRECTORY + "SmallGeneratedShapes")
#define COMBINED_TEST_DIRECTORY (TESTING_DIRECTORY + "CombinedCanvasVaried")

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;

OCRWrapper::OCRWrapper(VisionController ^ parent)
{
	ColorBlob::runTests();
	tessOCR = gcnew TessWrapper();

	buildTrainingSet();
}


OCRWrapper::OCRWrapper( Object ^ parent, int nThreads )
{
	_nThreads = nThreads;
	_parent = parent;

	buildTrainingSet();
	_threads = gcnew array<Thread ^>( _nThreads );
	startProcessing();
}

void
OCRWrapper::startProcessing( void )
{
	for( int i = 0; i < _nThreads; ++i )
	{
		_threads[i] = gcnew Thread( gcnew ThreadStart(this, &OCRWrapper::ocrThread) );
		_threads[i]->Name = "OCRWrapper Thread " + i;
		_threads[i]->Start();
	}
}

void
OCRWrapper::stopProcessing( void )
{
	for( int i = 0; i < _nThreads; ++i )
	{
		try
		{
			_threads[i]->Abort();
		}
		catch( Exception ^ )
		{

		}
	}
}

void OCRWrapper::computeImageForShape(Auvsi_Ocr * theOCR, cv::Mat src, float **shapeDescriptor, String ^ filepath)
{
	if (src.size().width != 0)
	{
		theOCR->processTrainingImageForShape(src, shapeDescriptor, Util::managedToSTL(filepath));
	}
	else 
	{
		PRINT("ERROR: null image");
	}
}

void OCRWrapper::computeImageForBoth(Auvsi_Ocr * chenOCR, cv::Mat src, wchar_t & letterChar, float **shapeDescriptor, String ^filepath)
{
	bool srcIsValid = src.size().width != 0;
	if (srcIsValid) 
	{
		cv::Mat letterImg, shapeImg;
		chenOCR->segmentImageIntoShapeAndLetter(src, letterImg, shapeImg, Util::managedToSTL(filepath));
		return; // DEBUG: this will break ocr
		chenOCR->computeFeatureDescriptor(shapeImg, shapeDescriptor);
		bool letterIsValid = letterImg.rows > 0 && letterImg.cols > 0;
		if (letterIsValid)
		{
			float conf;
			letterChar = recognizeLetter(letterImg, conf);
		}
		else
		{
			letterChar = 0;
		}
	}
	else 
	{
		PRINT("ERROR: null image");
	}

}

void
OCRWrapper::runTestingSet()
{
	// flags
	bool testLetters = false;
	bool testShapes = true;
	bool testCombined = false;

	if (testLetters)
		testArtificialLetters();
	
	if (testShapes)
		testArtificialShapes();
	
	if (testCombined)
		testRealImages();

}

void OCRWrapper::testArtificialLetters()
{
	array<String^ >^ files = Directory::GetFiles(LETTER_TEST_DIRECTORY,"*.png");
	float conf;

	PRINT("Testing Letters...");
	int numCorrect = 0;
	int numFiles = files->Length;
	for each(String^ filepath in files)
	{
		String^ filename = extractFilename(filepath);

		const wchar_t truth = parseFilenameForLetter(filename);

		wchar_t guess = recognizeLetter(filepath, conf);
		guess = disambiguateLetter(guess);

		if (guess == truth)
		{
			PRINT(filename + " -> Correct for " + guess);
			numCorrect += 1;
		}
		else
		{
			PRINT(filename + " -> Error on " + filename + ", recognized " + guess);
		}
	}
	PRINT("Letter accuracy: " + 100.0*(numCorrect/(float)numFiles) + "%");
}

void OCRWrapper::testArtificialShapes()
{
	array<String^ >^ files = Directory::GetFiles(SHAPE_TEST_DIRECTORY,"*.png");
	ShapeRecognizer ^ shapeRecognizer = gcnew ShapeRecognizer();
	//float conf;
	
	PRINT("Testing Shapes...");
	int numCorrect = 0;
	int numFiles = files->Length;
	for each(String^ filepath in files)
	{
		String^ filename = extractFilename(filepath);

		String^ truth = parseFilenameForSeparateShape(filename);
		//String^ guess = analyzeImageForShape(filepath, conf);
		String ^ guess = shapeRecognizer->recognizeShape(filepath);

		if (truth->Equals(guess))
		{
			PRINT(filename + " -> Correct for " + guess);
			numCorrect += 1;
		}
		else
		{
			PRINT(filename + " -> Error on " + filename + ", recognized " + guess);
		}
	}
	PRINT("Shape accuracy: " + 100.0*(numCorrect/(float)numFiles) + "%");
}

void OCRWrapper::testRealImages()
{
	array<String^ >^ files = Directory::GetFiles(COMBINED_TEST_DIRECTORY,"*.png");
	float conf;

	PRINT("Testing Combined...");
	int numCorrectLetters = 0;
	int numCorrectShape = 0;
	int numFiles = files->Length;
	for each(String^ filepath in files)
	{
		String^ filename = extractFilename(filepath);

		String^ shapeGuess;
		wchar_t letterGuess;
		analyzeImageForBoth(filepath, letterGuess, shapeGuess, conf, conf);

		wchar_t letterTruth = parseFilenameForLetter(filename);
		
		letterGuess = disambiguateLetter(letterGuess);
		letterTruth = disambiguateLetter(letterTruth);

		if (letterGuess == 0)
		{
			PRINT("ERROR: missing letter from segmentation");
		}
		else if (letterTruth == letterGuess)
		{
			PRINT(filename + " -> Correct for " + letterTruth);
			numCorrectLetters += 1;
		}
		else
		{
			PRINT(filename + " -> Error on " + filename + ", recognized " + letterGuess + "(truth:" + letterTruth + ")");
		}

		// compute shape accuracy
		String^ shapeTruth = parseFilenameForShape(filename);

		if (shapeTruth->Equals(shapeGuess))
		{
			PRINT(filename + " -> Correct for " + shapeTruth);
			numCorrectShape += 1;
		}
		else
		{
			PRINT(filename + " -> Error on " + filename + ", recognized " + shapeGuess + "(truth:" + shapeTruth + ")" );
		}
	}
	
	PRINT("**** OCR Results (natural) ****");
	PRINT("Letters accuracy: " + 100.0*(numCorrectLetters/(float)numFiles) + "%");
	PRINT("Shape accuracy: " + 100.0*(numCorrectShape/(float)numFiles) + "%");

}


void
OCRWrapper::buildTrainingSet()
{
	OCRTrainer ^ trainer = gcnew OCRTrainer(this);
	trainer->BuildDatasets();
	shapeDataset = trainer->getShapeDataset();

	runTestingSet();

}


wchar_t OCRWrapper::recognizeLetter(String^ const imageFilename, float & conf)
{
	ManagedIPL ^image = gcnew ManagedIPL(imageFilename, CV_LOAD_IMAGE_GRAYSCALE);
	
	Auvsi_Ocr *chenOCR = new Auvsi_Ocr();
	cv::Mat processedImage = chenOCR->prepareTrainingImage(image->img, false, Util::managedToSTL(imageFilename));
	delete chenOCR;

	return recognizeLetter(processedImage, conf);
}

wchar_t OCRWrapper::recognizeLetter(cv::Mat image, float & conf)
{
	String ^ letterString = tessOCR->computeImage(image);
	wchar_t letterChar = (wchar_t)letterString->ToLower()[0];
	return letterChar;
}

String^ OCRWrapper::analyzeImageForShape (String^ const imageFilename, float & conf) 
{
	Auvsi_Ocr *chenOCR = new Auvsi_Ocr();
	
	
	float *shapeFeatDesc;
	ManagedIPL ^image = gcnew ManagedIPL(imageFilename, CV_LOAD_IMAGE_GRAYSCALE);
	computeImageForShape(chenOCR, image->img, &shapeFeatDesc);
	float responseShape = shapeDataset->testOn(shapeFeatDesc, SIZE_OF_FEATURE_VECTOR);
	conf = 0.0f;

	delete chenOCR;
	return shapeFloatToString(responseShape);
}

void OCRWrapper::analyzeImageForBoth(String^ const imageFilename, wchar_t & letter, String ^ &shape, float & letterConf, float & shapeConf)
{
	ManagedIPL ^image = gcnew ManagedIPL(imageFilename, CV_LOAD_IMAGE_COLOR);
	
	//ColorBlobDetector ^ blobDetector = gcnew ColorBlobDetector();
	//blobDetector->findBlobs(image->img, imageFilename);
	ShapeFinder ^ shapeFinder = gcnew ShapeFinder();
	shapeFinder->findShape(image->img, imageFilename);
	//HistogramSegmenter ^histSeg = gcnew HistogramSegmenter();
	//histSeg->segmentImage(image->img, imageFilename);

	return; // DEBUG this breaks the OCR
	recognizeImage(image->img, letter, shape, letterConf, shapeConf, imageFilename);
}

OCRWrapper::Results^ OCRWrapper::recognizeImage(cv::Mat image)
{
	wchar_t letter;
	String^ shape;
	float letterConf, shapeConf;
	recognizeImage(image, letter, shape, letterConf, shapeConf);
	return gcnew Results(letter,shape,letterConf,shapeConf);
}

void OCRWrapper::recognizeImage(cv::Mat image, wchar_t & letter, 
				String ^ &shape, float & letterConf, float & shapeConf, String^ filename)
{
	Auvsi_Ocr *ocr = new Auvsi_Ocr();
	
	float *shapeFeatDesc;
	wchar_t letterGuess;
	computeImageForBoth(ocr, image, letterGuess, &shapeFeatDesc, filename);

	String ^ shapeGuess;

	if (shapeFeatDesc == NULL)
	{
		PRINT("ERROR: null shape descriptor in OCRWrapper::recognizeImage");
	}
	else
	{
		float responseShape = shapeDataset->testOn(shapeFeatDesc, SIZE_OF_FEATURE_VECTOR);
		shapeGuess = shapeFloatToString(responseShape);
		shapeConf = 0.0f;

		bool letterIsValid = letterGuess != 0;
		if (!letterIsValid)
			letterConf = -1.0f;

		// store results
		shape = shapeGuess;
		letter = letterGuess;
	}
	// clean up memory
	delete ocr;
}

void OCRWrapper::ocrThread( void )
{
	return;
	
	/////////////////////////////////////////////
	OcrItem ^ currentData;

	// main run loop
	while( true )
	{		
	//	 Get new data
		mutex_Data->WaitOne();
		// currentData = ((Skynet::Form1 ^)_parent)->getCandidateData();
		mutex_Data->ReleaseMutex();
		bool noValidData = currentData == nullptr;
		if (noValidData)
			continue; 


	//	 load image 
		String ^ filename = currentData->_path;
		std::string filenameStr = Util::managedToSTL(filename);
		cv::Mat image = cv::imread(filenameStr);

		Results^ ocrGuesses = recognizeImage(image);

		System::Diagnostics::Trace::WriteLine("Shape: " + ocrGuesses->shape + " letter: " + ocrGuesses->letter);

	//	 TODO: build data to send back, put in shape and letter

	//	 Put back into table
		((Skynet::Form1 ^)_parent)->Invoke( ((Skynet::Form1 ^)_parent)->ocrDelegate/*, DATA_GOES_HERE*/ );

	//	 Wait a bit and then keep going
		Thread::Sleep( OCR_THREAD_INTERVAL );
	}
}