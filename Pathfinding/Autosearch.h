/* Autosearch Header File */
#pragma once
#define NOT_SEEN 1
using namespace System;

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	ref class PlaneWatcher;
}

namespace Intelligence {
	ref class IntelligenceController;

	ref struct GPSCoord;
	ref struct ArrayCoord;
	public ref class Autosearch
	{
	public:
		array<int>^ map;
		Autosearch(String ^ filename,Intelligence::IntelligenceController^ controller);
		Autosearch(array<String^> ^ coords,Intelligence::IntelligenceController^ controller);
		Autosearch(array<GPSCoord ^> ^coords,Intelligence::IntelligenceController^ controller);
		void updateImage();
		void queryCameraAndUpdateMapAndImage();
		//void saveImage(String^ filename);
		void setPlaneWatcher(Communications::PlaneWatcher ^ planeWatcher);
		void Autosearch::markPolygon(array<GPSCoord ^> ^coords, short value);
		void Autosearch::startUpdaterThread();
		void Autosearch::abortUpdaterThread();
		String^ toString();
		static double area(array<ArrayCoord ^> ^coords);

		bool searchAround(int x, int y, int size);
		GPSCoord^ arrayCoordToGPS(ArrayCoord^ coord);
		int width,height;

	private:
		DateTime ^ lastImageUpdateTime;		
		//Drawing::Bitmap^ image;
		Intelligence::IntelligenceController ^ intelligenceController;
		Communications::PlaneWatcher ^ planeWatcher;

		ArrayCoord^ gpsToArrayCoord(GPSCoord^ coord);
		array<double>^ getBounds(array<GPSCoord ^> ^coords);

		void construct(array<GPSCoord ^> ^coords, Intelligence::IntelligenceController^ controller);
		void connectLines(array<GPSCoord^> ^coords, short value);
		void drawLine(ArrayCoord ^ start, ArrayCoord ^ stop, short  value);
		void fill(int x, int y, short value);
		double bottomBound, leftBound, topBound,rightBound,widthGPS,heightGPS;
		double widthMeters, heightMeters, BORDER;
		bool shouldDie;
		System::Threading::Thread ^ updaterThread;
	};

}
