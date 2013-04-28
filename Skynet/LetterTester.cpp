#include "StdAfx.h"
#include "LetterTester.h"
#include "VisionUtil.h"
#include "Util.h"
#include "ShapeRecognizer.h"
#include "OCRWrapper.h"

using namespace Vision;

LetterTester::LetterTester(array<String^ >^ theFiles, OCRWrapper ^ theOCRWrapper) 
	: OCRTester(theFiles) 
{
	typeOfTesting="Letter";
	useMultithreading = false;
	ocrWrapper = theOCRWrapper;
}

String ^ 
LetterTester::computeGuess(String ^ filepath)
{
	String ^ guess = ocrWrapper->recognizeLetter(filepath);
	guess = disambiguateLetter(guess);
	return "" + guess;
}

String ^ 
LetterTester::parseFilenameForClass(String ^ filename)
{
	String^ truth = "" + parseFilenameForLetter(filename);
	return truth;
}
