#include "DuplicateResolver.h"
#include "MasterHeader.h"
#include "Delegates.h"
#include "GeoReference.h"
#include "DatabaseStructures.h"
#include "Util.h"
#include "SkynetController.h"


using namespace Vision;
using namespace Delegates;
using namespace Database;

DuplicateResolver::DuplicateResolver(Skynet::SkynetController^ skynetController):
	skynetController(skynetController)
{}

bool
DuplicateResolver::betterThanDuplicate(CandidateRowData^ candidate, UnverifiedRowData^ duplicate)
{
	const float duplicateBlurFactor = (float)duplicate->candidate->telemetry->blurFactor;
	const float candidateBlurFactor = (float)candidate->telemetry->blurFactor;
	const int duplicateAreaPixels = duplicate->candidate->telemetry->widthPixels * duplicate->candidate->telemetry->heightPixels;
	const int candidateAreaPixels = candidate->telemetry->widthPixels * candidate->telemetry->heightPixels;;

	// determine if matchingUnverified is better than unverified
	return duplicateAreaPixels > candidateAreaPixels || ( duplicateAreaPixels*1.5 > candidateAreaPixels && duplicateBlurFactor < candidateBlurFactor);

}

UnverifiedRowData^ 
DuplicateResolver::maybeFindDuplicate(CandidateRowData ^ candidate)
{

	/**
	 * TODO: This whole method could probably be replaced with a well-crafted 
	 *       SQL query.
	 */

	UnverifiedRowData^ unverified = gcnew UnverifiedRowData(candidate);

	GPSPositionRowData ^ const candidateGPS = unverified->location->centerGPS();
	const float candidateArea = (float)unverified->location->widthMeters() * (float)unverified->location->heightMeters();

	// find unverified row with gps w/in 5 meters, and size w/in 1 meter
	for each (UnverifiedRowData ^ const row in skynetController->getAllUnverified())
	{
		GPSPositionRowData ^ const rowGPS = row->location->centerGPS();
		const float rowArea = (float)row->location->widthMeters() * (float)row->location->heightMeters();

		if (candidateGPS->distanceTo(rowGPS) < 5 && abs(rowArea - candidateArea) < 1)
		{
			return row;
		}
	}
	
	return nullptr;
}