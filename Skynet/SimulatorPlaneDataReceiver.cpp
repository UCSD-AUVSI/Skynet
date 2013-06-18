#include "SimulatorPlaneDataReceiver.h"
#include "ImageAndGPSFiles.h"
#using <System.dll>

using namespace System;
using namespace System::Threading;
using namespace Intelligence;

SimulatorPlaneDataReceiver::SimulatorPlaneDataReceiver(String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher): 
				  PlaneDataReceiver(directory, planeWatcher){
	 Thread ^simulationThread = gcnew Thread(
		 gcnew ThreadStart(this, &SimulatorPlaneDataReceiver::run)
		);

	auto files = Linq::Enumerable::Select(IO::Directory::EnumerateFiles(directory, "*.jpg"), gcnew Func<String^, ImageAndGPSFiles^>(&ImageAndGPSFiles::fromImageFilename));
	auto sortedEnumerable = Linq::Enumerable::OrderBy(files,gcnew Func<ImageAndGPSFiles^,UInt64>(&ImageAndGPSFiles::getTimestamp));
	auto sorted = Linq::Enumerable::ToArray(sortedEnumerable);
	 simulationThread->Start();
}

void SimulatorPlaneDataReceiver::run() {
	/**
	 * Do nothing for now
	 */
}