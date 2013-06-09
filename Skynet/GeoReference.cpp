#include "StdAfx.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#pragma warning(push)
#pragma warning(disable:4996 4267)
#include <cv.h>
#pragma warning(pop)
#include "GeoReference.h"
#include "MasterHeader.h"
#include "PlaneWatcher.h"
#include "DatabaseStructures.h"
#include "ImageWithPlaneData.h"

using namespace System;
using namespace Vision;
using namespace Communications;
using namespace Database;
using System::Math;


#define GIMBAL_YAW		0.0
#define X_PIXELS		5184
#define Y_PIXELS		3456
#define CAMERA_X_FOV_DEGREES 36
#define CAMERA_Y_FOV_DEGREES 24


String ^ GeoReference::matToString(cv::Mat in)
{
	String ^ ret = "{";
	typedef cv::Vec<double, 1> VT;

	for (int r = 0; r < in.rows; r++)
	{
		ret += "{";
		for (int c = 0; c < in.cols; c++)
		{
			ret += in.at<VT>(r, c)[0];
		}
		if (r+1 == in.rows)
			ret += "}";
		else
			ret += "}, ";
	}

	ret += "}";
	return ret;

}
/*
 * Determines if two numbers are very close to each other
 */
inline bool veryNearlyEqual(double one, double two){
	return (fabs(one-two) < 0.000001);
}

bool Vision::approxEqual(double one, double two)
{
	if (fabs(one - two) > 0.001)
		return false;
	else 
		return true;
}

/**
	Converts a number in meters to the equivalent number of GPS degrees
*/
double GeoReference::metersToGPS(double meters){
	return meters / 111122;
}

/**
	Converts a number of GPS degrees to the equivalent number of meters
*/
double GeoReference::GPStoMeters(double gpsDegrees){
	return gpsDegrees * 111122;
}


double cosd(double input)
{
	return cos(input*PI/180.0);
}

double sind(double input)
{
	return sin(input*PI/180.0);
}

double atand(double input)
{
	return atan(input)*180.0/PI;
}

double GeoReference::distanceBetweenGPS(double lat1, double lon1, double lat2, double lon2)
{
	double radius = 6378000; // radius of earth!
	double deltaLat = lat2 - lat1;
	double deltaLon = lon2 - lon1;
	double a = sind(deltaLat/2)*sind(deltaLat*2) + cosd(lat1)*cosd(lat2)*sind(deltaLon/2)*sind(deltaLon*2);
	double c = 2*atan2(sqrt(a), sqrt((1-a)));
	double d = radius*c;

	return d;
}
cv::Mat GeoReference::Quaternion(double theta, double X, double Y, double Z)
{
	double mag = sqrt(X*X + Y*Y + Z*Z);
	double x = X/mag;
	double y = Y/mag;
	double z = Z/mag;

	double quatarr[4] = {cos(theta/2.0), x * sin(theta/2.0),  y * sin(theta/2.0),  z * sin(theta/2.0) };
	cv::Mat Quat(1, 4, CV_64FC1, quatarr );

	double magnitude = 0.0;
	for (int i = 0; i < 4; i++) 
		magnitude += Quat.at<double>(0,i)*Quat.at<double>(0,i);

	magnitude = sqrt(magnitude);

	for (int i = 0; i < 4; i++) 
		Quat.at<double>(0,i) = Quat.at<double>(0,i)/magnitude;

	return Quat.t();


}

cv::Mat GeoReference::Quaternion_Transform(cv::Mat Orig_Vector, cv::Mat Quat)
{
	cv::Mat Q = Quat;
	double o = Q.at<double>(0,0);
	double x = Q.at<double>(1,0);
	double y = Q.at<double>(2,0);
	double z = Q.at<double>(3,0);

	double transferarr[9] = {x*x + o*o - y*y - z*z, 2.0*(x*y + z*o), 2.0*(x*z-y*o),
							2.0*(x*y-z*o), y*y + o*o - x*x - z*z, 2.0*(y*z + x*o),
							2.0*(x*z+y*o), 2.0*(y*z-x*o), z*z + o*o - x*x - y*y};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transferarr).t();

	return Transfer*Orig_Vector;

}

cv::Mat GeoReference::ECEF_to_NED(cv::Mat ECEF, double Latitude, double Longitude)
{
	double sinlat = sin(Latitude);
	double coslat = cos(Latitude);
	double sinlon = sin(Longitude);
	double coslon = cos(Longitude);

	double transferarr[9] = {-sinlat*coslon, -sinlat*sinlon, coslat,
							-sinlon,		coslon,			 0.0,
							-coslat*coslon, -coslat*sinlon, -sinlat};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transferarr);

	return Transfer*ECEF;

}

cv::Mat GeoReference::NED_to_ECEF(cv::Mat NED, double Latitude, double Longitude)
{
	double sinlat = sin(Latitude);
	double coslat = cos(Latitude);
	double sinlon = sin(Longitude);
	double coslon = cos(Longitude);

	double transferarr[9] = {-sinlat*coslon, -sinlon, -coslat*coslon,
							-sinlat*sinlon, coslon, -coslat*sinlon,
							coslat,			0.0,	-sinlat};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transferarr);

	return Transfer*NED;
}

cv::Mat GeoReference::ECEF_to_GEO(cv::Mat ECEF, double flatness, double eccentricity, double semi_major_axis)
{


	double X = ECEF.at<double>(0,0);
	double Y = ECEF.at<double>(1,0);
	double Z = ECEF.at<double>(2,0);

	double f = flatness;
	double e = eccentricity;
	double e2 = e*e;
	double a = semi_major_axis;

	double h = 0.0;
	double N = a;
	double p = sqrt(X*X + Y*Y);
	double lon = atan(Y/X);
	double lat = 0.0;
	double delta = 1;
	double oldlat = 0;

	for (int i = 0; i < 15 && delta > 0.00000000001; i++)
	{
		double sin_lat = Z / (N*(1.0 - e2) + h);
		lat = atan((Z + e2*N*sin_lat) / p);
		N = a / sqrt(1.0 - e2*sin(lat)*sin(lat));
		h = p / cos(lat) - N;

		delta = abs(lat-oldlat); oldlat = lat;
		//PRINT("lat:"+lat+" N:"+N+" h:"+h+" delta:"+delta);
	}


	cv::Mat New_Vector(3, 1, CV_64FC1 );
	New_Vector.at<double>(0,0) = lat;
	New_Vector.at<double>(1,0) = lon;
	New_Vector.at<double>(2,0) = N+h;
	
	
	return New_Vector;
}

cv::Mat GeoReference::EulerAngles_Plane(cv::Mat Orig_Vector, double Roll, double Pitch, double Yaw)
{
	double R = Roll;
	double P = Pitch;
	double Y = Yaw;

	double rollarr[9] = {1,0,0,
						0, cos(R), sin(R),
						0, -sin(R), cos(R)};
	cv::Mat Roll_Transform(3, 3, CV_64FC1, rollarr );

	double pitcharr[9] = {cos(P), 0, -sin(P),
						0, 1, 0,
						sin(P), 0, cos(P)};
	cv::Mat Pitch_Transform(3, 3, CV_64FC1, pitcharr );

	double yawarr[9] = {cos(Y), sin(Y), 0,
						-sin(Y), cos(Y), 0,
						0, 0, 1};
	cv::Mat Yaw_Transform(3, 3, CV_64FC1, yawarr );


	cv::Mat Transfer_t(3,3,CV_64FC1);
	cv::Mat Transfer = Roll_Transform * Pitch_Transform * Yaw_Transform;
	cv::transpose(Transfer, Transfer_t);

	//PRINT(matToString((Transfer*Orig_Vector)));
	return Transfer_t*Orig_Vector;
}

// GPS in degrees
// plane and gimbal orientation in radians
// target_x/y in pixels, where 0,0 is center
bool GeoReference::forwardGeoreferencing(double plane_latitude, double plane_longitude, double plane_altitude, double plane_roll, double plane_pitch, double plane_heading, double gimbal_roll, double gimbal_pitch, double gimbal_yaw, 
				double target_x, double target_y, double zoom, const int image_width, const int image_height, double & Target_Latitude, double & Target_Longitude, double & Target_Height)
{
	/////////////////////
	// Funcs used by forwardGeoreferencing:
	//	EulerAngles_Plane(), Mat Quaternion(double, double, double, double), Mat Quaternion_Transform(Mat, Mat), 
	//	Mat NED_to_ECEF(Mat, double, double), Mat ECEF_to_GEO(Mat, double, double, double)
	/////////////////////
	

	double x_fov = CAMERA_X_FOV_DEGREES * PI_TO_RAD;
	double y_fov = CAMERA_Y_FOV_DEGREES * PI_TO_RAD;
	double x_pixels = image_width;
	double y_pixels = image_height;
	double a = 6378137;
	double b = 6356752.3142;
	double ground_altitude = 0.0;


	double pixel_x = target_x;// + x_pixels/2.0;
	double pixel_y = target_y;// + y_pixels/2.0;
	double zoom_factor = zoom;

	
	plane_latitude = plane_latitude * PI_TO_RAD;
	plane_longitude = plane_longitude * PI_TO_RAD;

	


	/////////////// Part A /////////////////////
	double N_vec_arr[3] = {1,0,0};
	double E_vec_arr[3] = {0,1,0};
	double D_vec_arr[3] = {0,0,1};
	cv::Mat Plane_N_vector = cv::Mat(3, 1, CV_64FC1, N_vec_arr);
	cv::Mat Plane_E_vector = cv::Mat(3, 1, CV_64FC1, E_vec_arr);
	cv::Mat Plane_D_vector = cv::Mat(3, 1, CV_64FC1, D_vec_arr);

	cv::Mat Camera_Point_Vector = EulerAngles_Plane(Plane_D_vector, plane_roll, plane_pitch, plane_heading);
	cv::Mat Camera_Up_Vector = EulerAngles_Plane(Plane_N_vector, plane_roll, plane_pitch, plane_heading);



	/////////////// Part B /////////////////////
	cv::Mat Q_gimbal_roll = Quaternion(gimbal_roll, Camera_Up_Vector.at<double>(0,0),  Camera_Up_Vector.at<double>(1,0),  Camera_Up_Vector.at<double>(2,0));
	Camera_Point_Vector = Quaternion_Transform(Camera_Point_Vector, Q_gimbal_roll);
	Camera_Up_Vector = Quaternion_Transform(Camera_Up_Vector, Q_gimbal_roll);

	cv::Mat axis = Camera_Point_Vector.cross(Camera_Up_Vector); 
	cv::Mat Q_gimbal_pitch = Quaternion(gimbal_pitch, axis.at<double>(0,0), axis.at<double>(1,0), axis.at<double>(2,0));
	Camera_Point_Vector = Quaternion_Transform(Camera_Point_Vector, Q_gimbal_pitch);
	Camera_Up_Vector = Quaternion_Transform(Camera_Up_Vector, Q_gimbal_pitch);

	/////////////// Part C /////////////////////
	double fovarr[3] = {x_fov, y_fov, 1};
	cv::Mat FOV(3, 1, CV_64FC1, fovarr );
	double scalearr[9] = {1/zoom_factor, 0, 0, 0, 1/zoom_factor, 0, 0, 0, 1};
	cv::Mat Scale(3, 3, CV_64FC1, scalearr );

	cv::Mat FOV_zoom_accounted = Scale*FOV;

	/////////////// Part D /////////////////////

	cv::Mat c_p = Camera_Point_Vector;
	cv::Mat c_u = Camera_Up_Vector;
	cv::Mat c_s = c_p.cross(c_u);


	double max_w = tan(FOV_zoom_accounted.at<double>(0,0)/2)*2;
	double max_h = tan(FOV_zoom_accounted.at<double>(1,0)/2)*2;

	double w = max_w * (0.5 - pixel_x/(x_pixels - 1));
	double h = max_h * (pixel_y/(y_pixels - 1) - 0.5);

	cv::Mat c_f = c_p - (w * c_s) + (h * c_u);
	cv::Mat Pixel_Point_Vector = c_f;


	/////////////// Part E  /////////////////////
	double f = a/(a-b);
	double e=sqrt((1/f)*(2-(1/f)));
	double N=a/(sqrt(1-e*e*sin(plane_latitude)*sin(plane_latitude)));

	double Plane_XYZ_arr[3] = {0,0,0};
	Plane_XYZ_arr[0] = (N+plane_altitude)*cos(plane_latitude)*cos(plane_longitude);
	Plane_XYZ_arr[1] = (N+plane_altitude)*cos(plane_latitude)*sin(plane_longitude);
	Plane_XYZ_arr[2] = (N*(1-e*e)+plane_altitude)*sin(plane_latitude);
	cv::Mat Plane_XYZ(3, 1, CV_64FC1, Plane_XYZ_arr );

	/////////////// Part F  /////////////////////
	
	double temparr[3] = {0, 0, plane_altitude + ground_altitude};
	cv::Mat temp(3, 1, CV_64FC1, temparr );

	double Ground_XYZ_arr[3] = {0,0,0}; // TODO: fix "plane_latitude"
	Ground_XYZ_arr[0] = (N)*cos(plane_latitude)*cos(plane_longitude);
	Ground_XYZ_arr[1] = (N)*cos(plane_latitude)*sin(plane_longitude);
	Ground_XYZ_arr[2] = (N*(1-e*e))*sin(plane_latitude);
	cv::Mat Ground_XYZ(3, 1, CV_64FC1, Ground_XYZ_arr );

	cv::Mat Pixel_XYZ = NED_to_ECEF(Pixel_Point_Vector, plane_latitude, plane_longitude);
	
	double dist = Plane_XYZ.dot(Ground_XYZ - Plane_XYZ) / Pixel_XYZ.dot(Plane_XYZ);

	cv::Mat Target_XYZ = Plane_XYZ + Pixel_XYZ*dist;
	
	cv::Mat Target_GEO = ECEF_to_GEO(Target_XYZ, f, e, a);
	
	Target_Latitude = Target_GEO.at<double>(0,0) * 180.0 / PI;
	Target_Longitude = Target_GEO.at<double>(1,0) * 180.0 / PI - 180.0;
	Target_Height = Target_GEO.at<double>(2,0) - N;
	
	/////////////// Error Checking /////////////////////
	bool success = true;
	if (abs(Target_Latitude) > 180.0 || approxEqual(Target_Latitude, 0.0) || approxEqual(abs(Target_Latitude), 180.0))
	{
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}

	if (abs(Target_Longitude) > 180.0 || approxEqual(Target_Longitude, 0.0) || approxEqual(abs(Target_Longitude), 180.0))
	{
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}

	if (Pixel_Point_Vector.at<double>(2,0) < 0.0)
	{
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}

	return success;
}

cv::Mat GeoReference::EulerAngles(bool transpose, cv::Mat Orig_Vector, double Roll, double Pitch, double Yaw)
{
	double transarr[9] = {1,1,1,
							1,1,1,  
							1,1,1};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transarr).inv();

	return Transfer;


}

void GeoReference::reverseGeoreference(double plane_latitude, double plane_longitude, double plane_altitude, double plane_roll, double plane_pitch, double plane_heading, 
				double Target_Latitude, double Target_Longitude, double Target_Height, double & gimbal_roll, double & gimbal_pitch)
{
	typedef cv::Vec<double, 1> VT;

	// Step 1: Moving Plane and Target LAt/Lon into ECEF
	double Plane_yaw = plane_heading;
	double G_lat = 32.0;
	double G_long = -117.0;

	double a = 6378137.0;
	double b = 6356752.3142;
	double f = a/(a-b);
	double e = sqrt((1/f)*(2.0-(1.0/f)));
	double N_P = a/(sqrt(1.0-(e*e)*sind(plane_latitude)*sind(plane_latitude)));
	double N_T = a/(sqrt(1.0-(e*e)*sind(Target_Latitude)*sind(Target_Latitude)));

	double X_P = (N_P+plane_altitude)*cosd(plane_latitude)*cosd(plane_longitude);
	double Y_P = (N_P+plane_altitude)*cosd(plane_latitude)*sind(plane_longitude);
	double Z_P = (N_P*(1-e*e)+plane_altitude)*sind(plane_latitude);
	
	double X_T = (N_T+Target_Height)*cosd(Target_Latitude)*cosd(Target_Longitude);
	double Y_T = (N_T+Target_Height)*cosd(Target_Latitude)*sind(Target_Longitude);
	double Z_T = (N_T*(1-e*e)+Target_Height)*sind(Target_Latitude);

	// Step 2: Form Positions, find vector that connects the two points 
	double xyzparr[3] = {X_P, Y_P, Z_P};
	cv::Mat XYZP(3, 1, CV_64FC1, xyzparr );
	
	double xyztarr[3] = {X_T, Y_T, Z_T};
	cv::Mat XYZT(3, 1, CV_64FC1, xyzparr );

	double transGarr[9] = {	-sind(G_lat)*cosd(G_long),		-sind(G_lat)*sind(G_long),	cosd(G_lat), 
								-sind(G_long),					cosd(G_long),				0.0, 
								-cosd(G_lat)*cosd(G_long),		-cosd(G_lat)*sind(G_long),	-sind(G_lat)	};
	cv::Mat transG(3, 3, CV_64FC1, transGarr );

	cv::Mat NEDP = transG*XYZP;
	cv::Mat NEDT = transG*XYZT;

	cv::Mat NED_TRUE = NEDT-NEDP;


	// Step 3:  find NED plane

	cv::Mat NED_PLANE = EulerAngles(true, NED_TRUE, plane_roll, plane_pitch, Plane_yaw);
	
	double up = NED_PLANE.at<VT>(0, 0)[0];
	double vp = NED_PLANE.at<VT>(1, 0)[0];
	double wp = NED_PLANE.at<VT>(2, 0)[0];

	// Step 4: Find Gimbal Angles

	gimbal_roll = -atand(vp/wp);
	gimbal_pitch = -atand((-up)/(sind(gimbal_roll)*vp+cosd(gimbal_roll)*wp));

}

String ^ GeoReference::getHeadingString( double angle )
{
	String ^ retVal;

	if (angle > 180.0)
		angle -= 180.0;
	if (angle < -180.0)
		angle += 180.0;

	if( angle > -22.5 && angle <= 22.5 )
		retVal = "N";
	else if( angle > 22.5 && angle <= 67.5 )
		retVal = "NE";
	else if( angle > 67.5 && angle <= 112.5 )
		retVal = "E";
	else if( angle > 112.5 && angle <= 157.5 )
		retVal = "SE";
	else if( angle > 157.5 || angle < -157.5 )
		retVal = "S";
	else if( angle < -112.5 && angle >= -157.5 )
		retVal = "SW";
	else if( angle < -65.7 && angle >= -112.5 )
		retVal = "W";
	else
		retVal = "NW";

	return retVal;
}

void GeoReference::getTargetOrientation(Database::UnverifiedRowData ^ data, double & orientation)
{
	double centerlat, centerlon, centeralt;
	GeoReference::getTargetGPS(data, centerlat, centerlon, centeralt);

	double toplat, toplon, topalt;

	TelemetryRowData ^ telemetry = data->candidate->telemetry;

	getGPS(	telemetry->planeLocation->lat,
			telemetry->planeLocation->lon,
			telemetry->planeLocation->alt,
			telemetry->planeRoll,
			telemetry->planePitch,
			telemetry->planeHeading,
			telemetry->gimbalRoll,
			telemetry->gimbalPitch,
			telemetry->gimbalYaw,
			data->description->targetX,
			data->description->targetY,
			telemetry->gimbalZoom,
			X_PIXELS, // TODO: Fix this
			Y_PIXELS,
			toplat,
			toplon,
			topalt);


	// TODO: Tim's cals
	double lat_1 = centerlat;
	double lon_1 = centerlon;
	double alt_1 = centeralt;

	double lat_2 = toplat;
	double lon_2 = toplon;
	double alt_2 = topalt;

	lat_1 = lat_1 * PI/180.0;
	lon_1 = lon_1 * PI/180.0;
	lat_2 = lat_2 * PI/180.0;
	lon_2 = lon_2 * PI/180.0;

	double a = 6378137.0;
	double b = 6356752.3142;
	double f = a/(a-b);
	double e = sqrt((1.0/f)*(2.0-(1.0/f)));
	double N=a/(sqrt(1-(e*e)*sin(lat_1)*sin(lat_1)));

	double target1xyz_arr[3] = {0,0,0};
	target1xyz_arr[0] = (N+alt_1)*cos(lat_1)*cos(lon_1);
	target1xyz_arr[1] = (N+alt_1)*cos(lat_1)*sin(lon_1);
	target1xyz_arr[2] = (N*(1-e*e)+alt_1)*sin(lat_1);
	cv::Mat target_1_XYZ(3, 1, CV_64FC1, target1xyz_arr );

	double target2xyz_arr[3] = {0,0,0};
	target2xyz_arr[0] = (N+alt_2)*cos(lat_2)*cos(lon_2);
	target2xyz_arr[1] = (N+alt_2)*cos(lat_2)*sin(lon_2);
	target2xyz_arr[2] = (N*(1-e*e)+alt_2)*sin(lat_2);
	cv::Mat target_2_XYZ(3, 1, CV_64FC1, target2xyz_arr );

	cv::Mat point_vector_XYZ = target_2_XYZ - target_1_XYZ;

	cv::Mat point_vector_NED = ECEF_to_NED(point_vector_XYZ, lat_1, lon_1);

	point_vector_NED.at<double>(2,0) = 0;
	point_vector_NED = point_vector_NED / norm(point_vector_NED);

	double headingAngleRadians = atan2(point_vector_NED.at<double>(1,0), point_vector_NED.at<double>(0,0));
	double headingAngleDeg = headingAngleRadians * 180/PI;

	orientation = headingAngleDeg;
}

array<String ^>^ GeoReference::latLonToDMS (double lat, double lon){
	String ^ Lat = "";
	String ^ Lon = "";

	if (lat < 0.0)
		Lat += "S";
	else
		Lat += "N";

	if (lon < 0.0)
		Lon += "W";
	else
		Lon += "E";

	Lat += Int32(lat).ToString("00");
	Lon += Int32(lon).ToString("000");

	lat = lat - (double)(int)lat;
	lon = lon - (double)(int)lon;

	Lat += " ";
	Lon += " ";

	lat = lat*60;
	lon = lon*60;

	Lat += Int32(lat).ToString("00");
	Lon += Int32(lon).ToString("00");
	
	lat = lat - (double)(int)lat;
	lon = lon - (double)(int)lon;

	Lat += " ";
	Lon += " ";
	
	lat = lat*60;
	lon = lon*60;
	
	Lat += Double(lat).ToString("00.000");
	Lon += Double(lon).ToString("00.000");
	
	return gcnew array<String^>{Lat,Lon};
}

void GeoReference::getTargetGPS(Database::CandidateRowData ^ data, double & centerLatitude, double & centerLongitude, double & centerAltitude )
{
	if (data == nullptr) {
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}

	TelemetryRowData ^ telemetry = data->telemetry;

	getGPS(	telemetry->planeLocation->lat,
			telemetry->planeLocation->lon,
			telemetry->planeLocation->alt,
			telemetry->planeRoll,
			telemetry->planePitch,
			telemetry->planeHeading,
			telemetry->gimbalRoll,
			telemetry->gimbalPitch,
			telemetry->gimbalYaw,
			telemetry->originX + telemetry->widthPixels/2 - X_PIXELS/2,
			telemetry->originY + telemetry->heightPixels/2 - Y_PIXELS/2,
			X_PIXELS,
			Y_PIXELS,
			telemetry->gimbalZoom,
			centerLatitude,
			centerLongitude,
			centerAltitude);
}

void GeoReference::getTargetGPS(Database::UnverifiedRowData ^ data, double & centerLatitude, double & centerLongitude, double & centerAltitude )
{
	if (data == nullptr) {
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}
	TelemetryRowData ^ telemetry = data->candidate->telemetry;

	getGPS(	telemetry->planeLocation->lat,
			telemetry->planeLocation->lon,
			telemetry->planeLocation->alt,
			telemetry->planeRoll,
			telemetry->planePitch,
			telemetry->planeHeading,
			telemetry->gimbalRoll,
			telemetry->gimbalPitch,
			telemetry->gimbalYaw,
			telemetry->originX + data->description->targetX - X_PIXELS / 2,
			telemetry->originY + data->description->targetY - Y_PIXELS / 2,
			telemetry->gimbalZoom,
			X_PIXELS,
			Y_PIXELS,
			centerLatitude,
			centerLongitude,
			centerAltitude);
}


void GeoReference::getCenterGPSFromState(ImageWithPlaneData ^ planeState, double & centerLatitude, double & centerLongitude, double & centerAltitude )
{
	if (planeState == nullptr) {
		throw gcnew GeoReferenceException("Georeferencing Error.");
	}

	getGPS(planeState->latitude, planeState->longitude, planeState->altitude, (planeState->roll), (planeState->pitch), (planeState->yaw),
		planeState->gimbalRoll * PI/180, planeState->gimbalPitch * PI/180, GIMBAL_YAW, X_PIXELS / 2, Y_PIXELS / 2, 1 /*Zoom*/,
		planeState->image->cols, planeState->image->rows,
		centerLatitude, centerLongitude, centerAltitude);
}


void GeoReference::getCenterGPSFromUnverifiedData(Database::UnverifiedRowData ^ data, double & centerLatitude, double & centerLongitude, double & centerAltitude )
{
	getTargetGPS(data->candidate, centerLatitude, centerLongitude, centerAltitude);

}


void GeoReference::getCorners(ImageWithPlaneData ^ planeState, double & TLLat, double & TLLon, 
	double & TRLat, double & TRLon, double & BRLat, double & BRLon, double & BLLat, double & BLLon)
{
	double altitude = 0.00;


	getGPS(planeState->latitude, planeState->longitude, planeState->altitude, (planeState->roll), (planeState->pitch), (planeState->yaw),
		PI/180*(planeState->gimbalRoll), PI/180*(planeState->gimbalPitch), GIMBAL_YAW, 0, 0, 1,planeState->image->cols, planeState->image->rows,
		BLLat, BLLon, altitude);

	getGPS(planeState->latitude, planeState->longitude, planeState->altitude, (planeState->roll), (planeState->pitch), (planeState->yaw),
		PI/180*(planeState->gimbalRoll), PI/180*(planeState->gimbalPitch), GIMBAL_YAW, planeState->image->cols-1, 0, 1,planeState->image->cols, planeState->image->rows,
		BRLat, BRLon, altitude);

	getGPS(planeState->latitude, planeState->longitude, planeState->altitude, (planeState->roll), (planeState->pitch), (planeState->yaw),
		PI/180*(planeState->gimbalRoll), PI/180*(planeState->gimbalPitch), GIMBAL_YAW, planeState->image->cols-1, planeState->image->rows-1, 1,planeState->image->cols, planeState->image->rows,
		TRLat, TRLon, altitude);

	getGPS(planeState->latitude, planeState->longitude, planeState->altitude, (planeState->roll), (planeState->pitch), (planeState->yaw),
		PI/180*(planeState->gimbalRoll), PI/180*(planeState->gimbalPitch), GIMBAL_YAW, 0, planeState->image->rows-1, 1,planeState->image->cols, planeState->image->rows,
		TLLat, TLLon, altitude);
}

void GeoReference::getCorners(Database::TelemetryRowData ^ data, double & BLLat, double & BLLon, double & BRLat, double & BRLon, double & TRLat, double & TRLon,
			double & TLLat, double & TLLon)
{
	double altitude = 0.00;
	
	getGPS(	data->planeLocation->lat,
			data->planeLocation->lon,
			data->planeLocation->alt,
			data->planeRoll * PI / 180.0,
			data->planePitch * PI / 180.0,
			data->planeHeading * PI / 180.0,
			data->gimbalRoll * PI / 180.0,
			data->gimbalPitch * PI / 180.0,
			data->gimbalYaw,
			data->originX - X_PIXELS / 2,
			data->originY - Y_PIXELS / 2,
			data->gimbalZoom,
			X_PIXELS, 
			Y_PIXELS,
			BLLat,
			BLLon,
			altitude);

	getGPS(	data->planeLocation->lat,
			data->planeLocation->lon,
			data->planeLocation->alt,
			data->planeRoll * PI / 180.0,
			data->planePitch * PI / 180.0,
			data->planeHeading * PI / 180.0,
			data->gimbalRoll * PI / 180.0,
			data->gimbalPitch * PI / 180.0,
			data->gimbalYaw,
			data->originX + data->widthPixels-1 - X_PIXELS / 2,
			data->originY - Y_PIXELS / 2,
			data->gimbalZoom,
			X_PIXELS, 
			Y_PIXELS,
			BRLat,
			BRLon,
			altitude);

	getGPS(	data->planeLocation->lat,
			data->planeLocation->lon,
			data->planeLocation->alt,
			data->planeRoll * PI / 180.0,
			data->planePitch * PI / 180.0,
			data->planeHeading * PI / 180.0,
			data->gimbalRoll * PI / 180.0,
			data->gimbalPitch * PI / 180.0,
			data->gimbalYaw,
			data->originX - X_PIXELS / 2,
			data->originY + data->heightPixels-1 - Y_PIXELS / 2,
			data->gimbalZoom,
			X_PIXELS,
			Y_PIXELS,
			TLLat,
			TLLon,
			altitude);

	getGPS(	data->planeLocation->lat,
			data->planeLocation->lon,
			data->planeLocation->alt,
			data->planeRoll * PI / 180.0,
			data->planePitch * PI / 180.0,
			data->planeHeading * PI / 180.0,
			data->gimbalRoll * PI / 180.0,
			data->gimbalPitch * PI / 180.0,
			data->gimbalYaw,
			data->originX + data->widthPixels-1 - X_PIXELS / 2,
			data->originY + data->heightPixels-1 - Y_PIXELS / 2,
			data->gimbalZoom,
			X_PIXELS,
			Y_PIXELS,
			TRLat,
			TRLon,
			altitude);

}

void GeoReference::getCorners(Database::UnverifiedRowData ^ data, double & BLLat, double & BLLon, double & BRLat, double & BRLon, double & TRLat, double & TRLon,
			double & TLLat, double & TLLon)
{
	getCorners(data->candidate->telemetry, BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon);
}

bool GeoReference::getGPS(double plane_latitude, double plane_longitude, double plane_altitude, double plane_roll, double plane_pitch, double plane_heading, double gimbal_roll, double gimbal_pitch, double gimbal_yaw, 
				double target_x, double target_y, double zoom, int image_width, int image_height, double & Target_Latitude, double & Target_Longitude, double & Target_Height)
{
	try{
		return forwardGeoreferencing(plane_latitude, plane_longitude, plane_altitude, 
			plane_roll, plane_pitch, plane_heading, 
			gimbal_roll, gimbal_pitch, gimbal_yaw, 
			target_x, target_y, zoom,
			image_width, image_height,
			Target_Latitude, Target_Longitude, Target_Height) ;
	} catch(GeoReferenceException^ e){
		return false;
	}
}






/*Reference ellipsoids derived from Peter H. Dana's website- 
http://www.utexas.edu/depts/grg/gcraft/notes/datum/elist.html
Department of Geography, University of Texas at Austin
Internet: pdana@mail.utexas.edu
3/22/95

Source
Defense Mapping Agency. 1987b. DMA Technical Report: Supplement to Department of Defense World Geodetic System
1984 Technical Report. Part I and II. Washington, DC: Defense Mapping Agency
*/

cv::Mat EulerAngles(bool transpose, cv::Mat Orig_Vector, float Roll, float Pitch, float Yaw)
{

	double transarr[9] = {1,1,1,
						  1,1,1,  
						  1,1,1};
	cv::Mat Transfer = cv::Mat(3, 3, CV_64FC1, transarr).inv();

	return Transfer;

	
}


