/* Autosearch Header File */
#pragma once

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

	public ref class GPSCoord
	{
	public:
		double lon, lat, alt;
	
		GPSCoord(double lat, double lon);
		GPSCoord(double lat, double lon, double alt);

		static double metersToGPS(double meters);
		static double GPSToMeters(double gps);
	};


	public ref class ArrayCoord
	{
	public:
		int x;
		int y;
		ArrayCoord(int x, int y);
		ArrayCoord(ArrayCoord ^other);
		static ArrayCoord^ getCenterCoord(array<ArrayCoord ^> ^ arrayCoords);
	};

	public ref class Autosearch
	{
	public:
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

	private:
		DateTime ^ lastImageUpdateTime;
		int* map;
		int width,height;
		GPSCoord^ arrayCoordToGPS(ArrayCoord^ coord);
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
	

