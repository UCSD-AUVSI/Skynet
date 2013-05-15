#include "TestDatabase.h"
#include "MasterHeader.h"

namespace Testing {
	CandidateTester::CandidateTester(){}

	bool CandidateTester::testInsertAndRetrieveCandidate(){
		return false;
	}

	bool CandidateTester::testUpdateCandidate(){
		return false;
	}

	bool CandidateTester::testDeleteCandidate(){
		return false;
	}

	bool CandidateTester::testGetAllCandidates(){
		return false;
	}

	bool CandidateTester::testGetCandidateById(){
		return false;
	}

	bool CandidateTester::testToVerified(){
		return false;
	}

	bool CandidateTester::testToUnverified(){
		return false;
	}

	bool CandidateTester::testToDialogData(){
		return false;
	}

	bool CandidateTester::runTests() {
		if (testInsertAndRetrieveCandidate() && 
			testUpdateCandidate() &&
			testDeleteCandidate() &&
			testGetAllCandidates() &&
			testGetCandidateById() &&
			testToVerified() &&
			testToUnverified() &&
			testToDialogData() ){
			return true;
		} else {
			PRINT("Testing Candidates Failed");
			return false;
		}
	}
}