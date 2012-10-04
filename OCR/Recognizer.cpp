#include "StdAfx.h"

#include "Recognizer.h"
#include "Auvsi_Ocr.h"
#include "Auvsi_Radon.h"
#include "Auvsi_DualTree.h"
#include "Auvsi_Fft.h"
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#include <opencv2\opencv.hpp>
#pragma warning(pop)


#define SIZE_OF_FEATURE_VECTOR 320
#define NUMBER_K_NEAREST_NEIGHBORS 2
#define STAR 1
#define HALF_CIRCLE 2
#define CIRCLE 3
#define HEXAGON 4
#define PARALLELOGRAM 5
#define PENTAGON 6
#define SQUARE 7
#define TRAPEZOID 8
#define TRIANGLE 9
#define UNKNOWN_SHAPE 100

using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace Vision;

#define LETTER_DATA_FILE "/SkynetFiles/OCR/database/letterData"
#define SHAPE_DATA_FILE "/SkynetFiles/OCR/database/shapeData"
#define LETTER_TRAIN_DIR "/SkynetFiles/OCR/train/letter/"
#define SHAPE_TRAIN_DIR "/SkynetFiles/OCR/train/shape/"
std::string ManagedToSTL(String ^ s) ;

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
	/*Thread ^ loadThread = gcnew Thread(gcnew ThreadStart(this, &Recognizer::loadData));
	loadThread->Name = "Recognizer Load Thread";
	loadThread->Start();*/
	loadData();
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
	IplImage * inputImg = new IplImage(input);

	// compute feature vector
	theOCR->processImage(inputImg, &letterVec, &shapeVec);


	// get letter
	cv::Mat sample = cvCreateMat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	rowPointer = sample.ptr<VT>(0);
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		rowPointer[j] = letterVec[j];

	cv::Mat nearests = cvCreateMat( 1, 1, CV_32FC1);

	/*PRINT(sample);
	PRINT(NUMBER_K_NEAREST_NEIGHBORS);
	PRINT(&nearests);*/

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

void Recognizer::train(String ^ directory, String ^ outputFilename)
{
	PRINT("Training started");
	typedef cv::Vec<float, 1> VT;
	
	VT *rowPointer;

	Auvsi_Radon *theRadon = new Auvsi_Radon();
	Auvsi_DualTree *theDualTree = new Auvsi_DualTree();
	Auvsi_Fft *theFft = new Auvsi_Fft();
	

	// get list of files in directory
	array<String^>^ files = Directory::GetFiles(directory);

	int trainSampleCount = files->Length;
	
	cv::Mat trainData, trainClasses;
	
	trainData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	trainClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents letter

	// for file in directory
	int index = -1;
	PRINT("Found "+files->Length + " files");
	float *objectVec;
	for each (String ^ s in files)
	{
		++index;

		// load
		cv::Mat img = cv::imread(ManagedToSTL(s));
		cv::Mat floatImage = cv::Mat(img.size(), CV_32F);

		img.convertTo(floatImage,floatImage.type(), 1.0/255.0f);


		// convert to inverted black and white (black=1, white=0)
		cv::Mat bwImg = cv::Mat(img.size(), CV_32FC1);
		cv::cvtColor( floatImage, bwImg, CV_RGB2GRAY );

		cv::threshold(bwImg,bwImg,0.5,1.0,CV_THRESH_BINARY);

		//cv::imshow(bwImg); // DEBUG
		//cv::waitKey(0); // DEBUG

		// recognize
		objectVec = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(bwImg)));
		
		// add letter + descriptor to letter database
		rowPointer = trainData.ptr<VT>(index);		// save feature vector
		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
			rowPointer[j] = objectVec[j];

		
		array<String^>^ fragments = s->Split('/');
		String ^ objectStr=fragments[fragments->Length-1];

		rowPointer = trainClasses.ptr<VT>(index); // save letter name
		rowPointer[0] = objectStrToInt(objectStr);
		

		// add to samples
		PRINT("TRAINED object: " + objectStr + " = " +objectStrToInt(objectStr) + "\n");
		//delete letterVec; Weirdly, this causes an error
	}

	// save letter data
	saveDatabase(trainData, trainClasses, outputFilename);

	delete theRadon;
	delete theDualTree;
	delete theFft;
}

void Recognizer::trainLetters(Object ^ directory)
{
	train((String ^)directory,LETTER_DATA_FILE);
}

void Recognizer::trainShapes(Object ^ directory)
{
	train((String ^)directory,SHAPE_DATA_FILE);
}

void Recognizer::loadData()
{
	//trainLetters(LETTER_TRAIN_DIR);
	//trainShapes(SHAPE_TRAIN_DIR);

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

	if (letterDataMat == nullptr || letterClassesMat == nullptr) {
		PRINT("Letter data unavailable)");
		trainLetters(LETTER_TRAIN_DIR);
		letterDataMat = (CvMat*)cvLoad( ManagedToSTL(letterData).c_str() );
		letterClassesMat = (CvMat*)cvLoad( ManagedToSTL(letterClasses).c_str() );
	}

	if (shapeDataMat == nullptr || shapeClassesMat == nullptr) {
		PRINT("Shape data unavailable)");
		trainShapes(SHAPE_TRAIN_DIR);
		shapeDataMat = (CvMat*)cvLoad( ManagedToSTL(shapeData).c_str() );
		shapeClassesMat = (CvMat*)cvLoad( ManagedToSTL(shapeClasses).c_str() );
	}
	
	// build KNN
	knnLetter = CvKNearest(letterDataMat, letterClassesMat, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);

	knnShape = CvKNearest(shapeDataMat, shapeClassesMat, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);

	// done
	isReady = true;

	PRINT("Image: "+recognizeImage(cv::imread("/SkynetFiles/OCR/test/circle.png"))->toString());
}

void Recognizer::saveDatabase(cv::Mat data, cv::Mat classes, String ^ filename)
{
	CvMat dataMat = data;
	CvMat classesMat = classes;

	String ^ dataFileName = "";
	dataFileName += filename;
	dataFileName += ".data.cv";

	String ^ classFileName = "";
	classFileName += filename;
	classFileName += ".classes.cv";
	cvSave( ManagedToSTL(dataFileName).c_str(),&dataMat);
	cvSave( ManagedToSTL(classFileName).c_str(),&classesMat);
}



// return ASCII value of letter
int Recognizer::objectStrToInt(String ^ object)
{
	char objectChar = ManagedToSTL(object).c_str()[0];
	if ( objectChar == 'x' ) return shapeStrToInt(object);
	else return objectChar;

}

String ^ Recognizer::letterIntToStr(int input) 
{
	char letter = (char)input;
	return "" + letter;
}

// return an int. 100 == unknown
int Recognizer::shapeStrToInt(String ^ shape)
{
	if ( shape->Contains("star") ) return STAR;
	if ( shape->Contains("halfcircle") ) return HALF_CIRCLE;
	if ( shape->Contains("circle") ) return CIRCLE;
	if ( shape->Contains("hexagon") ) return HEXAGON;
	if ( shape->Contains("parallelogram") ) return PARALLELOGRAM;
	if ( shape->Contains("pentagon") ) return PENTAGON;
	if ( shape->Contains("square") ) return SQUARE;
	if ( shape->Contains("trapezoid") ) return TRAPEZOID;
	if ( shape->Contains("triangle") ) return TRIANGLE;
	return UNKNOWN_SHAPE;	
}

String ^ Recognizer::shapeIntToStr(int input) 
{
	switch (input)
	{
		case STAR: return "star";
		case HALF_CIRCLE: return "halfcircle";
		case CIRCLE: return "circle";
		case HEXAGON: return "hexagon";
		case PARALLELOGRAM: return "parallelogram";
		case PENTAGON: return "pentagon";
		case SQUARE: return "square";
		case TRAPEZOID: return "trapezoid";
		case TRIANGLE: return "triangle";
		default: return "Unknown";
	}

}


