#include "TestDatabase.h"
namespace Testing {

	GPSPositionTester::GPSPositionTester(){}
	bool GPSPositionTester::testInsertAndRetrieveGPS(){
		return false;
	}
	bool GPSPositionTester::testUpdateGPS(){
		return false;
	}
	bool GPSPositionTester::testDeleteGPS(){
		return false;
	}
	bool GPSPositionTester::runTests(){
		if (testInsertAndRetrieveGPS() &&
			testUpdateGPS() &&
			testDeleteGPS()){
			return true;
		} else {
			PRINT("TestGPSPosition failed");
			return false;
		}
	}
}