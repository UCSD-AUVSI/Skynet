#include "stdafx.h"
#include "VisionController.h"
#include "SkynetController.h"
#include "DuplicateResolver.h"
#include "MasterHeader.h"
#include "OpenGLForm.h"
//#include "Auvsi_Saliency.h"
#include "Saliency.h"
#include "OCR.h"
#include "PlaneWatcher.h"

using namespace Vision;
using namespace System;
using namespace System::Threading;
using namespace Skynet;

VisionController::VisionController(OpenGLForm::COpenGL ^ openGL, SkynetController ^ skynetCtrl)
{
	openGLForm = openGL;
	this->skynetController = skynetCtrl;

	// init queue
	frameQueue = gcnew Queue(60);


	// make and init DuplicateResolver,Saliency,OCR
	initImagingPathway();

	runLoopThread = gcnew Thread(gcnew ThreadStart(this,&VisionController::runLoop));
	runLoopThread->Name = "Vision Controller runLoop Thread";
	runLoopThread->Start();
}

VisionController::~VisionController()
{
	//saliency = nullptr;
	if (runLoopThread != nullptr)
		runLoopThread->Abort();

}

void 
VisionController::initImagingPathway()
{
	duplicateResolver = gcnew DuplicateResolver(skynetController->getDatabase(), skynetController->getForm1());
	/* saliency = gcnew Saliency(skynetController->getPlaneWatcher(), duplicateResolver);
	if ( saliency == nullptr ) {
		PRINT("Saliency wasn't initialized properly");
		throw gcnew Exception("Saliency was not initialized properly");
	} else {
		PRINT("Saliency initialized");
	} */
}

void 
VisionController::runLoop()
{
	endRunLoop = false;
	while (!endRunLoop)
	{
		// wait for non-empty queue
		while (frameQueue->Count == 0)
			Thread::Sleep(20); // ms

		Frame ^frame = (Frame ^)frameQueue->Dequeue();
		//PRINT("QUEUE count:" + frameQueue->Count);

		// wait until frame is ready for analysis
		while (!frame->isReady())
			Thread::Sleep(100);

		analyzeFrame(frame);
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
	frame->planeState = skynetController->getPlaneWatcher()->predictLocationAtTime((float)frame->timestamp.Subtract(DateTime::Now).TotalSeconds);

//	saliency->analyzeFrame(frame);
}

void 
VisionController::receiveFrame(unsigned char *buffer)
{
	if (!skynetController->appIsAlive) {
		//Thread::Sleep(1000);
		//PRINT("SkynetController is not alive");
	}

	// display frame in GUI right away
	openGLForm->UpdateBuffer(buffer);
	openGLForm->SwapOpenGLBuffers();

	// queue frame for later analysis
	Frame ^frame = gcnew Frame(buffer, width, height);
	frameQueue->Enqueue(frame);
}

void 
VisionController::gotFirstFrame(int imgWidth, int imgHeight)
{
	width = imgWidth;
	height = imgHeight;
	openGLForm->SetSize( width, height );
/*	if ( saliency != nullptr ){
		saliency->setValues(width, height, duplicateResolver);
	}else{
		PRINT("SALIENCY IS NULL");
	} */
}




















