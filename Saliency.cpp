#include "StdAfx.h"
#include "Form1.h"
#include "Saliency.h"
#include "SaveImage.h"
#include "PlaneWatcher.h"
#include "DuplicateResolver.h"
#include "DatabaseStructures.h"

#include "SalientGreenGPU/SalientGreenGPU.H"
#include "SalientGreenGPU/Filters/Utility.H"  
//#include "SalientGreenGPU/Timer.H"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cvblob.h>

// Saliency update frequency (in miliseconds)
#define UPDATE_FREQUENCY 5

#define BORDER_PADDING	10 // in pixels

//#define IMAGE_SAVE_BASE_PATH HTTP_SERVER_IMAGERY_PATH//"D:\\Skynet Files\\Candidates\\bb"

using namespace Vision;
using namespace Database;
using namespace System::Collections::Generic;

//[DllImport("Saliency_New", CharSet=CharSet::Ansi)]
//extern "C" void Auvsi_Saliency();	

std::string ManagedToSTL(String ^ s) 
{
	using namespace Runtime::InteropServices;
	const char* chars = 
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	std::string retVal = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return retVal;
}

Saliency::Saliency()
{
	PRINT("WARNING: DEFAULT constructor for saliency does not function properly and is only for testing. Use Saliency(pointer) if you want saliency to work.");

}

cv::Mat testImage;
Saliency::Saliency( PlaneWatcher ^ watcher, DuplicateResolver ^ resolver )
{
	saliencyThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saliencyThreadFunction));
	saliencyThread->Name = "Saliency Analysis Thread";
	//saliencyThread->Start(); // DEBUG uncomment to run saliency

	saveImagesThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saveImagesThreadFunction));
	saveImagesThread->Name = "Image Saving Thread";
	saveImagesThread->Start(); 

	planeWatcher = watcher;
	duplicateResolver = resolver;

	newFrameReady = false;
	width = 0;
	height = 0;
	threshold = 0.7f;
	frameCount = 0;
	//boundingBoxes = NULL;
	//postSaliency = NULL;
	tempPause = false;
	
	System::DateTime start = System::DateTime::Now;
	
	Communications::PlaneState ^ state = planeWatcher->predictLocationAtTime( 0 );
	
	double plane_lat = state->gpsData->gpsLatitude, plane_lon = state->gpsData->gpsLongitude, plane_alt = state->gpsData->gpsAltitude;
	double plane_roll = state->telemData->roll, plane_pitch = state->telemData->pitch, plane_heading = state->telemData->heading;
	double gimbal_roll = state->gimbalInfo->roll, gimbal_pitch = state->gimbalInfo->pitch, gimbal_yaw = 0;
	
	
	System::Diagnostics::Trace::WriteLine("Saliency: Running Georeference: lat:" + plane_lat + " lon:" + plane_lon + " alt:" + plane_alt + " roll:" + plane_roll + " pitch:" + plane_pitch + " heading:" + plane_heading + " groll:" + gimbal_roll + " gpitch:" + gimbal_pitch + " gyaw:" + gimbal_yaw);
	
	double target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
	
	double t_lat_1, t_lat_2, t_lon_1, t_lon_2;

	GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
	GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, 10, target_y, zoom, t_lat_2, t_lon_2, t_alt);

	System::Diagnostics::Trace::WriteLine("Distance: " + GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2));
			
	System::Diagnostics::Trace::WriteLine("Saliency: Done with Georeference lat: " + t_lat + " lon: " + t_lon + " alt: " + t_alt + " time (ms): " + System::DateTime::Now.Subtract(start).TotalMilliseconds);

	testImage = cv::imread(ManagedToSTL("C:\\Users\\ucsd_auvsi\\Desktop\\saliency_test2.jpg"), 1);
}

void Saliency::runTest()
{
	runTestOnImageNamed("C:\\Users\\ucsd_auvsi\\Desktop\\saliency_test2.jpg");
}

void Saliency::runTestOnImageNamed(String ^ filename)
{
	// load image from file
	cv::Mat image = cv::imread(ManagedToSTL(filename), 1);
	cv::imwrite(  ManagedToSTL("C:\\Users\\ucsd_auvsi\\Desktop\\saliencytest_preconvert.bmp"), image);

	Frame ^ localFrame = gcnew Frame(image);
	
	//sg::SalientGreenGPU green;

	computeSaliencyForFrame(localFrame,true);


	analyzeResults(localFrame, image.rows, image.cols, true );

	// TODO: check for valid results
}

sg::SalientGreenGPU green; // global variable, suck it
void
Saliency::computeSaliencyForFrame(Frame ^ frame, bool isTest)
{
	sg::SalientGreenGPU::labWeights lw;
	sg::SalientGreenGPU::Results results;

	cv::Mat inputImage = testImage; // DEBUG: should be *frame->img

	// compute saliency
	results = green.computeSaliencyGPU( inputImage, &lw );
	//if (isTest)
	//{
	//	results.show(); cvWaitKey(0);
	//}

	if (isTest)
		cv::imwrite(  ManagedToSTL("C:\\Users\\ucsd_auvsi\\Desktop\\saliencytest_output.bmp"), results.labSaliency); // DEBUG testing only

	// run connected components
	cvb::CvBlobs blobs;

	double threshold = 0.7;

	// convert saliency output to 8bit
	cv::Mat labSaliency8bit(results.labSaliency.size(), CV_8UC1);
	results.labSaliency.convertTo(labSaliency8bit,CV_8UC1,1.0);
	cv::threshold(labSaliency8bit,labSaliency8bit,threshold*255,255.0,CV_THRESH_BINARY);
	if (isTest)
		cv::imwrite(  ManagedToSTL("C:\\Users\\ucsd_auvsi\\Desktop\\saliencytest_output_thresh.bmp"), labSaliency8bit); // DEBUG testing only

	// do labelling
	IplImage saliencyOutputImgIpl = labSaliency8bit;
	IplImage *bloblsOutputImgIpl = cvCreateImage(results.labSaliency.size(), IPL_DEPTH_LABEL, 1);
	unsigned int result = cvb::cvLabel(&saliencyOutputImgIpl, bloblsOutputImgIpl, blobs);

	// store blob results in Frame object
	frame->saliencyBlobs = gcnew List<Box ^>();
	for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		cvb::CvBlob * blob = (*it).second;
		frame->saliencyBlobs->Add(gcnew Box(blob->minx,blob->miny,blob->maxx,blob->maxy));
		if (isTest)
			PRINT("box: " + blob->minx+", "+blob->miny+", "+blob->maxx+", "+blob->maxy);
	}
	cvReleaseImage(&bloblsOutputImgIpl);

	if (isTest)
		PRINT("Saliency: " + frame->saliencyBlobs->Count + " objects found");

}

void 
Saliency::saliencyThreadFunction(void)
{
	int saliencyCount = 0;
	DateTime startTime = DateTime::Now;

	try {
		FILE * fd;
		fopen_s(&fd, "D:\\Skynet Files\\Saliency Error Log.txt", "w");
		fprintf(fd, "Error Log for saliency:\n");
		fclose( fd );

		//sg::SalientGreenGPU green;
		sg::SalientGreenGPU::Results results;

		int localW = 0, localH = 0;

		while( true )
		{
			Thread::Sleep( UPDATE_FREQUENCY ); // dont run too fast, or we eat up cpu ... ;)
			//continue;	// DONT RUN SALIENCY 

			// check for new width, height
			if (localW != width || localH != height) 
			{
				localW = width;
				localH = height;

				if (localW != 0 && localH != 0) 
				{
					// update any data if necessary
				}
			}
		
			// check for new frame
			if (newFrameReady) 
			{			
				// set flags
				currentlyAnalyzing = true;
				newFrameReady = false;

				// compute saliency
				Frame ^localFrame = currentFrameData;

				computeSaliencyForFrame(localFrame, false);

				// send to saveImagesThread
				while (savingData) { Thread::Sleep( UPDATE_FREQUENCY ); }

				savingFrameData = localFrame;

				currentlyAnalyzing = false;
				newFrameForSaving = true;
			
				++saliencyCount;
				//PRINT("SALIENCY " + saliencyCount/(float)DateTime::Now.Subtract(startTime).TotalSeconds);
			}

		}


	}

	catch ( ThreadAbortException ^ theException) {
		System::Diagnostics::Trace::WriteLine( "Exception in saliencyThread: " + theException);
				
	}
}

void 
Saliency::analyzeResults ( Frame ^ frame, int imageHeight, int imageWidth, bool isTest )
{
	analyzeResults(frame,imageHeight,imageWidth,frame->planeState,isTest);
}

void 
Saliency::analyzeResults ( Frame ^ frame, int imageHeight, int imageWidth, Communications::PlaneState ^ state, bool isTest )
{
	double metersPerXPixel, metersPerYPixel;
	{ // georef calculations
		double plane_lat, plane_lon, plane_alt;
		double plane_roll, plane_pitch, plane_heading;
		double gimbal_roll, gimbal_pitch, gimbal_yaw;
		if ( isTest )
		{
			plane_lat = 0;
			plane_lon = 0;
			plane_alt = 0;
			plane_roll = 0;
			plane_pitch = 0;
			plane_heading = 0;
			gimbal_roll = 0;
			gimbal_pitch = 0;
			gimbal_yaw = 0;
		}
		else
		{
			plane_lat = state->gpsData->gpsLatitude;
			plane_lon = state->gpsData->gpsLongitude;
			plane_alt = state->gpsData->gpsAltitude;
			plane_roll = state->telemData->roll;
			plane_pitch = state->telemData->pitch;
			plane_heading = state->telemData->heading;
			gimbal_roll = state->gimbalInfo->roll;
			gimbal_pitch = state->gimbalInfo->pitch;
			gimbal_yaw = 0;
		}

		double target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
		double t_lat_1 , t_lat_2, t_lon_1, t_lon_2;

		target_x = 0; target_y = 100;
		GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
				
		target_x = 200; target_y = 100;
		GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);
		if (isTest)
		{
			t_lat_1 = 0;
			t_lat_2 = 0.00001;
			t_lon_1 = 0;
			t_lon_2 = 0;
		}
		metersPerXPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
				
		target_x = 100; target_y = 0;
		GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
		
		target_x = 100; target_y = 200;
		GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);

		if (isTest)
		{
			t_lat_1 = 0;
			t_lat_2 = 0.00001;
			t_lon_1 = 0;
			t_lon_2 = 0;
		}
		metersPerYPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
	} // end georef calcs


	// save blobs
	int i = 0;
	//PRINT("saliency found " + frame->saliencyBlobs->Count + " results");
	for each (Box ^box in frame->saliencyBlobs) 
	{
		// skip candidates of the wrong size
		if (!isTest && !(validSize(((double)box->width())*metersPerXPixel) && validSize(((double)box->height())*metersPerYPixel))) {
			continue;
		}
		else
			System::Diagnostics::Trace::WriteLine("Saliency: size of target included w:" + box->width()*metersPerXPixel + " h:" + box->height()*metersPerYPixel  );

		// copy and save candidate image, with padding
		int leftCorner = (box->minx > BORDER_PADDING ? box->minx - BORDER_PADDING : 0);
		int topCorner = (int) (box->miny > BORDER_PADDING ? box->miny - BORDER_PADDING : 0);
		int rightCorner = (int) (box->maxx + BORDER_PADDING < imageWidth ? box->maxx + BORDER_PADDING : imageWidth);
		int bottomCorner = (int) (box->maxy + BORDER_PADDING < imageHeight ? box->maxy + BORDER_PADDING : imageHeight);

		int blobWidth = (rightCorner - leftCorner); 
		int blobHeight = (bottomCorner - topCorner);


		cv::Mat blobImg = cv::Mat(*frame->img, cv::Rect(leftCorner, topCorner, blobWidth, blobHeight));


		if ( isTest ){
			String ^ path = "C:\\Users\\ucsd_auvsi\\Desktop\\saliencytestS_" + i + ".bmp";

			cv::imwrite(ManagedToSTL(path),blobImg);
		}
		else
		{
			String ^imageName =  frameCount + "_" + i + ".bmp";
			String ^path = HTTP_SERVER_TARGET_PATH + imageName;

			// make CandidateRowData (set targetX/Y)
			CandidateRowData ^ candidateData = gcnew CandidateRowData(state,leftCorner,topCorner,blobWidth,blobHeight);
			candidateData->imageName = imageName;
				
			cv::imwrite(ManagedToSTL(path),blobImg);
			
			try {
				parent->saliencyAddTarget( candidateData );
			}
			catch (Exception ^ e) {		
				System::Diagnostics::Trace::WriteLine( "WARNING: Saliency exception: " + e);
			}

		}
		
		i += 1;
	}
}

void 
Saliency::saveImagesThreadFunction()
{
	
	while( true )
	{
		Thread::Sleep( UPDATE_FREQUENCY ); // dont run too fast, or we eat up cpu ... ;)
		if ( newFrameForSaving ) {
			
			savingData = true;
			newFrameForSaving = false;
			frameCount++;
			// continue; // uncomment to stop saliency saving

			Frame ^ localFrame = savingFrameData;
			analyzeResults(localFrame, height, width, false );

			savingData = false;
		}
	}


}

bool Saliency::validSize(double size)
{
	return (size < MAX_SIZE) && (size > MIN_SIZE);

}

// set new width, height, delegate
void 
Saliency::setValues(int w, int h, Object ^ newDelegate) 
{
	
	tempPause = true;
	Thread::Sleep( 40 );
	

	width = w;
	height = h;

	//// (re) allocate all buffers
	//if (postSaliency)
	//{
	//	delete postSaliency;
	//	postSaliency = NULL;
	//}

	////postSaliency = new float[w*h];

	//if (savingBuffer)
	//{
	//	delete savingBuffer;
	//	savingBuffer = NULL;
	//}
	//
	//savingBuffer = new float[w*h*4];

	//if (inputBuffer)
	//{
	//	delete inputBuffer;
	//	inputBuffer = NULL;
	//}

	//inputBuffer = new float[w*h*4];

	if (newDelegate != nullptr)
		parent = (Vision::DuplicateResolver ^)(newDelegate);

	tempPause = false;
}

// copies new frame into buffer, sets flag
void 
Saliency::analyzeFrame(Frame ^ frame)	
{
	if (canAcceptFrame()) {
		//memcpy(inputBuffer, frame->buffer, width*height*4/2*sizeof(float));
		currentFrameData = frame;
		newFrameReady = true;
	}
	//System::Diagnostics::Trace::WriteLine( "analyzeFrame()");
}

Saliency::~Saliency()
{
	System::Diagnostics::Trace::WriteLine( "Saliency::~Saliency()");
	// kill threads
	if( saliencyThread != nullptr ) {
		System::Diagnostics::Trace::WriteLine( "Killing Saliency thread");

		saliencyThread->Abort();
		saliencyThread = nullptr;
	}

	if( saveImagesThread != nullptr  )
	{
		saveImagesThread->Abort();
		saveImagesThread = nullptr;
	}
}
