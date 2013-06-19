#include "Database.h"
#include "DatabaseStructures.h"

#include "GeoReference.h"
#include "MasterHeader.h"
#include <msclr/lock.h>
#include "Util.h"

using namespace System;
using namespace System::Data::Odbc;
using namespace Database;
using namespace Communications;
using namespace msclr;
using namespace Vision;

#define DATABASE_NAME	"postgres"
#define CANDIDATE_QUERY_STRING (gcnew String(CANDIDATE_QUERY_CHAR_ARRAY))
#define UNVERIFIED_QUERY_STRING (gcnew String(UNVERIFIED_QUERY_CHAR_ARRAY))
#define VERIFIED_QUERY_STRING (gcnew String(VERIFIED_QUERY_CHAR_ARRAY))

const char* VERIFIED_QUERY_CHAR_ARRAY = 
"SELECT verified_targets.submitid, "
	 "verified_targets.targetid, "
     "upper_right.gps_id AS upper_right_gps_id, "
     "upper_right.lat    AS upper_right_lat, "
     "upper_right.lon    AS upper_right_lon, "
     "upper_right.alt    AS upper_right_alt, "
     "upper_left.gps_id  AS upper_left_gps_id, "
     "upper_left.lat     AS upper_left_lat, "
     "upper_left.lon     AS upper_left_lon, "
     "upper_left.alt     AS upper_left_alt, "
     "lower_left.gps_id  AS lower_left_gps_id, "
     "lower_left.lat     AS lower_left_lat, "
     "lower_left.lon     AS lower_left_lon, "
     "lower_left.alt     AS lower_left_alt, "
     "lower_right.gps_id AS lower_right_gps_id, "
     "lower_right.lat    AS lower_right_lat, "
     "lower_right.lon    AS lower_right_lon, "
     "lower_right.alt    AS lower_right_alt, "
     "description.description_id, "
     "description.shape, "
     "description.shapecolor, "
     "description.letter, "
     "description.lettercolor, "
     "description.heading, "
     "description.target_x, "
     "description.target_y, "
     "candidates.candidateid, "
     "candidates.image_name, "
     "telemetry.telemetry_id, "
     "telemetry.gimbal_roll, "
     "telemetry.gimbal_pitch, "
     "telemetry.gimbal_yaw, "
	 "telemetry.gimbal_zoom, "
     "telemetry.plane_roll, "
     "telemetry.plane_pitch, "
     "telemetry.plane_heading, "
     "telemetry.origin_x, "
     "telemetry.origin_y, "
	 "planelocation.gps_id, "
	 "planelocation.lat, "
	 "planelocation.lon, "
	 "planelocation.alt, "
     "center_gps.gps_id, "
     "center_gps.lat, "
     "center_gps.lon, "
     "center_gps.alt, "
	 "telemetry.blurfactor, "
	 "telemetry.width_pixels, "
	 "telemetry.height_pixels "
"FROM verified_targets "
     "JOIN gps_position center_gps "
     "  ON verified_targets.center_gps_id = center_gps.gps_id "
     "JOIN unverified_targets "
     "  ON verified_targets.targetid = unverified_targets.targetid "
     "JOIN candidates "
     "  ON unverified_targets.candidateid = candidates.candidateid "
     "JOIN location "
     "  ON unverified_targets.location = location.loc_id "
     "JOIN gps_position lower_left "
     "  ON lower_left.gps_id = location.lower_left_gps "
     "JOIN gps_position lower_right "
     "  ON lower_right.gps_id = location.lower_right_gps "
     "JOIN gps_position upper_left "
     "  ON upper_left.gps_id = location.upper_left_gps "
     "JOIN gps_position upper_right "
     "  ON location.upper_right_gps = upper_right.gps_id "
     "JOIN telemetry "
     "  ON candidates.telemetryid = telemetry.telemetry_id "
	 "JOIN gps_position planelocation "
	 "  ON telemetry.planelocation_id = planelocation.gps_id "
     "JOIN description "
     "  ON unverified_targets.description_id = description.description_id";
//" ORDER BY submitid"; // was causing syntax err

const char* UNVERIFIED_QUERY_CHAR_ARRAY = 
	"SELECT unverified_targets.targetid, "
     "upper_right.gps_id AS upper_right_gps_id, "
     "upper_right.lat    AS upper_right_lat, "
     "upper_right.lon    AS upper_right_lon, "
     "upper_right.alt    AS upper_right_alt, "
     "upper_left.gps_id  AS upper_left_gps_id, "
     "upper_left.lat     AS upper_left_lat, "
     "upper_left.lon     AS upper_left_lon, "
     "upper_left.alt     AS upper_left_alt, "
     "lower_left.gps_id  AS lower_left_gps_id, "
     "lower_left.lat     AS lower_left_lat, "
     "lower_left.lon     AS lower_left_lon, "
     "lower_left.alt     AS lower_left_alt, "
     "lower_right.gps_id AS lower_right_gps_id, "
     "lower_right.lat    AS lower_right_lat, "
     "lower_right.lon    AS lower_right_lon, "
     "lower_right.alt    AS lower_right_alt, "
     "description.description_id, "
     "description.shape, "
     "description.shapecolor, "
     "description.letter, "
     "description.lettercolor, "
     "description.heading, "
     "description.target_x, "
     "description.target_y, "
     "candidates.candidateid, "
     "candidates.image_name, "
     "telemetry.telemetry_id, "
     "telemetry.gimbal_roll, "
     "telemetry.gimbal_pitch, "
     "telemetry.gimbal_yaw, "
	 "telemetry.gimbal_zoom, "
     "telemetry.plane_roll, "
     "telemetry.plane_pitch, "
     "telemetry.plane_heading, "
     "telemetry.origin_x, "
     "telemetry.origin_y, "
     "plane_position.gps_id, "
     "plane_position.lat, "
     "plane_position.lon, "
     "plane_position.alt, "
	 "telemetry.blurfactor, "
	 "telemetry.width_pixels, "
	 "telemetry.height_pixels "
"FROM unverified_targets "
     "JOIN candidates "
     "  ON unverified_targets.candidateid = candidates.candidateid "
     "JOIN location "
     "  ON unverified_targets.location = location.loc_id "
     "JOIN gps_position lower_left "
     "  ON lower_left.gps_id = location.lower_left_gps "
     "JOIN gps_position lower_right "
     "  ON lower_right.gps_id = location.lower_right_gps "
     "JOIN gps_position upper_left "
     "  ON upper_left.gps_id = location.upper_left_gps "
     "JOIN gps_position upper_right "
     "  ON location.upper_right_gps = upper_right.gps_id "
     "JOIN telemetry "
     "  ON candidates.telemetryid = telemetry.telemetry_id "
     "JOIN description "
     "  ON unverified_targets.description_id = description.description_id "
	 "JOIN gps_position plane_position"
	 "  ON telemetry.planelocation_id = plane_position.gps_id";
//"ORDER BY targetid"; // was causing syntax err


const char* CANDIDATE_QUERY_CHAR_ARRAY = 
	"SELECT candidates.candidateid, "
     "candidates.image_name, "
	 "candidates.processed, "
     "telemetry.telemetry_id, "
     "telemetry.gimbal_roll, "
     "telemetry.gimbal_pitch, "
     "telemetry.gimbal_yaw, "
	 "telemetry.gimbal_zoom, "
     "telemetry.plane_roll, "
     "telemetry.plane_pitch, "
     "telemetry.plane_heading, "
     "telemetry.origin_x, "
     "telemetry.origin_y, "
     "gps_position.gps_id, "
     "gps_position.lat, "
     "gps_position.lon, "
     "gps_position.alt, "
	 "telemetry.blurfactor, "
	 "telemetry.width_pixels, "
	 "telemetry.height_pixels "
"FROM candidates "
     "JOIN telemetry "
     "  ON candidates.telemetryid = telemetry.telemetry_id "
	 "JOIN gps_position"
	 "  ON telemetry.planelocation_id = gps_position.gps_id ";
//"ORDER BY candidateid"; // the last line is inserted in code. was causing syntax err in candidateWithID()

void DatabaseConnection::TestDatabase()
{
	PRINT("WARNING: DatabaseConnection::TestDatabase() just reset the database. All previous data was lost");
	
	int errorCount = 0;

	// make new db
	DatabaseConnection ^ db = gcnew DatabaseConnection();
	bool connected = db->connect();
	if (!connected)
	{ PRINT("ERR -5"); ++errorCount; }

	// reset
	db->reset();

	array<CandidateRowData ^>^ oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 0)
	{ PRINT("ERR -1"); ++errorCount; }

	
	array<UnverifiedRowData ^>^ unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 0)
	{ PRINT("ERR -2"); ++errorCount; }
	
	array<VerifiedRowData ^>^ verifArr = db->getAllVerified();
	if (verifArr->Length != 0)
	{ PRINT("ERR -3"); ++errorCount; }

	///////// TEST add candidates
	int candNum = 1; // so different candidates have different data
	CandidateRowData ^cand = gcnew CandidateRowData();
	cand->imageName = "cand1.jpg";
	cand->telemetry->originX = 5+candNum;
	cand->telemetry->originY = 6+candNum;
	cand->telemetry->gimbalRoll = 7+candNum;
	cand->telemetry->gimbalPitch = 8+candNum;
	cand->telemetry->gimbalYaw = 9+candNum;
	cand->telemetry->gimbalZoom = 10+candNum;
	cand->telemetry->blurFactor = 11+candNum;
	cand->telemetry->widthPixels = 12+candNum;
	cand->telemetry->heightPixels = 13+candNum;
	cand->telemetry->planeRoll = 14+candNum;
	cand->telemetry->planePitch = 15+candNum;
	cand->telemetry->planeHeading = 16+candNum;
	cand->telemetry->planeLocation = gcnew GPSPositionRowData(32+candNum, -117+candNum, 67+candNum);
	CandidateRowData ^cand1 = cand;

	db->addCandidate(cand1);

	// check database for accuracy
	oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 1)
	{
		PRINT("ERR 1"); ++errorCount;
	}
	else if (!oneCandArr[0]->Equals(cand1))
	{
		PRINT("ERR 2"); ++errorCount;
	}

	candNum = 20; // so different candidates have different data
	cand = gcnew CandidateRowData();
	cand->imageName = "cand1.jpg";
	cand->telemetry->originX = 5+candNum;
	cand->telemetry->originY = 6+candNum;
	cand->telemetry->gimbalRoll = 7+candNum;
	cand->telemetry->gimbalPitch = 8+candNum;
	cand->telemetry->gimbalYaw = 9+candNum;
	cand->telemetry->gimbalZoom = 10+candNum;
	cand->telemetry->blurFactor = 11+candNum;
	cand->telemetry->widthPixels = 12+candNum;
	cand->telemetry->heightPixels = 13+candNum;
	cand->telemetry->planeRoll = 14+candNum;
	cand->telemetry->planePitch = 15+candNum;
	cand->telemetry->planeHeading = 16+candNum;
	cand->telemetry->planeLocation = gcnew GPSPositionRowData(32+candNum, -117+candNum, 67+candNum);
	CandidateRowData ^cand2 = cand;

	db->addCandidate(cand2);

	// check database for accuracy
	oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 2)
	{
		PRINT("ERR 3"); ++errorCount;
	}
	else if (!oneCandArr[0]->Equals(cand1))
	{
		PRINT("ERR 4"); ++errorCount;
	}
	else if (!oneCandArr[1]->Equals(cand2))
	{
		PRINT("ERR 5"); ++errorCount;
	}




	//////// TEST modify candidate
	cand1->imageName = "cand1change.jpg";
	cand1->telemetry->blurFactor = 89;
	
	db->modifyCandidate(cand1);

	// check database for accuracy
	oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 2)
	{
		PRINT("ERR 6"); ++errorCount;
	}
	else if (!oneCandArr[0]->Equals(cand1))
	{
		PRINT("ERR 7"); ++errorCount;
	}
	else if (!oneCandArr[1]->Equals(cand2))
	{
		PRINT("ERR 8"); ++errorCount;
	}


	//////// TEST remove candidate
	db->removeCandidate("" + cand2->candidateid);

	// check database for accuracy
	oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 1)
	{
		PRINT("ERR 9"); ++errorCount;
	}
	else if (!oneCandArr[0]->Equals(cand1))
	{
		PRINT("ERR 10"); ++errorCount;
	}

	db->addCandidate(cand2);

	////////  candidateWithID
	CandidateRowData ^ cand2Alt = db->candidateWithID("" + cand2->candidateid);

	if (cand2Alt == nullptr)
	{
		PRINT("ERR 9.1"); ++errorCount;
	}
	else if (!cand2Alt->Equals(cand2))
	{
		PRINT("ERR 10.1"); ++errorCount;
	}

	/////////////////// Unverified
	int unverifNum = 1; // so different candidates have different data
	UnverifiedRowData ^ obj = gcnew UnverifiedRowData(cand1);
	obj->description->targetX = 1+unverifNum;
	obj->description->targetY = 2+unverifNum;
	obj->description->shape = "" + (3+unverifNum);
	obj->description->shapeColor = "" + (4+unverifNum);
	obj->description->letter = "" + (6+unverifNum);
	obj->description->letterColor = "" + (7+unverifNum);
	obj->description->heading = "" + (8+unverifNum);
	UnverifiedRowData ^ unverif1 = obj;

	unverifNum = 10;
	obj = gcnew UnverifiedRowData(cand2);
	obj->description->targetX = 1+unverifNum;
	obj->description->targetY = 2+unverifNum;
	obj->description->shape = "" + (3+unverifNum);
	obj->description->shapeColor = "" + (4+unverifNum);
	obj->description->letter = "" + (6+unverifNum);
	obj->description->letterColor = "" + (7+unverifNum);
	obj->description->heading = "" + (8+unverifNum);
	UnverifiedRowData ^ unverif2 = obj;

	db->addUnverified(unverif1);

	// check database for accuracy
	unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 1)
	{
		PRINT("ERR 11"); ++errorCount;
	}
	else if (!unverifArr[0]->Equals(unverif1))
	{
		PRINT("ERR 12"); ++errorCount;
	}


	db->addUnverified(unverif2);

	// check database for accuracy
	unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 2)
	{
		PRINT("ERR 13"); ++errorCount;
	}
	else if (!unverifArr[0]->Equals(unverif1))
	{
		PRINT("ERR 14"); ++errorCount;
	}
	else if (!unverifArr[1]->Equals(unverif2))
	{
		PRINT("ERR 15"); ++errorCount;
	}




	//////// TEST modify unverified
	unverif1->description->targetX = 1234;
	unverif1->description->shape = "myshapetest";
	
	db->modifyUnverified(unverif1);

	// check database for unverified
	unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 2)
	{
		PRINT("ERR 16"); ++errorCount;
	}
	else if (!unverifArr[0]->Equals(unverif1))
	{
		PRINT("ERR 17"); ++errorCount;
	}
	else if (!unverifArr[1]->Equals(unverif2))
	{
		PRINT("ERR 18"); ++errorCount;
	}


	//////// TEST remove unverified
	db->removeUnverified("" + unverif2->targetid);

	// check database for accuracy
	unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 1)
	{
		PRINT("ERR 19"); ++errorCount;
	}
	else if (!unverifArr[0]->Equals(unverif1))
	{
		PRINT("ERR 20"); ++errorCount;
	}

	db->addUnverified(unverif2);

	//////// unverifiedWithID
	UnverifiedRowData ^ unverif2Alt = db->unverifiedWithID("" + unverif2->targetid);

	if (unverif2Alt == nullptr)
	{
		PRINT("ERR 19.1"); ++errorCount;
	}
	else if (!unverif2Alt->Equals(unverif2))
	{
		PRINT("ERR 20.1"); ++errorCount;
	}

	/////////////////// verified
	
	VerifiedRowData ^ objVerif = gcnew VerifiedRowData(unverif1);
	VerifiedRowData ^ verif1 = objVerif;

	objVerif = gcnew VerifiedRowData(unverif2);
	VerifiedRowData ^ verif2 = objVerif;

	db->addVerified(verif1);

	// check database for accuracy
	verifArr = db->getAllVerified();
	if (verifArr->Length != 1)
	{
		PRINT("ERR 21"); ++errorCount;
	}
	else if (!verifArr[0]->Equals(verif1))
	{
		PRINT("ERR 22"); ++errorCount;
	}


	db->addVerified(verif2);

	// check database for accuracy
	verifArr = db->getAllVerified();
	if (verifArr->Length != 2)
	{
		PRINT("ERR 23"); ++errorCount;
	}
	else if (!verifArr[0]->Equals(verif1))
	{
		PRINT("ERR 24"); ++errorCount;
	}
	else if (!verifArr[1]->Equals(verif2))
	{
		PRINT("ERR 25"); ++errorCount;
	}




	//////// TEST modify unverified
	verif1->target->description->targetX = 4321;
	verif1->target->description->shape = "finaltest";
	
	db->modifyVerified(verif1);

	// check database for unverified
	verifArr = db->getAllVerified();
	if (verifArr->Length != 2)
	{
		PRINT("ERR 26"); ++errorCount;
	}
	else if (!verifArr[0]->Equals(verif1))
	{
		PRINT("ERR 27"); ++errorCount;
	}
	else if (!verifArr[1]->Equals(verif2))
	{
		PRINT("ERR 28"); ++errorCount;
	}


	//////// TEST remove unverified
	db->removeVerified("" + verif2->submitid);

	// check database for accuracy
	verifArr = db->getAllVerified();
	if (verifArr->Length != 1)
	{
		PRINT("ERR 29"); ++errorCount;
	}
	else if (!verifArr[0]->Equals(verif1))
	{
		PRINT("ERR 30"); ++errorCount;
	}

	db->addVerified(verif2);

	/////// TEST get verified
	VerifiedRowData ^ verif2Alt = db->verifiedWithID("" + verif2->submitid);

	if (verif2Alt == nullptr)
	{
		PRINT("ERR 29.1"); ++errorCount;
	}
	else if (!verif2Alt->Equals(verif2))
	{
		PRINT("ERR 30.1"); ++errorCount;
	}

	/////////////////////////

	// test submission data, visually check output
	//String ^ tabData = db->getTabDelimitedVerifiedTargetDataForSubmission();
	
	//PRINT("TAB DELIMITED DATA:\n" + tabData);

	
	
	// reset
	db->reset();
	

	oneCandArr = db->getAllCandidates();
	if (oneCandArr->Length != 0)
	{ PRINT("ERR 31"); ++errorCount; }

	
	unverifArr = db->getAllUnverified();
	if (unverifArr->Length != 0)
	{ PRINT("ERR 32"); ++errorCount; }
	
	verifArr = db->getAllVerified();
	if (verifArr->Length != 0)
	{ PRINT("ERR 33"); ++errorCount; }

	
	connected = db->disconnect();
	if (!connected)
	{ PRINT("ERR 34"); ++errorCount; }

	if (errorCount == 0)
		PRINT("\n\n\n\n\nSUCCESS - ALL Database tests passed\n\n\n\n\n");
	else
		PRINT("\n\n\n\n\n*\n*\n*\n*\n*\nERROR - "+errorCount+" Database tests failed\n*\n*\n*\n*\n*\n\n\n\n\n");

}

void DatabaseConnection::FillDatabaseForTesting()
{
	PRINT("WARNING: DatabaseConnection::TestDatabase() just reset the database. All previous data was lost");
	
	int errorCount = 0;

	// make new db
	DatabaseConnection ^ db = gcnew DatabaseConnection();
	
	db->connect();

	// reset
	db->reset();

	int candNum = 1; // so different candidates have different data
	CandidateRowData ^cand = gcnew CandidateRowData();
	cand->imageName = "cand1.jpg";
	cand->telemetry->originX = 5+candNum;
	cand->telemetry->originY = 6+candNum;
	cand->telemetry->gimbalRoll = 7+candNum;
	cand->telemetry->gimbalPitch = 8+candNum;
	cand->telemetry->gimbalYaw = 9+candNum;
	cand->telemetry->gimbalZoom = 10+candNum;
	cand->telemetry->blurFactor = 11+candNum;
	cand->telemetry->widthPixels = 12+candNum;
	cand->telemetry->heightPixels = 13+candNum;
	cand->telemetry->planeRoll = 14+candNum;
	cand->telemetry->planePitch = 15+candNum;
	cand->telemetry->planeHeading = 16+candNum;
	cand->telemetry->planeLocation = gcnew GPSPositionRowData(32+candNum, -117+candNum, 67+candNum);
	CandidateRowData ^cand1 = cand;

	db->addCandidate(cand1);


	candNum = 20; // so different candidates have different data
	cand = gcnew CandidateRowData();
	cand->imageName = "cand1.jpg";
	cand->telemetry->originX = 5+candNum;
	cand->telemetry->originY = 6+candNum;
	cand->telemetry->gimbalRoll = 7+candNum;
	cand->telemetry->gimbalPitch = 8+candNum;
	cand->telemetry->gimbalYaw = 9+candNum;
	cand->telemetry->gimbalZoom = 10+candNum;
	cand->telemetry->blurFactor = 11+candNum;
	cand->telemetry->widthPixels = 12+candNum;
	cand->telemetry->heightPixels = 13+candNum;
	cand->telemetry->planeRoll = 14+candNum;
	cand->telemetry->planePitch = 15+candNum;
	cand->telemetry->planeHeading = 16+candNum;
	cand->telemetry->planeLocation = gcnew GPSPositionRowData(32+candNum, -117+candNum, 67+candNum);
	CandidateRowData ^cand2 = cand;

	db->addCandidate(cand2);


	/////////////////// Unverified
	int unverifNum = 1; // so different candidates have different data
	UnverifiedRowData ^ obj = gcnew UnverifiedRowData(cand1);
	obj->description->targetX = 1+unverifNum;
	obj->description->targetY = 2+unverifNum;
	obj->description->shape = "" + (3+unverifNum);
	obj->description->shapeColor = "" + (4+unverifNum);
	obj->description->letter = "" + (6+unverifNum);
	obj->description->letterColor = "" + (7+unverifNum);
	obj->description->heading = "" + (8+unverifNum);
	UnverifiedRowData ^ unverif1 = obj;

	unverifNum = 10;
	obj = gcnew UnverifiedRowData(cand2);
	obj->description->targetX = 1+unverifNum;
	obj->description->targetY = 2+unverifNum;
	obj->description->shape = "" + (3+unverifNum);
	obj->description->shapeColor = "" + (4+unverifNum);
	obj->description->letter = "" + (6+unverifNum);
	obj->description->letterColor = "" + (7+unverifNum);
	obj->description->heading = "" + (8+unverifNum);
	UnverifiedRowData ^ unverif2 = obj;

	db->addUnverified(unverif2);

	
	/////////////////// verified
	
	VerifiedRowData ^ objVerif = gcnew VerifiedRowData(unverif1);
	VerifiedRowData ^ verif1 = objVerif;

	//obj = gcnew VerifiedRowData(unverif2);
	//VerifiedRowData ^ verif2 = obj;

	db->addVerified(verif1);

	//db->addVerified(verif2);
	
	db->disconnect();
}



//////////// Constructor/Destructor ///////////////

DatabaseConnection::DatabaseConnection()
{
	databaseOpen = false;
	try {
		_database = gcnew OdbcConnection("Driver={PostGreSQL Unicode(x64)};Server=localhost;Database=" + DATABASE_NAME + ";UID=postgres;PWD=triton");
		//_database = gcnew OdbcConnection("Driver={PostGreSQL};Server=localhost;Database=" + DATABASE_NAME + ";UID=postgres;PWD=triton");
		_command = gcnew OdbcCommand();	
		_database->Open();
		_command->Connection = _database;
		databaseOpen = true;
	}

	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::DatabaseConnection(): Could not connect to database - " + e);
	}
}

DatabaseConnection::~DatabaseConnection()
{
	disconnect();
}

//////////// SQL Operations ///////////////

bool
DatabaseConnection::connect(void)
{
	bool retVal = false;

	try
	{
		if( _database->State == System::Data::ConnectionState::Closed )
			_database->Open();
		retVal = true;
	}
	catch( Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR in database.cpp: connect(): " + e);

	}

	return retVal;
}

bool
DatabaseConnection::disconnect(void)
{
	bool retVal = false;

	try
	{
		_database->Close();
		retVal = true;
	}
	catch( Exception ^ e)
	{
	
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::disconnect() " + e);
	}

	return retVal;
}

bool
DatabaseConnection::reset(void)
{
	bool retVal = false;
	_command->Connection = _database;
	_command->CommandText = "DELETE FROM verified_targets;"
							"DELETE FROM unverified_targets;"
							"DELETE FROM candidates;" 
							"DELETE FROM telemetry;"
						    "DELETE FROM description;"
							"DELETE FROM location;" 
						    "DELETE FROM gps_position;";
	OdbcDataReader ^ reader;
	try
	{
		reader = _command->ExecuteReader();
		reader->Close();
		retVal = true;
	}
	catch( Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::reset() " + e);
	}
	
	return retVal;
}

OdbcDataReader ^ 
DatabaseConnection::runQuery(String ^ query)
{
	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::runQuery(): database not open.");
		return nullptr;
	}

	if (query == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::runQuery(): query == nullptr");
		return nullptr;
	}
	
	lock l(_database);
	_command->Connection = _database;

	// form command
	_command->CommandText = query;

	OdbcDataReader ^ reader;

	// execute command
	try
	{
		reader = _command->ExecuteReader();
		return reader;
	
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::runQuery(): Executing command failed :( " + e);
		_reader->Close();
		return nullptr;
	}
}

String ^
DatabaseConnection::getTableName( TableName table )
{
	String ^ retVal;

	switch( table )
	{
	case tableCandidateRegions:
		retVal = "candidates";
		break;
	case tableUnverifiedTargets:
		retVal = "unverified_targets";
		break;
	case tableVerifiedTargets:
		retVal = "verified_targets";
		break;
	}

	return retVal;
}

const int Target_Table = 0;
const int Unverified_Table = 1;
const int Candidate_Table = 2;

//////////// Clear Tables ///////////////

void 
DatabaseConnection::clearCandidatesTable()
{
	runQuery("TRUNCATE candidates CASCADE");
}

void 
DatabaseConnection::clearUnverifiedTable()
{
	runQuery("TRUNCATE unverified_targets CASCADE");
}

void 
DatabaseConnection::clearVerifiedTable()
{
	runQuery("TRUNCATE verified_targets CASCADE");
}

void
DatabaseConnection::removeTarget(TargetRowData^ target)
{
	runQuery(target->deleteSQL());
}

//////////// Add Rows ///////////////

int 
DatabaseConnection::addCandidate( CandidateRowData ^ data)
{
	String ^ query;
	int gps_id, telemetry_id;
	if ( data->telemetry->telemetry_id == 0 ){
		if ( data->telemetry->planeLocation->gps_id == 0 ){
			data->telemetry->planeLocation->gps_id = DatabaseConnection::addGPSPosition(data->telemetry->planeLocation);
		}
		gps_id = data->telemetry->planeLocation->gps_id;
		query = 
				"INSERT INTO telemetry"
								"(gimbal_roll,"
								" gimbal_pitch,"
								" gimbal_yaw,"
								" origin_x,"
								" origin_y,"
								" gimbal_zoom,"
								" plane_roll,"
								" plane_pitch,"
								" plane_heading,"
								" blurfactor,"
								" height_pixels,"
								" width_pixels,"
								" planelocation_id)"
					"VALUES		(" + data->telemetry->gimbalRoll +","
								   + data->telemetry->gimbalPitch + ","
								   + data->telemetry->gimbalYaw + ","
								   + data->telemetry->originX + ","
								   + data->telemetry->originY + ","
								   + data->telemetry->gimbalZoom + ","
								   + data->telemetry->planeRoll + ","
								   + data->telemetry->planePitch + ","
								   + data->telemetry->planeHeading + ","
								   + data->telemetry->blurFactor + ","
								   + data->telemetry->heightPixels + ","
								   + data->telemetry->widthPixels + ","
								   + gps_id + ")"
					"RETURNING telemetry_id;";
		auto result = runQuery(query);
		result->Read();
		data->telemetry->telemetry_id = result->GetInt32(0);
		result->Close();
	}
	telemetry_id = data->telemetry->telemetry_id;
	query =
			"INSERT INTO candidates"
								"(image_name,"
								" telemetryid,"
								" processed)"
				"VALUES			('" + data->imageName + "\',"
								   + telemetry_id + ","
								   + data->processed + ")"
				"RETURNING candidateid;";

	auto result = runQuery(query);
	result->Read();
	data->candidateid = result->GetInt32(0);
	result->Close();
	return data->candidateid;
}

int 
DatabaseConnection::addGPSPosition ( GPSPositionRowData ^ data )
{
	String ^ query = "INSERT INTO gps_position (lat,lon,alt) VALUES (" +
						data->lat + "," +
						data->lon + "," +
						data->alt + ")" +
					" RETURNING gps_id;";
	auto result = runQuery(query);
	result->Read();
	data->gps_id = result->GetInt32(0);
	result->Close();
	return data->gps_id;
}


int 
DatabaseConnection::addUnverified( UnverifiedRowData ^ data)
{
	String ^ query;
	int loc_id,description_id, candidate_id;

	if ( data->candidate->candidateid == 0 ){
		data->candidate->candidateid = addCandidate(data->candidate);
	}
	candidate_id = data->candidate->candidateid;

	if ( data->location->loc_id == 0){
		int lowerLeftGPSID = DatabaseConnection::addGPSPosition(data->location->lowerLeftGPS);
		int lowerRightGPSID = DatabaseConnection::addGPSPosition(data->location->lowerRightGPS);
		int upperLeftGPSID = DatabaseConnection::addGPSPosition(data->location->upperLeftGPS);
		int upperRightGPSID = DatabaseConnection::addGPSPosition(data->location->upperRightGPS);
		query = "INSERT INTO location"
									"(lower_left_gps,"
									" lower_right_gps,"
									" upper_left_gps,"
									" upper_right_gps)"
							"VALUES (" + lowerLeftGPSID + ","
									   + lowerRightGPSID + ","
									   + upperLeftGPSID + ","
									   + upperRightGPSID + ")"
							"RETURNING loc_id;";
		auto result = runQuery(query);
		result->Read();
		data->location->loc_id = result->GetInt32(0);
		result->Close();
	}
	loc_id = data->location->loc_id;

	if (data->description->description_id == 0){
		query = "INSERT INTO description"
									"(shape,"
									" shapecolor,"
									" letter,"
									" lettercolor,"
									" target_x,"
									" target_y,"
									" heading)"
						" VALUES ('" + data->description->shape + "','"
									 + data->description->shapeColor + "','"
									 + data->description->letter + "','"
									 + data->description->letterColor + "',"
									 + data->description->targetX + ","
									 + data->description->targetY + ",'"
									 + data->description->heading + "')"
						" RETURNING description_id;";
		auto result = runQuery(query);
		result->Read();
		data->description->description_id = result->GetInt32(0);
		result->Close();
	}
	description_id = data->description->description_id;

	query = "INSERT INTO unverified_targets "
										"(candidateid,"
										" revision,"
										" location,"
										" description_id,"
										" processed)"
							"VALUES (" + candidate_id + ","
									   + data->revision + ","
									   + loc_id + ","
									   + description_id + ","
									   + "true" + ")"
							"RETURNING targetid;";
	auto result = runQuery(query);
	result->Read();
	data->targetid = result->GetInt32(0);
	result->Close();
	return data->targetid;
}

int
DatabaseConnection::upsertVerified( VerifiedRowData^ data) 
{
	if (data->submitid) {
		modifyVerified(data);
		return data->submitid;
	} else {
		return addVerified(data);
	}
}

int 
DatabaseConnection::addVerified( VerifiedRowData ^ data)
{
	int targetid, center_gps_id;
	
	if ( data->target->targetid == 0){
		data->target->targetid = DatabaseConnection::addUnverified(data->target);
	}
	targetid = data->target->targetid;

	if (data->centerGPS->gps_id == 0){
		data->centerGPS->gps_id = DatabaseConnection::addGPSPosition(data->centerGPS);
	}
	center_gps_id = data->centerGPS->gps_id;

	String ^ query = "INSERT INTO verified_targets "
								"(targetid,"
								" center_gps_id)"
						"VALUES (" + targetid + "," +
								   + center_gps_id + ") "
						"RETURNING submitid";

	modifyUnverified(data->target); // Update the unverified 
	auto result = runQuery(query);
	result->Read();
	data->submitid = result->GetInt32(0);
	result->Close();
	return data->submitid;
}

VerifiedRowData ^ 
DatabaseConnection::addVerifiedWithDialogData(DialogEditingData ^ data)
{
	UnverifiedRowData^ target = unverifiedWithID("" + data->id);
	VerifiedRowData^ verified = gcnew VerifiedRowData(target);

	DescriptionRowData^ description = target->description;
	
	description->targetX = data->targetX;
	description->targetY = data->targetY;
	description->shape = data->shape;
	description->shapeColor = data->shapeColor;
	description->letter = data->letter;
	description->letterColor = data->letterColor;
	modifyDescription(description);
	int id = addVerified(verified);
	verified->submitid = id;
	return verified;
}

//////////// Modify Rows ///////////////


void
DatabaseConnection::modifyLocation( LocationRowData ^ data )
{
	DatabaseConnection::modifyGPSPosition(data->lowerLeftGPS);
	DatabaseConnection::modifyGPSPosition(data->lowerRightGPS);
	DatabaseConnection::modifyGPSPosition(data->upperLeftGPS);
	DatabaseConnection::modifyGPSPosition(data->upperRightGPS);
}

void
DatabaseConnection::modifyDescription( DescriptionRowData ^ data )
{
	String ^ query = "UPDATE description "
					 "SET shape = '" + data->shape + "',"
						 "shapecolor = '" + data->shapeColor + "',"
						 "letter = '" + data->letter + "',"
						 "lettercolor = '" + data->letterColor + "',"
						 "target_x = " + data->targetX + ","
						 "target_y = " + data->targetY + ","
						 "heading = '" + data->heading + "'"
					" WHERE description_id = " + data->description_id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void
DatabaseConnection::modifyGPSPosition ( GPSPositionRowData ^ data )
{
	String ^ query = "UPDATE gps_position "
					 "SET lat = " + data->lat + ","
						 "lon = " + data->lon + ","
						 "alt = " + data->alt +
					 " WHERE gps_id = " + data->gps_id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void
DatabaseConnection::modifyTelemetry ( TelemetryRowData ^ data )
{
	DatabaseConnection::modifyGPSPosition(data->planeLocation);
	String ^ query = "UPDATE telemetry "
					 "SET gimbal_roll = " + data->gimbalRoll + ","
						 "gimbal_pitch = " + data->gimbalPitch + ","
						 "gimbal_yaw = " + data->gimbalYaw + ","
						 "origin_x = " + data->originX + ","
						 "origin_y = " + data->originY + ","
						 "gimbal_zoom = " + data->gimbalZoom + ","
						 "plane_roll = " + data->planeRoll + ","
						 "plane_pitch = " + data->planePitch + ","
						 "plane_heading = " + data->planeHeading + ","
						 "blurfactor = " + data->blurFactor + ","
						 "width_pixels = " + data->widthPixels + ","
						 "height_pixels = " + data->heightPixels +
					" WHERE telemetry_id = " + data->telemetry_id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void 
DatabaseConnection::modifyCandidate( CandidateRowData ^ data)
{
	DatabaseConnection::modifyTelemetry(data->telemetry);
	String ^ query = "UPDATE candidates "
					 "SET image_name = '" + data->imageName + "',"
					     "processed = " + data->processed + 
					 " WHERE candidateid = " + data->candidateid + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void 
DatabaseConnection::modifyUnverified( UnverifiedRowData ^ data)
{
	DatabaseConnection::modifyCandidate(data->candidate);
	DatabaseConnection::modifyLocation(data->location);
	DatabaseConnection::modifyDescription(data->description);
	
	String ^ query = "UPDATE unverified_targets "
					 "SET revision = " + data->revision + ","
						 "processed = " + data->processed +
					 " WHERE targetid = " + data->targetid + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void 
DatabaseConnection::modifyVerified( VerifiedRowData ^ data)
{
	DatabaseConnection::modifyUnverified(data->target);
	DatabaseConnection::modifyGPSPosition(data->centerGPS);
}

//////////// Remove Rows ///////////////
//////////// Deprecated  ///////////////
// TODO: Remove
void 
DatabaseConnection::removeCandidate( String ^ id )
{
	String ^ query = "DELETE FROM candidates " 
					 "WHERE candidateid = " + id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void 
DatabaseConnection::removeUnverified( String ^ id )
{
	String ^ query = "DELETE FROM unverified_targets " 
					 "WHERE targetid = " + id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

void 
DatabaseConnection::removeVerified( String ^ id )
{
	String ^ query = "DELETE FROM verified_targets " 
					 "WHERE submitid = " + id + ";";
	DatabaseConnection::runQuery(query)->Close();
}

//////////// Get Row For ID ///////////////

CandidateRowData ^ 
DatabaseConnection::candidateWithID(String ^ id)
{
	String ^ query = CANDIDATE_QUERY_STRING + " WHERE candidateid = " + id;

	array<Object ^> ^ results = getAllRows(query,Candidate_Table);

	if (results != nullptr && results->Length > 0)
		return (CandidateRowData ^)results[0];
	else
		return nullptr;
}

UnverifiedRowData ^ 
DatabaseConnection::unverifiedWithID(String ^ id)
{
	String ^ query = UNVERIFIED_QUERY_STRING + " WHERE targetid = " + id;

	array<Object ^> ^ results = getAllRows(query,Unverified_Table);

	if (results != nullptr && results->Length > 0)
		return (UnverifiedRowData ^)results[0];
	else
		return nullptr;
}

VerifiedRowData ^ 
DatabaseConnection::verifiedWithID(String ^ id)
{
	String ^ query = VERIFIED_QUERY_STRING + " WHERE submitid = " + id;
	array<Object ^> ^ results = getAllRows(query,Target_Table);
	if (results != nullptr && results->Length > 0)
		return (VerifiedRowData ^)results[0];
	else
		return nullptr;
}


//////////// Get All Rows ///////////////

array<CandidateRowData ^>^ 
DatabaseConnection::getAllCandidates()
{
	array<Object ^>^ rows;
	String ^ query = CANDIDATE_QUERY_STRING + " ORDER BY candidateid ";
	rows = getAllRows(query, Candidate_Table);
	if (rows == nullptr)
		rows = gcnew array<Object ^>(0);
	auto results = gcnew array<CandidateRowData ^>(rows->Length);
	for (int i = 0; i < rows->Length; i ++)
	{
		results[i] = (CandidateRowData ^)rows[i];
	}
	return results;
}

array<UnverifiedRowData ^>^ 
DatabaseConnection::getAllUnverified()
{
	array<Object ^>^ rows;
	String ^ query = UNVERIFIED_QUERY_STRING + " ORDER BY targetid ";


	rows = getAllRows(query, Unverified_Table);
	if (rows == nullptr)
		rows = gcnew array<Object ^>(0);
	auto results = gcnew array<UnverifiedRowData ^>(rows->Length);
	for (int i = 0; i < rows->Length; i ++)
	{
		results[i] = (UnverifiedRowData ^)rows[i];
	}
	return results;
}

array<VerifiedRowData ^>^ 
DatabaseConnection::getAllVerified()
{
	array<Object ^>^ rows;
	String ^ query = VERIFIED_QUERY_STRING + " ORDER BY submitid ";

	rows = getAllRows(query, Target_Table);
	if (rows == nullptr)
		rows = gcnew array<Object ^>(0);

	auto results = gcnew array<VerifiedRowData ^>(rows->Length);
	for (int i = 0; i < rows->Length; i ++)
	{
		results[i] = (VerifiedRowData ^)rows[i];
	}
	return results;
}

array<Object ^> ^
DatabaseConnection::getAllRows(String ^query, int tableType)
{
	int size = 20;

	array<Object ^>^ retArr = gcnew array<Object ^>(size);

	if (_database->State == System::Data::ConnectionState::Closed) {
		System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::getAllRows(): database not open.");
		return nullptr;
	}

	
	lock l(_database);
	_command->Connection = _database;

	// form command
	String ^ commandText = query;

	_command->CommandText = commandText;

	// execute command
	try
	{
		_reader = _command->ExecuteReader();
		if (_reader->HasRows)
		{
			int i = 0;
			while (_reader->Read())
			{
				Object ^rowData;
				switch(tableType)
				{
					case Target_Table:
						rowData = verifiedFromReader(_reader);
						break;
					case Unverified_Table:
						rowData = unverifiedFromReader(_reader);
						break;
					case Candidate_Table:
						rowData = candidateFromReader(_reader);
						break;
					default:
						rowData = nullptr;
						break;
				}

				retArr[i++] = rowData;

				if (i >= size) {
					size += 10;
					Array::Resize(retArr, size);
				}
			}

			Array::Resize(retArr, i);
		}
		else {
			retArr = nullptr;
		}
		_reader->Close();
	}

	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("DatabaseConnection::getAllRows(): Executing command "+query+" failed " + e);
		_reader->Close();
		return nullptr;
	}

	return retArr;
}



String ^ DatabaseConnection::imageNameForID(String ^ id)
{
	if (id == nullptr)
		return nullptr;

	String ^ retval = nullptr;

	String ^ query = "SELECT image_name FROM candidates WHERE candidateid=" + id;
	OdbcDataReader ^ reader = runQuery(query);
	
	try {
		if (reader != nullptr && reader->Read())
			retval = reader->GetString(0);
	}
	catch (Exception ^ e) {
		PRINT("DatabaseConnection::imageNameForID() ERROR failed read query results: " + e);
	}

	if (reader != nullptr)
		reader->Close();

	return retval;
}

VerifiedRowData ^ DatabaseConnection::verifiedFromReader(OdbcDataReader ^ theReader)
{
	VerifiedRowData ^ row = gcnew VerifiedRowData();
	int col = 0;

	try {
				
		row->submitid = theReader->GetInt32(col++);
		row->target = gcnew UnverifiedRowData();
		row->target->targetid = theReader->GetInt32(col++);
		row->target->location = gcnew LocationRowData();
		row->target->location->upperRightGPS = gcnew GPSPositionRowData();
		row->target->location->upperRightGPS->gps_id = theReader->GetInt32(col++);
		row->target->location->upperRightGPS->lat = theReader->GetDouble(col++);
		row->target->location->upperRightGPS->lon = theReader->GetDouble(col++);
		row->target->location->upperRightGPS->alt = theReader->GetDouble(col++);

		row->target->location->upperLeftGPS = gcnew GPSPositionRowData();
		row->target->location->upperLeftGPS->gps_id = theReader->GetInt32(col++);
		row->target->location->upperLeftGPS->lat = theReader->GetDouble(col++);
		row->target->location->upperLeftGPS->lon = theReader->GetDouble(col++);
		row->target->location->upperLeftGPS->alt = theReader->GetDouble(col++);

		row->target->location->lowerLeftGPS = gcnew GPSPositionRowData();
		row->target->location->lowerLeftGPS->gps_id = theReader->GetInt32(col++);
		row->target->location->lowerLeftGPS->lat = theReader->GetDouble(col++);
		row->target->location->lowerLeftGPS->lon = theReader->GetDouble(col++);
		row->target->location->lowerLeftGPS->alt = theReader->GetDouble(col++);

		row->target->location->lowerRightGPS = gcnew GPSPositionRowData();
		row->target->location->lowerRightGPS->gps_id = theReader->GetInt32(col++);
		row->target->location->lowerRightGPS->lat = theReader->GetDouble(col++);
		row->target->location->lowerRightGPS->lon = theReader->GetDouble(col++);
		row->target->location->lowerRightGPS->alt = theReader->GetDouble(col++);

		row->target->description = gcnew DescriptionRowData();
		row->target->description->description_id = theReader->GetInt32(col++);
		row->target->description->shape = theReader->GetString(col++);
		row->target->description->shapeColor = theReader->GetString(col++);
		row->target->description->letter = theReader->GetString(col++);
		row->target->description->letterColor = theReader->GetString(col++);
		row->target->description->heading = theReader->GetString(col++);
		row->target->description->targetX = theReader->GetInt32(col++);
		row->target->description->targetY = theReader->GetInt32(col++);

		row->target->candidate = gcnew CandidateRowData();

		row->target->candidate->candidateid = theReader->GetInt32(col++);
		row->target->candidate->imageName = theReader->GetString(col++);

		row->target->candidate->telemetry = gcnew TelemetryRowData();
		row->target->candidate->telemetry->telemetry_id = theReader->GetInt32(col++);
		row->target->candidate->telemetry->gimbalRoll = theReader->GetDouble(col++);
		row->target->candidate->telemetry->gimbalPitch = theReader->GetDouble(col++);
		row->target->candidate->telemetry->gimbalYaw = theReader->GetDouble(col++);
		row->target->candidate->telemetry->gimbalZoom = theReader->GetInt32(col++);
		row->target->candidate->telemetry->planeRoll = theReader->GetDouble(col++);
		row->target->candidate->telemetry->planePitch = theReader->GetDouble(col++);
		row->target->candidate->telemetry->planeHeading = theReader->GetDouble(col++);
		row->target->candidate->telemetry->originX = theReader->GetInt32(col++);
		row->target->candidate->telemetry->originY = theReader->GetInt32(col++);
		row->target->candidate->telemetry->planeLocation = gcnew GPSPositionRowData();
		row->target->candidate->telemetry->planeLocation->gps_id = theReader->GetInt32(col++);
		row->target->candidate->telemetry->planeLocation->lat = theReader->GetDouble(col++);
		row->target->candidate->telemetry->planeLocation->lon = theReader->GetDouble(col++);
		row->target->candidate->telemetry->planeLocation->alt = theReader->GetDouble(col++);



		row->centerGPS = gcnew GPSPositionRowData();
		row->centerGPS->gps_id = theReader->GetInt32(col++);
		row->centerGPS->lat = theReader->GetDouble(col++);
		row->centerGPS->lon = theReader->GetDouble(col++);
		row->centerGPS->alt = theReader->GetDouble(col++);

		row->target->candidate->telemetry->blurFactor = theReader->GetDouble(col++);
		row->target->candidate->telemetry->widthPixels = theReader->GetInt32(col++);
		row->target->candidate->telemetry->heightPixels = theReader->GetInt32(col++);
	}
	catch (InvalidCastException ^)
	{
		//System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::targetFromReader() ");
	}

	return row;
}

UnverifiedRowData ^ DatabaseConnection::unverifiedFromReader(OdbcDataReader ^ theReader)
{
	UnverifiedRowData ^ row = gcnew UnverifiedRowData();

	// fill row
	int col = 0;

	try {
	
		
		row->targetid = theReader->GetInt32(col++);
		row->location = gcnew LocationRowData();


		row->location->upperRightGPS = gcnew GPSPositionRowData();
		row->location->upperRightGPS->gps_id = theReader->GetInt32(col++);
		row->location->upperRightGPS->lat = theReader->GetDouble(col++);
		row->location->upperRightGPS->lon = theReader->GetDouble(col++);
		row->location->upperRightGPS->alt = theReader->GetDouble(col++);

		row->location->upperLeftGPS = gcnew GPSPositionRowData();
		row->location->upperLeftGPS->gps_id = theReader->GetInt32(col++);
		row->location->upperLeftGPS->lat = theReader->GetDouble(col++);
		row->location->upperLeftGPS->lon = theReader->GetDouble(col++);
		row->location->upperLeftGPS->alt = theReader->GetDouble(col++);
		
		row->location->lowerLeftGPS = gcnew GPSPositionRowData();
		row->location->lowerLeftGPS->gps_id = theReader->GetInt32(col++);
		row->location->lowerLeftGPS->lat = theReader->GetDouble(col++);
		row->location->lowerLeftGPS->lon = theReader->GetDouble(col++);
		row->location->lowerLeftGPS->alt = theReader->GetDouble(col++);

		row->location->lowerRightGPS = gcnew GPSPositionRowData();
		row->location->lowerRightGPS->gps_id = theReader->GetInt32(col++);
		row->location->lowerRightGPS->lat = theReader->GetDouble(col++);
		row->location->lowerRightGPS->lon = theReader->GetDouble(col++);
		row->location->lowerRightGPS->alt = theReader->GetDouble(col++);


		row->description = gcnew DescriptionRowData();
		row->description->description_id = theReader->GetInt32(col++);
		row->description->shape = theReader->GetString(col++);
		row->description->shapeColor = theReader->GetString(col++);
		row->description->letter = theReader->GetString(col++);
		row->description->letterColor = theReader->GetString(col++);
		row->description->heading = theReader->GetString(col++);
		row->description->targetX = theReader->GetInt32(col++);
		row->description->targetY = theReader->GetInt32(col++);

		row->candidate = gcnew CandidateRowData();

		row->candidate->candidateid = theReader->GetInt32(col++);
		row->candidate->imageName = theReader->GetString(col++);
		
		row->candidate->telemetry = gcnew TelemetryRowData();
		row->candidate->telemetry->telemetry_id = theReader->GetInt32(col++);
		row->candidate->telemetry->gimbalRoll = theReader->GetDouble(col++);
		row->candidate->telemetry->gimbalPitch = theReader->GetDouble(col++);
		row->candidate->telemetry->gimbalYaw = theReader->GetDouble(col++);
		row->candidate->telemetry->gimbalZoom = theReader->GetInt32(col++);
		row->candidate->telemetry->planeRoll = theReader->GetDouble(col++);
		row->candidate->telemetry->planePitch = theReader->GetDouble(col++);
		row->candidate->telemetry->planeHeading = theReader->GetDouble(col++);
		row->candidate->telemetry->originX = theReader->GetInt32(col++);
		row->candidate->telemetry->originY = theReader->GetInt32(col++);
		row->candidate->telemetry->planeLocation = gcnew GPSPositionRowData();
		row->candidate->telemetry->planeLocation->gps_id = theReader->GetInt32(col++);
		row->candidate->telemetry->planeLocation->lat = theReader->GetDouble(col++);
		row->candidate->telemetry->planeLocation->lon = theReader->GetDouble(col++);
		row->candidate->telemetry->planeLocation->alt = theReader->GetDouble(col++);
		row->candidate->telemetry->blurFactor = theReader->GetDouble(col++);
		row->candidate->telemetry->widthPixels = theReader->GetInt32(col++);
		row->candidate->telemetry->heightPixels = theReader->GetInt32(col++);
	}
	catch (InvalidCastException ^)
	{
		//System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::targetFromReader() ");
	}

	return row;
}

CandidateRowData ^ DatabaseConnection::candidateFromReader(OdbcDataReader ^ theReader)
{
	CandidateRowData ^ row = gcnew CandidateRowData();

	// fill row
	int col = 0;

	try {

		row->candidateid = theReader->GetInt32(col++);
		row->imageName = theReader->GetString(col++);
		row->processed = theReader->GetBoolean(col++);
		
		row->telemetry = gcnew TelemetryRowData();
		row->telemetry->telemetry_id = theReader->GetInt32(col++);
		row->telemetry->gimbalRoll = theReader->GetDouble(col++);
		row->telemetry->gimbalPitch = theReader->GetDouble(col++);
		row->telemetry->gimbalYaw = theReader->GetDouble(col++);
		row->telemetry->gimbalZoom = theReader->GetInt32(col++);
		row->telemetry->planeRoll = theReader->GetDouble(col++);
		row->telemetry->planePitch = theReader->GetDouble(col++);
		row->telemetry->planeHeading = theReader->GetDouble(col++);
		row->telemetry->originX = theReader->GetInt32(col++);
		row->telemetry->originY = theReader->GetInt32(col++);
		row->telemetry->planeLocation = gcnew GPSPositionRowData();
		row->telemetry->planeLocation->gps_id = theReader->GetInt32(col++);
		row->telemetry->planeLocation->lat = theReader->GetDouble(col++);
		row->telemetry->planeLocation->lon = theReader->GetDouble(col++);
		row->telemetry->planeLocation->alt = theReader->GetDouble(col++);
		row->telemetry->blurFactor = theReader->GetDouble(col++);
		row->telemetry->widthPixels = theReader->GetInt32(col++);
		row->telemetry->heightPixels = theReader->GetInt32(col++);


	}
	catch (InvalidCastException ^)
	{
		//System::Diagnostics::Trace::WriteLine("ERROR in DatabaseConnection::targetFromReader() ");
	}

	return row;
}


