#include "stdafx.h"
#include "IntelligenceController.h"
#include "../Pathfinding/Autosearch.h"
#include "SkynetController.h"
#include "PlaneWatcher.h"
#include "MasterHeader.h"
#include "ImageWithPlaneData.h"
#include "GPSCoord.h"

using namespace System;
using namespace System::Drawing;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace System::Diagnostics;
using namespace Intelligence;
using namespace Skynet;


ProcessStartInfo^ IntelligenceController::getPathfinderProcessInfo()
{
	ProcessStartInfo ^processInfo = gcnew ProcessStartInfo(PYTHON_EXECUTABLE_PATH, PATHFINDER_SCRIPT_PATH);
	processInfo->UseShellExecute = false;
	processInfo->RedirectStandardOutput = true;
	processInfo->RedirectStandardInput = true;
	return processInfo;
}

void IntelligenceController::processPlaneData(ImageWithPlaneData^ planeData){
	autosearch->update(planeData);
}

String^ IntelligenceController::getPathfinderResult(ProcessStartInfo^ processInfo, GPSCoord^ planeData, array<GPSCoord^>^ fieldBoundaries) {
	return "12.3456, 104.2132\n12.345214, 93.3490\n";
	Diagnostics::Process ^finder = Diagnostics::Process::Start(processInfo);
	for each (GPSCoord^ line in fieldBoundaries){
		finder->StandardInput->WriteLine(line);
	}
	finder->StandardInput->WriteLine(planeData);
	finder->StandardInput->Close();

	return finder->StandardOutput->ReadToEnd();
}

IntelligenceController::IntelligenceController(array<GPSCoord^>^ fieldBoundaries, SkynetController^ skynetController):
	skynetController(skynetController)
{
	autosearch = gcnew Autosearch(fieldBoundaries, this);
	startPathfinderThread(fieldBoundaries);
}

void IntelligenceController::startPathfinderThread(array<GPSCoord^>^ fieldBoundaries) {
	ParameterizedThreadStart^ threadStart = gcnew ParameterizedThreadStart(this, &IntelligenceController::doPathfinding);
	Thread^ thread = gcnew Thread(threadStart);
	thread->Start(fieldBoundaries);
}

void IntelligenceController::doPathfinding(Object^ fieldBoundariesObj) {
	array<GPSCoord^>^ fieldBoundaries = (array<GPSCoord^>^) fieldBoundariesObj;
	ProcessStartInfo^ pathfinderProcess = getPathfinderProcessInfo();
	GPSCoord^ planeCoord = skynetController->getPlaneWatcher()->getState()->toGPSCoord();
	String^ pathfinderResult = getPathfinderResult(pathfinderProcess, planeCoord, fieldBoundaries);
	handlePathfinderResult(pathfinderResult);
}

array<GPSCoord^>^ IntelligenceController::parsePathfinderResultString(String^ result){
	List<GPSCoord^>^ list = gcnew List<GPSCoord^>();
	array<String^>^ lines = result->Split((gcnew String("\n"))->ToCharArray(),StringSplitOptions::RemoveEmptyEntries);
	for each(String^ line in lines){
		array<String^>^ latAndLon = line->Split(',');
		double lat = Double::Parse(latAndLon[0]);
		double lon = Double::Parse(latAndLon[1]);
		list->Add(gcnew GPSCoord(lat, lon));
	}
	return list->ToArray();
}

void IntelligenceController::handlePathfinderResult(String^ result) {
	array<GPSCoord^>^ coords = parsePathfinderResultString(result);
	IntelligenceResult^ intelligence = gcnew IntelligenceResult(coords, "C:/output.jpg");
	skynetController->handleIntelligenceResult(intelligence);
}

void IntelligenceController::updateAutosearchImage(Image^ image) {
	skynetController->updateAutosearchImage(image);
}