#pragma once
#include "Tester.h"

namespace Testing {
	public ref class PathfindingTester : Tester {
	public:
		bool testDoesGeneratePath();
		virtual bool runTests() override;
	};
}