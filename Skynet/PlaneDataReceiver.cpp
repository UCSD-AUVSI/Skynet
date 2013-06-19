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
using namespace System::Collections::Generic;

#include "../Skynet/PlaneWatcher.h"
#include "ImageAndGPSFiles.h"

using namespace Intelligence;



void PlaneDataReceiver::sendToPlaneWatcher(ImageAndGPSFiles^ files) {
	ImageWithPlaneData^ data = files->toData();
	if (files != nullptr){
		planeWatcher->updateInfo(data);
	}
}

bool PlaneDataReceiver::gotoFrame(int frameIndex){
	if (frameIndex >= frames->Count){
		return false;
	} else {
		this->index = frameIndex;
		sendToPlaneWatcher(frames[frameIndex]);
		return true;
	}
}


PlaneDataReceiver::PlaneDataReceiver(String ^ directory,
							 Communications::PlaneWatcher ^ planeWatcher): 
	directory(directory), planeWatcher(planeWatcher), index(-1),
	frames(gcnew List<Intelligence::ImageAndGPSFiles^>())
	{}

bool PlaneDataReceiver::next(){
	if (index >= frames->Count - 1) {
		return false;
	} else {
		sendToPlaneWatcher(frames[++index]);
		return true;
	}
}

bool PlaneDataReceiver::previous(){
	if (index == 0) {
		return false;
	} else {
		sendToPlaneWatcher(frames[--index]);
		return true;
	}
}

bool PlaneDataReceiver::play(){
	if (isPlaying){
		return false;
	} else { 
		isPlaying = true;
		return true;
	}
}

bool PlaneDataReceiver::stop(){
	if (frames->Count == 0) return false;
	if (!isPlaying && index == 0){
		return false;
	} else { 
		isPlaying = false;
		index = 0;
		sendToPlaneWatcher(frames[0]);
		return true;
	}
}

bool PlaneDataReceiver::pause(){
	if (!isPlaying){
		return false;
	} else { 
		isPlaying = false;
		return true;
	}
}