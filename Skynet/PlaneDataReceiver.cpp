
// ImageDownloader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "PlaneDataReceiver.h"
#include "ImageWithPlaneData.h"
#include <cv.h>
#include <highgui.h>



#using<System.dll>

#define RETRY_MS 1000
#define MAX_ATTEMPTS 5

using cv::Mat;
using System::String;
using System::Int64;
using System::Console;
using System::IO::File;
using System::Threading::Thread;
using System::IO::FileNotFoundException;
using System::Threading::ThreadStart;
using System::DateTime;

#include "../Skynet/PlaneWatcher.h"

void MarshalString ( String ^ s, std::string& os ) {
   using namespace Runtime::InteropServices;
   const char* chars = 
      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
   os = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));
}

String ^ PlaneDataReceiver::extractFilename(String^ path){
	auto segments = path->Split('\\');
	return segments[segments->Length-1];
}
UInt64 PlaneDataReceiver::filenameToTimestamp(String ^ filename) {
	String ^ namepart = filename->Split('.')[0];
	try {
		long long tick = DateTime::MaxValue.Ticks;

		return (Convert::ToUInt64(namepart) * 10) % tick;
	} catch (System::FormatException^) {
		return 0;
	}
}

String ^ PlaneDataReceiver::imageFilenameToDataFilename(String ^ imageFilename) {
	return imageFilename->Split('.')[0] + ".txt";
}


void PlaneDataReceiver::processImage(String ^ imagePath, String ^ dataPath) {
	System::Diagnostics::Trace::WriteLine("Image path: " + imagePath + "\nData path: " + dataPath);
	for(int attempts = 0; attempts < MAX_ATTEMPTS; attempts ++){
		try {
			array<String ^>^ lines = File::ReadAllLines(dataPath);
			if (lines->Length == 0){
				System::Diagnostics::Trace::WriteLine("Data file was empty, not analyzing " + imagePath);
				return;
			}
			String^ imageData = lines[0];
			ImageWithPlaneData ^ data = gcnew ImageWithPlaneData(imagePath,imageData);
			if ( planeWatcher == nullptr ){
				System::Diagnostics::Trace::WriteLine("PlaneWatcher is null");
			}
			planeWatcher->updateInfo(data);
			return;
		} catch (FileNotFoundException ^) {
			System::Diagnostics::Trace::WriteLine("Could not load " + dataPath + ". Sleeping for " + RETRY_MS + " ms");
			Thread::Sleep(RETRY_MS);
		}
	}
	System::Diagnostics::Trace::WriteLine("Too many failed attempts. Giving up");
}


PlaneDataReceiver::PlaneDataReceiver(String ^ directory,
							 Communications::PlaneWatcher ^ planeWatcher): 
	directory(directory), planeWatcher(planeWatcher){}