#pragma once

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
		IntelligenceController(SkynetController ^ skynetController);
	void restart();
    void setPlaneWatcher(PlaneWatcher ^ planeWatcher);
		void displayAutosearchImage(System::Drawing::Bitmap^ image);
		void startPathfinder();
		void startPathfinderThread();
		SkynetController ^ skynetController;
		Autosearch ^ autosearch;
        
        
    };
}