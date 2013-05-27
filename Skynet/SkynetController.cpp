#include "StdAfx.h"

#include "SkynetController.h"
#include "IntelligenceController.h"
#include "VisionController.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include "Delegates.h"
#include "SaveImage.h"
#include "SimulatorPlaneDataReceiver.h"
#include "RealPlaneDataReceiver.h"
#include "GPSCoord.h"
#include "Util.h"
#include "RealPlaneDataReceiver.h"


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
	visionController = gcnew VisionController(this);
}

void SkynetController::processPlaneData(ImageWithPlaneData^ imageWithPlaneData){
	if (visionController != nullptr ){
		visionController->receiveFrame(imageWithPlaneData);
	}
	form1View->displayPlaneData(imageWithPlaneData);
}

void SkynetController::startSimulation(String^ folder){
	receiver = gcnew SimulatorPlaneDataReceiver(folder, theWatcher);
}

void SkynetController::startMission(String^ folder){
	receiver = gcnew RealPlaneDataReceiver(folder, theWatcher);
}
void SkynetController::lockPosition(Intelligence::GPSCoord^ coordinate)
{
	theWatcher->lockPosition(coordinate);
}

void SkynetController::unlockPosition()
{
	theWatcher->unlockPosition();
}
void SkynetController::startIntelligenceController(array<GPSCoord^>^ fieldBoundaries)
{
	intelligenceController = gcnew IntelligenceController(fieldBoundaries, this);
}

void SkynetController::handleIntelligenceResult(IntelligenceResult^ result)
{
	form1View->setPath(result->path);
	form1View->displayPathfinderImage(result->imageFilename);
}
IntelligenceController^ SkynetController::getIntelligenceController()
{
	return intelligenceController;
}

void SkynetController::updateAutosearchImage(Image^ image) {
	form1View->updateAutosearchImage(image);
}



SkynetController::~SkynetController()
{
	form1View = nullptr;
	cameraView = nullptr;
	theDatabase = nullptr;
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

String ^ SkynetController::getTabDelimitedVerifiedTargetDataForSubmission(array<VerifiedRowData^>^ verifiedRows)
{
	String ^ retval = "";

//	9 fields, tab delimited, new target on each line
//Field 1: Target Number, two digits, starting at 01 and increment by one for each additional
//target.
//Example: 01, 02, 03, etc.
//Field 2: Latitude in the following format, first character N or S, two digit degrees (use leading
//zeros if necessary), followed by space, two digit minutes, followed by space, two digit seconds
//followed by decimal point and up to 3 digits (thousandths of a second)
//Example N30 35 34.123
//Field 3: Longitude if the following format, first character E or W, three digit degrees (use
//leading zeros if necessary), followed by space, two digit minutes, followed by space, tow digit
//seconds followed by decimal point and up to 3 digits (thousandths of a second)
//Example W075 48 47.123
//Field 4: Target orientation, up to two characters: N, NE, E, SE, S, SW, W, NW
//Field 5 Target shape, list geometric shape as appropriate:
//Example, rectangle, square, isosolese triangle
//Field 6: Target color, as appropriate.
//Example: Red, Orange, Yellow, etc.
//Field 7: Alphanumeric, as appropriate
//Example: A, b, 2, &
//Field 8: Alphanumeric color, as appropriate
//Example: Red, Orange, Yellow, etc.
//Field 9: Name of jpeg file with image of target
//Example for two targets
//01 N30 35 34.123 W075 48 47.123 N rectangle red A orange
//target1.jpg
//02 S34 00 12.345 E002 01 12.345 SE square orange 4 yellow
//target2.jpg

	int i = 0;

	for each (VerifiedRowData ^ row in verifiedRows){
		i++;
		String^ targetNumber = Int32(i).ToString("00");
		array<String^>^ latLon = GeoReference::latLonToDMS(row->centerGPS->lat,row->centerGPS->lon);
		if(String::IsNullOrWhiteSpace(row->target->description->heading)){
			row->target->description->heading = "N";
		}
		String^ imageName = Util::extractFilename(row->target->candidate->imageName);
		retval += targetNumber; // 1
		retval += latLon[0] + "\t" + latLon[1] + "\t"; // 2, 3
		retval += row->target->description->heading + "\t"; // 4
		retval += row->target->description->shape + "\t"; // 5
		retval += row->target->description->shapeColor + "\t"; // 6
		retval += row->target->description->letter + "\t"; // 7
		retval += row->target->description->letterColor + "\t"; // 8
		retval += imageName + "\n"; // 9
	}

	PRINT("Finished building tab-delimited data:" + retval);
	return retval;
}
void SkynetController::exportData(String ^ basePath)
{
	// get data
	array<VerifiedRowData^>^ verifiedRows = theDatabase->getAllVerified();
	Util::copyImageFilesToPath(verifiedRows, basePath);
	String ^ result = getTabDelimitedVerifiedTargetDataForSubmission(verifiedRows);

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
		form1View->Invoke(gcnew unverifiedRowDataToVoid( form1View, &Skynet::Form1::insertUnverifiedData), unverified  );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in SkynetController::saveCandidate(): Failed to save imageS - " + e);
	}
}


void SkynetController::upsertVerified(VerifiedRowData ^ data)
{

	int result = theDatabase->upsertVerified(data);

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
	theDatabase->removeVerified(id);
	// TODO: form1View->removeVerified(id);
}

void SkynetController::removeTarget(TargetRowData^ target)
{
	theDatabase->removeTarget(target);
	// TODO: form1View->removeTarget(target);
}

void SkynetController::removeUnverified(Database::UnverifiedRowData ^ data)
{
	removeUnverified("" + data->targetid);
}

void SkynetController::removeUnverified(String ^ id)
{
	PRINT("SkynetController::removeTarget() REMOVED");
}


void SkynetController::modifyUnverified(Database::UnverifiedRowData ^ data)
{
	PRINT("SkynetController::modifyTarget() REMOVED");
}

Database::CandidateRowData ^ SkynetController::candidateWithID(String ^ id)
{
	return theDatabase->candidateWithID(id);
}

Database::UnverifiedRowData ^ SkynetController::unverifiedWithID(String ^ id)
{
	return theDatabase->unverifiedWithID(id);
}

Database::VerifiedRowData ^ SkynetController::verifiedWithID(String ^ id) // not yet
{
	return theDatabase->verifiedWithID(id);
}