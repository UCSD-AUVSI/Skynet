#include "ImageAndGPSFiles.h"
#include "ImageWithPlaneData.h"

using namespace System;
using namespace Intelligence;

ImageAndGPSFiles^ ImageAndGPSFiles::fromDataFilename(String^ dataFilename){
	String^ imageFilename = dataFilename->Replace("jpg","txt");
	return gcnew ImageAndGPSFiles(dataFilename, imageFilename);
}

ImageAndGPSFiles^ ImageAndGPSFiles::fromImageFilename(String^ imageFilename){
	String^ dataFilename = dataFilename->Replace("txt","jpg");
	return gcnew ImageAndGPSFiles(dataFilename, imageFilename);
}

ImageAndGPSFiles::ImageAndGPSFiles(String^ dataFilename, String^ imageFilename):
	dataFilename(dataFilename), imageFilename(imageFilename) {}
ImageWithPlaneData^ ImageAndGPSFiles::toData(){
	String^ data = IO::File::ReadAllLines(dataFilename)[0];
	return gcnew ImageWithPlaneData(imageFilename, data);
}
String ^ ImageAndGPSFiles::extractFilename(String^ path){
	auto segments = path->Split('\\');
	return segments[segments->Length-1];
}

UInt64 ImageAndGPSFiles::getTimestamp() {
	return filenameToTimestamp(dataFilename);
}

UInt64 ImageAndGPSFiles::filenameToTimestamp(String ^ filename) {
	String ^ namepart = filename->Split('.')[0];
	try {
		return Convert::ToUInt64(namepart);
	} catch (System::FormatException^) {
		return 0;
	}
}

String ^ ImageAndGPSFiles::imageFilenameToDataFilename(String ^ imageFilename) {
	return imageFilename->Split('.')[0] + ".txt";
}

String ^ ImageAndGPSFiles::dataFilenameToImageFilename(String ^ imageFilename) {
	return imageFilename->Split('.')[0] + ".txt";
}