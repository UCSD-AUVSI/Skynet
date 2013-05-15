#include "TestDatabase.h"
#include "MasterHeader.h"

namespace Testing {
	VerifiedTester::VerifiedTester(){}

	bool VerifiedTester::testInsertAndRetrieveVerified(){
		return false;
	}

	bool VerifiedTester::testUpdateVerified(){
		return false;
	}

	bool VerifiedTester::testDeleteVerified(){
		return false;
	}

	bool VerifiedTester::testGetAllVerified(){
		return false;
	}

	bool VerifiedTester::testGetVerifiedById(){
		return false;
	}

	bool VerifiedTester::testToVerified(){
		return false;
	}


	bool VerifiedTester::testToDialogData(){
		return false;
	}

	bool VerifiedTester::runTests() {
		if (testInsertAndRetrieveVerified() && 
			testUpdateVerified() &&
			testDeleteVerified() &&
			testGetAllVerified() &&
			testGetVerifiedById() &&
			testToVerified() &&
			testToDialogData() ){
			return true;
		} else {
			PRINT("Testing Verifieds Failed");
			return false;
		}
	}
}