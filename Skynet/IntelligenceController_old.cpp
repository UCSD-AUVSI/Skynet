#include "stdafx.h"
#include "IntelligenceController.h"
#include "Autosearch.h"
#include "SkynetController.h"
#include "MasterHeader.h"
#include "Comport.h"
#include "AutopilotComport.h"
#include "SimHandler.h"

using namespace Intelligence;
using namespace PathfinderWrapper;
using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;
using namespace System::Linq;

IntelligenceController::IntelligenceController(String^ gpsCoordFilename, SkynetController^ skynetController)
{
	autosearch = gcnew Autosearch(gpsCoordFilename,this);
	this->skynetController = skynetController;
}

//void IntelligenceController::initializeIronPython()
//{
//    Options.PrivateBinding = true;
//    pythonEngine = gcnew PythonEngine();
//}

void IntelligenceController::startPathfinderThread(){
	Thread^ pathfinderThread = gcnew Thread(gcnew ThreadStart(this,&Intelligence::IntelligenceController::startPathfinder));
	pathfinderThread->Name = "Pathfinder Thread";
	pathfinderThread->Start();
}

 void IntelligenceController::startPathfinder()
{
	skynetController->printConsoleMessageInGreen("Starting Pathfinder...");
	Pathfinder::Create();
	skynetController->pathfinderComplete(gcnew Bitmap("C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\Skynet\\map_dotnet_fix.bmp"));
	skynetController->printConsoleMessageInGreen("Path created.");
}

void IntelligenceController::queryCameraAndUpdateMapAndImage()
{
    autosearch->queryCameraAndUpdateMapAndImage();
}

array<UAVCommand ^>^ IntelligenceController::getUAVCommands()
{
	StreamReader ^ pathReader = File::OpenText("path.txt");
	String ^ line;
	LinkedList<UAVCommand ^>^ commandList = gcnew LinkedList<UAVCommand ^>();
	while ( (line = pathReader->ReadLine()) != nullptr ){
		Waypoint ^ waypoint = gcnew Waypoint();
		array<String ^>^ coords = line->Split(',');
		waypoint->lat = Convert::ToSingle(coords[0]);
		waypoint->lon = Convert::ToSingle(coords[1]);
		waypoint->alt = 100.0f;
		waypoint->speed = 14.0f;
		UAVCommand ^ command  = AutopilotComport::commandFromWaypoint(waypoint);
		commandList->AddLast(command);
	}
	return Enumerable::ToArray(commandList);

}



void IntelligenceController::setPlaneWatcher(PlaneWatcher^ planeWatcher)
{
    autosearch->setPlaneWatcher(planeWatcher);
}

void IntelligenceController::displayAutosearchImage(Drawing::Bitmap^ image){
	skynetController->displayAutosearchImage(image);
}