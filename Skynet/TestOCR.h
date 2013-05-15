#pragma once
#include "Tester.h"

namespace Testing {
	public ref class OCRTester : Tester {
	public:
		OCRTester();
		bool testCanDetectLetters();
		bool testSegmentation();
		bool testCanDetectNumbers();
		bool testOrientation();
		bool testColor();
		virtual bool runTests() override;
	};
}