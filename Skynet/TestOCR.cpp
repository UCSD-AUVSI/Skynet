#include "TestOCR.h"

namespace Testing {
	OCRTester::OCRTester(){}
	bool testCanDetectLetters();
	bool testSegmentation();
	bool testCanDetectNumbers();
	bool testOrientation();
	bool testColor();
	virtual bool runTests() override;
}