#include "StdAfx.h"

#include "DatabaseStructures.h"
#include "TelemetryStructures.h"
#include "GeoReference.h"
#include "ImageWithPlaneData.h"

using namespace Database;
using namespace Vision;
using namespace System::Drawing;

String^ DialogEditingData::getHeadingString() {
	// TODO: Implement
	return nullptr;
}

String^ CandidateRowData::deleteSQL(){
	return "DELETE FROM candidates WHERE candidateid = " + candidateid + ";";
}

String^ UnverifiedRowData::deleteSQL(){
	return "DELETE FROM unverified_targets WHERE targetid = " + targetid + ";";
}

String^ VerifiedRowData::deleteSQL(){
	return "DELETE FROM verified_targets WHERE submitid = " + submitid + ";";
}

DialogEditingData^ CandidateRowData::toDialogData() {
	return gcnew DialogEditingData(this);
}

DialogEditingData^ UnverifiedRowData::toDialogData() {
	return gcnew DialogEditingData(this);
}

DialogEditingData^ VerifiedRowData::toDialogData() {
	return gcnew DialogEditingData(this);
}


void VerifiedRowData::useImageRegion(Tuple<int, int>^ topLeft, Tuple<int, int>^ bottomRight) {
	target->candidate->useImageRegion(topLeft, bottomRight);
}

void UnverifiedRowData::useImageRegion(Tuple<int, int>^ topLeft, Tuple<int, int>^ bottomRight) {
	candidate->useImageRegion(topLeft, bottomRight);
}

VerifiedRowData^ TargetRowData::asVerified(DialogEditingData^ dialogData,
											 Tuple<int, int>^ topLeftPx,
											 Tuple<int, int>^ bottomRightPx) {
	auto target = asVerified(dialogData);
	target->useImageRegion(topLeftPx, bottomRightPx);
	return target;
}

void CandidateRowData::useImageRegion(Tuple<int, int>^ topLeft, Tuple<int, int>^ bottomRight) {
	auto originalImage = gcnew Bitmap(imageName);
	int originX = Math::Min(topLeft->Item1, bottomRight->Item1);
	int originY = Math::Min(topLeft->Item2, bottomRight->Item2);
	int width = Math::Abs(bottomRight->Item1 - topLeft->Item1);
	int height = Math::Abs(bottomRight->Item2 - topLeft->Item2);
	auto cropped = originalImage->Clone(System::Drawing::Rectangle(originX, originY, width, height), originalImage->PixelFormat);
	imageName = imageName + ".crop.jpg";
	cropped->Save(imageName);
}

VerifiedRowData^ VerifiedRowData::asVerified(DialogEditingData^ dialogData) {
	applyDialogData(dialogData);
	return this;
}

VerifiedRowData^ UnverifiedRowData::asVerified(DialogEditingData^ dialogData) {
	applyDialogData(dialogData);
	return gcnew VerifiedRowData(this);
}

VerifiedRowData^ CandidateRowData::asVerified(DialogEditingData^ dialogData) {
	return gcnew VerifiedRowData(this, dialogData);
}

void DescriptionRowData::applyDialogEditingData(DialogEditingData^ dialogData) {
	shape = dialogData->shape;
	shapeColor = dialogData->shapeColor;
	letter = dialogData->letter;
	letterColor = dialogData->letterColor;
	targetX = dialogData->targetX;
	targetY = dialogData->targetY;
	heading = dialogData->getHeadingString();
}

void UnverifiedRowData::applyDialogData(DialogEditingData^ dialogData) {
	description->applyDialogEditingData(dialogData);
}

void VerifiedRowData::applyDialogData(DialogEditingData^ dialogData) {
	target->applyDialogData(dialogData);
}

CandidateRowData::CandidateRowData()
{
	this->telemetry = gcnew TelemetryRowData();
}

CandidateRowData::CandidateRowData(ImageWithPlaneData ^ planeState, int originX, int originY, int widthPixels, int heightPixels)
{
	this->telemetry = gcnew TelemetryRowData(planeState, originX, originY, widthPixels, heightPixels);
}

bool CandidateRowData::Equals(CandidateRowData ^ object)
{
	if (!imageName->Equals(object->imageName)) return false;
	if (!telemetry->Equals(object->telemetry)) return false;
	if (processed != object->processed) return false;
	return true;
}

DescriptionRowData::DescriptionRowData()
{
	this->targetX = 0;
	this->targetY = 0;
	this->shape = "-";
	this->shapeColor = "-";
	this->letter = "-";
	this->letterColor = "-";
	this->heading = "-";
}

bool DescriptionRowData::Equals(DescriptionRowData ^ object)
{
	if (!shape->Equals(object->shape)) return false;
	if (!shapeColor->Equals(object->shapeColor)) return false;
	if (!letter->Equals(object->letter)) return false;
	if (!letterColor->Equals(object->letterColor)) return false;
	if (!heading->Equals(object->heading)) return false;
	if (targetX != object->targetX) return false;
	if (targetY != object->targetY) return false;
	return true;
}

DialogEditingData::DialogEditingData(VerifiedRowData ^ data)
{
	this->imageName = data->target->candidate->imageName;
	this->shape = data->target->description->shape;
	this->shapeColor = data->target->description->shapeColor;
	this->letter = data->target->description->letter;
	this->letterColor = data->target->description->letterColor;
	this->targetX = data->target->description->targetX;
	this->targetY = data->target->description->targetY;
}

DialogEditingData::DialogEditingData(UnverifiedRowData ^ data)
{
	this->imageName = data->candidate->imageName;
	this->shape = data->description->shape;
	this->shapeColor = data->description->shapeColor;
	this->letter = data->description->letter;
	this->letterColor = data->description->letterColor;
	this->targetX = data->description->targetX;
	this->targetY = data->description->targetY;

}

DialogEditingData::DialogEditingData(CandidateRowData ^ data)
{
	this->imageName = data->imageName;
	this->id = data->candidateid;
}

bool DialogEditingData::Equals(DialogEditingData ^ object)
{
	if (!imageName->Equals(object->imageName)) return false;
	if (!shape->Equals(object->shape)) return false;
	if (!shapeColor->Equals(object->shapeColor)) return false;
	if (!letter->Equals(object->letter)) return false;
	if (!letterColor->Equals(object->letterColor)) return false;
	if (dataWidth != object->dataWidth) return false;
	if (dataHeight != object->dataHeight) return false;
	if (targetX != object->targetX) return false;
	if (targetY != object->targetY) return false;
	if (topOfTargetX != object->topOfTargetX) return false;
	if (topOfTargetY != object->topOfTargetY) return false;
	return true;
}

LocationRowData::LocationRowData(TelemetryRowData ^ telemetry)
{
	// calculate from telemetry
	double BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon;
	GeoReference::getCorners(telemetry, BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon);

	this->loc_id = 0;
	this->lowerLeftGPS = gcnew GPSPositionRowData(BLLat, BLLon, 0);
	this->lowerRightGPS = gcnew GPSPositionRowData(BRLat, BRLon, 0);
	this->upperLeftGPS = gcnew GPSPositionRowData(TLLat, TLLon, 0);
	this->upperRightGPS = gcnew GPSPositionRowData(TRLat, TRLon, 0);
}

bool LocationRowData::Equals(LocationRowData ^ object)
{
	if (!lowerLeftGPS->Equals(object->lowerLeftGPS)) return false;
	if (!lowerRightGPS->Equals(object->lowerRightGPS)) return false;
	if (!upperLeftGPS->Equals(object->upperLeftGPS)) return false;
	if (!upperRightGPS->Equals(object->upperRightGPS)) return false;
	return true;
}

TelemetryRowData::TelemetryRowData(ImageWithPlaneData ^ planeState, int originXIn, int originYIn, int widthPixelsIn, int heightPixelsIn)
{
	this->originX = originXIn;
	this->originY = originYIn;
	this->widthPixels = widthPixelsIn;
	this->heightPixels = heightPixelsIn;

	this->gimbalRoll = planeState->gimbalRoll;
	this->gimbalPitch = planeState->gimbalPitch;
	this->gimbalZoom = 1;
	this->planeRoll = planeState->roll;
	this->planePitch = planeState->pitch;
	this->planeHeading =  planeState->yaw;
	this->planeLocation = gcnew GPSPositionRowData(planeState->latitude, planeState->longitude, planeState->altitude);
	this->blurFactor = 0;

}

//TelemetryRowData::TelemetryRowData(ImageWithPlaneData ^ planeState)
//{
//	this->originX = 0;
//	this->originY = 0;
//	this->widthPixels = 720;
//	this->heightPixels = 480;
//
//	this->gimbalRoll = planeState->gimbalRoll;
//	this->gimbalPitch = planeState->gimbalPitch;
//	this->gimbalZoom = planeState->gimbalInfo->zoom;
//	this->planeRoll = planeState->roll;
//	this->planePitch = planeState->pitch;
//	this->planeHeading =  planeState->yaw;
//	this->planeLocation = gcnew GPSPositionRowData(planeState->latitude, planeState->longitude, planeState->altitude);
//	this->blurFactor = planeState->blurFactor;
//
//}

UnverifiedRowData::UnverifiedRowData(CandidateRowData ^ candidate)
{
	this->candidate = candidate;
	this->location = gcnew LocationRowData(candidate->telemetry);
	this->description = gcnew DescriptionRowData();
}

UnverifiedRowData::UnverifiedRowData(ImageWithPlaneData ^ planeState, int originXIn, int originYIn, int widthPixelsIn, int heightPixelsIn)
{
	this->candidate = gcnew CandidateRowData(planeState, originXIn, originYIn, widthPixelsIn, heightPixelsIn);
	this->location = gcnew LocationRowData(this->candidate->telemetry);
	this->description = gcnew DescriptionRowData();
}

void UnverifiedRowData::updateCandidate(CandidateRowData ^ candidate)
{
	this->candidate = candidate;
	this->location = gcnew LocationRowData(this->candidate->telemetry);
}


VerifiedRowData::VerifiedRowData(UnverifiedRowData ^ unverified)
{
	this->target = unverified;
	this->centerGPS = gcnew GPSPositionRowData(unverified);
}

VerifiedRowData::VerifiedRowData(CandidateRowData ^ candidate, DialogEditingData^ dialogData) {
	this->target = gcnew UnverifiedRowData(candidate, dialogData);
	this->centerGPS = gcnew GPSPositionRowData(target);
}

UnverifiedRowData::UnverifiedRowData(CandidateRowData^ candidate, DialogEditingData^ dialogData):
	candidate(candidate),
	location(gcnew LocationRowData(candidate->telemetry)),
	description(gcnew DescriptionRowData)
{
	applyDialogData(dialogData);
}


GPSPositionRowData::GPSPositionRowData(UnverifiedRowData ^ unverified)
{
	double lat, lon, alt;

	/**
	 * TODO: Output args shouldn't be passed in as inputs
	 */
	try {
		GeoReference::getTargetGPS(unverified, lat, lon, alt);
	} catch (Vision::GeoReferenceException^) {	
		lat = unverified->candidate->telemetry->planeLocation->lat;
		lon = unverified->candidate->telemetry->planeLocation->lon;
		alt = unverified->candidate->telemetry->planeLocation->alt;
	}
	this->lat = lat;
	this->lon = lon;
	this->alt = alt;
}

double GPSPositionRowData::distanceTo(GPSPositionRowData ^ gps)
{
	double latDiff = fabs(gps->lat - this->lat);
	double lonDiff = fabs(gps->lon - this->lon);

	double latDiffMeters = Vision::GeoReference::GPStoMeters(latDiff);
	double lonDiffMeters = Vision::GeoReference::GPStoMeters(lonDiff);

	return sqrt(latDiffMeters*latDiffMeters + lonDiffMeters*lonDiffMeters);
}

bool GPSPositionRowData::Equals(GPSPositionRowData ^ object)
{
	if (!Vision::approxEqual(lat,object->lat)) return false;
	if (!Vision::approxEqual(lon,object->lon)) return false;
	if (!Vision::approxEqual(alt,object->alt)) return false;
	return true;
}