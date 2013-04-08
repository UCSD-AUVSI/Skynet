#include "StdAfx.h"
#include "ShapeTester.h"
#include "VisionUtil.h"
#include "Util.h"
#include "ShapeRecognizer.h"

using namespace Vision;

ShapeTester::ShapeTester(array<String^ >^ theFiles) : OCRTester(theFiles) 
{
	typeOfTesting="Shape";
}

String ^ 
ShapeTester::computeGuess(String ^ filepath)
{
	ShapeRecognizer ^ shapeRecognizer = gcnew ShapeRecognizer();
	return shapeRecognizer->recognizeShape(filepath);
}

String ^ 
ShapeTester::parseFilenameForClass(String ^ filename)
{
	String^ truth = parseFilenameForSeparateShape(filename);
	return truth;
}
