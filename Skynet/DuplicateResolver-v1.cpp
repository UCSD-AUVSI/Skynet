#include "StdAfx.h"
#include "DuplicateResolver.h"
#include "Database.h"
#include "MasterHeader.h"
#include "Delegates.h"
#include "OCR.h"
#include "Form1.h"


using namespace Vision;
using namespace Delegates;
//using namespace Skynet;
using namespace Database;

DuplicateResolver::DuplicateResolver(DatabaseConnection ^ db, OCR ^ theOCR, Skynet::Form1 ^ gui)
{
	database = db;
	ocr = theOCR;
	form1 = gui;

	saliencyAddTarget = gcnew candidateRowDataToVoid( this, &DuplicateResolver::receiveCandidate );

	recognizer = gcnew Recognizer();
	//unverifiedRows = db->getAllUnverified();
}

void
DuplicateResolver::receiveCandidate(CandidateRowData ^ candidate)
{
	// save candidate in database
	database->addCandidate(candidate);
	form1->Invoke(gcnew candidateRowDataToVoid( form1, &Skynet::Form1::insertCandidateData), candidate );

	bool isUnique = true;
	bool isImproved = false;
	UnverifiedRowData ^ unverified = nullptr;

	// TODO: add centerGPS, physicalSize to unverified
	// TODO: add GPS.distanceTo(gps2) function to GPSPositionRowData, return meters

	//GPSPositionRowData ^ candidateGPS = nullptr; // TODO: calculate
	//float candidateArea = 0; // TODO: calculate



	//// find unverified row within 5 meters, with size within 1 meter
	//for each (UnverifiedRowData ^ row in unverifiedRows)
	//{
	//	GPSPositionRowData ^rowGPS = row->centerGPS;

	//	float rowArea = row->widthMeters*row->heightMeters;

	//	if (candidateGPS.distanceTo(rowGPS) < 5)

	//}




	// TODO resolve duplicates, set isUnique and isImproved

	if (isUnique)
	{
		unverified = gcnew UnverifiedRowData(candidate);

		database->addUnverified(unverified);
		form1->Invoke(gcnew unverifiedRowDataToVoid( form1, &Skynet::Form1::insertUnverifiedData), unverified );
		/*form1->Invoke(gcnew unverifiedRowDataToVoid( form1, &Skynet::Form1::addUnverifiedToTable), unverified );*/

	}

	else if (isImproved)
	{
		unverified->updateCandidate(candidate);

		database->modifyUnverified(unverified);
		form1->Invoke(gcnew unverifiedRowDataToVoid( form1, &Skynet::Form1::modifyUnverifiedInTable), unverified  );
	}

	if (isUnique || isImproved)
	{
		// todo - run OCR - should modify database if not already approved by human
	}

}