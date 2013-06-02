#include "StdAfx.h"
#include "PlaneWatcher.h"
#include "Form1.h"
#include <msclr/lock.h>
#include "GeoReference.h"
#include "PlaneDataReceiver.h"
#include "ImageWithPlaneData.h"
#include "GPSCoord.h"

using namespace Communications;
using namespace Intelligence;
using namespace msclr;
using namespace System::Runtime::InteropServices;
using namespace Intelligence;

#define MAX_INT_32 2147483647

PlaneWatcher::PlaneWatcher(Skynet::SkynetController^ skynetController): skynetController(skynetController), groundLevel(15) {}

void PlaneWatcher::updateInfo (ImageWithPlaneData ^ data) {
	if ( data != nullptr && skynetController != nullptr){
		state = data;
		if(lockedPosition) {
			state->setPosition(lockedPosition);
		}
		data->altitude -= groundLevel;
		skynetController->processPlaneData(data);
	}
}

void PlaneWatcher::setGroundLevel(double groundLevel) {
	this->groundLevel = groundLevel;
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

void PlaneWatcher::lockPosition(GPSCoord^ position)
{
	lockedPosition = position;
}

void PlaneWatcher::unlockPosition()
{
	lockedPosition = nullptr;
}