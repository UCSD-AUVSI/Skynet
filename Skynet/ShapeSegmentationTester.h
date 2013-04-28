#pragma once
#include "ShapeSegmentationTester.h"

namespace Vision {
	using namespace System;
	using namespace Collections::Generic;

	ref class OCRWrapper;

	ref class ShapeSegmentationTester
	{
	public:
		ShapeSegmentationTester();
		static void runTest( String^ testDirectory, OCRWrapper ^ theOCRWrapper );
	};

}