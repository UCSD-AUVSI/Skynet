#include "RealPlaneDataReceiver.h"
using System::IO::FileSystemEventHandler;
using System::IO::FileSystemEventArgs;
using System::IO::FileSystemWatcher;
using namespace System::IO;
using System::String;

RealPlaneDataReceiver::RealPlaneDataReceiver(String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher):
PlaneDataReceiver(directory, planeWatcher){ run(); }

void RealPlaneDataReceiver::run() {
	System::Diagnostics::Trace::WriteLine("Starting PlaneDataReceiver...");
	FileSystemWatcher ^ watcher = gcnew FileSystemWatcher(directory);
	watcher->Renamed += gcnew RenamedEventHandler(this,&RealPlaneDataReceiver::fileRenamed);
	watcher->Created += gcnew FileSystemEventHandler(this,&RealPlaneDataReceiver::fileAdded);
	watcher->EnableRaisingEvents = true;
}

void RealPlaneDataReceiver::fileRenamed(Object ^ sender, RenamedEventArgs ^ e){
	System::Diagnostics::Trace::WriteLine("Renamed: "+ e->Name);
	String ^ imageFilename = e->Name;
	processFile(imageFilename);
}

void RealPlaneDataReceiver::processFile(String^ imageFilename){
	auto split = imageFilename->Split('.');
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
	String ^ dataFilename = imageFilenameToDataFilename(directory + "\\" + imageFilename);
	processImage(directory + "\\" + imageFilename,dataFilename);
}


void RealPlaneDataReceiver::fileAdded(Object ^ sender, FileSystemEventArgs ^ e){
	System::Diagnostics::Trace::WriteLine("Added: "+ e->Name);
	String ^ imageFilename = e->Name;
	processFile(imageFilename);
}