#include "StdAfx.h"

#include "SkynetController.h"

#include "GEarthHandler.h"
#include "PlaneWatcher.h"
#include "OpenGLForm.h"
#include "Form1.h"
#include "Delegates.h"
#include "TargetLock.h"
#include "VoteCounter.h"
#include "Autosearch.h"

using namespace System;
using namespace System::Data::Odbc;
using namespace Skynet;
using namespace OpenGLForm;
using namespace Database;
using namespace msclr;
using namespace Communications;
using namespace Intelligence;

SkynetController::SkynetController(Form1 ^ mainView)
{
	openGLView = nullptr;
	theDatabase = nullptr;
	theWatcher = nullptr;
	form1View = mainView;
	theGEarthHandler = gcnew GEarthHandler(nullptr, nullptr);
	theGEarthHandler->setController(this);

	voteCounter = gcnew VoteCounter(this);
	voteCounter->setForm1(mainView);
	
	// autosearch = gcnew Autosearch("D:\\Skynet Files\\Field Boundaries.txt",this); DEBUG
	// autosearch->updateImage(); DEBUG
	guiHasData = false;
	hasTelemetry = false;
	hasVideo = false;
	frameCount = 0;
}

SkynetController::~SkynetController()
{
	form1View = nullptr;
	openGLView = nullptr;
	theDatabase = nullptr;
	theGEarthHandler = nullptr;
	voteCounter = nullptr;
}

Autosearch^ SkynetController::getAutosearch()
{
	return this->autosearch;
}

array<GPSCoord^>^ 
SkynetController::getSampleGPSCoords()
{
	array<GPSCoord^>^ coords = gcnew array<GPSCoord^>(9);
	coords[0] = gcnew GPSCoord ( GPSCoord::metersToGPS(800), GPSCoord::metersToGPS(0) , 0 );
	coords[1] = gcnew GPSCoord ( GPSCoord::metersToGPS(1000), GPSCoord::metersToGPS(500) , 0 );
	coords[2] = gcnew GPSCoord ( GPSCoord::metersToGPS(600), GPSCoord::metersToGPS(750) , 0 );
	coords[3] = gcnew GPSCoord ( GPSCoord::metersToGPS(600), GPSCoord::metersToGPS(1750) , 0 );
	coords[4] = gcnew GPSCoord ( GPSCoord::metersToGPS(500), GPSCoord::metersToGPS(1900) , 0 );
	coords[5] = gcnew GPSCoord ( GPSCoord::metersToGPS(0), GPSCoord::metersToGPS(800) , 0 );
	coords[6] = gcnew GPSCoord ( GPSCoord::metersToGPS(350), GPSCoord::metersToGPS(400) , 0 );
	coords[7] = gcnew GPSCoord ( GPSCoord::metersToGPS(500), GPSCoord::metersToGPS(0) , 0 );
	coords[8] = gcnew GPSCoord ( GPSCoord::metersToGPS(600), GPSCoord::metersToGPS(250) , 0 );
	return coords;
}

void SkynetController::exportData()
{
	// get data
	String ^ result = theDatabase->getTabDelimitedVerifiedTargetDataForSubmission();

	if (result == nullptr)
	{
		PRINT("SkynetController::exportData() ERROR: unable to get exported data");
	}

	if (File::Exists(TAB_DELIMITED_TURNIN_FILE_PATH))
		File::Delete(TAB_DELIMITED_TURNIN_FILE_PATH);

	FileStream^ fs = gcnew FileStream( TAB_DELIMITED_TURNIN_FILE_PATH, FileMode::CreateNew);
	StreamWriter ^ file = gcnew StreamWriter( fs, Encoding::UTF8 );
	//file->NewLine = "\n";

	file->Write(result);
	file->Close();


	// write images
	array<VerifiedTargetRowData ^>^ targets = theDatabase->getAllVerifiedTargets();

	int targetNumber = 1;
	for each(VerifiedTargetRowData ^ target in targets)
	{
		String ^ imageName = theDatabase->imageNameForID("" + target->candidateid);
		String ^ sourcePath = HTTP_SERVER_TARGET_PATH + imageName->Remove(0, 8);
		String ^ destPath = HTTP_SERVER_EXPORT_PATH + Int32(targetNumber).ToString("00") + ".jpg";
		
		try {
			if (File::Exists(destPath))
				File::Delete(destPath);
			File::Copy(sourcePath, destPath);
		}
		catch(Exception ^ e)
		{
			PRINT("ERROR: failed to copy image: " + e);
			form1View->errorMessageUniversal("Failed to export image");
		}
		targetNumber++;
	}

}

void SkynetController::comeAlive()
{
	theGEarthHandler->start();
	//autosearch->updateState(); DEBUG
}

void SkynetController::setCameraView(OpenGLForm::COpenGL ^ cameraView) 
{ 
	openGLView = cameraView; 
	theGEarthHandler->setOpenGL(cameraView);
	openGLView->theController = this;

}

void SkynetController::setPlaneWatcher(PlaneWatcher ^ newWatcher)
{ 
	// autosearch->setPlaneWatcher(newWatcher); DEBUG
	theWatcher = newWatcher; 
	theGEarthHandler->setWatcher(newWatcher);
	theWatcher->setController(this);
}

void SkynetController::setDatabase(Database::DatabaseConnection ^ newDatabase)
{ 
	theDatabase = newDatabase;
	theGEarthHandler->setDatabase(newDatabase);
	voteCounter->setDatabase(newDatabase);
}

void SkynetController::gotGPS()
{
	hasTelemetry = true;
	if (hasVideo)
		guiHasData = true;
}


void SkynetController::gotVideo()
{
	hasVideo = true;
	frameCount++;

	if (hasTelemetry)
		guiHasData = true;
}

void SkynetController::stopTargetLock()
{
	targetLock->endLock();
}

void SkynetController::intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees )
{
	((Form1 ^)form1View)->updateIntendedGimbalPosition( rollDegrees, pitchDegrees );
}

void SkynetController::intendedCameraZoomUpdated( float zoom )
{
	((Form1 ^)form1View)->updateIntendedCameraZoom( zoom );
}


String ^ SkynetController::saveCurrentFrameAsImage()
{
	// invoke saveCurrentFrameAsCandidateOnMainThread()
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but openGLView == nullptr");
		return nullptr;
	}

	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	// save current image to a file
	String ^ pathbase = HTTP_SERVER_IMAGERY_PATH;
	String ^ filename = "img_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".jpg";
	theOpenGL->saveImage( pathbase + filename );

	return filename;
}

void SkynetController::saveCurrentFrameAsCandidate()
{
	// invoke saveCurrentFrameAsCandidateOnMainThread()
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but openGLView == nullptr");
		return;
	}

	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	int width = theOpenGL->frameW; 
	int height = theOpenGL->frameH;
	int numChannels = 4;
	int originX = 0;
	int originY = 0;
	PlaneState ^ stateOfPlane = theWatcher->stateOfCurrentImage();

	// save current image to a file
	String ^ pathbase = HTTP_SERVER_TARGET_PATH;
	String ^ filename = "img_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".jpg";
	theOpenGL->saveImage( pathbase + filename );

	// insert data into database
	CandidateRowData ^ imageData = gcnew CandidateRowData(stateOfPlane);
	imageData->imageName = "targets/" + filename;
	imageData->dataWidth = width;
	imageData->dataHeight = height;
	imageData->dataNumChannels = numChannels;
	imageData->originX = originX;
	imageData->originY = originY;

	Delegates::candidateRowDataToVoid ^ saveDelegate = gcnew Delegates::candidateRowDataToVoid(this, &SkynetController::addCandidate );

	try {
		((Form1 ^)form1View)->Invoke( saveDelegate, gcnew array<Object ^>{imageData} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCurrentFrameAsCandidate(): Failed to add image to target - " + e);
	}

}

void SkynetController::printConsoleMessageInGreen(String ^ message)
{
	form1View->printGreenMessage(message);
}

String ^ SkynetController::imageNameForID(String ^ id)
{
	return theDatabase->imageNameForID(id);
}

void SkynetController::saveCurrentFrameAsCandidateOnMainThread()
{
	if (openGLView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but openGLView == nullptr");
		return;
	}
	System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ERROR: function no longer used. use saveCurrentFrameAsCandidate() instead.");
	throw gcnew Exception();
	return;
	
	COpenGL ^theOpenGL = (COpenGL ^)openGLView;

	float *imageBuffer = new float[ theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2];
	{
		lock l(theOpenGL);
		memcpy(imageBuffer, theOpenGL->buffer, theOpenGL->frameW * theOpenGL->frameH * sizeof(float) * 4 / 2); // 4 is numChannels, 2 is because deinterlacing cuts height in half	
	}

	saveCandidate(imageBuffer, theOpenGL->frameW, theOpenGL->frameH/2, 4, 0, 0, theWatcher->stateOfCurrentImage());

	((Form1 ^)form1View)->printToConsole("Saved image to database", Color::Orange);
}

// theObject is actually an array of one CandidateRowData object
void SkynetController::addCandidateToGUITable(Object ^ theObject)
{
	array<CandidateRowData ^>^ theArr = (array<CandidateRowData ^> ^)theObject;
	CandidateRowData ^ data = (CandidateRowData ^)(theArr[0]);
	Delegates::candidateRowDataToVoid ^ blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::insertCandidateData );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addCandidateToGUITable(): Failed to add image to GUI Table - " + e);
	}
}


void SkynetController::addTargetToGUITable(Object ^ theObject)
{	
	PRINT("SkynetController::addTargetToGUITable() REMOVED AND SHOULD NOT BE USED");
	return;

	array<TargetRowData ^>^ theArr = (array<TargetRowData ^> ^)theObject;
	TargetRowData ^ data = (TargetRowData ^)(theArr[0]);
	Delegates::targetRowDataToVoid ^ blahdelegate = gcnew Delegates::targetRowDataToVoid((Form1 ^)form1View, &Form1::insertTargetData );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addTargetToGUITable(): Failed to add image to GUI Table - " + e);
	}

}

void SkynetController::loadAllTablesFromDisk()
{
	loadCandidateTableFromDisk();
	loadVerifiedTargetsTableFromDisk();
}

void SkynetController::loadCandidateTableFromDisk()
{
	array<CandidateRowData ^>^ theRows = theDatabase->getAllCandidates();
	
	if (theRows == nullptr)
		return;

	for (int i = 0; i < theRows->Length; i++)
		((Form1 ^)form1View)->insertCandidateData(theRows[i]);
}

void SkynetController::loadVerifiedTargetsTableFromDisk()
{
	array<VerifiedTargetRowData ^>^ theRows = theDatabase->getAllVerifiedTargets();
	
	if (theRows == nullptr)
		return;

	
	((Form1 ^)form1View)->setVerifiedTableContents(theRows);
}

void SkynetController::loadTargetTableFromDisk()
{
	PRINT("WARNING: SkynetController::loadTargetTableFromDisk() REMOVED AND SHOULD NOT BE USED");
	return;

	array<TargetRowData ^>^ theRows = theDatabase->getAllTargets();

	if (theRows == nullptr)
		return;

	for (int i = 0; i < theRows->Length; i++)
		((Form1 ^)form1View)->insertTargetData(theRows[i]);
}

void SkynetController::clearAllTables()
{
	theDatabase->clearCandidatesTable();
	theDatabase->clearVotesTable();
	theDatabase->clearVerifiedTargetsTable();
}


void SkynetController::addCandidate(CandidateRowData ^ data)
{	
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addCandidate() ran, but theDatabase == nullptr");
		return;
	}

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addCandidate() ran, but data == nullptr");
		return;
	}
	try {
		theDatabase->saveNewCandidate(data);

		TimerCallback^ tcb =
           gcnew TimerCallback(this, &SkynetController::addCandidateToGUITable);
		Threading::Timer^ addTargetTimer = gcnew Threading::Timer(tcb,  gcnew array<CandidateRowData ^>{data}, 250, Timeout::Infinite);

		//stateTimer->Start();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}
}

bool SkynetController::addTarget(Database::TargetRowData ^ data)
{	
	PRINT("SkynetController::addTarget() REMOVED AND SHOULD NOT BE USED");
	return false;

	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but theDatabase == nullptr");
		return false;
	}

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but data == nullptr");
		return false;
	}
	try {
		theDatabase->saveNewTarget(data);

		TimerCallback^ tcb =
           gcnew TimerCallback(this, &SkynetController::addTargetToGUITable);
		Threading::Timer^ addTargetTimer = gcnew Threading::Timer(tcb,  gcnew array<TargetRowData ^>{data}, 250, Timeout::Infinite);

		//stateTimer->Start();
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addTarget(): Failed to save imageS - " + e);
		return false;
	}

	return true;
}

void SkynetController::addVerifiedTarget(VerifiedTargetRowData ^ data) // not yet
{
	if (data == nullptr)
		return;


	bool result = theDatabase->saveNewVerifiedTarget(data);

	if (!result)
		form1View->errorMessageUniversal("Failed to save verified target");
	else
		addVerifiedTargetToGUITable(data);

}

void SkynetController::addVerifiedTargetWithDialogData(DialogEditingData ^ data)
{
	if (data == nullptr)
		return;


	VerifiedTargetRowData ^ verifiedTarget = theDatabase->addVerifiedTargetWithDialogData(data);

	if (verifiedTarget == nullptr)
		form1View->errorMessageUniversal("Failed to save verified target");
	else
		addVerifiedTargetToGUITable(verifiedTarget);

}

void SkynetController::addVerifiedTargetToGUITable(VerifiedTargetRowData ^ data)
{
	if (data == nullptr)
		return;

	data->imageName = theDatabase->imageNameForID("" + data->candidateid);


	Delegates::verifiedTargetRowDataToVoid ^ blahdelegate = gcnew Delegates::verifiedTargetRowDataToVoid(form1View, &Form1::insertVerifiedTargetData );

	try {
		form1View->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addVerifiedTargetToGUITable(): Failed to set unverified table contents: " + e);
	}
}

void SkynetController::removeVerifiedTargetForID(String ^ id)
{
	if (id == nullptr)
		return;
	
	theDatabase->removeVerifiedTarget(id);
	form1View->removeVerifiedTargetFromTable(id);

}

bool SkynetController::addVote(Database::VoteRowData ^ data)
{
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addVote() ran, but theDatabase == nullptr");
		return false;
	}

	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::addVote() ran, but data == nullptr");
		return false;
	}
	try {
		theDatabase->addVote(data);
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addVote(): Failed to add vote - " + e);
		return false;
	}

	return true;
}


void SkynetController::removeVotesForID(String ^ id)
{
	
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::removeVotesForID() ran, but theDatabase == nullptr");
		return;
	}

	if (id == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::removeVotesForID() ran, but id == nullptr");
		return;
	}

	try {
		theDatabase->removeVotesForId(id);
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeVotesForID(): Failed to add vote - " + e);
		return;
	}

	return;
}

void SkynetController::removeCandidate(Database::CandidateRowData ^ data)
{
	removeCandidate("" + data->id);
}

void SkynetController::removeCandidate(String ^ id)
{	
	//theDatabase->removeCandidate(id);

	Delegates::stringToVoid ^ blahdelegate = gcnew Delegates::stringToVoid((Form1 ^)form1View, &Form1::removeCandidateFromTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeCandidate(): Failed to remove candidate from GUI table - " + e);
	}

}

void SkynetController::removeTarget(Database::TargetRowData ^ data)
{
	removeTarget("" + data->id);
}

void SkynetController::removeTarget(String ^ id)
{
	PRINT("SkynetController::removeTarget() REMOVED");
	theDatabase->removeTarget(id);

	Delegates::stringToVoid ^ blahdelegate = gcnew Delegates::stringToVoid((Form1 ^)form1View, &Form1::removeTargetFromTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeTarget(): Failed to remove target from GUI table - " + e);
	}
}


void SkynetController::modifyCandidate(Database::CandidateRowData ^ data)
{
	theDatabase->modifyCandidate(data);

	Delegates::candidateRowDataToVoid ^ blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::modifyCandidateInTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyCandidate(): Failed to modify candidate in GUI table - " + e);
	}
}

void SkynetController::modifyTarget(Database::TargetRowData ^ data)
{
	PRINT("SkynetController::modifyTarget() REMOVED");
	return;

	theDatabase->modifyTarget(data);

	Delegates::targetRowDataToVoid ^ blahdelegate = gcnew Delegates::targetRowDataToVoid((Form1 ^)form1View, &Form1::modifyTargetInTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyTarget(): Failed to modify target in GUI table - " + e);
	}
}

void SkynetController::updateAutosearchBox(Bitmap ^ image)
{
	((Form1 ^)form1View)->autosearchBox->Image = image;
}

Database::CandidateRowData ^ SkynetController::candidateWithID(String ^ id)
{
	return theDatabase->candidateWithID(id);
}

Database::TargetRowData ^ SkynetController::targetWithID(String ^ id)
{
	return theDatabase->targetWithID(id);
}


Database::VotesOnCandidate ^ SkynetController::votesForID(String ^ id)
{
	return theDatabase->votesForID(id);
}

Database::VerifiedTargetRowData ^ SkynetController::verifiedTargetWithID(String ^ id) // not yet
{
	//System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::verifiedTargetWithID(): not implemented");

	return theDatabase->verifiedTargetForID(id);
}


void SkynetController::saveCandidate(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane)
{
	System::Diagnostics::Trace::WriteLine("ERROR SkynetController::saveCandidate() not implemented");
	return;
	/*
	if (theDatabase == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCandidate() ran, but theDatabase == nullptr");
		return;
	}

	if (width < 0 || height < 0 || originX < 0 || originY < 0) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCandidate() ran, but (width < 0 || height < 0 || originX < 0 || originY < 0)");
		return;
	}


	
	// TODO: get plane information
	// save current image to a file
	// insert data into database

try {
		theDatabase->saveNewCandidate(data, width, height, numChannels, originX, originY, stateOfPlane);
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}

	delete data;*/
	// for testing:
	//theDatabase->displayLastImageInCandidate();
}