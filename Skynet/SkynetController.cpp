#include "StdAfx.h"

#include "SkynetController.h"
#include "IntelligenceController.h"
#include "VisionController.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include "Delegates.h"
#include "SaveImage.h"
#include "SimulatorPlaneDataReceiver.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Data::Odbc;
using namespace Skynet;
using namespace Database;
using namespace Communications;
using namespace Intelligence;
using namespace Vision;
using namespace Delegates;

SkynetController::SkynetController(Form1 ^ mainView):
	theDatabase(gcnew Database::DatabaseConnection()),
	form1View(mainView),
	guiHasData(false),
	hasTelemetry(false),
	hasVideo(false),
	frameCount(0)
{
	if (!theDatabase->connect()){
		PRINT("WARNING: Could not connect to database");
	}
	appIsAlive = false;
	cameraView = nullptr;
	theWatcher = gcnew PlaneWatcher(this);
	//visionController = gcnew VisionController(this);
	receiver = gcnew SimulatorPlaneDataReceiver("C:\\good_flight_images1", theWatcher);
	
}

void SkynetController::processPlaneData(ImageWithPlaneData^ imageWithPlaneData){
	if (visionController != nullptr ){
		visionController->receiveFrame(imageWithPlaneData);
	}
	form1View->displayPlaneData(imageWithPlaneData);
}



void SkynetController::startIntelligenceController(array<String ^>^ fieldBoundaries)
{
		intelligenceController = gcnew IntelligenceController(fieldBoundaries, this,theWatcher->getState());

}

SkynetController::~SkynetController()
{
	form1View = nullptr;
	cameraView = nullptr;
	theDatabase = nullptr;
}

void SkynetController::handlePathfinderResult(String^ result)
{
	//form1View->setWayPointsText(result);
	pathfinderComplete(gcnew Bitmap(gcnew Bitmap("output.jpg")));
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

void SkynetController::exportData(String ^ basePath)
{
	// get data
	String ^ result = theDatabase->getTabDelimitedVerifiedTargetDataForSubmission();

	if (result == nullptr)
	{
		PRINT("SkynetController::exportData() ERROR: unable to get exported data");
		return;
	}

	String ^ turninPath = basePath + "turnin.txt";

	if (File::Exists(turninPath))
		File::Delete(turninPath);

	FileStream^ fs = gcnew FileStream( turninPath, FileMode::CreateNew);
	StreamWriter ^ file = gcnew StreamWriter( fs, Encoding::UTF8 );

	file->Write(result);
	file->Close();


	// write images
	array<VerifiedRowData ^>^ targets = theDatabase->getAllVerified();

	int targetNumber = 1;
	for each(auto target in targets)
	{
		String ^ imageName = target->target->candidate->imageName;
		String ^ sourcePath = HTTP_SERVER_TARGET_PATH + imageName;
		String ^ destPath = basePath + Int32(targetNumber).ToString("00") + ".jpg";
		
		try {
			if (File::Exists(destPath))
				File::Delete(destPath);
			File::Copy(sourcePath, destPath);
		}
		catch(Exception ^ e)
		{
			PRINT("ERROR: failed to copy image: " + e);
			form1View->errorMessageUniversal("Failed to export image " + imageName);
		}

		targetNumber++;
	}

}

void SkynetController::comeAlive()
{
	appIsAlive = true;
}

void SkynetController::setCameraView(PictureBox ^ cameraView) 
{ 
	cameraView = this->cameraView;
}

void SkynetController::setPlaneWatcher(PlaneWatcher ^ newWatcher)
{ 
 	theWatcher = newWatcher; 
}

void SkynetController::setDatabase(Database::DatabaseConnection ^ newDatabase)
{ 
	theDatabase = newDatabase;
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
}

void SkynetController::intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees )
{
	((Form1 ^)form1View)->updateIntendedGimbalPosition( rollDegrees, pitchDegrees );
}

void SkynetController::intendedCameraZoomUpdated( float zoom )
{
	((Form1 ^)form1View)->updateIntendedCameraZoom( zoom );
}


String ^ SkynetController::saveCurrentFrameAsImage(String ^ basePath)
{
	// invoke saveCurrentFrameAsCandidateOnMainThread()
	if (cameraView == nullptr) {
		System::Diagnostics::Trace::WriteLine("SkynetController::saveCurrentFrameAsCandidate() ran, but cameraView == nullptr");
		return nullptr;
	}

	// save current image to a file
	String ^ pathbase = basePath;
	String ^ filename = "img_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".jpg";

	return filename;
}

String ^ SkynetController::saveCurrentFrameAsImage()
{
	return saveCurrentFrameAsImage(HTTP_SERVER_IMAGERY_PATH);
}

void SkynetController::saveCurrentFrameAsUnverified()
{
	ImageWithPlaneData ^ stateOfPlane = theWatcher->getState();
	String ^ filename = HTTP_SERVER_TARGET_PATH + "img_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".jpg";
	cv::Mat image = *(stateOfPlane->image);

	/**
	 * If the image is based on a file on the disk, copy the file using the Windows API.
	 * Otherwise, use OpenCV.
	 */
	if (String::IsNullOrEmpty(stateOfPlane->imageFilename)){
		cv::imwrite(std::string((const char*)Marshal::StringToHGlobalAnsi(filename).ToPointer()),image);
	} else {
		File::Copy(stateOfPlane->imageFilename, filename, true);
	}

	// insert data into database
	int width = image.cols;
	int height = image.rows;
	int originX = 0;
	int originY = 0;
	auto unverified = gcnew UnverifiedRowData(stateOfPlane, originX, originY, width, height);
	unverified->candidate->imageName = filename;

	auto saveDelegate = gcnew Delegates::unverifiedRowDataToVoid(this, &SkynetController::addUnverified );

	try {
		((Form1 ^)form1View)->Invoke( saveDelegate, gcnew array<Object ^>{unverified} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCurrentFrameAsCandidate(): Failed to add image to target - " + e);
	}

		((Form1 ^)form1View)->Invoke(gcnew Delegates::unverifiedRowDataToVoid( ((Form1 ^)form1View), &Skynet::Form1::insertUnverifiedData), unverified );

}

void SkynetController::printConsoleMessageInGreen(String ^ message)
{
	form1View->printGreenMessage(message);
}

String ^ SkynetController::imageNameForID(String ^ id)
{
	return theDatabase->imageNameForID(id);
}


// theObject is actually an array of one UnverifiedRowData object
void SkynetController::addUnverifiedToGUITable(Object ^ theObject)
{
	array<UnverifiedRowData ^>^ theArr = (array<UnverifiedRowData ^> ^)theObject;
	UnverifiedRowData ^ data = (UnverifiedRowData ^)(theArr[0]);
	Delegates::unverifiedRowDataToVoid ^ blahdelegate = gcnew Delegates::unverifiedRowDataToVoid((Form1 ^)form1View, &Form1::insertUnverifiedData );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addUnverifiedToGUITable(): Failed to add image to GUI Table - " + e);
	}
}


void SkynetController::loadAllTablesFromDisk()
{
	loadCandidateTableFromDisk();
	loadVerifiedTableFromDisk();
	loadUnverifiedTableFromDisk();
}

void SkynetController::updateVerifiedTableFromDatabaseAtInterval(Object ^ interval){
	while(this->form1View){
		loadVerifiedTableFromDisk();
		Thread::Sleep((int)interval);
	}
}

void SkynetController::loadCandidateTableFromDisk()
{
	array<CandidateRowData ^>^ theRows = theDatabase->getAllCandidates();
	
	if (theRows == nullptr)
		return;

	((Form1 ^)form1View)->setCandidateTableContents(theRows);
}

void SkynetController::loadVerifiedTableFromDisk()
{
	array<VerifiedRowData ^>^ theRows = theDatabase->getAllVerified();
	
	if (theRows == nullptr)
		return;

	
	((Form1 ^)form1View)->setVerifiedTableContents(theRows);
}

void SkynetController::loadUnverifiedTableFromDisk()
{
	array<UnverifiedRowData ^>^ theRows = theDatabase->getAllUnverified();

	if (theRows == nullptr)
		return;

	((Form1 ^)form1View)->setUnverifiedTableContents(theRows);
}

void SkynetController::clearAllTables()
{
	theDatabase->clearCandidatesTable();
	theDatabase->clearUnverifiedTable();
	theDatabase->clearVerifiedTable();
}

Database::DatabaseConnection ^ SkynetController::getDatabase()
{
	return theDatabase;
}


void SkynetController::addCandidate(CandidateRowData^ candidate)
{
	theDatabase->addCandidate(candidate);
	form1View->Invoke(gcnew candidateRowDataToVoid( form1View, &Skynet::Form1::insertCandidateData), candidate );
}

array<UnverifiedRowData ^>^ SkynetController::getAllUnverified()
{
	/*
	 * TODO:
	 * This unverified targets should be stored in memory. DuplicateResolver will call this
	 * method every time saliency detects a target, so it can't be making a DB call each time
	 */
	return theDatabase->getAllUnverified(); 
}
void SkynetController::addUnverified(UnverifiedRowData ^ unverified)
{	
	try {
		theDatabase->addUnverified(unverified);
		form1View->Invoke(gcnew unverifiedRowDataToVoid( form1View, &Skynet::Form1::modifyUnverifiedInTable), unverified  );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}
}

//bool SkynetController::addTarget(Database::TargetRowData ^ data)
//{	
//	PRINT("SkynetController::addTarget() REMOVED AND SHOULD NOT BE USED");
//	return false;
//
//	//if (theDatabase == nullptr) {
//	//	System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but theDatabase == nullptr");
//	//	return false;
//	//}
//
//	//if (data == nullptr) {
//	//	System::Diagnostics::Trace::WriteLine("SkynetController::addTarget() ran, but data == nullptr");
//	//	return false;
//	//}
//	//try {
//	//	theDatabase->saveNewTarget(data);
//
//	//	TimerCallback^ tcb =
// //          gcnew TimerCallback(this, &SkynetController::addTargetToGUITable);
//	//	Threading::Timer^ addTargetTimer = gcnew Threading::Timer(tcb,  gcnew array<TargetRowData ^>{data}, 250, Timeout::Infinite);
//
//	//	//stateTimer->Start();
//	//}
//	//catch(Exception ^ e) {
//	//	System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addTarget(): Failed to save imageS - " + e);
//	//	return false;
//	//}
//
//	//return true;
//}

void SkynetController::addVerifiedTarget(VerifiedRowData ^ data) // not yet
{
	if (data == nullptr)
		return;


	int result = theDatabase->addVerified(data);

	if (!result)
		form1View->errorMessageUniversal("Failed to save verified target");
	else
		addVerifiedTargetToGUITable(data);

}

void SkynetController::addVerifiedTargetWithDialogData(DialogEditingData ^ data)
{
	if (data == nullptr)
		return;


	auto verifiedTarget = theDatabase->addVerifiedWithDialogData(data);

	if (verifiedTarget == nullptr)
		form1View->errorMessageUniversal("Failed to save verified target");
	else
		addVerifiedTargetToGUITable(verifiedTarget);

}

void SkynetController::addVerifiedTargetToGUITable(VerifiedRowData ^ data)
{
	if (data == nullptr)
		return;

	// data->imageName = theDatabase->imageNameForID("" + data->targetid);


	auto blahdelegate = gcnew Delegates::verifiedRowDataToVoid(form1View, &Form1::insertVerifiedTargetData );

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
	
	theDatabase->removeVerified(id);
	form1View->removeVerifiedTargetFromTable(id);

}

//bool SkynetController::addVote(Database::VoteRowData ^ data)
//{
//	if (theDatabase == nullptr) {
//		System::Diagnostics::Trace::WriteLine("SkynetController::addVote() ran, but theDatabase == nullptr");
//		return false;
//	}
//
//	if (data == nullptr) {
//		System::Diagnostics::Trace::WriteLine("SkynetController::addVote() ran, but data == nullptr");
//		return false;
//	}
//	try {
//		theDatabase->addVote(data);
//	}
//	catch(Exception ^ e) {
//		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::addVote(): Failed to add vote - " + e);
//		return false;
//	}
//
//	return true;
//}


//void SkynetController::removeVotesForID(String ^ id)
//{
//	
//	if (theDatabase == nullptr) {
//		System::Diagnostics::Trace::WriteLine("SkynetController::removeVotesForID() ran, but theDatabase == nullptr");
//		return;
//	}
//
//	if (id == nullptr) {
//		System::Diagnostics::Trace::WriteLine("SkynetController::removeVotesForID() ran, but id == nullptr");
//		return;
//	}
//
//	try {
//		theDatabase->removeVotesForId(id);
//	}
//	catch(Exception ^ e) {
//		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeVotesForID(): Failed to add vote - " + e);
//		return;
//	}
//
//	return;
//}

//void SkynetController::removeUnverified(Database::UnverifiedRowData ^ data)
//{
//	removeUnverified("" + data->targetid);
//}

//void SkynetController::removeCandidate(String ^ id)
//{	
//	//theDatabase->removeCandidate(id);
//
//	auto blahdelegate = gcnew Delegates::stringToVoid((Form1 ^)form1View, &Form1::removeCandidateFromTable );
//
//	try {
//		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
//	}
//	catch(Exception ^ e) {
//		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeUnveriefied(): Failed to remove unverified from GUI table - " + e);
//	}
//
//}

void SkynetController::removeUnverified(Database::UnverifiedRowData ^ data)
{
	removeUnverified("" + data->targetid);
}

void SkynetController::removeUnverified(String ^ id)
{
	PRINT("SkynetController::removeTarget() REMOVED");
	theDatabase->removeUnverified(id);

	Delegates::stringToVoid ^ blahdelegate = gcnew Delegates::stringToVoid((Form1 ^)form1View, &Form1::removeUnverifiedFromTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{id} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::removeTarget(): Failed to remove target from GUI table - " + e);
	}
}


//void SkynetController::modifyCandidate(Database::CandidateRowData ^ data)
//{
//	theDatabase->modifyCandidate(data);
//
//	auto blahdelegate = gcnew Delegates::candidateRowDataToVoid((Form1 ^)form1View, &Form1::modifyCandidateInTable );
//
//	try {
//		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
//	}
//	catch(Exception ^ e) {
//		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyCandidate(): Failed to modify candidate in GUI table - " + e);
//	}
//}

void SkynetController::modifyUnverified(Database::UnverifiedRowData ^ data)
{
	PRINT("SkynetController::modifyTarget() REMOVED");
	return;

	theDatabase->modifyUnverified(data);

	auto blahdelegate = gcnew Delegates::unverifiedRowDataToVoid((Form1 ^)form1View, &Form1::modifyUnverifiedInTable );

	try {
		((Form1 ^)form1View)->Invoke( blahdelegate, gcnew array<Object ^>{data} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::modifyUnverified(): Failed to modify target in GUI table - " + e);
	}
}

void SkynetController::displayAutosearchImage(Image ^ image)
{
	try{
		((Form1 ^)form1View)->autosearchBox->Image = image;
		((Form1 ^)form1View)->pathfinderBox->BackgroundImage = image;
	}catch(InvalidOperationException ^){
		
	}
	//image->Save("test.bmp");
}

void SkynetController::displayPathfinderImage(Image ^ image)
{
	((Form1 ^)form1View)->pathfinderBox->Image = image;
	//image->Save("test.bmp");
}

void SkynetController::pathfinderComplete(Image ^ image){
	displayPathfinderImage(image);
	// array <Waypoint ^>^ waypoints = intelligenceController->getWaypoints();
	// ((Form1 ^)form1View)->fillGpsCheckboxList(waypoints);
}

Database::CandidateRowData ^ SkynetController::candidateWithID(String ^ id)
{
	return theDatabase->candidateWithID(id);
}

Database::UnverifiedRowData ^ SkynetController::unverifiedWithID(String ^ id)
{
	return theDatabase->unverifiedWithID(id);
}


//Database::VotesOnCandidate ^ SkynetController::votesForID(String ^ id)
//{
//	return theDatabase->votesForID(id);
//}

Database::VerifiedRowData ^ SkynetController::verifiedWithID(String ^ id) // not yet
{
	//System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::verifiedTargetWithID(): not implemented");

	return theDatabase->verifiedWithID(id);
}


void SkynetController::saveUnverified(float * data, int width, int height, int numChannels, int originX, int originY, ImageWithPlaneData ^ stateOfPlane)
{
	System::Diagnostics::Trace::WriteLine("ERROR SkynetController::saveUnverified() not implemented");
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