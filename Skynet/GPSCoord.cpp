#include "GPSCoord.h"

namespace Intelligence {
	/**
	 * Constructor for a GPS coordinate. No altitude is given, so it is set to zero.
	 *
	 * @param lat	The latitude (degrees)
	 * @param lon	The longitude (degrees)
	 */
	GPSCoord::GPSCoord(double lat, double lon)
	{
		this->lat = lat;
		this->lon = lon;
	}

	/**
	 * Constructor for a GPS coordinate.
	 *
	 * @param lat	The latitude (degrees)
	 * @param lon	The longitude (degrees)
	 * @param alt	The altitude (feet)
	 */
	GPSCoord::GPSCoord(double lat, double lon, double alt)
	{
		this->lat = lat;
		this->lon = lon;
		this->alt = alt;
	}

	/**
	 * Determines the displacement in meters that corresponds to a displacement
	 * in GPS degrees.
	 * TODO: Make this more accurate.
	 * 
	 * @param gps	Displacement in gps degrees
	 * @return		Corresponding displacement in meters
	 */
	double GPSCoord::GPSToMeters(double gps)
	{
		return gps * 111122.0;
	}

	/**
	 * Determines the displacement in GPS degrees that corresponds to a displacement
	 * in meters.
	 * TODO: Make this more accurate.
	 * 
	 * @param meters Displacement in meters
	 * @return		 Corresponding displacement in GPS degrees.
	 */
	double GPSCoord::metersToGPS(double meters)
	{
		return meters / 111122.0;
	}
}