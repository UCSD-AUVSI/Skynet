#pragma once
#include "Tester.h"

namespace Testing {
	public ref class TestSaliency : Tester {
		void testCanFindTargetsInGoodImage();
		void testDoesNotFindTargetsInBadImage();
	};
}