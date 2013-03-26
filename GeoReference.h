#pragma once


#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#pragma warning(pop)

ref struct ImageWithPlaneData;

namespace Database {
	ref struct UnverifiedRowData;
	ref struct TelemetryRowData;
	ref struct CandidateRowData;
}

namespace Vision
{
	bool approxEqual(double one, double two);

	// Source code taken from http://www.gpsy.com/gpsinfo/geotoutm/
	class Ellipsoid
	{
	public:
		Ellipsoid(){};
		Ellipsoid(int Id, char* name, double radius, double ecc)
		{
			id = Id; ellipsoidName = name; 
			EquatorialRadius = radius; eccentricitySquared = ecc;
		}

		int id;
		char* ellipsoidName;
		double EquatorialRadius; 
		double eccentricitySquared;  
	};

	public ref class GeoReference {

	public:
		static array<System::String ^>^ latLonToDMS(double lat, double lon);

		static double GPStoMeters(double gpsDegrees);
		static double metersToGPS(double meters);

		static System::String ^ getHeadingString( double angle );
		static double distanceBetweenGPS(double lat1, double lon1, double lat2, double lon2);

		static void getCenterGPSFromState(ImageWithPlaneData^ planeState,
										  double & centerLatitude,
										  double & centerLongitude,
										  double & centeraltitudeMeters);

		static void getCenterGPSFromUnverifiedData(Database::UnverifiedRowData ^ data,
												   double & centerLatitude,
												   double & centerLongitude,double & centeraltitudeMeters );
		static void getCorners(Database::TelemetryRowData ^ data, double & BLLat, double & BLLon, double & BRLat, double & BRLon, double & TRLat, double & TRLon,
			double & TLLat, double & TLLon);
		static void getCorners(ImageWithPlaneData ^ planeState, double & BLLat, double & BLLon, double & BRLat, double & BRLon, double & TRLat, double & TRLon,
			double & TLLat, double & TLLon);
		static void getCorners(Database::UnverifiedRowData ^ data, double & BLLat, double & BLLon, double & BRLat, double & BRLon, double & TRLat, double & TRLon,
			double & TLLat, double & TLLon);

		static cv::Mat EulerAngles(bool transpose, cv::Mat Orig_Vector, double Roll, double Pitch, double Yaw);
		
		static bool getGPS(double plane_latitude, double plane_longitude, double plane_altitudeMeters,
						   double plane_roll, double plane_pitch, double plane_heading,
						   double gimbal_roll, double gimbal_pitch, double gimbal_yaw, 
						   double target_x, double target_y, double zoom,
						   int image_width_pixels, int image_height_pixels,
						   double & Target_Latitude, double & Target_Longitude, double & Target_Height);
		static void getTargetGPS(Database::CandidateRowData ^ data, double & centerLatitude, double & centerLongitude, double & centeraltitudeMeters );
		static void getTargetGPS(Database::UnverifiedRowData ^ data, double & centerLatitude, double & centerLongitude, double & centeraltitudeMeters );
		static void getTargetOrientation(Database::UnverifiedRowData ^ data, double & orientation);
		static void reverseGeoreference(double plane_latitude, double plane_longitude, double plane_altitudeMeters, double plane_roll, double plane_pitch, double plane_heading, 
				double Target_Latitude, double Target_Longitude, double Target_Height, double & gimbal_roll, double & gimbal_pitch);
		static bool forwardGeoreferencing(double plane_latitude, double plane_longitude, double plane_altitudeMeters, double plane_roll, double plane_pitch, double plane_heading, double gimbal_roll, double gimbal_pitch, double gimbal_yaw, 
				double target_x, double target_y, double zoom, const int image_width_pixels, const int image_height_pixels, double & Target_Latitude, double & Target_Longitude, double & Target_Height);

	private:
		static System::String ^ matToString(cv::Mat in);
		static cv::Mat Quaternion(double theta, double X, double Y, double Z);
		static cv::Mat Quaternion_Transform(cv::Mat Orig_Vector, cv::Mat Quat);
		static cv::Mat ECEF_to_NED(cv::Mat ECEF, double Latitude, double Longitude);
		static cv::Mat NED_to_ECEF(cv::Mat NED, double Latitude, double Longitude);
		static cv::Mat ECEF_to_GEO(cv::Mat ECEF, double flatness, double eccentricity, double semi_major_axis);
		static cv::Mat EulerAngles_Plane(cv::Mat Orig_Vector, double Roll, double Pitch, double Yaw);

	};
	
	public ref class GeoReferenceException: public System::Exception {
	public:
		GeoReferenceException(System::String^ reason): System::Exception(reason){};
	};
}

