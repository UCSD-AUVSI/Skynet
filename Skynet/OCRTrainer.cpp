#include "StdAfx.h"
#include "OCRTrainer.h"
#include "Util.h"
#include "Dataset.h"
#include "Auvsi_OCR.h"
#include "VisionUtil.h"
#include "OCRWrapper.h"
#include <cv.h>
#include <highgui.h>
#include <ml.h>


using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::IO;


void 
OCRTrainer::BuildDatasets()
{
	bool trainingDataExists = File::Exists(TRAINING_DIRECTORY + "\\trainingData.yml");
	if (trainingDataExists)
	{
		loadTrainingData();
	}
	else
	{
		computeNewTrainingData();
		saveDatasets();
	} 
}

void 
OCRTrainer::loadTrainingData()
{
	PRINT("** Loading existing training data **");
	
	cv::FileStorage fs = getTrainingFileStorage(cv::FileStorage::READ);

	shapeDataset = Dataset::LoadFrom(fs, "shape");
}



void OCRTrainer::computeNewTrainingData()
{
	PRINT("** Making new training data **");

	const bool UseShapes = true;
	const bool UseCombined = false;
	

	int numberOfShapeImages = (UseShapes ? Util::countFilesInDirectory(SHAPE_DIRECTORY,"*.png") : 0);
	int numberOfCombinedImages = (UseCombined ? Util::countFilesInDirectory(COMBINED_DIRECTORY,"*.png") : 0);
		
	int numberOfShapeSamples = numberOfShapeImages+numberOfCombinedImages;

	shapeDataset = gcnew Dataset(numberOfShapeSamples);
	
	auto start = time(nullptr);

	if (UseShapes)
		trainOnArtificialShapes();

	if (UseCombined)
		trainOnRealImages();
		
	auto elapsedTime = time(nullptr) - start;
	int numFiles = numberOfShapeImages + numberOfCombinedImages;
	PRINT( "Completed Training. Time: " + elapsedTime + "s for " + numFiles + " files (" + elapsedTime/(float)numFiles + "sec/file)");
}



void OCRTrainer::saveDatasets()
{
	cv::FileStorage fs = getTrainingFileStorage(cv::FileStorage::WRITE);
	shapeDataset->saveTo(fs, "shape");
}

cv::FileStorage 
OCRTrainer::getTrainingFileStorage(int flags)
{
	using Util::managedToSTL;
	cv::FileStorage fs(managedToSTL(TRAINING_DIRECTORY + "trainingData.yml"), flags);

	return fs;
}

void OCRTrainer::trainOnArtificialShapes()
{
	Auvsi_Ocr *chenOCR = new Auvsi_Ocr();

	array<String^ >^ files = Directory::GetFiles(SHAPE_DIRECTORY,"*.png");
	float *arraySample;
	PRINT("Processing Shapes...");
	for each (String ^filepath in files) 
	{
		String^ filename = extractFilename(filepath);
		PRINT("Processing " + filename);

		String^ const shape = parseFilenameForSeparateShape(filename);
		float shapeValue = (float)shapeStrToInt(shape);
		
		ManagedIPL ^trainImage = gcnew ManagedIPL(filepath, CV_LOAD_IMAGE_GRAYSCALE);
		ocrWrapper->computeImageForShape(chenOCR, trainImage->img, &arraySample, filepath);

		if (arraySample == NULL)
			throw gcnew Exception("Artificial Shape image is NULL");

		shapeDataset->trainOn(arraySample, SIZE_OF_FEATURE_VECTOR, shapeValue);
	}

	delete chenOCR;
}

void OCRTrainer::trainOnRealImages()
{
	Auvsi_Ocr *chenOCR = new Auvsi_Ocr();

	wchar_t dummyLetter;
	float *shapeVec;
	array<String^ >^ files = Directory::GetFiles(COMBINED_DIRECTORY,"*.png");

	PRINT("Processing Combined...");
	for each (String ^filepath in files) 
	{
		String^ filename = extractFilename(filepath);
		PRINT("Processing " + filename);

		String^ const shape = parseFilenameForShape(filename);
		float shapeValue = (float)shapeStrToInt(shape);
		
		ManagedIPL ^trainImage = gcnew ManagedIPL(filepath, CV_LOAD_IMAGE_COLOR);
		ocrWrapper->computeImageForBoth(chenOCR, trainImage->img, dummyLetter, &shapeVec, filepath);

		if (shapeVec == NULL)
			throw gcnew Exception("OCRTrainer - Segmentation couldn't find valid shape");
			
		shapeDataset->trainOn(shapeVec, SIZE_OF_FEATURE_VECTOR, shapeValue);
	}

	delete chenOCR;
}