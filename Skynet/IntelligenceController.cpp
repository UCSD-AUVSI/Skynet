#include "stdafx.h"
#include "IntelligenceController.h"
#include "../Pathfinding/Autosearch.h"
#include "SkynetController.h"
#include "MasterHeader.h"
#include "ImageWithPlaneData.h"

using namespace Intelligence;
using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace System::Diagnostics;

ProcessStartInfo^ IntelligenceController::getPathfinderProcessInfo()
{
	ProcessStartInfo ^processInfo = gcnew ProcessStartInfo(PYTHON_EXECUTABLE_PATH, PATHFINDER_SCRIPT_PATH);
	processInfo->UseShellExecute = false;
	processInfo->RedirectStandardOutput = true;
	processInfo->RedirectStandardInput = true;
	return processInfo;
}

String^ IntelligenceController::getPathfinderResult(ProcessStartInfo^ processInfo, array<String^>^ fieldBoundaries) {
	Diagnostics::Process ^finder = Diagnostics::Process::Start(processInfo);
	for each (String^ line in fieldBoundaries){
		finder->StandardInput->WriteLine(line);
	}
	
	finder->StandardInput->Close();
	return finder->StandardOutput->ReadToEnd();
}

IntelligenceController::IntelligenceController(array<String^>^ fieldBoundaries, SkynetController^ skynetController,ImageWithPlaneData ^ data):
	skynetController(skynetController)
{
	autosearch = createAutosearch(fieldBoundaries);
	autosearch->startUpdaterThread();
	startPathfinderThread(fieldBoundaries);
}

Autosearch^ IntelligenceController::createAutosearch(array<String^>^ fieldBoundaries) {
	return gcnew Autosearch(fieldBoundaries, this);
}

void IntelligenceController::startPathfinderThread(array<String^>^ fieldBoundaries) {
	ParameterizedThreadStart^ threadStart = gcnew ParameterizedThreadStart(this, &IntelligenceController::doPathfinding);
	Thread^ thread = gcnew Thread(threadStart);
	thread->Start(fieldBoundaries);
}

void IntelligenceController::doPathfinding(Object^ fieldBoundariesObj) {
	array<String^>^ fieldBoundaries = (array<String^>^) fieldBoundariesObj;
	ProcessStartInfo^ pathfinderProcess = getPathfinderProcessInfo();
	String^ pathfinderResult = getPathfinderResult(pathfinderProcess, fieldBoundaries);
	handlePathfinderResult(pathfinderResult);
}

void IntelligenceController::handlePathfinderResult(String^ result) {
	skynetController->handlePathfinderResult(result);
}

void IntelligenceController::setPlaneWatcher(PlaneWatcher^ planeWatcher)
{
    autosearch->setPlaneWatcher(planeWatcher);
}

void IntelligenceController::displayAutosearchImage(Drawing::Bitmap^ image){
	skynetController->displayAutosearchImage(image);
}

/*void IntelligenceController::startPlaneDatathread(ImageWithPlaneData ^ data){
	ParameterizedThreadStart^ threadStart = gcnew ParameterizedThreadStart(this, &IntelligenceController::doPathfinding);
	Thread^ thread = gcnew Thread(threadStart);
	thread->Start(data);
}*/