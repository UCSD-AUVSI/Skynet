#pragma once

using namespace System;

namespace Skynet
{
	ref class SkynetController;
}
namespace Communications
{
	ref class PlaneWatcher;
	ref class UAVCommand;
}
 
using namespace Skynet;
using namespace Communications;

namespace Intelligence
{
	ref class Autosearch;
    public ref class IntelligenceController
    {
	public:
		IntelligenceController(String ^ gpsCoordFilename, SkynetController ^ skynetController);
       
        void queryCameraAndUpdateMapAndImage();
        void setPlaneWatcher(PlaneWatcher ^ planeWatcher);
		void displayAutosearchImage(Drawing::Bitmap^ image);
		void startPathfinder();
		void startPathfinderThread();
		array <UAVCommand ^>^ getUAVCommands();

	private:
		/*void initializeIronPython();*/
        // ScriptEngine ^ pythonEngine;
        Autosearch ^ autosearch;
        SkynetController ^ skynetController;
    };
}