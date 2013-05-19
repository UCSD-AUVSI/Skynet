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
		void testInsertAndRetrieveCandidate();
		void testUpdateCandidate();
		void testDeleteCandidate();
		void testGetAllCandidate();
		void testGetCandidateById();
		virtual void runTests() override;
	};

	public ref class UnverifiedTester: Tester {
	public:
		void testInsertAndRetrieveUnverified();
		void testUpdateUnverified();
		void testDeleteUnverified();
		void testGetAllUnverified();
		void testGetUnverifiedById();
		virtual void runTests() override;
	};
	
	public ref class VerifiedTester: Tester {
	public:
		void testInsertAndRetrieveVerified();
		void testUpdateVerified();
		void testDeleteVerified();
		void testGetAllVerified();
		void testGetVerifiedById();
		virtual void runTests() override;
	};

	public ref class GPSPositionTester: Tester {
	public:
		void testInsertAndRetrieveVerified();
		void testUpdateVerified();
		void testDeleteVerified();
		void testGetAllVerified();
		void testGetVerifiedById();
		virtual void runTests() override;
	};


	public ref class DatabaseTester: Tester {
	public:
		virtual void runTests() override;

	};
}