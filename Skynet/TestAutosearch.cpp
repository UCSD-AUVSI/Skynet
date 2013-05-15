#include "TestAutosearch.h"
#include "MasterHeader.h"

using namespace System;
namespace Testing {
	AutosearchTester::AutosearchTester(): Tester("Autosearch") {
		registerTest("Generate Map", gcnew Func<bool>(this, testCanGenerateMap));
		registerTest("Can Mark Area as Seen", gcnew Func<bool>(this, testCanGenerateMap));
	}
	
	bool AutosearchTester::testCanGenerateMap(){
		return false;
	}

	bool AutosearchTester::testCanMarkAreaAsSeen(){
		return false;
	}

}