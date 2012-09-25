#pragma once

#include "Saliency.h"
#include "Autosearch.h"
//#include <atltime.h>
using namespace Intelligence;

namespace Skynet {

	public ref class Tester {
	public:
		static void testSaliency()
		{
			System::Diagnostics::Trace::WriteLine("Tester::testSaliency() STARTING test ");

			Vision::Saliency ^ sal = gcnew Vision::Saliency();

			sal->runTest();
			//"C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp");
			//"C:\\Users\\UCSD\\Desktop\\test.png");

			System::Diagnostics::Trace::WriteLine("Tester::testSaliency() FINISHED with test");
		}

		static void testGeoreferencing() 
		{
			System::Diagnostics::Trace::WriteLine("Tester::testGeoreferencing() STARTING test");
			Vision::GeoReference::runTests();
			System::Diagnostics::Trace::WriteLine("Tester::testGeoreferencing() FINISHED with test");
		}

		static void testPrediction()
		{
			System::Diagnostics::Trace::WriteLine("Tester::testPrediction() STARTING test ... not implemented yet");


			System::Diagnostics::Trace::WriteLine("Tester::testPrediction() FINISHED with test");
		}

		static void testautoSearch()
		{
		
			GPSCoord^ d = gcnew GPSCoord ( GPSCoord::metersToGPS(0), GPSCoord::metersToGPS(0) , 0 );
			GPSCoord^ c = gcnew GPSCoord ( GPSCoord::metersToGPS(0), GPSCoord::metersToGPS(2000) , 0 );
			GPSCoord^ b = gcnew GPSCoord ( GPSCoord::metersToGPS(1000), GPSCoord::metersToGPS(2000) , 0 );
			GPSCoord^ a = gcnew GPSCoord ( GPSCoord::metersToGPS(1000), GPSCoord::metersToGPS(0) , 0 );
			
			GPSCoord^ e = gcnew GPSCoord ( GPSCoord::metersToGPS(0), GPSCoord::metersToGPS(0) , 0 );
			GPSCoord^ f = gcnew GPSCoord ( GPSCoord::metersToGPS(100), GPSCoord::metersToGPS(0) , 0 );
			GPSCoord^ g = gcnew GPSCoord ( GPSCoord::metersToGPS(100), GPSCoord::metersToGPS(100) , 0 );
			GPSCoord^ h = gcnew GPSCoord ( GPSCoord::metersToGPS(0), GPSCoord::metersToGPS(100) , 0 );

			 array<GPSCoord^>^ coords = gcnew array<GPSCoord^>(4);
			coords[0] = a;
			coords[1] = b;
			coords[2] = c;
			coords[3] = d;


			array<GPSCoord^> ^polyCoords = gcnew array<GPSCoord^>(4);
			polyCoords[0] = e;
			polyCoords[1] = f;
			polyCoords[2] = g;
			polyCoords[3] = h;

			DateTime start = DateTime::Now;
			Intelligence::Autosearch^ testAS = gcnew Intelligence::Autosearch(coords,nullptr);
			testAS->markPolygon(polyCoords,2);
			//testAS->saveImage("field.bmp");
			DateTime stop = DateTime::Now;
			PRINT("elapsed time to make bitmap:" + (stop.Subtract(start).TotalSeconds));
		}

		static void testDatabase()
		{
			//Database::DatabaseConnection::TestDatabase();
			//Database::DatabaseConnection::FillDatabaseForTesting();
		}

		static void runTests() 
		{
			//return;
			//testGeoreferencing();
			
			//testautoSearch();


			//testPrediction();

			//testDatabase();
			//testSaliency();

		}
	};



}