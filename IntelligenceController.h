#pragma once

using namespace System;

namespace Skynet
{
	ref class SkynetController;
}
namespace Communications
{
	ref class PlaneWatcher;
	ref struct UAVCommand;
	ref struct Waypoint;
}
 
using namespace Skynet;
using namespace Communications;

namespace Intelligence
{
	ref class Autosearch;
    public ref class IntelligenceController
    {
	public:
		IntelligenceController(SkynetController ^ skynetController);
	void restart();
    void setPlaneWatcher(PlaneWatcher ^ planeWatcher);
		void displayAutosearchImage(Drawing::Bitmap^ image);
		void startPathfinder();
		void startPathfinderThread();
		array <Waypoint ^>^ getWaypoints();
		SkynetController ^ skynetController;
		Autosearch ^ autosearch;
        
        
    };
}