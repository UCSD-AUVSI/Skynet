#pragma once
#include "ImageWithPlaneData.h"

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	ref class PlaneWatcher;
}
 
using namespace Skynet;
using namespace Communications;

namespace Intelligence
{
	ref class Autosearch;

	public ref class IntelligenceController
    {
	public:
		IntelligenceController(array<System::String^>^ fieldBoundaries, SkynetController ^ skynetController,ImageWithPlaneData ^ data);
    void setPlaneWatcher(PlaneWatcher ^ planeWatcher);
		void displayAutosearchImage(System::Drawing::Bitmap^ image);
		SkynetController ^ skynetController;
		Autosearch ^ autosearch;
	private:
		System::Diagnostics::ProcessStartInfo^ getPathfinderProcessInfo();
		System::String^ getPathfinderResult(System::Diagnostics::ProcessStartInfo^ processInfo, GPSCoord planeData, array<System::String^>^ coordinates);
		Autosearch^ createAutosearch(array<System::String^>^ fieldBoundaries);
		void startPathfinderThread(array<System::String^>^ fieldBoundaries);
		void doPathfinding(System::Object^ fieldBoundariesObj);
		void handlePathfinderResult(System::String^ result);
        
    };
}