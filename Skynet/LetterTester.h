#pragma once
#include "OCRTester.h"

namespace Vision {
	using namespace System;
	using namespace Collections::Generic;

	ref class OCRWrapper;

	ref class LetterTester : OCRTester
	{
	public:
		LetterTester(array<String^ >^ theFiles, OCRWrapper ^ theOCRWrapper);

	protected:
		virtual String ^ computeGuess(String ^ filepath) override;
		virtual String ^ parseFilenameForClass(String ^ filename) override;

		OCRWrapper ^ ocrWrapper;
	};

}