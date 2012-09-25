#include "stdafx.h"
#include "VisionController.h"
#include "SkynetController.h"
#include "DuplicateResolver.h"
#include "MasterHeader.h"
#include "OpenGLForm.h"
#include "DecklinkCallback.h"
#include "Auvsi_Saliency.h"
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

	// make and init decklink
	callback = new Decklink::Callback( this );

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

	delete(callback);
}

void 
VisionController::initImagingPathway()
{
	duplicateResolver = gcnew DuplicateResolver(skynetController->getDatabase(), skynetController->getForm1());
	saliency = gcnew Saliency(skynetController->getPlaneWatcher(), duplicateResolver);
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

	saliency->analyzeFrame(frame);
}

void 
VisionController::receiveFrame(float *buffer)
{
	if (!skynetController->appIsAlive)
		return;

	// display frame in GUI right away
	openGLForm->UpdateBuffer(buffer);

	// queue frame for later analysis
	Frame ^frame = gcnew Frame(buffer, width, height);
	frameQueue->Enqueue(frame);
}

void 
VisionController::gotFirstFrame(int imgWidth, int imgHeight)
{
	width = imgWidth;
	height = imgHeight;
	openGLForm->CallbackSetup( width, height );
	if ( saliency != nullptr ){
		saliency->setValues(width, height, duplicateResolver);
	}else{
		//throw gcnew Exception("Saliency is null");
		PRINT("SALIENCY IS NULL");
	}
}




















