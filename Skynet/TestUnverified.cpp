#include "TestDatabase.h"
#include "MasterHeader.h"

namespace Testing {
	UnverifiedTester::UnverifiedTester(){}

	bool UnverifiedTester::testInsertAndRetrieveUnverified(){
		return false;
	}

	bool UnverifiedTester::testUpdateUnverified(){
		return false;
	}

	bool UnverifiedTester::testDeleteUnverified(){
		return false;
	}

	bool UnverifiedTester::testGetAllUnverified(){
		return false;
	}

	bool UnverifiedTester::testGetUnverifiedById(){
		return false;
	}

	bool UnverifiedTester::testToVerified(){
		return false;
	}


	bool UnverifiedTester::testToDialogData(){
		return false;
	}

	bool UnverifiedTester::runTests() {
		if (testInsertAndRetrieveUnverified() && 
			testUpdateUnverified() &&
			testDeleteUnverified() &&
			testGetAllUnverified() &&
			testGetUnverifiedById() &&
			testToVerified() &&
			testToDialogData() ){
			return true;
		} else {
			PRINT("Testing Unverifieds Failed");
			return false;
		}
	}
}