#include "StdAfx.h"

#include "Recognizer.h"
#include "Auvsi_Ocr.h"
#include "Auvsi_Radon.h"
#include "Auvsi_DualTree.h"
#include "Auvsi_Fft.h"
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#pragma warning(pop)


#define SIZE_OF_FEATURE_VECTOR 320
#define NUMBER_K_NEAREST_NEIGHBORS 2

using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace Vision;

#define LETTER_DATA_FILE "/Users/lewisanderson/Code/OCR/database/letterData"
#define SHAPE_DATA_FILE "/Users/lewisanderson/Code/OCR/database/shapeData"
#define LETTER_TRAIN_DIR "/Users/lewisanderson/Code/OCR/train/letter/"
#define SHAPE_TRAIN_DIR "/Users/lewisanderson/Code/OCR/train/shape/"

std::string ManagedToSTL(String ^ s) 
{
	using namespace Runtime::InteropServices;
	const char* chars = 
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	std::string retVal = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return retVal;
}

Auvsi_Ocr * theOCR;
CvKNearest knnLetter;
CvKNearest knnShape;
cv::Mat letterData;
cv::Mat letterClasses;
cv::Mat shapeData;
cv::Mat shapeClasses;

Recognizer::Recognizer()
{ 
	isReady = false;
	theOCR = new Auvsi_Ocr();

	// load data in other thread
	Thread ^ loadThread = gcnew Thread(gcnew ThreadStart(this, &Recognizer::loadData));
	loadThread->Name = "Recognizer Load Thread";
	loadThread->Start();
}

void Recognizer::trainRecognizer()
{
	Thread ^ lettersThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Recognizer::trainLetters));
	lettersThread->Name = "Recognizer Train Letters Thread";
	lettersThread->Start(LETTER_TRAIN_DIR);

	Thread ^ shapeThread = gcnew Thread(gcnew ParameterizedThreadStart(this, &Recognizer::trainShapes));
	shapeThread->Name = "Recognizer Train Shape Thread";
	shapeThread->Start(SHAPE_TRAIN_DIR);
}

ImageData ^ Recognizer::recognizeImage(cv::Mat input)
{
	typedef cv::Vec<float, 1> VT;
	if (!isReady)
		return nullptr;

	VT *rowPointer;
	float *letterVec, *shapeVec;
	IplImage * inputImg = input;

	// compute feature vector
	theOCR->processImage(inputImg, &letterVec, &shapeVec);


	// get letter
	cv::Mat sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	rowPointer = sample.ptr<VT>(0);
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		rowPointer[j] = letterVec[j];

	cv::Mat nearests = cvCreateMat( 1, 1, CV_32FC1);
	
	float response = knnLetter.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS, &nearests);

	String ^letterName = letterIntToStr(response);


	// get shape
	sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	rowPointer = sample.ptr<VT>(0);
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		rowPointer[j] = shapeVec[j];

	nearests = cvCreateMat( 1, 1, CV_32FC1);
	
	response = knnShape.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS, &nearests);

	String ^shapeName = shapeIntToStr(response);


	return gcnew ImageData(shapeName, letterName);
}

void Recognizer::trainLetters(Object ^ dirArg)
{
	String ^ directory = (String ^)dirArg;
	typedef cv::Vec<float, 1> VT;
	
	VT *rowPointer;
	
	float *letterVec, *shapeVec;
	Auvsi_Radon *theRadon = new Auvsi_Radon();
	Auvsi_DualTree *theDualTree = new Auvsi_DualTree();
	Auvsi_Fft *theFft = new Auvsi_Fft();
	

	// get list of files in directory
	array<String^>^ files = Directory::GetFiles(directory);

	int trainSampleCount = files->Length;
	
	cv::Mat trainLetterData, trainLetterClasses, trainShapeData, trainShapeClasses;
	
	trainLetterData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	trainLetterClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents letter
	trainShapeData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	trainShapeClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents shape

	// for file in directory
	int index = -1;
	for each (String ^ s in files)
	{
		++index;

		// load
		cv::Mat img = cv::imread(ManagedToSTL(s));
		cv::Mat floatImage = cv::Mat(img.size(), CV_32F);

		img.convertTo(floatImage,floatImage.type(), 1.0/255.0f);


		// convert to inverted black and white (black=1, white=0)
		cv::Mat bwImg = cv::Mat(img.size(), CV_32FC1);
		cv::cvtColor( floatImage, bwImg, CV_RGB2Gray );

		cv::threshold(bwImg,bwImg,0.5,1.0,CV_THRESH_BINARY);

		//cv::imshow(bwImg); // DEBUG
		//cv::waitKey(0); // DEBUG

		// recognize
		float *letterVec = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(bwImg)));
		
		// add letter + descriptor to letter database
		rowPointer = trainLetterData.ptr<VT>(index);		// save feature vector
		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
			rowPointer[j] = letterVec[j];

		rowPointer = trainLetterClasses.ptr<VT>(index); // save letter name
		rowPointer[0] = letterStrToInt(letterStr);
		

		// add to samples
		PRINT("TRAINED letter: " + letterStr + " = " + letterStrToInt(letterStr) + "\n";)

	}

	// save letter data
	saveDatabase(trainLetterData, trainLetterClasses, LETTER_DATA_FILE);

	delete theRadon;
	delete theDualTree;
	delete theFft;
}

void Recognizer::trainShapes(Object ^ dirArg)
{
	String ^ directory = (String ^)dirArg;
	// TODO later: copy-paste from trainLetters
}

void Recognizer::loadData()
{
	// make filenames
	String ^letterData = LETTER_DATA_FILE + ".data.cv";
	String ^letterClasses = LETTER_DATA_FILE + ".classes.cv";
	String ^shapeData = SHAPE_DATA_FILE + ".data.cv";
	String ^shapeClasses = SHAPE_DATA_FILE + ".classes.cv";

	// load data from file
	CvMat *letterDataMat = (CvMat*)cvLoad( ManagedToSTL(letterData).c_str() );
	CvMat *letterClassesMat = (CvMat*)cvLoad( ManagedToSTL(letterClasses).c_str() );
	CvMat *shapeDataMat = (CvMat*)cvLoad( ManagedToSTL(shapeData).c_str() );
	CvMat *shapeClassesMat = (CvMat*)cvLoad( ManagedToSTL(shapeClasses).c_str() );

	// build KNN
	knnLetter = CvKNearest(letterData, letterClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);

	knnShape = CvKNearest(shapeData, shapeClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);

	// done
	isReady = true;
}

void Recognizer::saveDatabase(cv::Mat data, cv::Mat classes, string filename)
{
	CvMat dataMat = data;
	CvMat classesMat = classes;

	string dataFileName = "";
	dataFileName += filename;
	dataFileName += ".data.cv";

	string classFileName = "";
	classFileName += filename;
	classFileName += ".classes.cv";

	cvSave(dataFileName.c_str(),&dataMat);
	cvSave(classFileName.c_str(),&classesMat);
}



// return ASCII value of letter
float Recognizer::letterStrToInt(String ^ letter)
{
	char letterChar = ManagedToSTL(letter).c_str()[0];
	return (float)letterChar;

}

String ^ Recognizer::letterIntToStr(float input) 
{
	char letter = (char)(int)input;
	return "" + letter;
}

// return an int. 100 == unknown
float Recognizer::shapeStrToInt(String ^ shape)
{
	if (0 == shape->Substring(0,4)->Equals("squa")) {
		return 0.0f;
	}

	if (0 == shape->Substring(0,4)->Equals( "diam" ))
		return 1.0f;
	if (0 == shape->Substring(0,4)->Equals( "rect" ))
		return 2.0f;
	if (0 == shape->Substring(0,4)->Equals( "para" ))
		return 3.0f;	
	if (0 == shape->Substring(0,4)->Equals( "hexa" ))
		return 4.0f;
	return 100.0f;
}

String ^ Recognizer::shapeIntToStr(float input) 
{
	switch (input)
	{
		case 0:
			return "Square";
		case 1:
			return "Diamond";
		case 2:
			return "Rectangle";
		case 3:
			return "Parallelogram";
		case 4:
			return "Hexagon";
		default:
			return "Unknown";
	}

	return "Unknown";
}


