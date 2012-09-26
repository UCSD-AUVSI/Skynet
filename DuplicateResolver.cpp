#include "StdAfx.h"
#include "DuplicateResolver.h"
#include "Database.h"
#include "MasterHeader.h"
#include "Delegates.h"
#include "OCR.h"
#include "Form1.h"
#include "GeoReference.h"
#include "OCR/Recognizer.h"
//#include "Recognizer.h"
#include "DatabaseStructures.h"


using namespace Vision;
using namespace Delegates;
//using namespace Skynet;
using namespace Database;

//std::string ManagedToSTL(String ^ s) 
//{
//	using namespace Runtime::InteropServices;
//	const char* chars = 
//		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
//	std::string retVal = chars;
//	Marshal::FreeHGlobal(IntPtr((void*)chars));
//
//	return retVal;
//}

DuplicateResolver::DuplicateResolver(DatabaseConnection ^ db, Skynet::Form1 ^ gui)
{
	database = db;
	form1 = gui;

	recognizer = gcnew Recognizer();

	saliencyAddTarget = gcnew candidateRowDataToVoid( this, &DuplicateResolver::receiveCandidate );

	unverifiedRows = db->getAllUnverified();
}

void
DuplicateResolver::receiveCandidate(CandidateRowData ^ candidate)
{
	// save candidate in database
	database->addCandidate(candidate);
	form1->Invoke(gcnew candidateRowDataToVoid( form1, &Skynet::Form1::insertCandidateData), candidate );

	bool isUnique = true;
	bool isImproved = false;
	UnverifiedRowData ^ unverified = gcnew UnverifiedRowData(candidate);
	UnverifiedRowData ^ matchingUnverified = nullptr;

	GPSPositionRowData ^ candidateGPS = unverified->location->centerGPS();
	float candidateArea = unverified->location->widthMeters()*unverified->location->heightMeters();



	// find unverified row with gps w/in 5 meters, and size w/in 1 meter
	for each (UnverifiedRowData ^ row in unverifiedRows)
	{
		GPSPositionRowData ^rowGPS = row->location->centerGPS();
		float rowArea = row->location->widthMeters()*row->location->heightMeters();

		if (candidateGPS->distanceTo(rowGPS) < 5 && abs(rowArea - candidateArea) < 1)
		{
			matchingUnverified = row;
		}
	}

	if (matchingUnverified == nullptr)
		isUnique = true;
	else
	{
		float unverifiedBlurFactor = unverified->candidate->telemetry->blurFactor;
		float matchingBlurFactor = matchingUnverified->candidate->telemetry->blurFactor;
		int unverifiedAreaPixels = unverified->candidate->telemetry->widthPixels*unverified->candidate->telemetry->heightPixels;
		int matchingAreaPixels = matchingUnverified->candidate->telemetry->widthPixels*matchingUnverified->candidate->telemetry->heightPixels;;

		// determine if matchingUnverified is better than unverified
		if (unverifiedAreaPixels > matchingAreaPixels || ( unverifiedAreaPixels*1.5 > matchingAreaPixels && unverifiedBlurFactor < matchingBlurFactor))
			isImproved = true;

	}

	if (isUnique)
	{
		database->addUnverified(unverified);
		form1->Invoke(gcnew unverifiedRowDataToVoid( form1, &Skynet::Form1::insertUnverifiedData), unverified );
	}

	else if (isImproved)
	{
		matchingUnverified->updateCandidate(candidate);

		database->modifyUnverified(matchingUnverified);
		form1->Invoke(gcnew unverifiedRowDataToVoid( form1, &Skynet::Form1::modifyUnverifiedInTable), matchingUnverified  );
	}

	if (isUnique || isImproved)
	{
		unverified = matchingUnverified;

		//// run OCR - should modify database
		//ImageData ^ ocrData = recognizer->recognizeImage(cv::imread(ManagedToSTL(HTTP_SERVER_TARGET_PATH + candidate->imageName)));

		//// handle data
		//unverified->description->shape = ocrData->shape;
		//unverified->description->letter = ocrData->letter;

		//database->modifyUnverified(unverified);
	}

}