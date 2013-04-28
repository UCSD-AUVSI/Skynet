
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

#include "../Skynet/PlaneWatcher.h"

void MarshalString ( String ^ s, std::string& os ) {
   using namespace Runtime::InteropServices;
   const char* chars = 
      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
   os = chars;
   Marshal::FreeHGlobal(IntPtr((void*)chars));
}

ImageWithPlaneData::ImageWithPlaneData(ImageWithPlaneData^ other) :
	imageFilename(other->imageFilename),
	image(other->image),
	latitude(other->latitude),
	longitude(other->longitude),
	altitude(other->altitude),
	roll(other->roll),
	pitch(other->pitch),
	yaw(other->yaw),
	gimbalRoll(other->gimbalRoll),
	gimbalPitch(other->gimbalPitch),
	xVelocity(other->xVelocity),
	yVelocity(other->yVelocity),
	zVelocity(other->zVelocity) {}

ImageWithPlaneData::ImageWithPlaneData(String ^ imageFilename, String ^data) {
	std::string imageFilenameStd;
	MarshalString(imageFilename,imageFilenameStd);
	this->imageFilename = imageFilename;
	image = new Mat(cv::imread(imageFilenameStd));
	image->addref();
	auto vars = data->Split(' ');
	latitude = Convert::ToDouble(vars[0]) / 1000000.0;
	longitude = Convert::ToDouble(vars[1]) / 1000000.0;
	altitude = Convert::ToDouble(vars[2]);
	roll = Convert::ToDouble(vars[3]);
	pitch = Convert::ToDouble(vars[4]);
	yaw = Convert::ToDouble(vars[5]);
	gimbalRoll = Convert::ToDouble(vars[6]);
	gimbalPitch = Convert::ToDouble(vars[7]);
	xVelocity = Convert::ToDouble(vars[8]) / 1000;
	yVelocity = Convert::ToDouble(vars[9]) / 1000;
	zVelocity = Convert::ToDouble(vars[10]) / 1000;
}

String ^ PlaneDataReceiver::extractFilename(String^ path){
	auto segments = path->Split('\\');
	return segments[segments->Length-1];
}
Int64 PlaneDataReceiver::filenameToTimestamp(String ^ filename) {
	String ^ namepart = filename->Split('.')[0];
	return Convert::ToInt64(namepart);
}

String ^ PlaneDataReceiver::imageFilenameToDataFilename(String ^ imageFilename) {
	return imageFilename->Split('.')[0] + ".txt";
}


void PlaneDataReceiver::processImage(String ^ imagePath, String ^ dataPath) {
	System::Diagnostics::Trace::WriteLine("Image path: " + imagePath + "\nData path: " + dataPath);
	for(int attempts = 0; attempts < MAX_ATTEMPTS; attempts ++){
		try {
			String ^ imageData = File::ReadAllLines(dataPath)[0];
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