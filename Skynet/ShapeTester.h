#pragma once

#include "OCRTester.h"

namespace Vision {
	using namespace System;
	using namespace Collections::Generic;

	ref class ShapeTester : OCRTester
	{
	public:
		ShapeTester(array<String^ >^ theFiles);

	protected:
		virtual String ^ computeGuess(String ^ filepath) override;
		virtual String ^ parseFilenameForClass(String ^ filename) override;
	};

}