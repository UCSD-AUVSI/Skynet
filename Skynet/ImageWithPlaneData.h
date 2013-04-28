#pragma once

#include <cv.h>

namespace Intelligence{ref struct GPSCoord;};
public ref struct ImageWithPlaneData {
public:
	System::String ^ imageFilename;
	cv::Mat* image;
	double latitude;
	double longitude;
	double altitude;
	double roll;
	double pitch;
	double yaw;
	double gimbalRoll;
	double gimbalPitch;
	double xVelocity;
	double yVelocity;
	double zVelocity;
 
	Intelligence::GPSCoord^ toGPSCoord();
	ImageWithPlaneData(System::String ^ imageFilename, System::String ^data);
	ImageWithPlaneData(ImageWithPlaneData^ other);
	ImageWithPlaneData(){};
};