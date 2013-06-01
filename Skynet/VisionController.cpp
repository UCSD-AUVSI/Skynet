#include "VisionController.h"
#include "SkynetController.h"
#include "DuplicateResolver.h"
#include "MasterHeader.h"
#include "Saliency.h"
#include "OCRWrapper.h"
#include "ImageWithPlaneData.h"
#include "Util.h"
#include "PlaneWatcher.h"
#include "DatabaseStructures.h"
#include "TargetRecognizer.h"
#include "ShapeTester.h"
#include "LetterTester.h"
#include "ShapeSegmentationTester.h"
#include "TessWrapper.h"

using namespace Vision;
using namespace System;
using namespace System::Threading;
using namespace Skynet;
using namespace System::Collections;
using namespace Database;

VisionController::VisionController(SkynetController ^ skynetCtrl):
	width(0), height(0), paused(false), skynetController(skynetCtrl)
{
	frameQueue = gcnew Collections::Queue(60);


	// make and init DuplicateResolver,Saliency,OCR
	initImagingPathway();

	runLoopThread = gcnew Thread(gcnew ThreadStart(this,&VisionController::runLoop));
	runLoopThread->Name = "Vision Controller runLoop Thread";
	runLoopThread->Start();
}

VisionController::~VisionController()
{
	saliency = nullptr;
	if (runLoopThread != nullptr)
		runLoopThread->Abort();

}

void 
VisionController::initImagingPathway()
{
	duplicateResolver = gcnew DuplicateResolver(skynetController);
	saliency = gcnew Saliency(this);
	recognizer = gcnew TargetRecognizer();

	/**
	 * TIM: Put testing code here
	 */

	//ShapeTester::runTest( "C:\\Users\\ucsd_auvsi\\Dropbox\\AUVSI\\Technical\\Software\\OCR testing data\\ShapeTestSet\\" );
	//LetterTester::runTest( "C:\\Users\\ucsd_auvsi\\Dropbox\\AUVSI\\Technical\\Software\\OCR testing data\\LetterTestSet\\", ocr );
	//ShapeSegmentationTester::runTest( "C:\\Users\\ucsd_auvsi\\Dropbox\\AUVSI\\Technical\\Software\\OCR testing data\\CombinedPlywood\\", ocr );
	//throw gcnew Exception();

	/*
	System::Collections::IEnumerator^ files = fileEntries->GetEnumerator();
    while ( files->MoveNext() )
    {
       String^ fileName = safe_cast<String^>(files->Current);
       PRINT( "found: " + fileName );
    }
	*/

	/*
	array<String^ >^ theFiles = gcnew array<String^ > (1);
	theFiles[0] = "C:\\test\\timtest\\circ.png";
	ShapeTester^ tester = gcnew ShapeTester( theFiles );
	tester->testAllFiles();
	*/
	
	
	// saliency->runTestOnImageNamed("C:\\46000.jpg");
	// outputs to: C:\\Saliency_Test_Output
	
	/*
	TargetResult^ result = ocr->targetRecognizer->recognizeTarget("C:\\test\\timtest\\a_circ_2_orig.png");
	PRINT(result->shape +", " + result->letter);
	throw gcnew Exception();
	*/

	
}

void 
VisionController::runLoop()
{
	endRunLoop = false;
	while (!endRunLoop)
	{
		while (frameQueue->Count == 0)
			Thread::Sleep(20); // ms

		Frame ^frame = (Frame ^)frameQueue->Dequeue();

		if ( !paused ) {
			analyzeFrame(frame);
		}
	}
}

void 
VisionController::receiveOCRResults()
{
	// TODO: implement
}

void 
VisionController::analyzeFrame(Frame ^ frame)
{
	saliency->analyzeFrame(frame);
	
}

void 
VisionController::receiveFrame(ImageWithPlaneData^ imageWithPlaneData)
{
	if (imageWithPlaneData->image->cols != width || imageWithPlaneData->image->rows != height){
		updateSaliencyImageSize(imageWithPlaneData->image->cols, imageWithPlaneData->image->rows);
	}
	Frame ^frame = gcnew Frame(imageWithPlaneData);
	if (frameQueue->Count < 20 ){
		frameQueue->Enqueue(frame);
	}
}

void 
VisionController::updateSaliencyImageSize(int imgWidth, int imgHeight)
{
	width = imgWidth;
	height = imgHeight;
	if ( saliency != nullptr ){
		saliency->setValues(width, height);
	}
}

bool
VisionController::processSaliencyCandidate(CandidateRowData^ candidate)
{
	using Util::managedToSTL;
	skynetController->addCandidate(candidate);
	UnverifiedRowData^ maybeDuplicate = duplicateResolver->maybeFindDuplicate(candidate);
	UnverifiedRowData^ unverified;
	bool isUnique = maybeDuplicate == nullptr;
	bool isImproved = false;

	if (isUnique) {
		unverified = gcnew UnverifiedRowData(candidate);
	} else {
		unverified = maybeDuplicate;
		isImproved = duplicateResolver->betterThanDuplicate(candidate, unverified);
		if (!isImproved) {
			return false;
		}
	}
	TargetResult ^ ocrData = recognizer->recognizeTarget(cv::imread(managedToSTL(candidate->imageName)));

	//// handle data
	unverified->description->shape = ocrData->shape;
	unverified->description->letter = ocrData->letter;
	unverified->description->shapeColor = ocrData->shapeColor;
	unverified->description->letterColor = ocrData->letterColor;
	unverified->description->heading = ocrData->getHeadingString();
		
	/* TODO: Implement SkynetController::upsertUnverified */
	if (unverified->targetid == 0){
		skynetController->addUnverified(unverified);
	} else {
		skynetController->modifyUnverified(unverified);
	}
	
	return true;
}

		
















