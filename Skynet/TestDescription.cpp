#include "TestDatabase.h"
#include "MasterHeader.h"
namespace Testing {
	DescriptionTester::DescriptionTester(){}

	bool DescriptionTester::testInsertAndRetrieveDescription(){
		return false;
	}
	bool DescriptionTester::testUpdateDescription(){
		return false;
	}
	bool DescriptionTester::testDeleteDescription(){
		return false;
	}
	bool DescriptionTester::testApplyDialogData(){
		return false;
	}
	bool DescriptionTester::testToDialogData(){
		return false;
	}
	bool DescriptionTester::runTests(){
		if (testInsertAndRetrieveDescription() &&
			testUpdateDescription() &&
			testDeleteDescription() &&
			testApplyDialogData() &&
			testToDialogData()){
			return true;
		} else {
			PRINT("DescriptionTester failed");
			return false;
		}
	}

}