#pragma once

namespace Vision {
	using namespace System;
	using namespace Collections::Generic;

	ref class OCRTester
	{
	public:
		OCRTester(array<String^ >^ theFiles);
		void testAllFiles();
		
	protected:
		virtual String ^ computeGuess(String ^ filepath);
		virtual String ^ parseFilenameForClass(String ^ filename);
	
		String ^ typeOfTesting;
		bool useMultithreading;
	private:
		void processFile(int index);
		int printAndSumAccuracy();

		Dictionary<String ^, int> ^ correctClasses;
		Dictionary<String ^, int> ^ incorrectClasses; 
		array<String^ >^ files;
		int nextFile;
	};

}

