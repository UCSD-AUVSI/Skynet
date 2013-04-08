#pragma once

#include "MasterHeader.h"

using namespace std;
using namespace System;

#define NUM_GPS_DATA 100
#define NUM_TELEM_DATA 100
#define NUM_GIMBAL_DATA 100

#define CAMERA_LATENCY 125

namespace Intelligence
{
	ref class Autosearch;
	ref class IntelligenceController;
}

ref struct ImageWithPlaneData;

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	public ref class PlaneWatcher {
	public:
		PlaneWatcher(Skynet::SkynetController ^ skynetController);

		void updateInfo(ImageWithPlaneData ^ data);
		
		double gimbalRollInDegrees();
		double gimbalPitchInDegrees();

		ImageWithPlaneData^ getState();

	private:
		ImageWithPlaneData^ state;
		Skynet::SkynetController ^ const skynetController;
	};

}