#pragma once
#include "Tester.h"

namespace Testing {
	public ref class TestOCR : Tester {
		void testCanDetectLetters();
		void testSegmentation();
		void testCanDetectNumbers();
		void testOrientation();
		void testColor();
	};
}