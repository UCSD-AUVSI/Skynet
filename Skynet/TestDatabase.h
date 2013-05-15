#include "Tester.h"
namespace Database {
	ref class VerifiedRowData; 
	ref class UnverifiedRowData;
	ref class CandidateRowData;
	ref class GPSPositionRowData;
	ref class DescriptionRowData;
}
namespace Testing {

	public ref class CandidateTester: Tester {
	public:
		CandidateTester();
		bool testInsertAndRetrieveCandidate();
		bool testUpdateCandidate();
		bool testDeleteCandidate();
		bool testGetAllCandidates();
		bool testGetCandidateById();
		bool testToVerified();
		bool testToUnverified();
		bool testToDialogData();
		virtual bool runTests() override;
	};

	public ref class UnverifiedTester: Tester {
	public:
		UnverifiedTester();
		bool testInsertAndRetrieveUnverified();
		bool testUpdateUnverified();
		bool testDeleteUnverified();
		bool testGetAllUnverified();
		bool testGetUnverifiedById();
		bool testToVerified();
		bool testToDialogData();
		virtual bool runTests() override;
	};
	
	public ref class VerifiedTester: Tester {
	public:
		VerifiedTester();
		bool testInsertAndRetrieveVerified();
		bool testUpdateVerified();
		bool testDeleteVerified();
		bool testGetAllVerified();
		bool testGetVerifiedById();
		bool testToVerified();
		bool testToDialogData();
		virtual bool runTests() override;
	};

	public ref class GPSPositionTester: Tester {
	public:
		GPSPositionTester();
		bool testInsertAndRetrieveGPS();
		bool testUpdateGPS();
		bool testDeleteGPS();
		virtual bool runTests() override;
	};

	public ref class DescriptionTester: Tester {
	public:
		DescriptionTester();
		bool testInsertAndRetrieveDescription();
		bool testUpdateDescription();
		bool testDeleteDescription();
		bool testApplyDialogData();
		bool testToDialogData();
		virtual bool runTests() override;
	};

	public ref class DatabaseTester: Tester {
	public:
		DatabaseTester();
		CandidateTester^ candidateTester;
		UnverifiedTester^ unverifiedTester;
		VerifiedTester^ verifiedTester;
		GPSPositionTester^ gpsPositionTester;
		DescriptionTester^ descriptionTester;
		virtual bool runTests() override;
	};
}