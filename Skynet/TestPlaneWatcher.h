
#pragma once
#include "Tester.h"

namespace Testing {
	public ref class PlaneWatcherTester : Tester {
	public:
		bool testCanFindTargetsInGoodImage();
		bool testDoesNotFindTargetsInBadImage();
		virtual bool runTests() override;
	};
}