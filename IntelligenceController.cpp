#include "stdafx.h"
#include "IntelligenceController.h"
#include "../Pathfinding/Autosearch.h"
#include "../Pathfinding/Pathfinder.h"
#include "SkynetController.h"
#include "MasterHeader.h"

using namespace Intelligence;
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
	
	Pathfinder^ finder = gcnew Pathfinder();
	finder->buildPath(autosearch);

	
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


void IntelligenceController::setPlaneWatcher(PlaneWatcher^ planeWatcher)
{
    autosearch->setPlaneWatcher(planeWatcher);
}

void IntelligenceController::displayAutosearchImage(Drawing::Bitmap^ image){
	skynetController->displayAutosearchImage(image);
}