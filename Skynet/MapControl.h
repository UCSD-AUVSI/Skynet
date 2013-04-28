#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace System::Threading;
using namespace System::Net;

#include "Delegates.h"
#include "MasterHeader.h"

#ifndef OPENCV_DISABLED
#include "GeoReference.h"
#endif

#include <math.h>


namespace Skynet {

	const int OSMTileWidth = 256;
	const int OSMTileHeight = 256;
	const int OSMHalfTileWidth = 128;
	const int OSMHalfTileHeight = 128;
	const int OSMMaxZoom = 18;
	const int OSMMinZoom = 0;

	const int MapHitPosition = -1;

	enum TileProvider
	{
		Mapnik,
		Osmarender,
		Cyclemap
	};

	enum DownloadOrder
	{
		ClosestToCenterFirst,
		YOuterXInnerLeftToRight
	};

	ref struct Target : IEquatable<Target ^> // Target * theTarget;
	{
		Target( double lat, double lon, int id )
		{
			_latitude = lat;
			_longitude = lon;
			_id = id;			
		}

		void setRect( int x, int y, int width, int height )
		{
			_rect = gcnew Drawing::Rectangle( x, y, width, height );
		}

		double _latitude;
		double _longitude;
		int _id;
		Drawing::Rectangle ^ _rect;

		virtual bool Equals(Target ^ other) 
		{
			if( this->_id == other->_id )
				return true;
			else
				return false;
		}
	};

	static void generateTargets()
	{
		Image ^ curImage = gcnew Bitmap( 64, 64 );		// make new image
		Graphics ^ g = Graphics::FromImage( curImage ); // get graphics context
		g->TextRenderingHint = System::Drawing::Text::TextRenderingHint::SingleBitPerPixel;
		// all valid letters
		array<String ^> ^ letters = {	"#", "A", "0", "1", "2", "3","4", "5","6", "7","8", "9", "B", "C", "D", "E","F","G",
										"H", "I","J", "K","M", "N","O", "P","Q", "R","S", "T", "U", "V","W", "X","Y", "Z"};		
		// all valid shapes (first 4 characters of each)
		array<String ^> ^ shapes = {	"CIRC", "CROS", "DIAM", "HEXA", "PENT", "RECT", "SEMI", "SQUA", "STAR", "TRAP", "TRIA" };	
		// get lengths
		int numLetters = letters->GetLength(0);
		int numShapes = shapes->GetLength(0);

		static int wildCard = 0;

		Pen ^ penBlack = gcnew Pen( Color::Black, 1.0f );

		//Draw the text for the tile not available bar
		SolidBrush ^ fontBrush = gcnew SolidBrush( Color::White ); // letter
		SolidBrush ^ shapeBrush = gcnew SolidBrush( Color::Red ); // letter
		System::Drawing::Font^ drawFont = gcnew System::Drawing::Font( "Arial", 16.0f );
		System::Drawing::StringFormat^ drawFormat = gcnew StringFormat;
		drawFormat->Alignment = System::Drawing::StringAlignment::Center;
		drawFormat->LineAlignment = StringAlignment::Center;

		array<Point> ^ trapOne = {	Point(17, 8), 
									Point(44, 7), 
									Point(51, 52), 
									Point(8, 52)};

		array<Point> ^ trapTwo = {	Point(15, 11), 
									Point(46, 11), 
									Point(54, 53), 
									Point(4, 52)};

		array<Point> ^ triOne = {	Point(31, 3), 
									Point(57, 51), 
									Point(5, 51)};

		array<Point> ^ triTwo = {	Point(28, 9), 
									Point(57, 30), 
									Point(4, 55)};

		array<Point> ^ triThree = {	Point(4, 43), 
									Point(30, 4), 
									Point(57, 43)};

		array<Point> ^ hexFat = {	Point(8, 19), 
									Point(34, 7), 
									Point(60, 19), 
									Point(60, 42),
									Point(34, 54), 
									Point(8, 42)};

		array<Point> ^ diamondSkinny = {Point(13, 29), 
									Point(32, 1), 
									Point(51, 30), 
									Point(32, 58)};

		array<Point> ^ diamondFat = {Point(3, 31), 
									Point(32, 13), 
									Point(60, 31), 
									Point(32, 49)};

		array<Point> ^ square = {	Point(6, 6), 
				                  	Point(54, 6), 
				                  	Point(55, 55), 
				                  	Point(6, 54)};

		array<Point> ^ square2 = {  Point(8, 10), 
		                  			Point(50, 10), 
				             		Point(50, 51), 
				         			Point(10, 50)};

		array<Point> ^ star = {  	Point( 6, 22), 
									Point(26, 22), 
									Point(33, 2), 
									Point(38, 22),
									Point(57, 22),
									Point(42, 36),
									Point(47, 53),
									Point(32, 43),
									Point(17, 55),
									Point(23, 36)};

		array<Point> ^ pentEquil = {  	Point(5, 24), 
										Point(32, 4), 
										Point(58, 24), 
										Point(48, 57),
										Point(15, 57)};

		array<Point> ^ pentSkinny = {  Point(11, 25), 
										Point(32, 4), 
										Point(51, 24), 
										Point(43, 57),
										Point(19, 57)};

		array<Point> ^ pentFat = {  Point(3, 25), 
										Point(31, 9), 
										Point(59, 25), 
										Point(48, 50),
										Point(14, 50)};

		array<Point> ^ hexEqual = {  Point(32, 7), 
										Point(55, 19), 
										Point(55, 43), 
										Point(32, 55),
										Point(7, 43),
										Point(7, 19)};


		// loop through letters
		int pass = -1;
		for (int i = 0; i < numLetters; i++) 
		{
			String ^ letter = letters[i];			// letter to draw
			String ^ shape = shapes[i % numShapes];	// shape to draw


			// clear graphics context
			g->Clear(Color::Black);

			// draw shape
			switch (i % numShapes)
			{
				case 0: // circle
					pass ++;
					if (pass == 0)
						g->FillEllipse(shapeBrush, 5, 5, 54, 54);
					else
						g->FillEllipse(shapeBrush, 15, 15, 34, 34);
					break;
				case 1:	// cross
					g->FillRectangle(shapeBrush, 4, 22, 56, 20);
					g->FillRectangle(shapeBrush, 22, 8, 20, 48);
					break;
				case 2:	// diamond
					if (pass == 0)
						g->FillPolygon(shapeBrush, diamondSkinny);
					else
						g->FillPolygon(shapeBrush, diamondFat);
					break;
				case 3:	// hexagon
					if (pass == 0)
						g->FillPolygon(shapeBrush, hexEqual);
					else
						g->FillPolygon(shapeBrush, hexFat);
					break;
				case 4:	// pentagon
					if (pass == 0)
						g->FillPolygon(shapeBrush, pentEquil);
					else if (pass == 1)
						g->FillPolygon(shapeBrush, pentSkinny);
					else
						g->FillPolygon(shapeBrush, pentFat);
					break;
				case 5:	// rectangle
					if (pass == 0)
						g->FillRectangle(shapeBrush, 17, 8, 30, 45);
					else 
						g->FillRectangle(shapeBrush, 22, 8, 20, 45);
					break;
				case 6:	// semi circle
					if (pass == 0)
						g->FillPie(shapeBrush, 12.0f, 10.0f, 70.0f, 40.0f, 90.0f, 180.0f);	// fat
					else
						g->FillPie(shapeBrush, 19.0f, 8.0f, 50.0f, 45.0f, 90.0f, 180.0f); // skinny
					break;
				case 7:	// square
					if (pass == 0)
						g->FillPolygon(shapeBrush, square);
					else
						g->FillPolygon(shapeBrush, square2);
					break;
				case 8:	// star
					g->FillPolygon(shapeBrush, star);
					break;
				case 9:	// trapezoid
					if (pass == 0)
						g->FillPolygon(shapeBrush, trapOne);
					else
						g->FillPolygon(shapeBrush, trapTwo);
					break;
				case 10:// triangle
					if (pass == 0)
						g->FillPolygon(shapeBrush, triOne);
					else if (pass == 1)
						g->FillPolygon(shapeBrush, triTwo);
					else
						g->FillPolygon(shapeBrush, triThree);
					break;
			}

			// draw letter
			g->DrawString( letter, drawFont, fontBrush, System::Drawing::RectangleF( 0, 0, 64, 64 ), drawFormat );

			// save to file: letter_shape_rand.bmp
			String ^ filename = "C:\\Users\\UCSD\\Pictures\\targets artificial\\" + letter + "_" + shape + "_" + pass + ".bmp";
			curImage->Save( filename );

		}
	}

	static Bitmap ^ RotateImage(Image ^ image, double angle, bool highDetail)
        {
            double pi2 = PI / 2.0;

            // Why can't C# allow these to be const, or at least readonly
            // *sigh*  I'm starting to talk like Christian Graus :omg:
            double oldWidth = (double)image->Width;
            double oldHeight = (double)image->Height;

            // Convert degrees to radians
            double theta = angle * PI / 180.0;
            double locked_theta = theta;

			// DEBUG: this fixes a problem with the while()
			if (locked_theta < 1000)
				locked_theta = 0;

            // Ensure theta is now [0, 2pi)
            while (locked_theta < 0.0)
                locked_theta += 2.0 * PI;

            double newWidth, newHeight;
            int nWidth, nHeight; // The newWidth/newHeight expressed as ints

            double adjacentTop, oppositeTop;
            double adjacentBottom, oppositeBottom;

            // We need to calculate the sides of the triangles based
            // on how much rotation is being done to the bitmap.
            //   Refer to the first paragraph in the explaination above for 
            //   reasons why.
            if ((locked_theta >= 0.0 && locked_theta < pi2) ||
                (locked_theta >= PI && locked_theta < (PI + pi2)))
            {
                adjacentTop = Math::Abs(Math::Cos(locked_theta)) * oldWidth;
                oppositeTop = Math::Abs(Math::Sin(locked_theta)) * oldWidth;

                adjacentBottom = Math::Abs(Math::Cos(locked_theta)) * oldHeight;
                oppositeBottom = Math::Abs(Math::Sin(locked_theta)) * oldHeight;
            }
            else
            {
                adjacentTop = Math::Abs(Math::Sin(locked_theta)) * oldHeight;
                oppositeTop = Math::Abs(Math::Cos(locked_theta)) * oldHeight;

                adjacentBottom = Math::Abs(Math::Sin(locked_theta)) * oldWidth;
                oppositeBottom = Math::Abs(Math::Cos(locked_theta)) * oldWidth;
            }

            newWidth = adjacentTop + oppositeBottom;
            newHeight = adjacentBottom + oppositeTop;

            nWidth = (int)newWidth;
            nHeight = (int)newHeight;

            Bitmap ^ rotatedBmp = gcnew Bitmap(nWidth, nHeight);

            Graphics ^ g = Graphics::FromImage(rotatedBmp);
			g->PixelOffsetMode = Drawing2D::PixelOffsetMode::HighQuality;

            if (highDetail)
            {
				g->SmoothingMode = Drawing2D::SmoothingMode::HighQuality;
				g->InterpolationMode = Drawing2D::InterpolationMode::HighQualityBicubic;
            }
            else
            {
                g->SmoothingMode = Drawing2D::SmoothingMode::None;
                g->InterpolationMode = Drawing2D::InterpolationMode::Low;
            }

            // This array will be used to pass in the three points that 
            // make up the rotated image
			array<PointF> ^ points;

            /*
             * The values of opposite/adjacentTop/Bottom are referring to 
             * fixed locations instead of in relation to the
             * rotating image so I need to change which values are used
             * based on the how much the image is rotating.
             * 
             * For each point, one of the coordinates will always be 0, 
             * nWidth, or nHeight.  This because the Bitmap we are drawing on
             * is the bounding box for the rotated bitmap.  If both of the 
             * corrdinates for any of the given points wasn't in the set above
             * then the bitmap we are drawing on WOULDN'T be the bounding box
             * as required.
             */
            if (locked_theta >= 0.0 && locked_theta < pi2)
            {
                points = gcnew array<PointF> { 
										 PointF( (float)oppositeBottom, 0.0 ), 
										 PointF( (float)newWidth, (float)oppositeTop ),
										 PointF( 0.0, (float) adjacentBottom )
									 };

            }
            else if (locked_theta >= pi2 && locked_theta < PI)
            {
                points = gcnew array<PointF> { 
										 PointF( (float)newWidth, (float) oppositeTop ),
										 PointF( (float) adjacentTop, (float)newHeight ),
										 PointF( (float) oppositeBottom, 0.0 )						 
									 };
            }
            else if (locked_theta >= PI && locked_theta < (PI + pi2))
            {
                points = gcnew array<PointF> { 
										 PointF( (float) adjacentTop, (float)newHeight ), 
										 PointF( 0.0, (float) adjacentBottom ),
										 PointF( (float)newWidth, (float)oppositeTop )
									 };
            }
            else
            {
                points = gcnew array<PointF> { 
										 PointF( 0.0, (float) adjacentBottom ), 
										 PointF( (float) oppositeBottom, 0.0 ),
										 PointF( (float) adjacentTop, (float)newHeight )		
									 };
            }
            g->DrawImage(image, points);

            return rotatedBmp;
        }

	public ref class MapControlDownloadTileItem : public IComparable
	{
	public:
		MapControlDownloadTileItem(void)
		{
			_nTileX = -1;
			_nTileY = -1;
			_nX = -1;
			_nY = -1;
			_distanceFromCenterPosition = 0;
		}

		MapControlDownloadTileItem( int tileX, int tileY, int nX, int nY, double distanceFromCenterPosition, String ^ file )
		{
			_nTileX = tileX;
			_nTileY = tileY;
			_nX = nX;
			_nY = nY;
			_distanceFromCenterPosition = distanceFromCenterPosition;
			_file = file;
		}

		MapControlDownloadTileItem% operator=(const MapControlDownloadTileItem% other)
		{
			_nTileX = other._nTileX;
			_nTileY = other._nTileY;
			_nX = other._nX;
			_nY = other._nY;
			_distanceFromCenterPosition = other._distanceFromCenterPosition;
			_file = other._file;

			return *this;
		}

		virtual int CompareTo( Object ^ obj )
		{
			if ( this->_distanceFromCenterPosition > dynamic_cast<MapControlDownloadTileItem^>(obj)->_distanceFromCenterPosition )
				return 1;
			else if ( this->_distanceFromCenterPosition < dynamic_cast<MapControlDownloadTileItem^>(obj)->_distanceFromCenterPosition )
				return -1;
			else
				return 0;
		}

		int _nTileX;
		int _nTileY;
		int _nX;
		int _nY;
		double _distanceFromCenterPosition;
		String ^ _file;		
	};

	/// <summary>
	/// Summary for MapControl
	/// </summary>
	public ref class MapControl : public System::Windows::Forms::UserControl
	{
	public:
		MapControl(void)
		{
			InitializeComponent();

			// Default values
			_zoom = 15;
			// Webster field: 38.145425, -76.427765
			_centerLongitude = -76.453841;
			_centerLatitude = 38.145425;
			//_airplaneLatitude = _centerLatitude;
			//_airplaneLongitude = _centerLongitude;
			_airplaneLatitude = 38.14412;
			_airplaneLongitude = -76.42849;
			_airplaneHeading = -135;

			//_airplaneNewLatitude = _airplaneLatitude;
			//_airplaneNewLongitude = _airplaneLongitude;

			_tileProvider = Mapnik;
			_downloadOrder = ClosestToCenterFirst;
			_cacheDirectory = "D:\\Skynet Files\\Map Cache";
			_downloadTiles = false;
			_allowPreviousZoomStretch = true;
			_allowNextZoomSqueeze = true;
			_airplaneIcon = Image::FromFile( DROPBOX_DIR + "Skynet\\Skynet\\Falco_small.png" );
			_targetIcon = Image::FromFile( DROPBOX_DIR + "Skynet\\Skynet\\thumbtack_icon.png" );
			_redrawDelegate = gcnew Delegates::voidToVoid( this, &MapControl::redraw );
			_uiLock = false;
			_targets = gcnew Generic::List<Target ^>;
			_cameraPoints = gcnew array<Point>(4);

			for( int i = 0; i < 4; ++i )
				_cameraPoints[i] = Point(0, 0);

			// Goldenrod
			_opAreaPoints = gcnew array<PointF>(12);
			_opAreaPoints[0].X = 38.145400f; _opAreaPoints[0].Y = -76.431586f;
			_opAreaPoints[1].X = 38.143425f; _opAreaPoints[1].Y = -76.435358f;
			_opAreaPoints[2].X = 38.140394f; _opAreaPoints[2].Y = -76.432628f;
			_opAreaPoints[3].X = 38.141569f; _opAreaPoints[3].Y = -76.429547f;
			_opAreaPoints[4].X = 38.139789f; _opAreaPoints[4].Y = -76.426203f;
			_opAreaPoints[5].X = 38.141606f; _opAreaPoints[5].Y = -76.423592f;
			_opAreaPoints[6].X = 38.144108f; _opAreaPoints[6].Y = -76.425353f;
			_opAreaPoints[7].X = 38.145283f; _opAreaPoints[7].Y = -76.423389f;
			_opAreaPoints[8].X = 38.147111f; _opAreaPoints[8].Y = -76.424292f;
			_opAreaPoints[9].X = 38.145817f; _opAreaPoints[9].Y = -76.42705f;
			_opAreaPoints[10].X = 38.151325f; _opAreaPoints[10].Y = -76.429144f;
			_opAreaPoints[11].X = 38.151094f; _opAreaPoints[11].Y = -76.434708f;

			// Gold
			_searchAreaPoints = gcnew array<PointF>(7);
			_searchAreaPoints[0].X = 38.14465f; _searchAreaPoints[0].Y = -76.431069f;
			_searchAreaPoints[1].X = 38.143239f; _searchAreaPoints[1].Y = -76.43435f;
			_searchAreaPoints[2].X = 38.140992f; _searchAreaPoints[2].Y = -76.432503f;
			_searchAreaPoints[3].X = 38.142022f; _searchAreaPoints[3].Y = -76.429306f;
			_searchAreaPoints[4].X = 38.14015f; _searchAreaPoints[4].Y = -76.426256f;
			_searchAreaPoints[5].X = 38.141797f; _searchAreaPoints[5].Y = -76.424075f;
			_searchAreaPoints[6].X = 38.143867f; _searchAreaPoints[6].Y = -76.425872f;

			// Blue

			// Various event callbacks
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler( this, &MapControl::paintMap );
         	this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler( this, &MapControl::onMouseWheel );			
        	this->MouseUp += gcnew System::Windows::Forms::MouseEventHandler( this, &MapControl::onMouseUp );
			this->MouseDown += gcnew System::Windows::Forms::MouseEventHandler( this, &MapControl::onMouseDown );
			this->MouseMove += gcnew System::Windows::Forms::MouseEventHandler( this, &MapControl::onMouseMove );

			// Reduce flicker
			this->DoubleBuffered = true;
		}

		// getters and setters
		bool GetXYPosition( double longitude, double latitude, int & x, int & y )
		{
			bool retVal = true;

			//Get the extent of the client area
			int height = this->Size.Height;
			int width = this->Size.Width;
			double dMaxTile = pow(2.0, _zoom);

			double dY = Latitude2TileY( latitude, _zoom );
			double dX = Longitude2TileX( longitude, _zoom );

			//// UnWrap the longitude if necessary
			//if( dX + dMaxTile >= dMaxTile )
			//	dX += dMaxTile;
			//if( dX - dMaxTile < 0 )
			//	dX -= dMaxTile;

			//double centerX = Longitude2TileX(_centerLongitude, _zoom);
			//double centerY = Latitude2TileY(_centerLatitude, _zoom);
			double centerX = dX;
			double centerY = dY;

			x = (int)(( dX - centerX ) * 256.0 + width/2.0);
			y = (int)(( dY - centerY ) * 256.0 + height/2.0);

			return retVal;
		}

		bool GetPosition( int x, int y, double & longitude, double & latitude )
		{
			//What will be the return value from this function assume the worst
			bool retVal = false;

			//Get the extent of the client area
			int height = this->Size.Height;
			int width = this->Size.Width;

			double dX = Longitude2TileX(_centerLongitude, _zoom);
			double dY = Latitude2TileY(_centerLatitude, _zoom);
			dX += ((x - width/2.0) / 256.0);
			dY += ((y - height/2.0) / 256.0);
			double dMaxTile = pow(2.0, _zoom);

			//Wrap the longitude if necessary
			if (dX < 0)
				dX += dMaxTile;
			if (dX >= dMaxTile)
				dX -= dMaxTile;

			if (dX >= 0 && dX <= dMaxTile && dY >= 0 && dY <= dMaxTile)
			{
				retVal = TRUE;
				latitude = TileY2Latitude(dY, _zoom);
				longitude = TileX2Longitude(dX, _zoom);
				//int temp1, temp2;
				//GetXYPosition( longitude, latitude, temp1, temp2 );
				//System::Diagnostics::Trace::WriteLine("In: " + x + " " + y + " Out: " + temp1 + " " + temp2);
			}

			return retVal;  
		}

		String ^ GetPositionString( int x, int y )
		{
			double dLongitude, dLatitude;

			if(	!GetPosition( x, y, dLongitude, dLatitude ) )
				return "Error";

    		double dAbsLatitude = fabs(dLatitude);
		    int nLatitudeD = static_cast<int>(dAbsLatitude);
		    int nLatitudeM = static_cast<int>((dAbsLatitude - nLatitudeD) * 60);
		    int nLatitudeS = static_cast<int>((dAbsLatitude - nLatitudeD - nLatitudeM/60.0) * 3600);
			String ^ pszLatitudeType = (dLatitude > 0) ? "N" : "S";

		    double dAbsLongitude = fabs(dLongitude);
		    int nLongitudeD = static_cast<int>(dAbsLongitude);
		    int nLongitudeM = static_cast<int>((dAbsLongitude - nLongitudeD) * 60);
		    int nLongitudeS = static_cast<int>((dAbsLongitude - nLongitudeD - nLongitudeM/60.0) * 3600);
    		String ^ pszLongitudeType = (dLongitude > 0) ? "E" : "W";

			return nLatitudeD + "° " + nLatitudeM + "' " + nLatitudeS + "\" " + pszLatitudeType + "  "  + nLongitudeD + "° " + nLongitudeM + "' " + nLongitudeS + "\" " + pszLongitudeType;
			//return dLatitude + " " + dLongitude;
		}

		int getZoom(void)
		{
			return _zoom;
		}

		bool setZoom( int zoom, Point newCenter )
		{
			// Validate input
			if (zoom < OSMMinZoom || zoom > OSMMaxZoom)
				return false;

			double newLat, newLon;
			GetPosition( newCenter.X, newCenter.Y, newLon, newLat );

			// Save the new setting
			mutex_Data->WaitOne();
			bool bChange = _zoom != zoom;
			_zoom = zoom;
			_centerLongitude = newLon;
			_centerLatitude = newLat;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if( bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}

			return true;
		}

		double getCenterLongitude(void)
		{
			return _centerLongitude;
		}

		double getCenterLatitude(void)
		{
			return _centerLatitude;
		}

		String ^ getCacheDirectory(void)
		{
			return _cacheDirectory;
		}

		static double Longitude2TileX( double longitude, int zoom )
		{
			return (longitude + 180.0) / 360.0 * pow(2.0, zoom); 
		}

		static double Latitude2TileY( double latitude, int zoom )
		{
		  return (1.0 - log(tan(latitude * PI/180.0) + 1.0 / cos(latitude * PI/180.0)) / PI) / 2.0 * pow(2.0, zoom); 
		}

		static double TileX2Longitude( double x, int zoom )
		{
			return x / pow(2.0, zoom) * 360.0 - 180;
		}

		static double TileY2Latitude( double y, int zoom )
		{
		  double n = PI - 2.0 * PI * y / pow(2.0, zoom);
		  return 180.0 / PI * atan(0.5 * (exp(n) - exp(-n)));
		}

		//This algorithm is taken from the authors AA+ class library (http://www.naughter.com/aa.html)
		static double DistanceBetweenPoints( double Latitude1, double Longitude1, double Latitude2, double Longitude2 )
		{
			//Convert from degress to radians
			Latitude1 = Latitude1 / 180 * PI;
			Latitude2 = Latitude2 / 180 * PI;
			Longitude1 = Longitude1 / 180 * PI;
			Longitude2 = Longitude2 / 180 * PI;

			double F = (Latitude1 + Latitude2) / 2;
			double G = (Latitude1 - Latitude2) / 2;
			double lambda = (Longitude1 - Longitude2) / 2;
			double sinG = sin(G);
			double cosG = cos(G);
			double cosF = cos(F);
			double sinF = sin(F);
			double sinLambda = sin(lambda);
			double cosLambda = cos(lambda);
			double S = (sinG*sinG*cosLambda*cosLambda) + (cosF*cosF*sinLambda*sinLambda);
			double C = (cosG*cosG*cosLambda*cosLambda) + (sinF*sinF*sinLambda*sinLambda);
			double w = atan(sqrt(S/C));
			double R = sqrt(S*C)/w;
			double D = 2*w*6378.14;
			double Hprime = (3*R - 1) / (2*C);
			double Hprime2 = (3*R + 1) / (2*S);
			double f = 0.0033528131778969144060323814696721;

			return D * (1 + (f*Hprime*sinF*sinF*cosG*cosG) - (f*Hprime2*cosF*cosF*sinG*sinG));
		}

		void redraw(void)
		{
			this->Refresh();
		}

		TileProvider GetTileProvider(void)
		{
			return _tileProvider;
		}

		void SetTileProvider( TileProvider provider )
		{
			mutex_Data->WaitOne();
			bool bChange = _tileProvider != provider;
			_tileProvider = provider;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}
		}

		void AddTarget( double latitude, double longitude, int id )
		{
			Target ^ toAdd = gcnew Target( latitude, longitude, id );

			if( _targets->Contains( toAdd ) )
			{
				_targets->Remove( toAdd );
			}

			_targets->Add( toAdd );

			if( FindForm() )
			{
				redraw();
			}
		}

		void RemoveTarget( int id )
		{
			Target ^ toRemove = gcnew Target( 0, 0, id );

			if( _targets->Contains( toRemove ) )
			{
				_targets->Remove( toRemove );
			}
		}

		void SetAirplaneLocation( double latitude, double longitude, double heading )
		{
			bool bChange = false;

			if( _airplaneLatitude != latitude || _airplaneLongitude != longitude || _airplaneHeading != heading )
			{
				bChange = true;
				//_airplaneNewLatitude = latitude;
				//_airplaneNewLongitude = longitude;
				// DrawPath();
			}

			_airplaneLatitude = latitude;
			_airplaneLongitude = longitude;
			_airplaneHeading = heading;

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				this->Invoke( this->_redrawDelegate );
			}
		}

		void SetCameraLookLocation( array<float> ^ homography, double airplane_lat, double airplane_lon )
		{
			//double targetLat, targetLon;		
			//bool bChange = false;
			//int x, y;
			
			// THIS REMOVED BECAUSE WE USE GOOGLE EARTH NOW. ALSO, THIS HOMOGRAPHY USES A DIFFERENT GIMBAL
			/*// Apply homography to four image corners
			GeoReference::applyHomography( homography, 0, 0, airplane_lat, airplane_lon, targetLat, targetLon );
			GetXYPosition( targetLon, targetLat, x, y );
			if( _cameraPoints[0].X != x || _cameraPoints[0].Y != y )
				bChange = true;
			_cameraPoints[0].X = x; _cameraPoints[0].Y = y;

			GeoReference::applyHomography( homography, 719, 0, airplane_lat, airplane_lon, targetLat, targetLon );
			GetXYPosition( targetLon, targetLat, x, y );
			if( _cameraPoints[0].X != x || _cameraPoints[0].Y != y )
				bChange = true;
			_cameraPoints[1].X = x; _cameraPoints[1].Y = y;

			GeoReference::applyHomography( homography, 719, 485, airplane_lat, airplane_lon, targetLat, targetLon );
			GetXYPosition( targetLon, targetLat, x, y );
			if( _cameraPoints[0].X != x || _cameraPoints[0].Y != y )
				bChange = true;
			_cameraPoints[2].X = x; _cameraPoints[2].Y = y;

			GeoReference::applyHomography( homography, 0, 485, airplane_lat, airplane_lon, targetLat, targetLon );
			GetXYPosition( targetLon, targetLat, x, y );
			if( _cameraPoints[0].X != x || _cameraPoints[0].Y != y )
				bChange = true;
			_cameraPoints[3].X = x; _cameraPoints[3].Y = y;


			//Force a redraw if required
			if (bChange && FindForm())
			{
				//this->Invoke( this->_redrawDelegate );
			}*/
		}

		bool SetCenterLongitude( double dCenterLongitude )
		{
			//Fix up input parameter values
			while (dCenterLongitude < -180)
				dCenterLongitude += 360;
			while (dCenterLongitude > 180)
				dCenterLongitude -= 360;

			//Fail any invalid input value
			double dX = Longitude2TileX(dCenterLongitude, _zoom);
			if (dX < 0 || dX > pow(2.0, _zoom))
				return false;

			//Hive away the new setting
			// Save new setting
			mutex_Data->WaitOne();
			bool bChange = dCenterLongitude != _centerLongitude;
			_centerLongitude = dCenterLongitude;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}

			return true;
		}

		bool SetCenterLatitude( double dCenterLatitude )
		{
			//Fail any invalid input value
			double dY = Latitude2TileY(dCenterLatitude, _zoom);
			if (dY < 0 || dY > pow(2.0, _zoom))
				return false;

			// Save new setting
			mutex_Data->WaitOne();
			bool bChange = dCenterLatitude != _centerLatitude;
			_centerLatitude = dCenterLatitude;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}

			return true;

		}

		bool SetCenterPosition( double dCenterLongitude, double dCenterLatitude )
		{
			//Fix up input parameter values
			while (dCenterLongitude < -180)
				dCenterLongitude += 360;
			while (dCenterLongitude > 180)
				dCenterLongitude -= 360;

			//Fail any invalid input value
			double dMaxTile = pow(2.0, _zoom);
			double dX = Longitude2TileX(dCenterLongitude, _zoom);
			if (dX < 0 || dX > dMaxTile)
				return false;
			double dY = Latitude2TileY(dCenterLatitude, _zoom);
			if (dY < 0 || dY > dMaxTile)
				return false;

			// Save new setting
			mutex_Data->WaitOne();
			bool bChange = dCenterLongitude != _centerLongitude || dCenterLatitude != _centerLatitude;
			_centerLongitude = dCenterLongitude;
			_centerLatitude = dCenterLatitude;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}

			return true;
		}

		bool SetCenterPosition( double dCenterLongitude, double dCenterLatitude, int nZoom )
		{
			// Validate
			if ( nZoom < OSMMinZoom || nZoom > OSMMaxZoom )
				return false;

			//Fix up input parameter values
			while (dCenterLongitude < -180)
				dCenterLongitude += 360;
			while (dCenterLongitude > 180)
				dCenterLongitude -= 360;

			//Fail any invalid input value
			double dMaxTile = pow(2.0, _zoom);
			double dX = Longitude2TileX(dCenterLongitude, _zoom);
			if (dX < 0 || dX > dMaxTile)
				return false;
			double dY = Latitude2TileY(dCenterLatitude, _zoom);
			if (dY < 0 || dY > dMaxTile)
				return false;

			// Save new setting
			mutex_Data->WaitOne();
			bool bChange = nZoom != _zoom || dCenterLongitude != _centerLongitude || dCenterLatitude != _centerLatitude;
			_zoom = nZoom;
			_centerLongitude = dCenterLongitude;
			_centerLatitude = dCenterLatitude;
			mutex_Data->ReleaseMutex();

			// Download if needed
			if(  bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}

			//Force a redraw if required
			if (bChange && FindForm())
			{
				redraw();
			}

			return true;
		}

		void SetCacheDirectory( String ^ cacheDir, bool invalidateCache )
		{
			// MAP DELETED
			// Save setting
			mutex_Data->WaitOne();
			bool bChange = _cacheDirectory != cacheDir;
			_cacheDirectory = cacheDir;
			mutex_Data->ReleaseMutex();

			// deletes everything in the cache
			if ( invalidateCache )
			{
				System::IO::Directory::Delete( cacheDir, true );
			}  

			// Create the cache directory if we can
			if ( bChange || invalidateCache )
				System::IO::Directory::CreateDirectory( cacheDir );

			// Download if needed
			if( bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}
		}

		String ^ GetDownloadObject( TileProvider tileProvider, int nZoom, int nTileX, int nTileY )
		{
			String ^ retVal;

			switch (tileProvider)
			{
				case Mapnik:
				{
					retVal = "/" + nZoom + "/" + nTileX + "/" + nTileY + ".png";
					break;
				}
				case Osmarender:
				{
					retVal = "/Tiles/tile/" + nZoom + "/" + nTileX + "/" + nTileY + ".png";
					break;
				}
				case Cyclemap:
				{
					retVal = "/tiles/cycle/" + nZoom + "/" + nTileX + "/" + nTileY + ".png";
					break;
				}
				default:
				{
					break;
				}
			}

			return retVal;
		}

		String ^ GetDownloadURL( TileProvider tileProvider, int nZoom, int nTileX, int nTileY )
		{
			return "http://" +  GetDownloadServer(tileProvider) + GetDownloadObject(tileProvider, nZoom, nTileX, nTileY);
		}

		String ^ GetDownloadServer( TileProvider tileProvider )
		{
			String ^ retVal;

			switch (tileProvider)
			{
				case Mapnik:
				{
					retVal = "tile.openstreetmap.org";
					break;
				}
				case Osmarender:
				{
					retVal = "tah.openstreetmap.org";
					break;
				}
				case Cyclemap:
				{
					retVal = "andy.sandbox.cloudmade.com";
					break;
				}
				default:
				{
					break;
				}
			}

			return retVal;
		}

		String ^ GetTileCachePath( String ^ cacheDirectory, int nZoom, int nTileX, int nTileY )
		{
			return cacheDirectory + "\\" + nZoom + "\\" + nTileX + "\\" + nTileY + ".png";
		}

		bool CreateDownloadThread( void )
		{
			bool retVal = false;
			//return retVal;

			_threadHeight = this->Size.Height;
			_threadWidth = this->Size.Width;

			// Ensure that we only start/stop this thread once at a time
			mutex_Download->WaitOne();
			try
			{
				if( _downloadThread == nullptr || !_downloadThread->IsAlive )
				{
					static int threadNum = 0;
					_downloadThread = gcnew Thread(gcnew ThreadStart(this, &MapControl::DownloadThread));
					_downloadThread->Name = "Map Download Thread" + (++threadNum);
					_downloadThread->Start();
				}
				retVal = true;
			}
			catch( Exception ^ )
			{
				// InvalidOperationException thrown when changing thread to an invalid state
				mutex_Download->ReleaseMutex();
		System::Diagnostics::Trace::WriteLine("catch in map control");
			}

			return retVal;
		}

		bool DestroyDownloadThread( void )
		{
			bool retVal = false;
			//return retVal;

			// Ensure that we only start/stop this thread once at a time
			try
			{
				if( _downloadThread != nullptr )
				{
					_downloadThread->Abort();
					// release the mutex we acquired when creating the thread
					mutex_Download->ReleaseMutex();					
				}
				retVal = true;
			}
			catch( Exception ^ )
			{
				// ThreadStateException thrown when invalid thread state thing
		System::Diagnostics::Trace::WriteLine("catch in map control");

			}

			return retVal;			  			
		}

		void DownloadThread(void)
		{
			bool dataLocked = false;
			try
			{
				// Get settings
				Thread::BeginCriticalRegion();
				mutex_Data->WaitOne();

				int nZoom(_zoom);
				TileProvider tileProvider(_tileProvider);
				String ^ sCacheDirectory(_cacheDirectory);
				double dCenterLongitude(_centerLongitude);
				double dCenterLatitude(_centerLatitude);
				DownloadOrder downloadOrder(_downloadOrder);
				int nDownlodTilesEdgeCount = 2;

				mutex_Data->ReleaseMutex();
				Thread::EndCriticalRegion();

				// Get server to connect to
				String ^ server = GetDownloadServer( tileProvider );

				//Next thing we need to do is get the X and Y values for the center point of the map
				double dStartX = Longitude2TileX(dCenterLongitude, nZoom);
				int nStartX = static_cast<int>(dStartX);
				double dStartY = Latitude2TileY(dCenterLatitude, nZoom);
				int nStartY = static_cast<int>(dStartY);

				//Next we need to find the X and Y values which occur just before the top left position of the client area
				double dInt;
				int nClientX = static_cast<int>((_threadWidth / 2) - (modf(dStartX, &dInt) * OSMTileWidth));
				while (nClientX > 0)
				{
					--nStartX;
					nClientX -= OSMTileWidth;
				}
				//Also take into account the edge count
				for (int i=0; i<nDownlodTilesEdgeCount && nStartX >= 0; i++)
				{
					--nStartX;
					nClientX -= OSMTileWidth;
				}

				int nClientY = static_cast<int>((_threadHeight / 2) - (modf(dStartY, &dInt) * OSMTileHeight));
				while (nClientY > 0)
				{
					--nStartY;
					nClientY -= OSMTileHeight;
				}
				//Also take into account the edge count
				for (int i=0; i<nDownlodTilesEdgeCount && nStartY >= 0; i++)
				{
					--nStartY;
					nClientY -= OSMTileHeight;
				}

				//Also work out the end X coordinate
				int nEndX = _threadWidth;
				//Also take into account the edge count
				for (int i=0; i<nDownlodTilesEdgeCount; i++)
					nEndX += OSMTileWidth;

				//Also work out the end Y coordinate
				int nEndY = _threadHeight;
				//Also take into account the edge count
				for (int i=0; i<nDownlodTilesEdgeCount; i++)
					nEndY += OSMTileWidth;

				// Build array of tiles we will be downloading
				Generic::List<MapControlDownloadTileItem ^> ^ tilesToDownload = gcnew Generic::List<MapControlDownloadTileItem ^>;

				int nMaxTile = static_cast<int>(pow(2.0, nZoom));
				int nY = nClientY;
				int nTileY = nStartY;
				int nClientCenterX = _threadWidth/2;
				int nClientCenterY = _threadHeight/2;

				while ( nY <= nEndY )
				{
					int nX = nClientX;
					int nTileX = nStartX;

					while ( nX <= nEndX )
					{
						//Perform wrapping of invalid x tile values to valid values
						while (nTileX < 0)
							nTileX += nMaxTile;
						while (nTileX >= nMaxTile)
							nTileX -= nMaxTile;

						//Form the path to the file to cache the tile into
						if (nTileY >= 0 && nTileY < nMaxTile) //Ensure we only look to download valid Y tiles
						{
							//Only bother trying to download the tile if the cached version does not exist or if we have been asked to do a refresh
							String ^ sFile = GetTileCachePath(sCacheDirectory, nZoom, nTileX, nTileY);
							bool bDownload = !System::IO::File::Exists( sFile );

							if (bDownload)
							{
								//Calculate the distance of the tile to the center of the client area. Note we use the pixel distance rather than the
								//true elipsoid distance as a speed optimization
								double dTileXCenter = nX + OSMHalfTileWidth - nClientCenterX;
								double dTileYCenter = nY + OSMHalfTileHeight - nClientCenterY;
								double dDistance = sqrt((dTileXCenter * dTileXCenter) + (dTileYCenter * dTileYCenter));

								//Add the tile to the array of items to download
								MapControlDownloadTileItem ^ tileToDownload = gcnew MapControlDownloadTileItem(nTileX, nTileY, nX, nY, dDistance, sFile);
								tilesToDownload->Add( tileToDownload );
							}
						}

						//Move onto the next column
						nX += OSMTileWidth;
						++nTileX;
					}

					//Move onto rhe next row
					nY += OSMTileHeight;
					++nTileY;
				}  

				if (downloadOrder == ClosestToCenterFirst)
					tilesToDownload->Sort();

				WebClient ^ fileWriter = gcnew WebClient();

				//Iterate across the array of tiles to donload
				for (int i=0; i<tilesToDownload->Count; i++)
				{
					//Pull out the next tile to download
					MapControlDownloadTileItem ^ tile = tilesToDownload[i];

					//Create the sub directories if we can
					String ^ subDirectory = sCacheDirectory + "\\" + nZoom;
					System::IO::Directory::CreateDirectory( subDirectory );
					subDirectory = sCacheDirectory + "\\" + nZoom + "\\" + tile->_nTileX;
					System::IO::Directory::CreateDirectory( subDirectory );

					//Form the name of the tile we will be downloading
					String ^ sURI = GetDownloadURL( tileProvider, nZoom, tile->_nTileX, tile->_nTileY );

					//Now download the specific tile to the cache
					bool result = DownloadTile( fileWriter, sURI, nZoom, tile->_nTileX, tile->_nTileY, tile->_file );
		
					if (!result)
					{
						System::Diagnostics::Trace::WriteLine("MAP: Failed to download tile " + sURI);

						//Ensure any remants of a bad download file are nuked
						try
						{
							System::IO::File::Delete( tile->_file );
						}
						catch( Exception ^ )
						{
		System::Diagnostics::Trace::WriteLine("catch in map control");
						}
					}
					else
					{
						//Invalidate the specific area of the client which we have just download a tile for
						//redraw();
					}
				}
			}
			catch( Threading::ThreadAbortException ^ )
			{
				// Clean up any locks that need cleaning
				// TODO: I'm not positive but there might be a chance that we can deadlock based upon the critical
				// section above, I don't know the behavior if we abort in the middle of that.
		System::Diagnostics::Trace::WriteLine("catch in map control");
			}
		}

		bool DownloadTile( WebClient ^ fileWriter, String ^ sURI, int nZoom, int nTileX, int nTileY, String ^ file )
		{
			bool retVal = false;
			  
			//Validate our parameters
			if ((nTileX < 0) || (nTileY < 0))
				return false;
			int nMaxTile = static_cast<int>(pow(2.0, nZoom));
			if ((nTileX >= nMaxTile) || (nTileY >= nMaxTile))
				return false;

			// Download the file
			try
			{
				// OpenStreetMap only allows up to two threads at once - so don't use Async since it'll just spawn
				// a bunch of threads all at once, we can live with just one thread for now
				fileWriter->DownloadFile( gcnew Uri(sURI), file );
				retVal = true;
			}
			catch( Exception ^ )
			{
				retVal = false;
		System::Diagnostics::Trace::WriteLine("catch in map control");
			}

			return retVal;
		}

		int HitTest( Point point )
		{
			for( int i = 0; i < _targets->Count; ++i )
			{
				if( _targets[i]->_rect->Contains( point ) )
					return i;
			}

			return MapHitPosition;
		}

		void SetUILocked( bool lock )
		{
			// Save setting
			mutex_Data->WaitOne();
			_uiLock = lock;
			mutex_Data->ReleaseMutex();
		}

		void SetDownload( bool dl )
		{
			// Save setting
			mutex_Data->WaitOne();
			bool bChange = _downloadTiles != dl;
			_downloadTiles = dl;
			mutex_Data->ReleaseMutex();

			if( bChange && _downloadTiles )
			{
				DestroyDownloadThread();
				CreateDownloadThread();
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MapControl()
		{
			if (components)
			{
				delete components;
			}
		}

		/// <summary>
		/// Just like the draw function in the MFC version
		/// paint: http://msdn.microsoft.com/en-us/library/system.windows.forms.control.paint(v=VS.100).aspx
		/// graphics: http://msdn.microsoft.com/en-us/library/system.drawing.graphics.aspx
		/// </summary>
		System::Void paintMap( System::Object ^ sender, System::Windows::Forms::PaintEventArgs ^ e )
		{
			// create the graphics object used to draw with
			Graphics ^ g = e->Graphics;

			try
			{
				DrawMap( g );	
				DrawSearchArea( g );
				DrawTargets( g );
				DrawCameraLocation( g );
				DrawAirplane( g );
				DrawScaleBar( g );
			}
			catch( Exception ^ )
			{
		System::Diagnostics::Trace::WriteLine("catch in map control");

			}
		}

		void DrawCameraLocation( Graphics ^ g )
		{
			int height = this->Size.Height;
			int width = this->Size.Width;

			for( int i = 0; i < 4; ++i )
			{
				if( _cameraPoints[i].X < 0 || _cameraPoints[i].X > width ||
					_cameraPoints[i].Y < 0 || _cameraPoints[i].Y > height )
					return;
			}
			

			Brush ^ theBrush = gcnew SolidBrush( Color::FromArgb( 128, Color::DarkRed) );
			g->FillPolygon( theBrush, _cameraPoints );
		}

		void DrawSearchArea( Graphics ^ g )
		{
			int height = this->Size.Height;
			int width = this->Size.Width;

			Pen ^ penBlack = gcnew Pen( Color::Black, 1.0f );
			Brush ^ brush1 = gcnew SolidBrush( Color::FromArgb( 64, Color::Goldenrod) );
			Brush ^ brush2 = gcnew SolidBrush( Color::FromArgb( 128, Color::LightSkyBlue) );
			//Brush ^ theBrush = gcnew SolidBrush( Color::FromArgb( 128, Color::DarkRed) );

			int x, y;
			array<Point> ^ pointsToDraw;
			bool shouldDraw = true;

			// Draw operational area
			pointsToDraw = gcnew array<Point>(12);
			for( int i = 0; i < 12; ++i )
			{
				GetXYPosition( _opAreaPoints[i].Y, _opAreaPoints[i].X, x, y );
				if( x > width + 1000 || x < 0 - 1000 || y < 0 - 1000 || y > height + 1000 )
					shouldDraw = false;
				pointsToDraw[i].X = x;
				pointsToDraw[i].Y = y;
			}

			if( shouldDraw )
			{				
				g->FillPolygon( brush1, pointsToDraw );
				g->DrawPolygon( penBlack, pointsToDraw );
			}

			// Draw search area
			pointsToDraw = gcnew array<Point>(7);
			for( int i = 0; i < 7; ++i )
			{
				GetXYPosition( _opAreaPoints[i].Y, _opAreaPoints[i].X, x, y );
				if( x > width + 1000 || x < 0 - 1000 || y < 0 - 1000 || y > height + 1000 )
					shouldDraw = false;
				pointsToDraw[i].X = x;
				pointsToDraw[i].Y = y;
			}

			if( shouldDraw )
				g->FillPolygon( brush2, pointsToDraw );
		}

		void DrawAirplane( Graphics ^ g )
		{
			int height = this->Size.Height;
			int width = this->Size.Width;

			// Rotate the airplane
			Bitmap ^ rotated = RotateImage( _airplaneIcon, _airplaneHeading, true );

			// get the X and Y values for the lat/lon of the airplane
			int x, y;
			GetXYPosition( _airplaneLongitude, _airplaneLatitude, x, y );

			// don't draw if it'd cause a problem
			if( x < 0 || x > width || y < 0 || y > height )
				return;

			g->DrawImage( rotated, x - rotated->Width/2, y - rotated->Height/2 );
		}

		void DrawTargets( Graphics ^ g )
		{
			int height = this->Size.Height;
			int width = this->Size.Width;
			
			// Iterate through target list and plot them!
			for( int i = 0; i < _targets->Count; ++i )
			{
				int x, y;
				GetXYPosition( _targets[i]->_longitude, _targets[i]->_latitude, x, y );

				// don't draw if it'd cause a problem
				if( x < 0 || x > width || y < 0 || y > height )
				{
					_targets[i]->setRect( 0, 0, 0, 0 );
					continue;
				}

				g->DrawImage( _targetIcon, x - _targetIcon->Width/2, y - _targetIcon->Height/2 );
				_targets[i]->setRect( x - _targetIcon->Width/2, y - _targetIcon->Height/2, _targetIcon->Width, _targetIcon->Height );

				//double temp1, temp2;
				////GetPosition( longitude, latitude, temp1, temp2 );
				////System::Diagnostics::Trace::WriteLine("In: " + x + " " + y + " Out: " + temp1 + " " + temp2);
			}
		}

		void DrawMap( Graphics ^ g )
		{
			int height = this->Size.Height;
			int width = this->Size.Width;

			//First thing we need to do is get the X and Y values for the center point of the map
			//double dStartX = Longitude2TileX(_centerLongitude, _zoom);
			double dStartX = Longitude2TileX(_airplaneLongitude, _zoom);
			int nStartX = static_cast<int>(dStartX);
			//double dStartY = Latitude2TileY(_centerLatitude, _zoom);
			double dStartY = Latitude2TileY(_airplaneLatitude, _zoom);
			int nStartY = static_cast<int>(dStartY);

			//Next we need to find the X and Y values which occur just before the top left position of the client area
			double dInt;
			int nClientX = static_cast<int>((width/2) - (modf(dStartX, &dInt) * OSMTileWidth));
			while (nClientX > 0)
			{
				--nStartX;
				nClientX -= OSMTileWidth;
			}
			int nClientY = static_cast<int>((height/2) - (modf(dStartY, &dInt) * OSMTileHeight));
			while (nClientY > 0)
			{
				--nStartY;
				nClientY -= OSMTileHeight;
			}

			//Iterate across the checkerboard of tiles
			int nMaxTile = static_cast<int>(pow(2.0, _zoom));
			int nY = nClientY;
			int nTileY = nStartY;

			while (nY <= height)
			{
				int nX = nClientX;
				int nTileX = nStartX;
				while (nX <= width)
				{
					//Perform wrapping of invalid x tile values to valid values
					while (nTileX < 0)
						nTileX += nMaxTile;
					while (nTileX >= nMaxTile)
						nTileX -= nMaxTile;

					//Default to the tile being not available
					bool bTileAvailable = false;
					if ((nTileY >= 0) && (nTileY < nMaxTile))
					{
						//Form the path to the cache file which we want to draw
						String ^ sFile = _cacheDirectory + "\\" + _zoom + "\\" + nTileX + "\\" + nTileY + ".png";

						//Load up the image from disk and display it if we can
						System::Drawing::Image ^ image;
						//try
						//{

						// First attempt to draw
						if( System::IO::File::Exists( sFile ) )
						{
							try
							{
								image = System::Drawing::Image::FromFile( sFile );
								g->DrawImage( image, nX, nY );
								bTileAvailable = true;
							}
							catch( Exception ^ )
							{
								// image file error
		System::Diagnostics::Trace::WriteLine("catch in map control");
							}							
						}

						if( !bTileAvailable ) // file not available
						{
							//Try loading the image the previous zoom level and use it instead if configured to allow it
							if (_allowPreviousZoomStretch && _zoom > OSMMinZoom)
							{
								System::Drawing::Image ^ prevImage;
								sFile = _cacheDirectory + "\\" + (_zoom - 1) + "\\" + (nTileX/2) + "\\" + (nTileY/2) + ".png";

								if( System::IO::File::Exists( sFile ) )
								{
									try
									{
										prevImage = System::Drawing::Image::FromFile( sFile );
										//If the image is rendered correctly then we do not need to draw the tile not available
										g->DrawImage( prevImage, Drawing::Rectangle( nX, nY, OSMTileWidth, OSMTileHeight), (nTileX % 2) ? OSMHalfTileWidth : 0, (nTileY % 2) ? OSMHalfTileHeight : 0, OSMHalfTileWidth, OSMHalfTileHeight, GraphicsUnit::Pixel );
										bTileAvailable = true;
									}
									catch( Exception ^ )
									{
										// image file error
		System::Diagnostics::Trace::WriteLine("catch in map control");
									}
								}
							}

							if (!bTileAvailable && _allowNextZoomSqueeze && (_zoom < OSMMaxZoom))
							{
								//Load up the 4 images which form part of the next zoom level
								int nNextZoom = _zoom + 1;
								int nNextTileX = nTileX*2;
								int nNextTileY = nTileY*2;
								System::Drawing::Image ^ nextImage1;
								System::Drawing::Image ^ nextImage2;
								System::Drawing::Image ^ nextImage3;
								System::Drawing::Image ^ nextImage4;

								String ^ sFile1 = _cacheDirectory + "\\" + nNextZoom + "\\" + nNextTileX + "\\" + nNextTileY + ".png";
								String ^ sFile2 = _cacheDirectory + "\\" + nNextZoom + "\\" + (nNextTileX+1) + "\\" + (nNextTileY+0) + ".png";
								String ^ sFile3 = _cacheDirectory + "\\" + nNextZoom + "\\" + (nNextTileX+0) + "\\" + (nNextTileY+1) + ".png";
								String ^ sFile4 = _cacheDirectory + "\\" + nNextZoom + "\\" + (nNextTileX+1) + "\\" + (nNextTileY+1) + ".png";

								if( System::IO::File::Exists( sFile1 ) && System::IO::File::Exists( sFile2 ) &&
									System::IO::File::Exists( sFile3 ) && System::IO::File::Exists( sFile4 ) )
								{
									try
									{
										nextImage1 = System::Drawing::Image::FromFile( sFile1 );
										nextImage2 = System::Drawing::Image::FromFile( sFile2 );
										nextImage3 = System::Drawing::Image::FromFile( sFile3 );
										nextImage4 = System::Drawing::Image::FromFile( sFile4 );

										//If all 4 images are rendered correctly then we do not need to draw the tile not available
										g->DrawImage( nextImage1, Drawing::Rectangle( nX, nY, OSMHalfTileWidth, OSMHalfTileWidth), 
											0, 0, OSMTileWidth, OSMTileHeight, GraphicsUnit::Pixel );
										g->DrawImage( nextImage2, Drawing::Rectangle( nX + OSMHalfTileWidth, nY, OSMHalfTileWidth, OSMHalfTileWidth), 
											0, 0, OSMTileWidth, OSMTileHeight, GraphicsUnit::Pixel );
										g->DrawImage( nextImage3, Drawing::Rectangle( nX, nY + OSMHalfTileWidth, OSMHalfTileWidth, OSMHalfTileWidth), 
											0, 0, OSMTileWidth, OSMTileHeight, GraphicsUnit::Pixel );
										g->DrawImage( nextImage4, Drawing::Rectangle( nX + OSMHalfTileWidth, nY + OSMHalfTileWidth, OSMHalfTileWidth, OSMHalfTileWidth), 
											0, 0, OSMTileWidth, OSMTileHeight, GraphicsUnit::Pixel );

										bTileAvailable = true;
									}
									catch( Exception ^ )
									{
										// some image error
		System::Diagnostics::Trace::WriteLine("catch in map control");
									}
								} // end if check for file exists
							}
						} // end if else file exists
					} // end if tile in bounds

					if (!bTileAvailable)
					{      
						//Draw the "Tile not available" cell
						DrawTileNotAvailable( g, nX, nY, OSMTileWidth, OSMTileHeight );
					}

					//Move onto the next column
					nX += OSMTileWidth;
					++nTileX;
				} // end inner while 

				//Move onto the next row
			    nY += OSMTileHeight;
			    ++nTileY;
			}// end outer while
		} // end DrawMap

		void DrawTileNotAvailable( Graphics ^ g, int nX, int nY, int width, int height )
		{
			Pen ^ penBlack = gcnew Pen( Color::Black, 1.0f );
			g->DrawRectangle( penBlack, nX, nY, width, height );

			//Draw the text for the tile not available bar
			String ^ sText = "Tile not available";
			SolidBrush ^ fontBrush = gcnew SolidBrush( Color::Black );
			System::Drawing::Font^ drawFont = gcnew System::Drawing::Font( "Arial", 8.0f );
			System::Drawing::StringFormat^ drawFormat = gcnew StringFormat;
			drawFormat->Alignment = System::Drawing::StringAlignment::Center;
			drawFormat->LineAlignment = System::Drawing::StringAlignment::Center;

			g->DrawString( sText, drawFont, fontBrush, System::Drawing::RectangleF( (float)nX, (float)nY, (float)width, (float)height ), drawFormat );
		}

		bool DrawScaleBar( Graphics ^ g )
		{
			bool retVal = false;
			int height = this->Size.Height;
			int width = this->Size.Width;

			//Work out if where we are going to draw the bar is over a valid part of the map and if not abandon drawing the bar
			double dScaleY = Latitude2TileY( _centerLatitude, _zoom );
			dScaleY += ((height - 25 - height/2)) / static_cast<double>(OSMTileHeight);
			if (dScaleY > pow(2.0, _zoom))
				return false;

			//Create the pens we need
			Pen ^ penBlack = gcnew Pen( Color::Black, 1.0f );

			//Work out the distance between the center point and half a tile to the left or right (note we calculate for half a
			//tile to ensure we do not end up where we started at if at zoom level 0)
			double dX = Longitude2TileX(_centerLongitude, _zoom);
			dX += (_zoom == 0 ? 0.5 : 1);
			double dLongitude2 = TileX2Longitude(dX, _zoom);
			double dDistance = DistanceBetweenPoints(_centerLatitude, _centerLongitude, _centerLatitude, dLongitude2);
			if (_zoom == 0)
				dDistance *= 2;

			// Do everything in metric units
			BOOL bMetric = TRUE;

			//Next lets scale down the measure tile width to a most significant digit unit
			double dMSDUnit = pow(10, floor(log10(dDistance)));
			double dScaleDistance = dMSDUnit;
			while (dScaleDistance < dDistance)
				dScaleDistance += dMSDUnit;
			int nScaleLength = static_cast<int>((dScaleDistance / dDistance * static_cast<double>(OSMHalfTileWidth)) + 0.5);

			////Setup the DC
			//CPen* pOldPen = dc.SelectObject(&penBlack);

			////Draw the lines for the scale
			int curX = 37;
			int curY = height - 30;
			int destX = 37;
			int destY = height - 27;

			g->DrawLine( penBlack, curX, curY, destX, destY);
			curX = destX; curY = destY;
			
			destX = 37 + nScaleLength; destY = height - 27;
			g->DrawLine( penBlack, curX, curY, destX, destY);
			curX = destX; curY = destY;

			destX = 37 + nScaleLength; destY = height - 30;
			g->DrawLine( penBlack, curX, curY, destX, destY);

			//Create the text mode version of the distance formatted correctly
			String ^ sText;
			if (dScaleDistance < 1)  
				sText = String::Format( "{0:F0}", dScaleDistance*1000 ) + " Meters";
			else
				sText = String::Format( "{0:F0}", dScaleDistance ) + " Kilometers";

			//Draw the text for the scale bar
			SolidBrush ^ fontBrush = gcnew SolidBrush( Color::Black );
			System::Drawing::Font^ drawFont = gcnew System::Drawing::Font( "Arial", 8.0f );
			g->DrawString( sText, drawFont, fontBrush, PointF( 37.0f, height - 40.0f ) );

			return retVal;
		}

		/*void DrawPath( Graphics ^ g )
		{
			Pen ^ penRed = gcnew Pen( Color::Red, 1.0f );
			g->DrawLine( penRed, _airplaneLatitude, _airplaneLogitude, _airplaneNewLatitude, _airplaneNewLongitude );
		}
		*/

		void onMouseWheel( Object^ sender, System::Windows::Forms::MouseEventArgs^ e )
		{
			if ( _uiLock )
				return;

			if( e->Delta > 0 ) // zoom in
			{
				int newZoom = getZoom();
				++newZoom;
				setZoom( newZoom, e->Location );
			}
			else if( e->Delta < 0 ) // zoom out
			{
				int newZoom = getZoom();
				--newZoom;
				setZoom( newZoom, e->Location );
			}
		}

		void onMouseDown( Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
		{
			if ( _uiLock )
				return;

			int position = HitTest( e->Location );

			switch( e->Button )
			{
			case ::MouseButtons::Left:
				switch( position )
				{
				case MapHitPosition:
					_mouseCapturedForDrag = true;
					_pointMouseCapture = e->Location;
					this->Cursor = gcnew System::Windows::Forms::Cursor( Cursors::Cross->Handle );
					break;
				}
				break;
			case ::MouseButtons::Right:
				if( position == MapHitPosition )
				{

				}
				else // a target
				{

				}
				break;
			case ::MouseButtons::Middle:
				break;
			}
		}

		void onMouseUp( Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
		{
			if ( _uiLock )
				return;

			if( _mouseCapturedForDrag )
			{
				_mouseCapturedForDrag = false;
				this->Cursor = gcnew System::Windows::Forms::Cursor( Cursors::Default->Handle );
			}

			//switch( e->Button )
			//{
			//case ::MouseButtons::Left:
			//	break;
			//case ::MouseButtons::Right:
			//	break;
			//case ::MouseButtons::Middle:
			//	break;
			//}
		}

		void onMouseMove( Object ^ sender, System::Windows::Forms::MouseEventArgs ^ e )
		{
			if ( _uiLock )
				return;

			if( _mouseCapturedForDrag )
			{
				//Work out the longitude of the position where we have dragged to
				double dDeltaX = (e->Location.X - _pointMouseCapture.X)/static_cast<double>(OSMTileWidth);
				double dX = Longitude2TileX(_centerLongitude, _zoom) - dDeltaX;
				double dNewLongitude = TileX2Longitude(dX, _zoom);

				//Work out the latitude of the position where we have dragged to
				double dDeltaY = (e->Location.Y - _pointMouseCapture.Y)/static_cast<double>(OSMTileHeight);
				double dY = Latitude2TileY(_centerLatitude, _zoom) - dDeltaY;
				double dNewLatitude = TileY2Latitude(dY, _zoom);

				//Hive away the mouse position for the next mouse move
				_pointMouseCapture = e->Location;

				//Move to the new position
				SetCenterPosition(dNewLongitude, dNewLatitude);
			}
		}

	public:
		//Delegates::voidToVoid ^ _redrawDelegate;
	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

		int _zoom;
		double _centerLongitude;
		double _centerLatitude;
		String ^ _cacheDirectory;
		static Mutex ^ mutex_Data = gcnew Mutex;
		static Mutex ^ mutex_Download = gcnew Mutex;
		TileProvider _tileProvider;
		DownloadOrder _downloadOrder;
		Thread ^ _downloadThread;
		int _threadHeight;
		int _threadWidth;
		bool _downloadTiles;
		bool _allowPreviousZoomStretch;
		bool _allowNextZoomSqueeze;
		bool _mouseCapturedForDrag;
		Point _pointMouseCapture;
		bool _uiLock;

		// Airplane icon stuff
		Image ^ _airplaneIcon;
		double _airplaneLatitude, _airplaneLongitude, _airplaneHeading;

		// Target icon stuff
		Generic::List<Target ^> ^ _targets;
		Image ^ _targetIcon;

		// Where we are looking stuff
		array<Point> ^ _cameraPoints;

		// search area things
		array<PointF> ^ _opAreaPoints;
		array<PointF> ^ _searchAreaPoints;
		array<PointF> ^ _waypointsPoints;
		Delegates::voidToVoid ^ _redrawDelegate; // THIS USED TO BE PRIVATE

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}
