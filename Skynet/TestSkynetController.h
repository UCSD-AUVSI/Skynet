#pragma once
#include "Tester.h"

namespace Testing {
	public ref class SkynetControllerTester : Tester {
	public:
		bool testCanRunAutonomousMission();
		bool testCanExportDatabase();
		virtual bool runTests() override;
	};
}