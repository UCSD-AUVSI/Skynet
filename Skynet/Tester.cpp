#include "Tester.h"
#include "MasterHeader.h"
#include "TestAutosearch.h"
#include "TestDatabase.h"
#include "TestOCR.h"
#include "TestPathfinding.h"
#include "TestPlaneDataReceiver.h"
#include "TestPlaneWatcher.h"
#include "TestSaliency.h"
#include "TestSkynetController.h"

using namespace System;
using namespace System::Collections::Generic;

namespace Testing {

	Tester::Tester(String^ name): 
		name(name),
		tests(gcnew List<TestCase^>()){}

	Tester::TestCase^ Tester::toTestCase() {
		return gcnew TestCase(name, gcnew Func<bool>(this, runTests));
	}

	Tester::TestCase::TestCase(String^ name, Func<bool>^ func): name(name), func(func){}

	void Tester::registerTest(String^ name, Func<bool>^ func) {
		tests->Add(gcnew TestCase(name, func));
	}

	void Tester::registerTester(Tester^ tester){
		tests->Add(tester->toTestCase());
	}
	void Tester::registerTesters(...array<Tester^>^testers){
		for each (auto^ tester in testers){
			tests->Add(tester->toTestCase());
		}
	}

	bool Tester::runTests(){
		bool result = true;
		for each (auto^ test in tests) {
			if (!test->run()){
				PRINT("Test " + test->name + " failed"); 
				result = false;
			} else {
				PRINT("Test " + test->name + " passed"); 
			}
		}
		return result;
	}

	bool Tester::TestCase::run(){
		return func();
	}

	SkynetTester::SkynetTester():
		Tester("Skynet Tester"),
		autosearchTester(gcnew AutosearchTester()),
		databaseTester(gcnew DatabaseTester()),
		ocrTester(gcnew OCRTester()),
		pathfindingTester(gcnew PathfindingTester()),
		dataReceiverTester(gcnew PlaneDataReceiverTester()),
		planeWatcherTester(gcnew PlaneWatcherTester()),
		saliencyTester(gcnew SaliencyTester()),
		skynetControllerTester(gcnew SkynetControllerTester()){
		registerTesters(autosearchTester, 
						databaseTester,
						ocrTester,
						pathfindingTester,
						dataReceiverTester,
						planeWatcherTester,
						saliencyTester,
						skynetControllerTester);

	}
}