#include "RealPlaneDataReceiver.h"
#include "ImageAndGPSFiles.h"
#include "SkynetController.h"

using System::IO::FileSystemEventHandler;
using System::IO::FileSystemEventArgs;
using System::IO::FileSystemWatcher;
using System::String;
using System::Predicate;

using namespace System;
using namespace System::IO;
using namespace Intelligence;
using namespace Skynet;


RealPlaneDataReceiver::RealPlaneDataReceiver(SkynetController^ skynetController, String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher):
PlaneDataReceiver(directory, planeWatcher), skynetController(skynetController) { run(); }

void RealPlaneDataReceiver::run() {
	auto files = Linq::Enumerable::Select(IO::Directory::EnumerateFiles(directory, "*.jpg"), gcnew Func<String^, ImageAndGPSFiles^>(&ImageAndGPSFiles::fromImageFilename));
	auto sortedEnumerable = Linq::Enumerable::OrderBy(files,gcnew Func<ImageAndGPSFiles^,UInt64>(&ImageAndGPSFiles::getTimestamp));
	frames = gcnew System::Collections::Generic::List<ImageAndGPSFiles^>(sortedEnumerable);
	System::Diagnostics::Trace::WriteLine("Starting PlaneDataReceiver...");
	FileSystemWatcher ^ watcher = gcnew FileSystemWatcher(directory);
	watcher->Renamed += gcnew RenamedEventHandler(this,&RealPlaneDataReceiver::fileRenamed);
	watcher->Created += gcnew FileSystemEventHandler(this,&RealPlaneDataReceiver::fileAdded);
	watcher->EnableRaisingEvents = true;
	play();
	next();
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
			index = frames->Count - 1;
		}
		skynetController->updateCurrentFrameInUI();
	}
}


void RealPlaneDataReceiver::fileAdded(Object ^ sender, FileSystemEventArgs ^ e){
	System::Diagnostics::Trace::WriteLine("Added: "+ e->Name);
	String ^ imageFilename = e->Name;
	processFile(imageFilename);
}