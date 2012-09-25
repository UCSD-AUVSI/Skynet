//#include "StdAfx.h"
//#include <cv.h>
//#include <highgui.h>
//#include <stdio.h>
//#include "Form1.h"
//#include "OCR.h"
//#include "Auvsi_Ocr.h"
//#include <ml.h>
//#include <cmath>
//
//#define OCR_THREAD_INTERVAL 5
//
//#define SIZE_OF_FEATURE_VECTOR 320
//#define NUMBER_K_NEAREST_NEIGHBORS 2
//
//#define DIRECTORY "C:\\Users\\UCSD\\Pictures\\targets artificial\\"
//
//using namespace Vision;
//using namespace System;
//
//// Letter/shape databases
//static cv::Mat trainLetterData;
//static cv::Mat trainLetterClasses; 
//static cv::Mat trainShapeData;
//static cv::Mat trainShapeClasses;
//static CvKNearest knnLetters;
//static CvKNearest knnShapes;
//
//OCR::OCR( Object ^ parent, int nThreads )
//{
//	_nThreads = nThreads;
//	_parent = parent;
//
//	_threads = gcnew array<Thread ^>( _nThreads );
//
//	
//
//
//}
//
//
//
//void
//OCR::startProcessing( void )
//{
//	for( int i = 0; i < _nThreads; ++i )
//	{
//		_threads[i] = gcnew Thread( gcnew ThreadStart(this, &OCR::ocrThread) );
//		_threads[i]->Name = "OCR Thread " + i;
//		_threads[i]->Start();
//	}
//}
//
//void
//OCR::stopProcessing( void )
//{
//	for( int i = 0; i < _nThreads; ++i )
//	{
//		try
//		{
//			_threads[i]->Abort();
//		}
//		catch( Exception ^ )
//		{
//
//		}
//	}
//}
//
//
//System::String ^ letterFloatToStr(float input)
//{
//	int in = Math::Round(input);
//	char outChar = (char)in;
//
//	if ((outChar > '#') && (outChar < 'z'))
//		return (System::String(outChar, 1))->ToUpper();
//
//	return "Unknown";
//
//	// convert int to ascii char to string
//
//}
//
//System::String ^ shapeFloatToStr(float input)
//{
//	int in = Math::Round(input);
//	System::String ^ retVal;
//	switch (in)
//	{
//	case 0:
//		retVal = "Square";
//	case 1:
//		retVal = "Diamond";
//		break;
//	case 2:
//		retVal = "Rectangle";
//		break;
//	case 3:
//		retVal = "Parallelogram";
//		break;
//	case 4:
//		retVal = "Hexagon";
//		break;
//	case 5:
//		retVal = "Circle";
//		break;
//	case 6:
//		retVal = "Pentagon";
//		break;
//	case 7:
//		retVal = "Semi-Circle";
//		break;
//	case 8:
//		retVal = "Star";
//		break;
//	case 9:
//		retVal = "Trapezoid";
//		break;
//	case 10:
//		retVal = "Cross";
//		break;
//	default:
//		retVal = "Unknown";
//		break;
//	}
//
//	return retVal;
//
//}
//
//
//// return ASCII value of letter
//float letterStrToInt(System::String ^ letter)
//{
//	char letterChar = (char)letter->ToLower()[0];
//	return (float)letterChar;
//
//}
//
//// return an int. 100 == unknown
//float shapeStrToInt(System::String ^ shape)
//{
//	//string str2;
//	shape = shape.ToLower();
//
//	if (0 == shape->Equals(0, 4, "squa"))
//		return 0.0f;
//	if (0 == shape->Equals("diam"))
//		return 1.0f;
//	if (0 == shape->Equals("rect"))
//		return 2.0f;
//	if (0 == shape->Equals("para"))
//		return 3.0f;	
//	if (0 == shape->Equals("hexa"))
//		return 4.0f;	
//	if (0 == shape->Equals("circ"))
//		return 5.0f;	
//	if (0 == shape->Equals("pent"))
//		return 6.0f;	
//	if (0 == shape->Equals("semi"))
//		return 7.0f;	
//	if (0 == shape->Equals("star"))
//		return 8.0f;	
//	if (0 == shape->Equals("trap"))
//		return 9.0f;	
//	if (0 == shape->Equals("cros"))
//		return 10.0f;	
//	return 100.0f;
//
//}
//
//void parseFilename(System::String ^ filename, System::String ^ &letter, System::String ^ &shape)
//{
//	shape = "abcd";
//	int startIndex = -1, finalIndex = -1;
//	for (int i = 0; i < filename->Length; i++) {
//		if (startIndex == -1) {
//			if (filename[i] == '_') 
//				startIndex = i + 1;
//		} else {
//			if (filename[i] == '_')
//				finalIndex = i - 1;
//		}
//	}
//
//	shape = filename->Substring(2, 4);
//	letter = filename->Substring(0, 1);
//}
//
//void computeImage(Auvsi_Ocr * theOCR, IplImage *src, float **letterDescriptor, float **shapeDescriptor) 
//{
//
//	if (src != NULL)
//		theOCR->processImage(src, letterDescriptor, shapeDescriptor);
//	else
//		std::printf("\t\tERROR: null image\n");
//}
//
//void
//OCR::buildTrainingSet()
//{
//	Auvsi_OCR * theOCR = new Auvsi_OCR();
//	VT *rowPointer;
//	
//	// ** to build database **
//	float *letterVec, *shapeVec;
//	System::String ^ letterStr, shapeStr, filePath;
//	
//	int trainSampleCount = 500;
//	
//	cv::Mat trainLetterData, trainLetterClasses, trainShapeData, trainShapeClasses;
//	
//	trainLetterData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
//	trainLetterClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents letter
//	trainShapeData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
//	trainShapeClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents shape
//	
//	
//	// get contents of directory
//	DIR *dp;
//	struct dirent *ep;
//
//	dp = opendir (DIRECTORY );
//	if (dp == NULL) {
//		System::Diagnostics::Trace::WriteLine("Couldn't open the directory");
//		return;
//	}	
//	System::Diagnostics::Trace::WriteLine("** listing contents of directory");
//	int index = 0;
//	while (ep = readdir (dp)) // loop through files
//	{
//		cout << ep->d_name << endl;		// print name
//		if (ep->d_name[0] == '.' || ep->d_name[0] == '_') // skip . .. _*
//			continue;
//		
//		// read and compute image
//		parseFilename(System::String(ep->d_name.c_str()), letterStr, shapeStr);				// read letter and shape from filename
//		filePath = directory;			// build full path for file
//		filePath.append(ep->d_name);
//		computeImage(theOCR, cvLoadImage(filePath.c_str(), 1), &letterVec, &shapeVec); // compute feature descriptors
//		
//		// add letter + descriptor to letter database
//		rowPointer = trainLetterData.ptr<VT>(index);		// save feature vector
//		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
//			rowPointer[j] = letterVec[j];
//		rowPointer = trainLetterClasses.ptr<VT>(index);		// save letter name
//		rowPointer[0] = letterStrToInt(letterStr);
//		
//		// add shape + descriptor to shape database
//		rowPointer = trainShapeData.ptr<VT>(index);			// save feature vector
//		for (int i = 0; i < SIZE_OF_FEATURE_VECTOR; i++)
//			rowPointer[i] = shapeVec[i];		
//		rowPointer = trainShapeClasses.ptr<VT>(index);		// save shape name
//		rowPointer[0] = shapeStrToInt(shapeStr);
//		
//		System::Diagnostics::Trace::WriteLine("\t" + letterStr + ", " + shapeStr + " = " + letterStrToInt(letterStr) + ", " + shapeStrToInt(shapeStr) + "\n");
//		//cout << "\t" << letterStr << ", " << shapeStr << " = " << letterStrToInt(letterStr) << ", " << shapeStrToInt(shapeStr) << "\n";
//	
//		index++;
//		if (index >= trainSampleCount)
//			break;	// too many samples ...
//
//	} // end loop
//	System::Diagnostics::Trace::WriteLine( "** done listing contents\n");
//	(void) closedir (dp); 
//
//	knnLetters(trainLetterData, trainLetterClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);
//	knnShapes(trainShapeClasses, trainShapeClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);
//}
//
//void
//OCR::ocrThread( void )
//{
//	ocrItem ^ currentData;
//
//	// instantiate OCR item
//	Auvsi_Ocr *theOCR = new Auvsi_Ocr();
//
//	typedef cv::Vec<float, 1> VT;
//	
//	VT *rowPointer;
//
//	// main run loop
//	while( true )
//	{		
//		// Get new data
//		mutex_Data->WaitOne();
//		currentData = ((Skynet::Form1 ^)_parent)->getCandidateData();
//		mutex_Data->ReleaseMutex();
//		if (currentData == nullptr)
//			continue; // no valid data
//
//		// Process data:
//
//		// load image 
//		String ^ filename = currentData->_path;
//		//CString str3(filename);
//
//		using namespace Runtime::InteropServices;
//		const char* chars = (const char*)(Marshal::StringToHGlobalAnsi(filename)).ToPointer();
//		std::string retVal = chars;
//		Marshal::FreeHGlobal(IntPtr((void*)chars));
//
//		cv::Mat image = cv::imread(retVal);
//
//
//		// pass into Auvsi_OCR
//		float *letterFeatDesc;
//		float *shapeFeatDesc;
//
//		theOCR->processImage(image, &letterFeatDesc, &shapeFeatDesc);	// process the image
//
//		// build feature vector
//		cv::Mat sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
//		rowPointer = sample.ptr<VT>(0);		// save feature vector
//		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
//			rowPointer[j] = letterFeatDesc[j];
//
//		// compare against training set
//		float responseLetter = knnLetters.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS);
//
//		sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
//		rowPointer = sample.ptr<VT>(0);		// save feature vector
//		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
//			rowPointer[j] = shapeFeatDesc[j];
//		
//		float responseShape = knnShapes.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS);
//
//		String ^ theShape = shapeFloatToStr(responseShape);
//		String ^ theLetter = letterFloatToStr(responseLetter);
//
//		// choose letter + shape, save in data
//		System::Diagnostics::Trace::WriteLine("Shape: " + theShape + " letter: " + theLetter);
//
//		// TODO: build data to send back, put in shape and letter
//
//		// Put back into table
//		((Skynet::Form1 ^)_parent)->Invoke( ((Skynet::Form1 ^)_parent)->ocrDelegate/*, DATA_GOES_HERE*/ );
//
//		// Wait a bit and then keep going
//		Thread::Sleep( OCR_THREAD_INTERVAL );
//	}
//}
//
//void 
//OCR::trainData( void )
//{
//
//}