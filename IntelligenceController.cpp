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

IntelligenceController::IntelligenceController(SkynetController^ skynetController)
{
	this->skynetController = skynetController;
	autosearch = gcnew Autosearch("D:\\Skynet Files\\Field Boundaries.txt",this);
	startPathfinderThread();
}

void IntelligenceController::restart()
{
	autosearch->abortUpdaterThread();
	autosearch = gcnew Autosearch("D:\\Skynet Files\\Field Boundaries.txt",this);
	startPathfinderThread();
}

void IntelligenceController::startPathfinderThread(){
	Thread^ pathfinderThread = gcnew Thread(gcnew ThreadStart(this,&Intelligence::IntelligenceController::startPathfinder));
	pathfinderThread->Name = "Pathfinder Thread";
	pathfinderThread->Start();
}

 void IntelligenceController::startPathfinder()
{
	try {
		skynetController->printConsoleMessageInGreen("Starting Pathfinder...");
	} 
	catch (InvalidOperationException ^ e) 
	{
		PRINT("ERROR in IntelligenceController::startPathfinder: " + e);
	}
	
	Pathfinder::Create();
	
	try {
		autosearch->updateImage();
		skynetController->pathfinderComplete(gcnew Bitmap(gcnew Bitmap("C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\Skynet\\map_dotnet_fix.bmp")));
	    autosearch->startUpdaterThread();
	} 
	catch (InvalidOperationException ^ e) 
	{
		PRINT("ERROR in IntelligenceController::startPathfinder: " + e);
	}
	
	
	skynetController->printConsoleMessageInGreen("Path created.");
}


array<Waypoint ^>^ IntelligenceController::getWaypoints()
{
	StreamReader ^ pathReader = File::OpenText("path.txt");
	String ^ line;
	LinkedList<Waypoint ^>^ waypointList = gcnew LinkedList<Waypoint ^>();
	while ( (line = pathReader->ReadLine()) != nullptr ){
		Waypoint ^ waypoint = gcnew Waypoint();
		array<String ^>^ coords = line->Split(',');
		waypoint->lat = Convert::ToSingle(coords[0]);
		waypoint->lon = Convert::ToSingle(coords[1]);
		waypoint->alt = 100.0f;
		waypoint->speed = 14.0f;
		waypointList->AddLast(waypoint);
	}
	return Enumerable::ToArray(waypointList);

}

void IntelligenceController::setPlaneWatcher(PlaneWatcher^ planeWatcher)
{
    autosearch->setPlaneWatcher(planeWatcher);
}

void IntelligenceController::displayAutosearchImage(Drawing::Bitmap^ image){
	skynetController->displayAutosearchImage(image);
}