#include "MasterHeader.h"
#include "TestDatabase.h"
namespace Testing {
	DatabaseTester::DatabaseTester():
		candidateTester(gcnew CandidateTester()),
		unverifiedTester(gcnew UnverifiedTester()),
		verifiedTester(gcnew VerifiedTester()),
		gpsPositionTester(gcnew GPSPositionTester()),
		descriptionTester(gcnew DescriptionTester()) {}

	bool DatabaseTester::runTests(){
		if (candidateTester->runTests() &&
			unverifiedTester->runTests() && 
			verifiedTester->runTests() &&
			gpsPositionTester->runTests() &&
			descriptionTester->runTests()) {
			return true;
		} else {
			PRINT("DatabaseTester failed");
			return false;
		}
	}
}