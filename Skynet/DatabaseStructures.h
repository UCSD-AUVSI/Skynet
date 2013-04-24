#pragma once
#include "MasterHeader.h"
using namespace System;

namespace Communications
{
	ref struct PlaneState;
}

ref struct ImageWithPlaneData;

namespace Database
{
	ref struct TargetRowData;
	ref struct CandidateRowData;
	ref struct DescriptionRowData;
	ref struct GPSPositionRowData;
	ref struct LocationRowData;
	ref struct TelemetryRowData;
	ref struct UnverifiedRowData;
	ref struct VerifiedRowData;
	ref struct DialogEditingData;

	/*
	public ref struct PrimaryKey {
		const int id;
	};
	*/

	public ref struct RowData abstract {
	public:
		/*
		virtual bool hasPrimaryKey() = 0;
		virtual String^ makeInsertString() = 0;
		virtual String^ makeUpdateString() = 0;
		virtual String^ makeDeleteString() = 0;
		*/
	};

	public ref struct TargetRowData abstract : public RowData {
	public:
		virtual VerifiedRowData^ asVerified(DialogEditingData^ dialogData) = 0;
		virtual DialogEditingData^ toDialogData() = 0;
		
	};
	
	public ref struct CandidateRowData: public TargetRowData
	{
		int candidateid;
		String ^ imageName;
		TelemetryRowData ^ telemetry;
		bool processed;

		CandidateRowData();
		CandidateRowData(ImageWithPlaneData ^ planeState, int originX, int originY, int widthPixels, int heightPixels);
	
		virtual DialogEditingData^ toDialogData() override;
		virtual VerifiedRowData^ asVerified(DialogEditingData^ dialogData) override;
		bool Equals(CandidateRowData ^ object);
	};

	
	public ref struct DescriptionRowData : public RowData
	{
		int description_id;
		int targetX;
		int targetY;
		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;
		String ^ heading;
		DescriptionRowData();
		
		void applyDialogEditingData(DialogEditingData^ data);
		bool Equals(DescriptionRowData ^ object);
	};

	public ref struct GPSPositionRowData : public RowData
	{
		int gps_id;
		double lat;
		double lon;
		double alt;

		GPSPositionRowData() 
		{
			gps_id = 0;
			lat = 0.0;
			lon = 0.0;
			alt = 0.0;
		}

		GPSPositionRowData(double latIn, double lonIn, double altIn)
		{
			gps_id = 0;
			lat = latIn;
			lon = lonIn;
			alt = altIn;
		}

		GPSPositionRowData(UnverifiedRowData ^ unverified);

		double distanceTo(GPSPositionRowData ^ gps);
		
		bool Equals(GPSPositionRowData ^ object);
	};

	public ref struct LocationRowData : public RowData
	{
		int loc_id;
		GPSPositionRowData ^ lowerLeftGPS;
		GPSPositionRowData ^ lowerRightGPS;
		GPSPositionRowData ^ upperLeftGPS;
		GPSPositionRowData ^ upperRightGPS;

		LocationRowData() {}
		LocationRowData(TelemetryRowData ^ telemetry);

		GPSPositionRowData ^ centerGPS()
		{
			// calculate centroid of the quadrilateral
			double lat = (lowerLeftGPS->lat + lowerRightGPS->lat + upperLeftGPS->lat + upperRightGPS->lat)/4.0;

			double lon = (lowerLeftGPS->lon + lowerRightGPS->lon + upperLeftGPS->lon + upperRightGPS->lon)/4.0;
			
			return gcnew GPSPositionRowData(lat, lon, lowerLeftGPS->alt);
		}

		double widthMeters()
		{
			double top = upperLeftGPS->distanceTo(upperRightGPS);
			double bottom = lowerLeftGPS->distanceTo(lowerRightGPS);

			return (top+bottom)/2.0;
		}

		double heightMeters()
		{
			double left = upperLeftGPS->distanceTo(lowerLeftGPS);
			double right = upperRightGPS->distanceTo(lowerRightGPS);

			return (left+right)/2.0;
		}
		
		bool Equals(LocationRowData ^ object);
	};

	public ref struct TelemetryRowData : RowData
	{
		int telemetry_id;
		int originX;
		int originY;
		double gimbalRoll;
		double gimbalPitch;
		double gimbalYaw;
		int gimbalZoom;
		double blurFactor;
		int widthPixels; 
		int heightPixels; 
		double planeRoll;
		double planePitch;
		double planeHeading;
		GPSPositionRowData ^ planeLocation;
		TelemetryRowData() {}
		//TelemetryRowData(ImageWithPlaneData ^ planeState);
		TelemetryRowData(ImageWithPlaneData ^ planeState, int originXIn, int originYIn, int widthPixelsIn, int heightPixelsIn);
	
		
		bool Equals(TelemetryRowData ^ object);
	};


	public ref struct UnverifiedRowData: public TargetRowData
	{
		int targetid;
		CandidateRowData ^ candidate;
		int revision;
		bool processed;
		LocationRowData ^ location;
		DescriptionRowData ^ description;
		UnverifiedRowData() {}
		UnverifiedRowData(CandidateRowData ^ candidate);
		UnverifiedRowData(ImageWithPlaneData ^ planeState, int originXIn, int originYIn, int widthPixelsIn, int heightPixelsIn);
		UnverifiedRowData(CandidateRowData^ candidate, DialogEditingData^ extra);

		virtual DialogEditingData^ toDialogData() override;
		virtual VerifiedRowData^ asVerified(DialogEditingData^ dialogData) override;
		void applyDialogData(DialogEditingData^ data);
		void updateCandidate(CandidateRowData ^ candidate);
		bool Equals(UnverifiedRowData ^ object);
	};

	public ref struct VerifiedRowData: public TargetRowData
	{
		UnverifiedRowData ^ target;
		int submitid; // this is submitid in the database
		GPSPositionRowData ^ centerGPS;
		VerifiedRowData() {}
		VerifiedRowData(UnverifiedRowData ^ unverified);
		VerifiedRowData(CandidateRowData ^ candindate, DialogEditingData^ dialogData);
		
		virtual DialogEditingData^ toDialogData() override;
		virtual VerifiedRowData^ asVerified(DialogEditingData^ dialogData) override;
		void applyDialogData(DialogEditingData^ data);
		bool Equals(VerifiedRowData ^ object);
	};

	public ref struct DialogEditingData : public RowData
	{
		DialogEditingData() {};
		DialogEditingData(VerifiedRowData ^ data);
		DialogEditingData(UnverifiedRowData ^ data);
		DialogEditingData(CandidateRowData ^ data);
		
		String ^ imageName;

		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;
		
		int id;

		int dataWidth;
		int dataHeight;

		int targetX;
		int targetY;

		int topOfTargetX;
		int topOfTargetY;

		bool Equals(DialogEditingData ^ object);
		String^ getHeadingString();
	};
}