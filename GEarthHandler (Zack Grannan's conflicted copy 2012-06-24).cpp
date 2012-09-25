#include "StdAfx.h"

#include "GEarthHandler.h"
#include "PlaneWatcher.h"
#include "GeoReference.h"
#include "TelemetryStructures.h"
#include "MasterHeader.h"
#include "DatabaseStructures.h"
#include "Database.h"
#include "SkynetController.h"

#define COUNTER_UPDATE_FREQ 50
#define PLANE_UPDATE_FREQ 1000
#define IMAGERY_UPDATE_FREQ 250
#define IMAGERY_FILE_UPDATE_FREQ 5000
#define TARGETS_UPDATE_FREQ 15000

#define GOOGLE_EARTH_KML_FILE_PLANE "C:\\xampp\\htdocs\\falco_flight.kml"
#define GOOGLE_EARTH_MASTERFILE "C:\\xampp\\htdocs\\auvsi_master.kml"
#define GOOGLE_EARTH_KML_FILE_IMAGERY_ROOT "C:\\xampp\\htdocs\\imagery\\"
#define GOOGLE_EARTH_KML_FILE_TARGETS "C:\\xampp\\htdocs\\targets.kml"

using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace Communications;
using namespace OpenGLForm;
using namespace Database;

GEarthHandler::GEarthHandler(PlaneWatcher ^ tpw, OpenGLForm::COpenGL ^ togl)
{
	runLoopThread = nullptr;
	theOpenGL = togl;
	thePlaneWatcher = tpw;
	theController = nullptr;
	lastFrame = 0;
	imageryFilename = "imagery_" + DateTime::Now.ToString("o")->Replace(":", "-") + ".kml";
	imageryData = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
				  "<kml xmlns=\"http://www.opengis.net/kml/2.2\" xmlns:gx=\"http://www.google.com/kml/ext/2.2\">\n"+
				  "<Document>\n";


	
	writeMasterFile();
}

void GEarthHandler::setController(Skynet::SkynetController ^ theCntl)
{
	theController = theCntl;
}

void GEarthHandler::setWatcher(PlaneWatcher ^ tpw)
{
	thePlaneWatcher = tpw;
}

void GEarthHandler::setDatabase(Database::DatabaseConnection ^ tdc)
{
	theDatabase = tdc;
}

void GEarthHandler::setOpenGL(OpenGLForm::COpenGL ^ togl)
{
	theOpenGL = togl;
}

void GEarthHandler::start()
{
	shouldEnd = false;
	paused = false;
	runLoopThread = gcnew Thread(gcnew ThreadStart(this, &GEarthHandler::runLoop));
	runLoopThread->Name = "GEarth Thread runLoop";
	runLoopThread->Start();
}

void GEarthHandler::writeMasterFile()
{
	StreamWriter^ outfile = gcnew StreamWriter(GOOGLE_EARTH_MASTERFILE);
	String ^ file = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
		"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"+
		"<Folder>\n"+
		"<name>AUVSI Telemetry</name>\n"+
		"<visibility>1</visibility>\n"+
		"<open>1</open>\n"+
		"<description>AUVSI Network link Loading falco_flight.kml</description>\n"+

		//falco file
		"<NetworkLink>\n"+
		"<name>UCSD AUVSI Telemetry Data</name>\n"+
		"<visibility>1</visibility>\n"+
		"<open>0</open>\n"+
		"<description>Location of plane and camera view.</description>\n"+
		"<refreshVisibility>0</refreshVisibility>\n"+
		"<flyToView>0</flyToView>\n"+
		"<Link>\n"+
		"<href>falco_flight.kml</href>\n"+
		"<refreshMode>onInterval</refreshMode>\n"+
		"<refreshInterval>1</refreshInterval>\n"+
		"</Link>\n"+
		"</NetworkLink>\n"+

		//targets file
		"<NetworkLink>\n"+
		"<name>UCSD AUVSI Target Data</name>\n"+
		"<visibility>1</visibility>\n"+
		"<open>1</open>\n"+
		"<description>Location of targets.</description>\n"+
		"<refreshVisibility>0</refreshVisibility>\n"+
		"<flyToView>0</flyToView>\n"+
		"<Link>\n"+
		"<href>targets.kml</href>\n"+
		"<refreshMode>onInterval</refreshMode>\n"+
		"<refreshInterval>15</refreshInterval>\n"+
		"</Link>\n"+
		"</NetworkLink>\n"+

		//imagery file
		"<NetworkLink>\n"+
		"<name>UCSD AUVSI Imagery</name>\n"+
		"<visibility>1</visibility>\n"+
		"<open>0</open>\n"+
		"<description>All images taken from plane.</description>\n"+
		"<refreshVisibility>0</refreshVisibility>\n"+
		"<flyToView>0</flyToView>\n"+
		"<Link>\n"+
		"<href>imagery/"+imageryFilename+"</href>\n"+
		"<refreshMode>onInterval</refreshMode>\n"+
		"<refreshInterval>5</refreshInterval>\n"+
		"</Link>\n"+
		"</NetworkLink>\n"+

		//end
		"</Folder>\n"+
		"</kml>";

	outfile->Write(file);
	outfile->Close();
}


void GEarthHandler::end()
{
	if(runLoopThread != nullptr)
	{
		shouldEnd = true;
		Thread::Sleep(100);
		runLoopThread->Abort();
		runLoopThread = nullptr;
	}
}
void GEarthHandler::pause()
{
	paused = true;
}
void GEarthHandler::unpause()
{
	paused = false;
}

void GEarthHandler::runLoop()
{
	int time_count = 0;
	try{
		while(!shouldEnd)
		{
			while(paused || theController == nullptr || !theController->guiHasData) // dont run if gui has no data
			{
				Thread::Sleep(100);
			}
			Thread::Sleep(COUNTER_UPDATE_FREQ);
			time_count++;

			if(time_count % (PLANE_UPDATE_FREQ/COUNTER_UPDATE_FREQ) == 0)
				updatePlaneFile(thePlaneWatcher->currentState());

			if(time_count % (IMAGERY_UPDATE_FREQ/COUNTER_UPDATE_FREQ) == 0)
				updateImageryData(thePlaneWatcher->currentState());

			if(time_count % (IMAGERY_FILE_UPDATE_FREQ/COUNTER_UPDATE_FREQ) == 0)
				updateImageryFile();

			if(time_count % (TARGETS_UPDATE_FREQ/COUNTER_UPDATE_FREQ) == 0)
				updateTargetsFile();

		}
	}
	catch(Exception ^ e)
	{
		PRINT("runLoop Error in GEHandler: " + e);
	}
}

void GEarthHandler::updateImageryData(PlaneState ^ state)
{
	// store this info in temp string, write to a file later
	if (theController == nullptr)
		return;

	// only save image if its a new image
	if (lastFrame == theController->frameCount)
		return;
	lastFrame = theController->frameCount;

	// take image, get string
	String ^imageName = theController->saveCurrentFrameAsImage();
	if (imageName == nullptr)
		return;
	
	// calc coordinates
	double BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon;
	Vision::GeoReference::getCorners(state, BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon);

	if (BLLat > 181.0 || BLLon > 181.0 || BRLat > 181.0 || BRLon > 181.0 || TRLat > 181.0 || 
		TRLon > 181.0 || TLLat > 181.0 || TLLon > 181.0) 
		return;

	// add GroundOverlay kml to imageryData
	imageryData += "<GroundOverlay>\n"+
					"\t<name>"+imageName+"</name>\n"+
					"\t<Icon>\n"+
					"\t\t<href>"+imageName+"</href>\n"+
					"\t\t<viewBoundScale>1.0</viewBoundScale>\n"+
					"\t</Icon>\n"+
					"\t<altitudeMode>clampToGround</altitudeMode>\n"+
					"\t<gx:LatLonQuad>\n"+
					"\t\t<coordinates>\n"+
					"\t\t"+TRLon+","+TRLat+" "+TLLon+","+TLLat+" "+BLLon+","+BLLat+" "+BRLon+","+BRLat+"\n"+
					"\t\t</coordinates>\n"+
					"\t</gx:LatLonQuad>\n"+
					"</GroundOverlay>\n";
}

void GEarthHandler::updateImageryFile()
{
	StreamWriter^ outfile = gcnew StreamWriter(GOOGLE_EARTH_KML_FILE_IMAGERY_ROOT + imageryFilename);

	outfile->Write(imageryData);
	outfile->Write("</Document>\n</kml>");

	// actually writes the kml data to disk
	outfile->Close();
}


void GEarthHandler::updateTargetsFile()
{
	StreamWriter^ outfile = gcnew StreamWriter(GOOGLE_EARTH_KML_FILE_TARGETS);

	array<VotesOnCandidate ^>^ targetData = theDatabase->getAllVotes();

	String ^ placemarks = "";
	if (targetData == nullptr)
		return;

	for each(VotesOnCandidate ^ votes in targetData)
	{
		if (votes == nullptr)
			continue;

		CandidateRowData ^ matchingCandidate = theDatabase->candidateWithID("" + votes->candidateid);
		TargetRowData ^ target = gcnew TargetRowData(matchingCandidate);
		target->updateFrom(votes->votes[0]); 

		double lat, lon, alt;

		Vision::GeoReference::getTargetGPS(target, lat, lon, alt);


		placemarks += "<Placemark>\n"+
				"\t<name>Target "+target->id+"</name>\n"+
				"\t<description>"+target->shapeColor +" "+ target->shape +", "+ target->letterColor +" "+ target->letter+" (un). GPS: "+
				Single(lat).ToString("######.#######")+","+Single(lon).ToString("######.#######")+"."+"</description>\n"+
				"\t<Point>\n"+
				"\t\t<coordinates>"+lon+","+lat+",0</coordinates>\n"+
				"\t</Point>\n"+
				"</Placemark>\n";
	}


	String ^ file = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
				"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"+
				"<Document>\n"+
				placemarks+
				"</Document>\n</kml>\n";

	outfile->Write(file);
	outfile->Close();
}

String ^ GEarthHandler::pointToString(double lat, double lon, double alt)
{
	return Single(lon).ToString("######.#######")+
				","+Single(lat).ToString("######.#######")+
				","+Single(alt).ToString("F");;
}

void GEarthHandler::updatePlaneFile(PlaneState ^ state)
{
	if(state == nullptr)
	{
		return;
	}

	double TLLat, TLLon, TRLat, TRLon, BLLat, BLLon, BRLat, BRLon;

	double centerLat = state->gpsData->gpsLatitude;
	double centerLon = state->gpsData->gpsLongitude;
	double centerAlt = state->telemData->altitudeHAL;

	double centerTargetLat, centerTargetLon, alt;
	Vision::GeoReference::getCenterGPSFromState(state, centerTargetLat, centerTargetLon, alt);


	double heading = (state->telemData->heading)*180.0/PI - 90;
	if (heading < 0)
		heading += 360.0;

	Vision::GeoReference::getCorners(state, TLLat, TLLon, TRLat, TRLon, BRLat, BRLon,
			BLLat, BLLon);
	

	//PRINT("head: " + Single((state->telemData->heading)*180.0/PI).ToString("######.#") + " roll: " + Single((state->telemData->pitch)*180.0/PI).ToString("######.#") + " pitch: " + Single((state->telemData->pitch)*180.0/PI).ToString("######.#") + ".");

	StreamWriter^ outfile = gcnew StreamWriter(GOOGLE_EARTH_KML_FILE_PLANE);
	String ^ file = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"+
				"<kml xmlns=\"http://www.opengis.net/kml/2.2\">\n"+
				"<Document>\n"+
				"<Style id=\"plane\">\n"+
				"\t<IconStyle>\n"+
				"\t\t<heading>"+Single(state->telemData->heading).ToString("F")+"</heading>\n"+
				"\t\t<Icon>\n"+
				"\t\t\t<href>http://localhost/falco.png</href>\n"+
				"\t\t\t<scale>1.0</scale>\n"+
				"\t\t</Icon>\n"+   
				"\t</IconStyle>\n"+
				"\t<PolyStyle>\n"+
				"\t\t<color>7fff0000</color>\n"+
				"\t\t<colorMode>normal</colorMode>\n"+
				"\t</PolyStyle>\n"+
				"</Style>\n"+

				// plane
				"<Placemark>\n"+
				"\t<styleUrl>#plane</styleUrl>\n"+
				"\t<name>Falco @ "+System::DateTime::Now.Second+"</name>\n"+
				"\t<description>Current location of UCSD Falco aircraft.</description>\n"+

				"\t<Model id=\"falco\">\n"+
				"\t\t<altitudeMode>relativeToGround</altitudeMode>\n"+
				"\t\t<Location>\n"+ 
				"\t\t\t<latitude>"+Single(state->gpsData->gpsLatitude).ToString("######.#######")+"</latitude>\n"+
				"\t\t\t<longitude>"+Single(state->gpsData->gpsLongitude).ToString("######.#######")+"</longitude>\n"+
				"\t\t\t<altitude>"+centerAlt/*Single(state->gpsData->gpsAltitude).ToString("F")*/+"</altitude>\n"+
				"\t\t</Location>\n"+ 
				"\t\t<Orientation>\n"+ 
				"\t\t\t<heading>"+ Single(heading).ToString("######.#") +"</heading>\n"+
				"\t\t\t<tilt>"+ 0.0 /*Single((state->telemData->roll)*180.0/PI).ToString("######.#")*/+"</tilt>\n"+ 
				"\t\t\t<roll>"+ 0.0 /*Single((state->telemData->pitch)*180.0/PI).ToString("######.#")*/+"</roll>\n"+ 
				"\t\t</Orientation>\n"+ 
				"\t\t<Scale>\n"+ 
				"\t\t\t<x>5.0</x>\n"+ 
				"\t\t\t<y>5.0</y>\n"+ 
				"\t\t\t<z>5.0</z>\n"+ 
				"\t\t</Scale>\n"+
				"\t\t<Link>\n"+
				"\t\t\t<href>falco_model.dae</href>\n"+
				"\t\t\t<refreshMode>once</refreshMode>\n"+
				"\t\t</Link>\n"+
				"\t</Model>\n"+
				"</Placemark>\n"+
				"<Placemark>\n"+
				"\t<styleUrl>#plane</styleUrl>\n"+
				"\t<name>Camera Box @ "+System::DateTime::Now.Second+"</name>\n"+
				"\t<description>Current camera view.</description>\n"+
				"\t<MultiGeometry>\n" + 

				//block 1
				"\t\t<Polygon>\n" + 
				"\t\t\t<extrude>0</extrude>\n"+
				"\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n"+
				"\t\t\t<outerBoundaryIs>\n"+
				"\t\t\t<LinearRing>\n"+
				"\t\t\t\t<coordinates>\n"+
				pointToString(centerLat, centerLon, centerAlt)+",\n"+
				pointToString(TLLat, TLLon, 0.00)+",\n"+
				pointToString(TRLat, TRLon, 0.00)+",\n"+
				pointToString(centerLat, centerLon, centerAlt)+"\n"+
				"\t\t\t\t</coordinates>\n"+
			    "\t\t\t</LinearRing>\n"+
				"\t\t</outerBoundaryIs>\n"+
				"\t</Polygon>\n"+
				//block 2
				"\t\t<Polygon>\n" + 
				"\t\t\t<extrude>0</extrude>\n"+
				"\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n"+
				"\t\t\t<outerBoundaryIs>\n"+
				"\t\t\t<LinearRing>\n"+
				"\t\t\t\t<coordinates>\n"+
				pointToString(centerLat, centerLon, centerAlt)+",\n"+
				pointToString(TRLat, TRLon, 0.00)+",\n"+
				pointToString(BRLat, BRLon, 0.00)+",\n"+
				pointToString(centerLat, centerLon, centerAlt)+"\n"+
				"\t\t\t\t</coordinates>\n"+
			    "\t\t\t</LinearRing>\n"+
				"\t\t</outerBoundaryIs>\n"+
				"\t</Polygon>\n"+
				//block 3
				"\t\t<Polygon>\n" + 
				"\t\t\t<extrude>0</extrude>\n"+
				"\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n"+
				"\t\t\t<outerBoundaryIs>\n"+
				"\t\t\t<LinearRing>\n"+
				"\t\t\t\t<coordinates>\n"+
				pointToString(centerLat, centerLon, centerAlt)+",\n"+
				pointToString(BRLat, BRLon, 0.00)+",\n"+
				pointToString(BLLat, BLLon, 0.00)+",\n"+
				pointToString(centerLat, centerLon, centerAlt)+"\n"+
				"\t\t\t\t</coordinates>\n"+
			    "\t\t\t</LinearRing>\n"+
				"\t\t\t</outerBoundaryIs>\n"+
				"\t\t</Polygon>\n"+
				//block 4
				"\t\t<Polygon>\n" + 
				"\t\t\t<extrude>0</extrude>\n"+
				"\t\t\t<altitudeMode>relativeToGround</altitudeMode>\n"+
				"\t\t\t<outerBoundaryIs>\n"+
				"\t\t\t<LinearRing>\n"+
				"\t\t\t\t<coordinates>\n"+
				pointToString(centerLat, centerLon, centerAlt)+",\n"+
				pointToString(BLLat, BLLon, 0.00)+",\n"+
				pointToString(TLLat, TLLon, 0.00)+",\n"+
				pointToString(centerLat, centerLon, centerAlt)+"\n"+
				"\t\t\t\t</coordinates>\n"+
			    "\t\t\t</LinearRing>\n"+
				"\t\t\t</outerBoundaryIs>\n"+
				"\t\t</Polygon>\n"+
				
				//end
				"\t</MultiGeometry>\n"+
				"</Placemark>\n"+
				"</Document>\n</kml>\n";
	outfile->Write(file);
	outfile->Close();
}

