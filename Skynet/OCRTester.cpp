#include "StdAfx.h"
#include "OCRTester.h"
#include "VisionUtil.h"
#include "Util.h"
#include <msclr/lock.h>

using namespace Vision;
using namespace Util;
using namespace System;
using namespace Collections;
using namespace Collections::Generic;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace msclr;

OCRTester::OCRTester(array<String^ >^ theFiles)
{
	correctClasses = gcnew Dictionary<String ^, int>();
	incorrectClasses = gcnew Dictionary<String ^, int>();
	files = theFiles;
	nextFile = 0;
	typeOfTesting = "";
	useMultithreading = true;
}


void
OCRTester::testAllFiles()
{
	PRINT(typeOfTesting + " testing ...");

	int numFiles = files->Length;
	if (useMultithreading)
		Parallel::For(0, numFiles, gcnew Action<int>(this, &OCRTester::processFile));
	else
	{
		for (int i = 0; i < numFiles; ++i)
			processFile(i);
	}
	int numCorrect = printAndSumAccuracy();
	
	PRINT(typeOfTesting + " accuracy: " + 100.0*(numCorrect/(float)numFiles) + "%");
}

void
OCRTester::processFile(int index)
{		
	int fileIndexToGrab;
	{
		lock fileLock(files);
		fileIndexToGrab = nextFile++;
	}

	String^ filepath = files[fileIndexToGrab];
	String^ filename = extractFilename(filepath);

	String^ truth = parseFilenameForClass(filename);
	String ^ guess = computeGuess(filepath);
		
	lock l(correctClasses);
	if (truth->Equals(guess))
	{
		PRINT(filename + " -> Correct for " + guess);
		if (!correctClasses->ContainsKey(truth))
			correctClasses->Add(truth, 0);
		correctClasses[truth] += 1;
	}
	else
	{
		PRINT(filename + " -> Error on " + filename + ", recognized " + guess);
		if (!incorrectClasses->ContainsKey(truth))
			incorrectClasses->Add(truth, 0);
		incorrectClasses[truth] += 1;
	}
}

String ^ 
OCRTester::parseFilenameForClass(String ^ filename)
{
	throw gcnew Exception("OCRTester is abstract. Use an implementation of it");
}

String ^ 
OCRTester::computeGuess(String ^ filepath)
{
	throw gcnew Exception("OCRTester is abstract. Use an implementation of it");
}

int 
OCRTester::printAndSumAccuracy()
{
	int numCorrect = 0;
	for each( KeyValuePair<String^, int> kvp in correctClasses )
    {
		String ^ className = kvp.Key;
		numCorrect += kvp.Value;
		int validOfThisClass = kvp.Value;
		int invalidOfThisClass = 0;
		if (incorrectClasses->ContainsKey(className))
			invalidOfThisClass = incorrectClasses[className];
		float accuracy = validOfThisClass/(float)(validOfThisClass+invalidOfThisClass);
        PRINT(className + ": " + accuracy*100.0f + " \t(" + validOfThisClass + "/" + (validOfThisClass+invalidOfThisClass) + ")");
    }
	return numCorrect;
}