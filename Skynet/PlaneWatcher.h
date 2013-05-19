#pragma once

#include "MasterHeader.h"

using namespace std;
using namespace System;

namespace Intelligence
{
	ref struct GPSCoord;
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

		void lockPosition(Intelligence::GPSCoord^ coordinate);
		void unlockPosition();
		void updateInfo(ImageWithPlaneData ^ data);
		
		double gimbalRollInDegrees();
		double gimbalPitchInDegrees();

		ImageWithPlaneData^ getState();

	private:
		Intelligence::GPSCoord^ lockedPosition;
		ImageWithPlaneData^ state;
		Skynet::SkynetController ^ const skynetController;
	};

}