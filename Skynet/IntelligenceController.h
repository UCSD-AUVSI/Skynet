#pragma once

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	ref class PlaneWatcher;
}

ref struct ImageWithPlaneData;
 
using namespace Skynet;
using namespace Communications;

namespace Intelligence
{
	ref class Autosearch;
	ref struct GPSCoord;

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
		System::String^ getPathfinderResult(System::Diagnostics::ProcessStartInfo^ processInfo, Intelligence::GPSCoord^ planeData, array<System::String^>^ coordinates);
		Autosearch^ createAutosearch(array<System::String^>^ fieldBoundaries);
		void startPathfinderThread(array<System::String^>^ fieldBoundaries);
		void doPathfinding(System::Object^ fieldBoundariesObj);
		void handlePathfinderResult(System::String^ result);
        
    };
}