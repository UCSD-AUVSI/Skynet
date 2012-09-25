#pragma once

using namespace System;

namespace Communications
{
	ref struct PlaneState;
}

namespace Database
{
	ref struct DialogEditingData;
	ref struct CandidateRowData;
	ref struct VoteRowData;
	ref struct TargetRowData;


	public ref struct VoteRowData
	{
		VoteRowData();

		void updateFrom(DialogEditingData ^ data);

		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;

		int targetX;
		int targetY;

		int topOfTargetX;
		int topOfTargetY;

		String ^ userid;
		
		int candidateid;
	};

	public ref struct VotesOnCandidate
	{
		VotesOnCandidate();
		void addVote(VoteRowData ^ newVote);
		VoteRowData ^getSummaryOfVotes();

		int candidateid;

		int numVotes;

		array<VoteRowData ^> ^ votes;
	};

	public ref struct CandidateRowData
	{
		CandidateRowData();
		CandidateRowData(Communications::PlaneState ^ inputState);

		void updateFrom(DialogEditingData ^ data);
		

		String ^ imageName;

		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;
		
		int id;

		int dataWidth;
		int dataHeight;
		int dataNumChannels;

		int originX;
		int originY;

		int targetX;
		int targetY;
		
		float gpsAltitude;
		float gpsLatitude;
		float gpsLongitude;
		float altitudeAboveLaunch;
		float velocity;
		float planeRollDegrees;
		float planePitchDegrees;
		float planeHeadingDegrees;

		float gimbalRollDegrees;
		float gimbalPitchDegrees;

		float zoom;

		float gimbalRollRateDegrees;
		float gimbalPitchRateDegrees;
		
		float planeRollRateDegrees;
		float planePitchRateDegrees;
		float planeHeadingRateDegrees;

		double timestamp;
	};

	public ref struct TargetRowData
	{
		TargetRowData();
		TargetRowData(CandidateRowData ^ inputRow);
		
		void updateFrom(DialogEditingData ^ data);
		void updateFrom(VoteRowData ^ data);

		String ^ imageName;

		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;
		
		int id;

		int dataWidth;
		int dataHeight;
		int dataNumChannels;

		int originX;
		int originY;

		int targetX;
		int targetY;

		int topOfTargetX;
		int topOfTargetY;
		
		float gpsAltitude;
		float gpsLatitude;
		float gpsLongitude;
		float altitudeAboveLaunch;
		float velocity;
		float planeRollDegrees;
		float planePitchDegrees;
		float planeHeadingDegrees;

		float gimbalRollDegrees;
		float gimbalPitchDegrees;

		float zoom;

		float gimbalRollRateDegrees;
		float gimbalPitchRateDegrees;
		
		float planeRollRateDegrees;
		float planePitchRateDegrees;
		float planeHeadingRateDegrees;

		double timestamp;
	};

	
	public ref struct VerifiedTargetRowData
	{
		VerifiedTargetRowData();
		VerifiedTargetRowData(TargetRowData ^ data);

		String ^ Latitude;
		String ^ Longitude;
		String ^ Orientation;


		String ^ shape;
		String ^ shapeColor;
		String ^ letter;
		String ^ letterColor;

		int candidateid;

		String ^ imageName;

	};

	public ref struct DialogEditingData
	{
		DialogEditingData();
		DialogEditingData(CandidateRowData ^ data);
		DialogEditingData(TargetRowData ^ data);
		DialogEditingData(VoteRowData ^ data);
		DialogEditingData(VerifiedTargetRowData ^ data);
		
		
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
	};

	public ref struct RowData // LEGACY: no longer used
	{
		RowData(void)
		{
			id = 0;							// Unique ID for this entry
			path = "not a path";					// Path to a file for the image
			target_latitude = 0.0;			// Latitude of point selected as target
			target_longitude = 0.0;			// Longitude of point selected as target
			target_X = 0;					// pixel X of target
			target_Y = 0;					// pixel Y of target
			heading = 0.0;					// heading of target
			letter = "-";				
			shape = "----";					
			fg_color = "----";				// foreground color
			bg_color = "----";				// background color
			processed = false;					// whether this has been processed by OpenCV
			verified = false;					// human verified
			center_latitude = 0.0;			// Latitude of center pixel
			center_longitude = 0.0;			// Longitude of center pixel
			mapping_latitude = 0.0;			// pixel to meter translation for latitude
			mapping_longitude = 0.0;
			homography = gcnew array<float>(9);
			for( int i = 0; i < 9; i++)
			{
				homography[i] = 0.0;
			}
		}

		int id;							// Unique ID for this entry
		String ^ path;					// Path to a file for the image
		float target_latitude;			// Latitude of point selected as target
		float target_longitude;			// Longitude of point selected as target
		int target_X;					// pixel X of target
		int target_Y;					// pixel Y of target
		float heading;					// heading of target
		String ^ letter;				
		String ^ shape;					
		String ^ fg_color;				// foreground color
		String ^ bg_color;				// background color
		bool processed;					// whether this has been processed by OpenCV
		bool verified;					// human verified
		float center_latitude;			// Latitude of center pixel
		float center_longitude;			// Longitude of center pixel
		float mapping_latitude;			// pixel to meter translation for latitude
		float mapping_longitude;		// pixel to meter translation for longitude
		array<float> ^ homography;		// homography used to transform this image
	};

}