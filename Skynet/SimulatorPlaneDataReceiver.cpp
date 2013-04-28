#include "SimulatorPlaneDataReceiver.h"
#using <System.dll>

using namespace System;
using namespace System::Threading;

SimulatorPlaneDataReceiver::SimulatorPlaneDataReceiver(String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher): 
				  PlaneDataReceiver(directory, planeWatcher){
	 Thread ^simulationThread = gcnew Thread(
		 gcnew ThreadStart(this, &SimulatorPlaneDataReceiver::run)
		);
	 simulationThread->Start();
}

void SimulatorPlaneDataReceiver::run() {
	System::Diagnostics::Trace::WriteLine("Simulation Starting...");
	auto files = Linq::Enumerable::Select(IO::Directory::EnumerateFiles(directory, "*.jpg"), gcnew Func<String^, String^>(&PlaneDataReceiver::extractFilename));
	auto sortedEnumerable = Linq::Enumerable::OrderBy(files,gcnew Func<String^,Int64>(&PlaneDataReceiver::filenameToTimestamp));
	auto sorted = Linq::Enumerable::ToArray(sortedEnumerable);
	System::Diagnostics::Trace::WriteLine("Found " +sorted->Length + " frames");
	for (int i = 0; i < sorted->Length; i++){
		String ^ imageFilename = extractFilename(sorted[i]);
		String ^ dataFilename = imageFilenameToDataFilename(imageFilename);
		processImage(directory + "\\" + imageFilename,directory + "\\" + dataFilename);
		if ( i < sorted->Length - 1) {
			DateTime firstTime = DateTime(0).AddMilliseconds((double)filenameToTimestamp(imageFilename));
			DateTime nextTime = DateTime(0).AddMilliseconds((double)filenameToTimestamp(extractFilename(sorted[i+1])));
			Thread::Sleep(nextTime - firstTime);
		}
	} 
	System::Diagnostics::Trace::WriteLine("Simulation Complete");
}