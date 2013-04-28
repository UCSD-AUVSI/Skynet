#include "StdAfx.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include <msclr/lock.h>
#include "GeoReference.h"
#include "PlaneDataReceiver.h"
#include "ImageWithPlaneData.h"

using namespace Communications;
using namespace msclr;
using namespace System::Runtime::InteropServices;

#define MAX_INT_32 2147483647

PlaneWatcher::PlaneWatcher(Skynet::SkynetController^ skynetController): skynetController(skynetController) {}

void PlaneWatcher::updateInfo (ImageWithPlaneData ^ data) {
	if ( data != nullptr && skynetController != nullptr){
		state = data;
		skynetController->processPlaneData(data);
	}
}


ImageWithPlaneData ^ PlaneWatcher::getState()
{
	return state;
}


double PlaneWatcher::gimbalRollInDegrees()
{
	return state->gimbalRoll;
}


double PlaneWatcher::gimbalPitchInDegrees()
{
	return state->gimbalPitch;
}