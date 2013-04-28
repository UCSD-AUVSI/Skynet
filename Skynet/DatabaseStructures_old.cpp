#include "StdAfx.h"

#include "DatabaseStructures.h"
#include "TelemetryStructures.h"

using namespace Database;

VoteRowData::VoteRowData()
{
	targetid = -1;

	shape = "";
	shapeColor = "";
	letter = "";
	letterColor = "";

	targetX = -1;
	targetY = -1;

	topOfTargetX = -1;
	topOfTargetY = -1;

	userid = "ground_station";
		

}

VotesOnCandidate::VotesOnCandidate()
{
	targetid = -1;

	numVotes = 0;

	votes = gcnew array<VoteRowData ^>(5);
}

VerifiedTargetRowData::VerifiedTargetRowData()
{
	targetid = -1;

	Latitude = nullptr;
	Longitude = nullptr;
	Orientation = nullptr;

	shape = nullptr;
	shapeColor = nullptr;
	letter = nullptr;
	letterColor = nullptr;

	imageName = nullptr;
}

VerifiedTargetRowData::VerifiedTargetRowData(TargetRowData ^ data)
{
	targetid = data->id; 
	imageName = data->imageName; 
	
	shape = data->shape; 
	shapeColor = data->shapeColor; 
	letter = data->letter; 
	letterColor = data->letterColor; 

}


void VotesOnCandidate::addVote(VoteRowData ^ newVote)
{
	if (newVote == nullptr)
		return;

	// check size
	if (numVotes >= votes->GetLength(0)) 
	{
		Array::Resize(votes, numVotes+1);

	}

	votes[numVotes] = newVote;

	numVotes++;
}

VoteRowData ^ VotesOnCandidate::getSummaryOfVotes()
{
	// TODO:
	return nullptr;
}


DialogEditingData::DialogEditingData(VoteRowData ^ data)
{
	imageName = nullptr;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	id = data->targetid;

	dataWidth = -1;
	dataHeight = -1;

	targetX = data->targetX;
	targetY = data->targetY;

	topOfTargetX = data->topOfTargetX;
	topOfTargetY = data->topOfTargetY;

}


DialogEditingData::DialogEditingData(VerifiedTargetRowData ^ data)
{
	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	id = data->targetid;

	dataWidth = -1;
	dataHeight = -1;

	targetX = -1;
	targetY = -1;

	topOfTargetX = -1;
	topOfTargetY = -1;
}

void VoteRowData::updateFrom(DialogEditingData ^ data)
{
	targetid = data->id;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	targetX = data->targetX;
	targetY = data->targetY;
	
	topOfTargetX = data->topOfTargetX;
	topOfTargetY = data->topOfTargetY;

}

CandidateRowData::CandidateRowData()
{
	id = System::DateTime::Now.Minute*100000 + System::DateTime::Now.Second*1000 + System::DateTime::Now.Millisecond;

	imageName = "Unknown";

	shape = "Unknown";
	shapeColor = "Unknown";
	letter = "Unknown";
	letterColor = "Unknown";
			

	dataWidth = 0;
	dataHeight = 0;
	dataNumChannels = 0;

	originX = 0;
	originY = 0;

	targetX = -1;
	targetY = -1;
		
	gpsAltitude = 0;
	gpsLatitude = 0;
	gpsLongitude = 0;
	altitudeAboveLaunch = 0;
	velocity = 0;
	planeRollDegrees = 0;
	planePitchDegrees = 0;
	planeHeadingDegrees = 0;

	gimbalRollDegrees = 0;
	gimbalPitchDegrees = 0;

	zoom = 0;

	gimbalRollRateDegrees = 0;
	gimbalPitchRateDegrees = 0;
		
	planeRollRateDegrees = 0;
	planePitchRateDegrees = 0;
	planeHeadingRateDegrees = 0;

	timestamp = System::DateTime::Now.Subtract(System::DateTime(1970,1,1)).TotalSeconds;
}

CandidateRowData::CandidateRowData(Communications::PlaneState ^ inputState)
{
	id = System::DateTime::Now.Minute*100000 + System::DateTime::Now.Second*1000 + System::DateTime::Now.Millisecond;

	imageName = "unknown";

	shape = "Unknown";
	shapeColor = "Unknown";
	letter = "Unknown";
	letterColor = "Unknown";
			
	dataWidth = 0;
	dataHeight = 0;
	dataNumChannels = 0;

	originX = 0; 
	originY = 0;

	targetX = -1;
	targetY = -1;
		
	gpsAltitude = inputState->gpsData->gpsAltitude;
	gpsLatitude = inputState->gpsData->gpsLatitude;
	gpsLongitude = inputState->gpsData->gpsLongitude;
	altitudeAboveLaunch = inputState->telemData->altitudeHAL;
	velocity = inputState->telemData->velocity;
	planeRollDegrees = inputState->telemData->roll*180.0f/(float)Math::PI;
	planePitchDegrees = inputState->telemData->pitch*180.0f/(float)Math::PI;
	planeHeadingDegrees = inputState->telemData->heading*180.0f/(float)Math::PI;

	gimbalRollDegrees = inputState->gimbalInfo->roll;
	gimbalPitchDegrees = inputState->gimbalInfo->pitch;
			

	zoom = inputState->gimbalInfo->zoom;

	gimbalRollRateDegrees = 0;
	gimbalPitchRateDegrees = 0;
		
	planeRollRateDegrees = inputState->telemData->rollRate*180.0f/(float)Math::PI;
	planePitchRateDegrees = inputState->telemData->pitchRate*180.0f/(float) Math::PI;
	planeHeadingRateDegrees = inputState->telemData->yawRate*180.0f/(float)Math::PI;
			
	timestamp = System::DateTime::Now.Subtract(System::DateTime(1970,1,1)).TotalSeconds;			
}

void CandidateRowData::updateFrom(DialogEditingData ^ data)
{
	imageName = data->imageName;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	id = data->id;

	dataWidth = data->dataWidth;
	dataHeight = data->dataHeight;

	targetX = data->targetX;
	targetY = data->targetY;
}

TargetRowData::TargetRowData() 
{

	id = System::DateTime::Now.Minute*100000 + System::DateTime::Now.Second*1000 + System::DateTime::Now.Millisecond;

	imageName = "Unknown";

	shape = "Unknown"; shapeColor = "Unknown"; letter = "Unknown"; letterColor = "Unknown";
			

	dataWidth = 0; dataHeight = 0; dataNumChannels = 0;

	originX = 0; originY = 0;

	targetX = -1; targetY = -1; topOfTargetX = -1; topOfTargetY = 0;
		
	gpsAltitude = 0; gpsLatitude = 0; gpsLongitude = 0; altitudeAboveLaunch = 0;
	velocity = 0;
	planeRollDegrees = 0; planePitchDegrees = 0; planeHeadingDegrees = 0;

	gimbalRollDegrees = 0; gimbalPitchDegrees = 0;

	zoom = 0;

	gimbalRollRateDegrees = 0; gimbalPitchRateDegrees = 0;
		
	planeRollRateDegrees = 0; planePitchRateDegrees = 0; planeHeadingRateDegrees = 0;

	timestamp = System::DateTime::Now.Subtract(System::DateTime(1970,1,1)).TotalSeconds;
}

TargetRowData::TargetRowData(CandidateRowData ^  inputRow) 
{

	id = inputRow->id; 
	imageName = inputRow->imageName; 
	shape = inputRow->shape; 
	shapeColor = inputRow->shapeColor; 
	letter = inputRow->letter; 
	letterColor = inputRow->letterColor; 
	dataWidth = inputRow->dataWidth; 
	dataHeight = inputRow->dataHeight; 
	dataNumChannels = inputRow->dataNumChannels; 
	originX = inputRow->originX; 
	originY = inputRow->originY; 
	targetX = inputRow->targetX; 
	targetY = inputRow->targetY; 

	topOfTargetX = -1; 
	topOfTargetY = -1; 
			
	gpsAltitude = inputRow->gpsAltitude; 
	gpsLatitude = inputRow->gpsLatitude; 
	gpsLongitude = inputRow->gpsLongitude; 
	altitudeAboveLaunch = inputRow->altitudeAboveLaunch; 
	velocity = inputRow->velocity; 
	planeRollDegrees = inputRow->planeRollDegrees; 
	planePitchDegrees = inputRow->planePitchDegrees; 
	planeHeadingDegrees = inputRow->planeHeadingDegrees; 
	gimbalRollDegrees = inputRow->gimbalRollDegrees; 
	gimbalPitchDegrees = inputRow->gimbalPitchDegrees; 
	zoom = inputRow->zoom; 
	gimbalRollRateDegrees = inputRow->gimbalRollRateDegrees; 
	gimbalPitchRateDegrees = inputRow->gimbalPitchRateDegrees; 
	planeRollRateDegrees = inputRow->planeRollRateDegrees; 
	planePitchRateDegrees = inputRow->planePitchRateDegrees; 
	planeHeadingRateDegrees = inputRow->planeHeadingRateDegrees; 
	timestamp = inputRow->timestamp;		
}

void TargetRowData::updateFrom(DialogEditingData ^ data)
{
	imageName = data->imageName;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
	
	id = data->id;

	dataWidth = data->dataWidth;
	dataHeight = data->dataHeight;

	targetX = data->targetX;
	targetY = data->targetY;

	topOfTargetX = data->topOfTargetX;
	topOfTargetY = data->topOfTargetY;
}


void TargetRowData::updateFrom(VoteRowData ^ data)
{

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
	
	targetX = data->targetX;
	targetY = data->targetY;

	topOfTargetX = data->topOfTargetX;
	topOfTargetY = data->topOfTargetY;

}

DialogEditingData::DialogEditingData()
{
	imageName = "";

	shape = "";
	shapeColor = "";
	letter = "";
	letterColor = "";
		
	id = -1;

	dataWidth = -1;
	dataHeight = -1;

	targetX = -1;
	targetY = -1;

	topOfTargetX = -1;
	topOfTargetY = -1;
}

DialogEditingData::DialogEditingData(CandidateRowData ^ data)
{
	imageName = data->imageName;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	id = data->id;

	dataWidth = data->dataWidth;
	dataHeight = data->dataHeight;

	targetX = data->targetX;
	targetY = data->targetY;

	topOfTargetX = -1;
	topOfTargetY = -1;
}

DialogEditingData::DialogEditingData(TargetRowData ^ data)
{
	imageName = data->imageName;

	shape = data->shape;
	shapeColor = data->shapeColor;
	letter = data->letter;
	letterColor = data->letterColor;
		
	id = data->id;

	dataWidth = data->dataWidth;
	dataHeight = data->dataHeight;

	targetX = data->targetX;
	targetY = data->targetY;

	topOfTargetX = data->topOfTargetX;
	topOfTargetY = data->topOfTargetY;
}