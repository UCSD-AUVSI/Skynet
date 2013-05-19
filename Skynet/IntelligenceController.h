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

namespace Intelligence
{
	ref class Autosearch;
	ref struct GPSCoord;

	public ref struct IntelligenceResult {
		array<Intelligence::GPSCoord^>^ path;
		System::String^ imageFilename;
		IntelligenceResult(array<Intelligence::GPSCoord^>^ path, System::String^ imageFilename):
			path(path), imageFilename(imageFilename) {}
	};

	public ref class IntelligenceController
    {
	public:
		IntelligenceController(array<Intelligence::GPSCoord^>^ fieldBoundaries, Skynet::SkynetController ^ skynetController);
		void updateAutosearchImage(System::Drawing::Image^ image);
		void processPlaneData(ImageWithPlaneData^ planeData);
		Skynet::SkynetController ^ skynetController;
	private:
		System::Diagnostics::ProcessStartInfo^ getPathfinderProcessInfo();
		System::String^ getPathfinderResult(System::Diagnostics::ProcessStartInfo^ processInfo, Intelligence::GPSCoord^ planeData, array<Intelligence::GPSCoord^>^ coordinates);
		void startPathfinderThread(array<Intelligence::GPSCoord^>^ fieldBoundaries);
		void doPathfinding(System::Object^ fieldBoundariesObj);
		void handlePathfinderResult(System::String^ result);
		array<Intelligence::GPSCoord^>^ parsePathfinderResultString(System::String^ result);
        
		Intelligence::Autosearch ^ autosearch;
    };
}