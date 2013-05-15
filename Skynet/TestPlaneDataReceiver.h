
#pragma once
#include "Tester.h"

namespace Testing {
	public ref class PlaneDataReceiverTester : Tester {
	public:
		bool testInformsPlaneWatcher();
		bool testCanListenToDirectory();
		bool testCanGetImage();
		bool testCanLoadTelemetry();
		bool testCanRunSimulation();
		bool testImagenameToDataFilename();
		bool testCanExtractFilename();
		virtual bool runTests() override;
	};
}