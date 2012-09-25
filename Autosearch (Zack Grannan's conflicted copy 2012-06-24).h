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
		Autosearch(String ^ filename,Skynet::SkynetController^ controller);
		Autosearch(array<GPSCoord ^> ^coords,Skynet::SkynetController^ controller);
		void fill(int x, int y, short value);
		void updateImage();
		void updateState();
		void saveImage(String^ filename);
		void setPlaneWatcher(Communications::PlaneWatcher ^ planeWatcher);
		array<double>^ getBounds(array<GPSCoord ^> ^coords);
		void Autosearch::markPolygon(array<GPSCoord ^> ^coords, short value);
		ArrayCoord^ gpsToArrayCoord(GPSCoord^ coord);
		String^ toString();

	private:
		int* map;
		int width,height;
		GPSCoord^ arrayCoordToGPS(ArrayCoord^ coord);
		Drawing::Bitmap^ image;
		Skynet::SkynetController ^ skynetController;
		Communications::PlaneWatcher ^ planeWatcher;

		void construct(array<GPSCoord ^> ^coords, Skynet::SkynetController^ controller);
		void connectLines(array<GPSCoord^> ^coords, short value);
		void drawLine(ArrayCoord ^ start, ArrayCoord ^ stop, short  value);
		double bottomBound, leftBound, topBound,rightBound,widthGPS,heightGPS;
		double widthMeters, heightMeters, BORDER;
	};

}
	

