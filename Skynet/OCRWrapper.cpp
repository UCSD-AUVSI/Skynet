#include "StdAfx.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include "Form1.h"
#include "OCRWrapper.h"
#include "Dataset.h"
#include "Auvsi_OCR.h"
#include "ColorBlob.h"
#include "ColorBlobDetector.h"
#include "ShapeFinder.h"
#include "HistogramSegmenter.h"
#include <ml.h>
#include <cmath>
#include "Util.h"
#include "VisionUtil.h"
#include "ShapeRecognizer.h"
#include <msclr/lock.h>
#include "ShapeTester.h"
#include "LetterTester.h"
#include "TargetRecognizer.h"

#define OCR_THREAD_INTERVAL 5

#define NUMBER_K_NEAREST_NEIGHBORS 2


#define LETTER_TEST_DIRECTORY (TESTING_DIRECTORY + "LetterTrainSet")
//#define SHAPE_TEST_DIRECTORY (TESTING_DIRECTORY + "SingleShape")
#define SHAPE_TEST_DIRECTORY (TESTING_DIRECTORY + "GeneratedShapes")
//#define SHAPE_TEST_DIRECTORY (TESTING_DIRECTORY + "ShapeTestSet")
//#define COMBINED_TEST_DIRECTORY (TESTING_DIRECTORY + "CombinedCanvasVaried")
#define COMBINED_TEST_DIRECTORY (TESTING_DIRECTORY + "CombinedPlywood")

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::IO;
using namespace System::Runtime::InteropServices;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace msclr;

OCRWrapper::OCRWrapper(VisionController ^ parent)
{
	targetRecognizer = gcnew TargetRecognizer();
	ColorBlob::runTests();

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

void
OCRWrapper::runTestingSet()
{
	// flags
	// TODO: Fix this
	bool testLetters = false;
	bool testShapes = false;
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
	LetterTester ^ letterTester = gcnew LetterTester(files, this);
	letterTester->testAllFiles();
}

void OCRWrapper::testArtificialShapes()
{
	array<String^ >^ files = Directory::GetFiles(SHAPE_TEST_DIRECTORY,"*.png");
	ShapeTester ^ shapeTester = gcnew ShapeTester(files);
	shapeTester->testAllFiles();
}

void OCRWrapper::testRealImages()
{
	array<String^ >^ files = Directory::GetFiles(COMBINED_TEST_DIRECTORY,"*.png");

	PRINT("Testing Combined...");
	int numCorrectLetters = 0;
	int numCorrectShape = 0;
	int numFiles = files->Length;
	for each(String^ filepath in files)
	{
		String^ filename = extractFilename(filepath);

		String^ shapeGuess;
		String ^ letterGuess;
		//recognizeImage(filepath, letterGuess, shapeGuess);
		// TODO: Fix this shit

		String ^ letterTruth = parseFilenameForLetter(filename);
		
		letterGuess = disambiguateLetter(letterGuess);
		letterTruth = disambiguateLetter(letterTruth);

		if (letterGuess == nullptr)
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
	/*OCRTrainer ^ trainer = gcnew OCRTrainer(this);
	trainer->BuildDatasets();
	shapeDataset = trainer->getShapeDataset();
	*/
	runTestingSet();

}


String ^ OCRWrapper::recognizeLetter(String^ const imageFilename)
{
	ManagedIPL ^image = gcnew ManagedIPL(imageFilename, CV_LOAD_IMAGE_GRAYSCALE);
	
	Auvsi_Ocr *chenOCR = new Auvsi_Ocr();
	cv::Mat processedImage = chenOCR->prepareTrainingImage(image->img, false, Util::managedToSTL(imageFilename));
	delete chenOCR;

	return recognizeLetter(processedImage);
}

String ^ OCRWrapper::recognizeLetter(cv::Mat image)
{
	String ^ letterString = targetRecognizer->recognizeLetter(image);
	return letterString;
}

TargetResult^ OCRWrapper::recognizeImage(cv::Mat image)
{
	try {
		TargetResult ^ result = targetRecognizer->recognizeTarget(image);
		return result;
	}
	catch (TargetNotFound ^)
	{
		PRINT("Could not find a target");
		return nullptr;
	}
}

TargetResult^ OCRWrapper::recognizeImage(cv::Mat image, String ^ filepath)
{
	try {
		TargetResult ^ result = targetRecognizer->recognizeTarget(image, filepath);
		return result;
	}
	catch (TargetNotFound ^)
	{
		PRINT("Could not find a target");
		return nullptr;
	}
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

		TargetResult^ ocrGuesses = recognizeImage(image);

		System::Diagnostics::Trace::WriteLine("Shape: " + ocrGuesses->shape + " letter: " + ocrGuesses->letter);

	//	 TODO: build data to send back, put in shape and letter

	//	 Put back into table
		((Skynet::Form1 ^)_parent)->Invoke( ((Skynet::Form1 ^)_parent)->ocrDelegate/*, DATA_GOES_HERE*/ );

	//	 Wait a bit and then keep going
		Thread::Sleep( OCR_THREAD_INTERVAL );
	}
}