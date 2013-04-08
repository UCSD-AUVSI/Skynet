/* Autosearch Source File */

#include "../Skynet/MasterHeader.h"
#include "../Skynet/GeoReference.h"
#include "../Skynet/PlaneWatcher.h"
#include "../Skynet/intelligenceController.h"
#include "../Skynet/SkynetController.h"
#include "../Skynet/TelemetryStructures.h"
#include <limits.h>
#include <math.h>
#define MAX_MARKABLE_AREA 15000
#include "Autosearch.h"
#using <System.Drawing.dll>
using namespace System;
using namespace System::Threading;
using namespace System::IO;
using namespace System::Collections;
using namespace System::Drawing;
using namespace Communications;
using namespace Vision;
using namespace Intelligence;


/**
 * Calculates the area, in meters of the polygon that has vertices denoted by
 * coords
 *
 * Formula taken from: http://www.mathopenref.com/coordpolygonarea.html
 *
 * @param coords An array of meter coordinates
 * @return The area of the polygon
 */
double Autosearch::area(array<ArrayCoord ^>^ coords){
	if ( coords->Length < 3 ){
		return 0;
	}
	double area = 0;
	for (int i = 0; i < coords->Length - 1; i++){
		area += coords[i]->x * coords[i+1]->y - coords[i]->y * coords[i+1]->x;
	}
	area += coords[coords->Length-1]->x*coords[0]->y - coords[coords->Length-1]->y * coords[0]->x;
	area /= 2;
	return abs(area);
}

/**
 * Retrieves the GPS coordinate fom a coordinate on the map.
 *
 * @param coord The coordinate on the map.
 * @return		The corresponding GPS coordinate.
 */
GPSCoord^ Autosearch::arrayCoordToGPS(ArrayCoord^ coord)
{
	GPSCoord ^ result = gcnew GPSCoord(GPSCoord::metersToGPS(coord->y) + bottomBound,
								   GPSCoord::metersToGPS(coord->x) + leftBound);
	return result;
}


Autosearch::Autosearch(String ^ filename,Intelligence::IntelligenceController^ controller)
{
	StreamReader^ stream = gcnew StreamReader(filename);
	ArrayList^ coords = gcnew ArrayList(10);
	String^ line;
	double lat,lon;
	while (!stream->EndOfStream){
		line = stream->ReadLine();
		if (line->Length > 0)
		{
			array<String^>^ data = line->Split(',');
			try 
			{
				lat = Double::Parse(data[0]);
				lon = Double::Parse(data[1]);

				coords->Add(gcnew GPSCoord(lat,lon));
			}
			catch (System::FormatException ^) {}
		}
	}
	stream->Close();

	array<GPSCoord ^> ^ coordsArray = gcnew array<GPSCoord ^>(coords->Count);
	for (int i = 0; i < coords->Count; i++)
	{
		coordsArray[i] = (GPSCoord ^)coords[i];
	}
	construct(coordsArray,controller);
}

Autosearch::Autosearch(array<String ^>^coords, Intelligence::IntelligenceController^ controller){
	array<GPSCoord ^> ^ coordsArray = gcnew array<GPSCoord ^>(coords->Length);
	for (int i = 0; i < coords->Length; i++)
	{
		array<String^>^ coord = coords[i]->Split(',');
		double lat = Double::Parse(coord[0]);
		double lon = Double::Parse(coord[1]);
		coordsArray[i] = gcnew GPSCoord(lat,lon);
	}
	construct(coordsArray,controller);
}
/**
 * Constructor for Autosearch. Establishes the bounds and initializes the map.
 * The map area that needs to be searched is filled with ones, the map area
 * that does not need to be searched is filled with zeroes.
 *
 * @param coords	The GPS coordinates of the map boundaries.
 */
 
Autosearch::Autosearch(array<GPSCoord ^> ^coords,Intelligence::IntelligenceController^ controller)
{
	construct(coords,controller);
}

void Autosearch::construct(array<GPSCoord ^> ^coords, Intelligence::IntelligenceController^ controller){
	intelligenceController = controller;
	planeWatcher = controller->skynetController->getPlaneWatcher();
	BORDER = GPSCoord::metersToGPS(3); // Map border is three meters.

	// Get the boundaries of the map (GPS)
	array<double> ^bounds = getBounds(coords);
	bottomBound = bounds[0];
	topBound = bounds[1];
	leftBound = bounds[2];
	rightBound = bounds[3];

	// Establish width and height of the map in GPS and meters
	widthGPS = rightBound - leftBound;
	heightGPS = topBound - bottomBound;
	widthMeters = GPSCoord::GPSToMeters(widthGPS);
	heightMeters = GPSCoord::GPSToMeters(heightGPS);
	width = (int)(widthMeters) + 1;
	height = (int)(heightMeters) + 1;

	// Create the map, fill it with zeroes.
	map = gcnew array<int>(height*width);
	for(int y = 0; y < height; y++)
		for (int x = 0; x < width; x++)
				map[y * width + x] = 0;


	// Outline the area of the map that needs to be searched
	markPolygon(coords,1);
}

/** searchAround */
bool Autosearch::searchAround(int x, int y, int size)
{
        int minY = max(y-size/2,0);
        int maxY = min(y+size/2,height-1);
        int     minX = max(x-size/2,0);
        int     maxX = min(x+size/2, width-1);
        for(int j = minY; j < maxY; j++)
				{
                for (int i = minX; i < maxX; i++)
								{
                                if ( map[j * width + i] == NOT_SEEN)
                                        return true;
								}
				}
        return false;
}

/**
 * Connects lines on the map, given a series of GPS coordinates.
 *
 * @param coords The GPS coordinates of the vertexes that will define the lines
 * @param value	 The number that will be used to draw the line.
 */
void Autosearch::connectLines(array<GPSCoord ^> ^coords, short value)
{
	ArrayCoord ^leadingArrayCoord;
	ArrayCoord ^trailingArrayCoord = gpsToArrayCoord(coords[0]);
	for (int i = 1; i < coords->Length; i++){
		leadingArrayCoord = gcnew ArrayCoord(gpsToArrayCoord(coords[i]));
		drawLine(trailingArrayCoord,leadingArrayCoord,value);
		trailingArrayCoord = gcnew ArrayCoord(leadingArrayCoord);
	}

	drawLine(leadingArrayCoord,gpsToArrayCoord(coords[0]),value);
}

/**
 * Draws a line on the map
 *
 * @param start	The starting point of the line.
 * @param stop	The ending point of the line.
 * @param value	The number that will be used to draw the line.
 *
 */
void Autosearch::drawLine(ArrayCoord ^ start, ArrayCoord ^ stop, short value)
{
	// Establish starting and stopping points
	int startX = start->x, startY = start->y, stopX = stop->x, stopY = stop->y;

	// The slope of the line ( rise / run)
	double slope;

	// Get slope (ensure that div / 0 does not occur)
	if ( stopX != startX){
		slope = (double)(stopY - startY)/(double)(stopX - startX);
	}else{
		slope = INT_MAX;
	}

	if (abs(slope) < 1){
		if ( startX < stopX ){
			for (int x = startX; x <= stopX; x++){
				int y = (int)(startY + (x - startX) * slope + 0.5);
				if ( x < 0 ) x = 0;
				if ( x >= width ) x = width -1;
				if ( y >= height ) y = height - 1;
				if ( y < 0 ) y = 0;
				map[width * y + x] = value;
			}
		}
		else{
			for (int x = startX; x >= stopX; x--){
				int y = (int)(startY + (x - startX) * slope + 0.5);
				if ( x < 0 ) x = 0;
				if ( x >= width ) x = width -1;
				if ( y >= height ) y = height - 1;
				if ( y < 0 ) y = 0;
				map[width * y + x] = value;
			}
		}
	}else{
		if ( startY < stopY ){
			for (int y = startY; y <= stopY; y++){
				int x = (int)(startX + (y - startY) / slope + 0.5);
				if ( x < 0 ) x = 0;
				if ( x >= width ) x = width -1;
				if ( y >= height ) y = height - 1;
				if ( y < 0 ) y = 0;
				map[width * y + x] = value;
			}
		}
		else{
			for (int y = startY ; y >= stopY; y--){
				int x = (int)(startX + (y - startY) / slope + 0.5);
				if ( x < 0 ) x = 0;
				if ( x >= width ) x = width -1;
				if ( y >= height ) y = height - 1;
				if ( y < 0 ) y = 0;
				map[width * y + x] = value;
			}
		}
	}
}

/**
 * Fills in the a region of the map bounded by [value] with [value]
 * Ensure that the region is actually bounded, or this could fill up the whole
 * map.
 * @param x	The x coordinate of the initial element
 * @param y The y coordinate of the initial element
 * @param value	The value that is used to fill the map.
 */
void Autosearch::fill(int x, int y, short value)
{
	ArrayCoord^ currentCoord;
	Queue ^coords = gcnew Queue();
	coords->Enqueue(gcnew ArrayCoord(x,y));
	while (coords->Count > 0){
		currentCoord = (ArrayCoord^)coords->Dequeue();
		x = currentCoord->x;
		y = currentCoord->y;
		if ( x < 0 || x >= width || y < 0 || y >= height)
			continue;
		if (map[y * width + x] != value){
			if ( x > 0 )
				coords->Enqueue(gcnew ArrayCoord(x-1,y));
			if ( y > 0 )
				coords->Enqueue(gcnew ArrayCoord(x,y-1));
			if ( x < width -1 )
				coords->Enqueue(gcnew ArrayCoord(x+1,y));
			if ( y < height - 1 )
				coords->Enqueue(gcnew ArrayCoord(x,y+1));
			map[y * width + x] = value;
		}
	}
}

/**
 * Generates the bounds (GPS coordinates) of the map.
 * 
 * @param coords	The coordinates of the area that needs to be searched.
 * @return			An array of 4 doubles (GPS) that form the border of the map:
 *					[0]			Bottom
 *					[1]			Top
 *					[2]			Left
 *					[3]			Right
 */
array<double>^ Autosearch::getBounds(array<GPSCoord^> ^coords)
{
	// Set first coordinate to be the bottommost,topmost, leftmost, rightmost
	double bottomBound = coords[0]->lat;
	double topBound = coords[0]->lat;
	double leftBound = coords[0]->lon;
	double rightBound = coords[0]->lon;

	// If any coord in coords has a better value for bottom,top,left,right, it
	// will replace that value.
	for(int i = 1; i < coords->Length; i ++){
		if (coords[i]->lat < bottomBound){
			bottomBound = coords[i]->lat;
		}
		if (coords[i]->lat > topBound){
			topBound = coords[i]->lat;
		}
		if (coords[i]->lon < leftBound){
			leftBound = coords[i]->lon;
		}
		if (coords[i]->lon > rightBound){
			rightBound = coords[i]->lon;
		}
	}

	array<double>^result = gcnew array<double>(4);
	result[0] = bottomBound - BORDER;
	result[1] = topBound + BORDER;
	result[2] = leftBound - BORDER;
	result[3] = rightBound + BORDER;
	return result;
}

/**
 * Gets the array coordinate on the map that corresponds to a GPS coordinate.
 *
 * @param coord		The GPS coordinate
 * @return			The corresponding map coordinate.
 */
ArrayCoord^ Autosearch::gpsToArrayCoord(GPSCoord^ coord)
{
	return gcnew ArrayCoord(
					 (int)(GPSCoord::GPSToMeters(coord->lon - leftBound)),((int)(GPSCoord::GPSToMeters(coord->lat - bottomBound))));
}

/**
 * Marks a polygon as having been seen.
 *
 * @param coords	The coordinates describing the vertices of this polygon
 */
void Autosearch::markPolygon(array<GPSCoord^> ^coords, short value)
{
	// Convert the GPS coordinates into array coordinates.
	array<ArrayCoord^> ^aCoords = gcnew array<ArrayCoord^>(coords->Length);
	for (int i = 0; i < coords->Length; i++){
		aCoords[i] = gpsToArrayCoord(coords[i]);
	}

	if (value == 2 && area(aCoords) > MAX_MARKABLE_AREA){
		return;
	}
	// Get the center of the polygon
	ArrayCoord ^center = gcnew ArrayCoord(ArrayCoord::getCenterCoord(aCoords));
	if ( center->x < 0 || center->x >= width || center->y < 0 || center->y >= height){
		PRINT("Warning: Center of polygon to mark is outside of map bounds");
		return;
	}

	// Connect the lines of the polygon
	connectLines(coords,99);



	// Fill the polygon (from the center)
	fill(center->x,center->y,99);

	int bottomBound = 0, leftBound = 0, rightBound = aCoords[0]->x, topBound = aCoords[0]->y;
	for(int i = 1; i < aCoords->Length; i ++){
		if (aCoords[i]->y < bottomBound){
			bottomBound = aCoords[i]->y;
		}
		if (aCoords[i]->y > topBound){
			topBound = aCoords[i]->y;
		}
		if (aCoords[i]->x < leftBound){
			leftBound = aCoords[i]->x;
		}
		if (aCoords[i]->x > rightBound){
			rightBound = aCoords[i]->x;
		}
	}

	// error check
	if (bottomBound < 0) bottomBound = 0;
	if (leftBound < 0) leftBound = 0;
	if (topBound >= height) topBound = height-1;
	if (rightBound >= width) rightBound = width-1;

	int offset,i;
	for (int y = bottomBound; y < topBound; y ++){
		offset = width * y;
		for ( int x = leftBound; x < rightBound; x ++){
			i = offset + x;
			if (i < height * width)
				if (map[i] == 99) map[i] = value;
		}
	}
}

/**
 * Updates the picture that represents the map.
 * Ignored -- Red
 * Unseen -- Blue
 * Seen -- Green
 * 
 */

void Autosearch::updateImage()
{
	Bitmap ^ image = gcnew Bitmap(width,height);
	for (int y = 0; y < height; y++){
		for (int x  = 0; x < width; x++){
			switch(map[y * width + x]){
			case 0:
				// Does not need to be seen
				image->SetPixel(x,height - y - 1,Color::DimGray);
				break;
			case 1:
				// Needs to be seen
				image->SetPixel(x,height - y - 1,Color::White);
				break;
			case 2:
				// Seen
				image->SetPixel(x, height - y - 1,Color::Black);
				break;
			}
		}
	}
	if (intelligenceController != nullptr)
	{
		intelligenceController->displayAutosearchImage(image);
	}
	else
	{
		PRINT("ERROR: in Autosearch::updateImage(): intelligenceController is null");
	}
}

void Autosearch::queryCameraAndUpdateMapAndImage()
{
	try{
		while (!shouldDie)
		{
			Thread::Sleep(500);

			if (planeWatcher == nullptr)
			{
				PRINT("ERROR in Autosearch::queryCameraAndUpdateMapAndImage() planeWatcher is null");
				continue;
			}

			ImageWithPlaneData ^ planeState = planeWatcher->getState();

			double BLLat, BLLon, BRLat, BRLon, TRLat, TRLon, TLLat, TLLon;
			GeoReference::getCorners(planeState, BLLat,  BLLon,  BRLat,  BRLon,  TRLat,  TRLon, TLLat,  TLLon);
			array<GPSCoord ^> ^ coords = gcnew array<GPSCoord ^>(4);
			coords[0] = gcnew GPSCoord(BLLat,BLLon);
			coords[1] = gcnew GPSCoord(BRLat,BRLon);
			coords[2] = gcnew GPSCoord(TRLat,TRLon);
			coords[3] = gcnew GPSCoord(TLLat,TLLon);
			markPolygon(coords,2);
			updateImage();

		}
	}catch(Exception ^e){
		PRINT("EXCEPTION IN Autosearch .... ");
		throw e;
	}
	PRINT("Autosearch updater thread ended");
}

//array<array<int>^>^ getPositionsWithValue(int value){
//	ArrayList^ coordList = gcnew ArrayList(20);
//	for (int y = 0; y < height; y++){
//		for (int x  = 0; x < width; x++){
//			if (map[y * width + x] == value){
//				coords->Add(array<int>{x,y});
//			}
//		}
//	}
//	array<array<int>^>^ coords = gcnew array<array<int>^
//	return Enumurable::ToArray(coords);
//}


void Autosearch::startUpdaterThread()
{
	updaterThread = gcnew Thread(gcnew ThreadStart(this,&Autosearch::queryCameraAndUpdateMapAndImage));
	updaterThread->Name = "Autosearch Updater Thread";
	updaterThread->Start();
}

void Autosearch::abortUpdaterThread()
{
	shouldDie = true;
}

/**
 * Saves the image to a file
 *
 * @param filename	The name of the file that this image will be saved to.
 */
//void Autosearch::saveImage(String ^ filename)
//{
//	// Save the bitmap
//	image->Save(filename);
//}

void Autosearch::setPlaneWatcher (PlaneWatcher ^ planeWatcher)
{
	this->planeWatcher = planeWatcher;
}



/////////////////// BELOW HERE GOOD //////////////////////

/**
 * Generates a String that represents the map. Useful for debugging.
 */
String^ Autosearch::toString()
{
	String^ result = "";
	for (int y = 0; y < height; y++){
		for (int x = 0; x < width; x++){
			result += map[y * width + x];
		}
		result += '\n';
	}
	return result;
}

/**
 * Constructor for ArrayCoord
 *
 * @param x		The x coordinate (meters)
 * @param y		The y coordinate (meters)
 */
ArrayCoord::ArrayCoord(int x, int y)
{
	this->x = x;
	this->y = y;
}

/**
 * Copy Constructor for ArrayCoord
 *
 * @param other	The ArrayCoord that this will copy
 */
ArrayCoord::ArrayCoord(ArrayCoord ^other)
{
	x = other->x;
	y = other->y;
}

/**
 * Creates a coordinate that lies at the center of an array of other coordinates.
 *
 * @param coords An array of coordinates
 */
ArrayCoord^ ArrayCoord::getCenterCoord(array<ArrayCoord^>^coords)
{
	double x = 0, y = 0;
	for (int i = 0; i < coords->Length; i++){
		x += coords[i]->x;
		y += coords[i]->y;
	}
	ArrayCoord^ result = gcnew ArrayCoord((int) (x / coords->Length + .5), (int) (y / coords->Length +.5));
	return result;
}

/**
 * Constructor for a GPS coordinate. No altitude is given, so it is set to zero.
 *
 * @param lat	The latitude (degrees)
 * @param lon	The longitude (degrees)
 */
GPSCoord::GPSCoord(double lat, double lon)
{
	this->lat = lat;
	this->lon = lon;
}

/**
 * Constructor for a GPS coordinate.
 *
 * @param lat	The latitude (degrees)
 * @param lon	The longitude (degrees)
 * @param alt	The altitude (feet)
 */
GPSCoord::GPSCoord(double lat, double lon, double alt)
{
	this->lat = lat;
	this->lon = lon;
	this->alt = alt;
}

/**
 * Determines the displacement in meters that corresponds to a displacement
 * in GPS degrees.
 * TODO: Make this more accurate.
 * 
 * @param gps	Displacement in gps degrees
 * @return		Corresponding displacement in meters
 */
double GPSCoord::GPSToMeters(double gps)
{
	return gps * 111122.0;
}

/**
 * Determines the displacement in GPS degrees that corresponds to a displacement
 * in meters.
 * TODO: Make this more accurate.
 * 
 * @param meters Displacement in meters
 * @return		 Corresponding displacement in GPS degrees.
 */
double GPSCoord::metersToGPS(double meters)
{
	return meters / 111122.0;
}
