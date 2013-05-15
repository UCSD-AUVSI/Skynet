#pragma once
#include "Tester.h"

namespace Testing {
	public ref class AutosearchTester: Tester {
	public:
		AutosearchTester();
		bool testCanGenerateMap();
		bool testCanMarkAreaAsSeen();
		virtual bool runTests() override;
	};
}