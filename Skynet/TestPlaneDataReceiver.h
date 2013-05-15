
#pragma once
#include "Tester.h"

namespace Testing {
	public ref class TestPlaneDataReceiver : Tester {
		void testInformsPlaneWatcher();
		void testCanListenToDirectory();
		void testCanGetImage();
		void testCanLoadTelemetry();
		void testCanRunSimulation();
		void testImagenameToDataFilename();
		void testCanExtractFilename();
	};
}