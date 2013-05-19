#pragma once

namespace Intelligence {

	public ref struct GPSCoord
	{
	public:
		double lon, lat, alt;

		GPSCoord(double lat, double lon);
		GPSCoord(double lat, double lon, double alt);

		static double metersToGPS(double meters);
		static double GPSToMeters(double gps);
	};


	public ref struct ArrayCoord
	{
	public:
		int x;
		int y;
		ArrayCoord(int x, int y);
		ArrayCoord(ArrayCoord ^other);
		static ArrayCoord^ getCenterCoord(array<ArrayCoord ^> ^ arrayCoords);
	};
}