#include "RealPlaneDataReceiver.h"
using System::IO::FileSystemEventHandler;
using System::IO::FileSystemEventArgs;
using System::IO::FileSystemWatcher;
using System::String;

RealPlaneDataReceiver::RealPlaneDataReceiver(String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher):
PlaneDataReceiver(directory, planeWatcher){ run(); }

void RealPlaneDataReceiver::run() {
	System::Diagnostics::Trace::WriteLine("Starting PlaneDataReceiver...");
	FileSystemWatcher ^ watcher = gcnew FileSystemWatcher(directory);
	watcher->Created += gcnew FileSystemEventHandler(this,&RealPlaneDataReceiver::fileAdded);
	watcher->EnableRaisingEvents = true;
}

void RealPlaneDataReceiver::fileAdded(Object ^ sender, FileSystemEventArgs ^ e){
	String ^ imageFilename = e->Name;
	auto split = imageFilename->Split('.');
	System::Diagnostics::Trace::WriteLine("Candidate: "+ e->Name);
	if (imageFilename[0] == '.') {
		System::Diagnostics::Trace::WriteLine("Hidden file");
		return;
	}
	if( split->Length < 2 ){
		System::Diagnostics::Trace::WriteLine("No extension");
		return;
	}
	if (split[1] != "jpg"){
		System::Diagnostics::Trace::WriteLine("Invalid extension: " + split[1]);
		return;
	}
	System::Diagnostics::Trace::WriteLine("Image Added");
	String ^ dataFilename = imageFilenameToDataFilename(imageFilename);
	processImage(directory + "\\" + imageFilename,directory + "\\" + dataFilename);
}