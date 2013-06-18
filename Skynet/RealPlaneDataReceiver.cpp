#include "RealPlaneDataReceiver.h"
#include "ImageAndGPSFiles.h"

using System::IO::FileSystemEventHandler;
using System::IO::FileSystemEventArgs;
using System::IO::FileSystemWatcher;
using System::String;
using System::Predicate;

using namespace System::IO;
using namespace Intelligence;


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

void RealPlaneDataReceiver::processFile(String^ filename){
	auto split = filename->Split('.');
	String^ extension = split[1]->ToLower();
	if (filename[0] == '.') {
		System::Diagnostics::Trace::WriteLine("Hidden file");
		return;
	}
	else if( split->Length < 2 ){
		System::Diagnostics::Trace::WriteLine("No extension");
		return;
	}
	ImageAndGPSFiles^ files = nullptr;
	if (extension == "jpg"){
		files = ImageAndGPSFiles::fromImageFilename(directory + "\\" + filename);
	}
	else if (extension == "txt"){
		files = ImageAndGPSFiles::fromDataFilename(directory + "\\" + filename);
	}
	if (files != nullptr){

		/**
		 * Don't add duplicates
		 */
		for each (ImageAndGPSFiles^ otherFiles in frames){
			if (otherFiles->dataFilename == files->dataFilename &&
				otherFiles->imageFilename == files->imageFilename){
					return;
			}
		}

		frames->Add(files);
		if (isPlaying){
			sendToPlaneWatcher(files);
			index = frames->Count;
		}
	}
}


void RealPlaneDataReceiver::fileAdded(Object ^ sender, FileSystemEventArgs ^ e){
	System::Diagnostics::Trace::WriteLine("Added: "+ e->Name);
	String ^ imageFilename = e->Name;
	processFile(imageFilename);
}