#include <cv.h>
#include "stdafx.h"
#include <highgui.h>
#include "ImageWithPlaneData.h"
#include "../Pathfinding/Autosearch.h"
#include "PlaneDataReceiver.h"
#include "Util.h"
#include "GPSCoord.h"

using Intelligence::GPSCoord;

ImageWithPlaneData::ImageWithPlaneData(ImageWithPlaneData^ other) :
	imageFilename(other->imageFilename),
	image(other->image),
	latitude(other->latitude),
	longitude(other->longitude),
	altitude(other->altitude),
	roll(other->roll),
	pitch(other->pitch),
	yaw(other->yaw),
	gimbalRoll(other->gimbalRoll),
	gimbalPitch(other->gimbalPitch),
	xVelocity(other->xVelocity),
	yVelocity(other->yVelocity),
	zVelocity(other->zVelocity) {}

ImageWithPlaneData::ImageWithPlaneData(String ^ imageFilename, String ^data) {
	std::string imageFilenameStd = Util::managedToSTL(imageFilename);
	this->imageFilename = imageFilename;
	image = new cv::Mat(cv::imread(imageFilenameStd));
	image->addref();
	auto vars = data->Split(' ');
	latitude = Convert::ToDouble(vars[0]) / 1000000.0;
	longitude = Convert::ToDouble(vars[1]) / 1000000.0;
	altitude = Convert::ToDouble(vars[2]);
	roll = Convert::ToDouble(vars[3]);
	pitch = Convert::ToDouble(vars[4]);
	yaw = Convert::ToDouble(vars[5]);
	gimbalRoll = Convert::ToDouble(vars[6]);
	gimbalPitch = Convert::ToDouble(vars[7]);
	xVelocity = Convert::ToDouble(vars[8]) / 1000;
	yVelocity = Convert::ToDouble(vars[9]) / 1000;
	zVelocity = Convert::ToDouble(vars[10]) / 1000;
}

Intelligence::GPSCoord^ ImageWithPlaneData::toGPSCoord()
{
	return gcnew GPSCoord(longitude, latitude, altitude);
}

void ImageWithPlaneData::setPosition(GPSCoord^ coord)
{
	altitude = coord->alt;
	latitude = coord->lat;
	longitude = coord->lon;
}