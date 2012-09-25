#include "StdAfx.h"
#include "Form1.h"
#include "Saliency.h"
#include "SaveImage.h"
#include "PlaneWatcher.h"


// Saliency update frequency (in miliseconds)
#define UPDATE_FREQUENCY 5

#define BORDER_PADDING	10 // in pixels

#define IMAGE_SAVE_BASE_PATH "D:\\Skynet Files\\Candidates\\bb"

using namespace Vision;

//[DllImport("Saliency_New", CharSet=CharSet::Ansi)]
//extern "C" void Auvsi_Saliency();	

// TODO: check for memory leaks

Saliency::Saliency()
{
	PRINT("WARNING: DEFAULT constructor for saliency does not function properly and is only for testing. Use Saliency(pointer) if you want saliency to work.");

}

Saliency::Saliency( Object ^ watcher )
{
	saliencyThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saliencyThreadFunction));
	saliencyThread->Name = "Saliency Analysis Thread";
	//saliencyThread->Start(); DEBUG

	saveImagesThread = gcnew Thread(gcnew ThreadStart(this, &Saliency::saveImagesThreadFunction));
	saveImagesThread->Name = "Image Saving Thread";
	//saveImagesThread->Start(); DEBUG

	planeWatcher = watcher;

	newFrameReady = false;
	width = 0;
	height = 0;
	threshold = 0.7f;
	frameCount = 0;
	boundingBoxes = NULL;
	postSaliency = NULL;
	tempPause = false;
	
	System::DateTime start = System::DateTime::Now;
	
	Communications::PlaneState ^ state = ((Communications::PlaneWatcher ^)planeWatcher)->predictLocationAtTime( 0 );
	
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

}

std::string ManagedToSTL(String ^ s) 
{
	using namespace Runtime::InteropServices;
	const char* chars = 
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	std::string retVal = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));

	return retVal;
}


void Saliency::runTestOnImageNamed(String ^ filename)
{
	// load image from file
	cv::Mat image = cv::imread(ManagedToSTL(filename), 1);
	cv::imwrite(  ManagedToSTL("C:\\Users\\UCSD\\Desktop\\saliencytest_preconvert.bmp"), image);


	// convert into float * buffer
	typedef cv::Vec<unsigned __int8, 3> VT;
	int channels = 4, width = image.cols, height = image.rows;
	int fullWidth = width * channels;
	float scale = 1.0f/255.0f;
	float thresholdTest = 0.4f; //This should be 0.7f
	float * buffer = new float[width*height*channels];

	PRINT("cols/width: "+width+" rows/height: "+height+"");
	
	for( int row = 0; row < height; ++row )
	{
		for( int col = 0; col < width; ++col )
		{
			// B G R format
			VT pixel = image.at<VT>(row, col);
			buffer[row * fullWidth + col * channels + 0]  = pixel[2]*scale; // r
			buffer[row * fullWidth + col * channels + 1]  = pixel[1]*scale; // g
			buffer[row * fullWidth + col * channels + 2]  = pixel[0]*scale; // b
			buffer[row * fullWidth + col * channels + 3]  = 1.0f; // a
		}
		
	}

	ImageUtil::SaveImage theImSaver2( (int)height, (int)width, (int)4 );
	theImSaver2.saveFrameNonInterlaced(buffer, ManagedToSTL("C:\\Users\\UCSD\\Desktop\\saliencytest_inputPostConvert.bmp"));

	// run saliency on it
	Auvsi_Saliency * theSaliency = new Auvsi_Saliency();
		
	theSaliency->setInformation( width, height );
	theSaliency->prepareFilters();
	theSaliency->prepareForSaliency();
	
	//theSaliency->loadImage( "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp" );
	theSaliency->loadImage( width, height, buffer );
	theSaliency->computeSaliency(thresholdTest); // (threshold)
	theSaliency->labelConnectedComponents();


	// extract info
	float * imageReturn = new float[ width*height*channels ];
	box * boxReturn;
	int numBoxReturn;

	theSaliency->getFinalData( imageReturn, boxReturn, numBoxReturn); // save data for image saving thread to grab
	

	Communications::PlaneState ^ state = gcnew Communications::PlaneState ();

	analyzeResults( buffer, height, width, state, boxReturn, numBoxReturn, true);
	
	ImageUtil::SaveImage theImSaver( (int)height, (int)width, (int)1 );
	theImSaver.saveFrameNonInterlaced(imageReturn, ManagedToSTL("C:\\Users\\UCSD\\Desktop\\saliencytest_out.bmp"));
	//theImSaver.saveFrame(buffer, ManagedToSTL("C:\\Users\\UCSD\\Desktop\\saliencytest_input.bmp"));

	
	delete buffer;
	delete theSaliency;
}

void 
Saliency::saliencyThreadFunction(void)
{
	try {
		FILE * fd;
		fopen_s(&fd, "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\errorLog.txt", "w");
		fprintf(fd, "Error Log for saliency:\n");
		fclose( fd );

		Auvsi_Saliency * theSaliency = new Auvsi_Saliency();
		/*const char * imagePath = "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp";
		theSaliency->loadImage( imagePath);
		theSaliency->prepareFilters();
		theSaliency->prepareForSaliency();
		theSaliency->computeSaliency(0.6);*/

		int localW = 0, localH = 0;
		FrameData ^ localData;

		while( true )
		{
			Thread::Sleep( UPDATE_FREQUENCY ); // dont run too fast, or we eat up cpu ... ;)
			//continue;	// DONT RUN SALIENCY 
			// check for new width, height
			if (localW != width || localH != height) 
			{
				localW = width;
				localH = height;

				// set new data in saliency, prepare shit
				if (localW != 0 && localH != 0) 
				{
					//theSaliency->loadImage( "C:\\Users\\UCSD\\Pictures\\Saliency Sample\\heli.bmp" );
					theSaliency->setInformation( localW, localH );
					theSaliency->prepareFilters();
					theSaliency->prepareForSaliency();
					//theSaliency->computeSaliency(0.6);
				}
			}
		
			// check for new frame
			if (newFrameReady) 
			{
				//System::Diagnostics::Trace::WriteLine( "New frame Ready");
			
				// set flags
				currentlyAnalyzing = true;
				newFrameReady = false;
				localData = currentFrameData; // data of current frame

				// run saliency
				theSaliency->loadImage( localW, localH, inputBuffer );
				theSaliency->computeSaliency(threshold);
				theSaliency->labelConnectedComponents();

			
				//System::Diagnostics::Trace::WriteLine( "Computing Saliency frame. frameW: " + width + " frameH: " + height);
			

				while (savingData) { Thread::Sleep( UPDATE_FREQUENCY ); }

				if( boundingBoxes != NULL )
				{
					delete boundingBoxes;
					boundingBoxes = NULL;
				}

				if( postSaliency != NULL )
				{
					delete postSaliency;
					postSaliency = NULL;
				}

				float * imageReturn = new float[ localW * localH ];
				box * boxReturn;
				int numBoxReturn;

				theSaliency->getFinalData( imageReturn, boxReturn, numBoxReturn); // save data for image saving thread to grab
			
				numBoxes = numBoxReturn;
				boundingBoxes = boxReturn;
				postSaliency = imageReturn;

			
				memcpy(savingBuffer, inputBuffer, width * height * 4 * sizeof(float)); // copy full color buffer for saving images
			
				savingFrameData = localData;
				currentlyAnalyzing = false;
				newFrameForSaving = true;
			
			}

			//TODO: fix memory leaks, free the buffer
		}


	}

	catch ( Exception ^ theException) {
		System::Diagnostics::Trace::WriteLine( "Exception in saliencyThread: " + theException);
				
	}
}


void 
Saliency:: analyzeResults ( float * image, int imageHeight, int imageWidth, Communications:: PlaneState ^ state, box * results, int numResults,  bool isTest )
{
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
	}else
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
	double metersPerXPixel, metersPerYPixel;

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


	// print info
	for (int i = 0; i < numResults; i++) {
		box currentBox = results[i]; // get bounding box

		// check for correct size ... if wrong size, skip this result
		if (!(validSize(((double)currentBox.width)*metersPerXPixel) && validSize(((double)currentBox.height)*metersPerYPixel))) {
			continue;
		}
		else
			System::Diagnostics::Trace::WriteLine("Saliency: size of target included w:" + currentBox.width*metersPerXPixel + " h:" + currentBox.height*metersPerYPixel  );

		int leftCorner = (int) (currentBox.x > BORDER_PADDING ? currentBox.x - BORDER_PADDING : 0);
		int topCorner = (int) (currentBox.y > BORDER_PADDING ? currentBox.y - BORDER_PADDING : 0);
		int rightCorner = (int) (currentBox.x + currentBox.width + BORDER_PADDING < imageWidth ? currentBox.x + currentBox.width + BORDER_PADDING : imageWidth);
		int bottomCorner = (int) (currentBox.y + currentBox.height + BORDER_PADDING < imageHeight? currentBox.y + currentBox.height + BORDER_PADDING : imageHeight);

		int littleImageWidth = (rightCorner - leftCorner); 
		int littleImageHeight = (bottomCorner - topCorner);

		float * tempBuffer = new float[(bottomCorner - topCorner) * (rightCorner - leftCorner) * 4];
		int counter = 0;

		for (int y = topCorner; y < bottomCorner; y++) {
			for (int x =  leftCorner; x < rightCorner; x++) {
				for (int c = 0; c < 4; c++) {
					tempBuffer[counter] = image[(y * imageWidth + x)*4 + c];
					counter++;
				}
			}
		}
		ImageUtil::SaveImage theImSaver( littleImageHeight, littleImageWidth, (int)4 );
		if ( isTest ){
			theImSaver.saveFrameNonInterlaced(tempBuffer, ManagedToSTL("C:\\Users\\UCSD\\Desktop\\saliencytestS_" + i + ".bmp"));
		}
		else
		{
			// make RowData, pass to callback in form1.h
			Database::RowData ^ data = gcnew Database::RowData();

			data->path = (IMAGE_SAVE_BASE_PATH + frameCount + "_" + i + ".bmp")->Replace("\\", "\\\\");
			data->target_latitude = 0;			// Latitude of point selected as target
			data->target_longitude = 0;			// Longitude of point selected as target
			data->target_X = (int)(currentBox.x + currentBox.width / 2);					// pixel X of target
			data->target_Y = (int)(currentBox.y + currentBox.height / 2);					// pixel Y of target
			data->heading = (float)savingFrameData->heading;	// heading of target
			data->letter = "A";				
			data->shape = "Circle";					
			data->fg_color = "White";				// foreground color
			data->bg_color = "Yellow";				// background color
			data->processed = FALSE;				// whether this has been processed by OpenCV
			//data->verified = FALSE;				// human verified
			data->center_latitude = (float)savingFrameData->latitude;			// Latitude of center pixel
			data->center_longitude = (float)savingFrameData->longitude;			// Longitude of center pixel
			data->mapping_latitude = 0;			// pixel to meter translation for latitude
			data->mapping_longitude = 0;		// pixel to meter translation for longitude
			data->homography = savingFrameData->homography;

			// what we really need for first database (candidate regions):
			//	plane info, gimbal info, camera info
			//	image size
			//	image data (the image itself, unrectified)

			// second database (unconfirmed targets):
			//	OCR info (letter, shape, letter color, letter shape)
			//	target location, heading, width and height
			//	image size
			//	rectified image data (the image itself)
			//	revision number (increment everytime we change the row)

			// third database (confirmed targets):
			//	all from second database except revision number

				
			theImSaver.saveFrame(tempBuffer, ManagedToSTL(IMAGE_SAVE_BASE_PATH + frameCount + "_" + i + ".bmp"));

			array<Object ^>^myArr = {data};
			try {
					
				((Skynet::Form1 ^ )parent)->Invoke( ((Skynet::Form1 ^ )parent)->saliencyAddTarget, myArr );
			}

			catch (Exception ^ theEx) {
					
				System::Diagnostics::Trace::WriteLine( "WARNING: Saliency exception: " + theEx);
			}
		}
		delete tempBuffer;

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
			continue; // DEBUG: dont save images right now

			
			Communications::PlaneState ^ state = ((Communications::PlaneWatcher ^)planeWatcher)->predictLocationAtTime( 0 );
	
			analyzeResults(savingBuffer, height, width, state, boundingBoxes, numBoxes, false );
			/*
			double plane_lat = state->gpsData->gpsLatitude, plane_lon = state->gpsData->gpsLongitude, plane_alt = state->gpsData->gpsAltitude;
			double plane_roll = state->telemData->roll, plane_pitch = state->telemData->pitch, plane_heading = state->telemData->heading;
			double gimbal_roll = state->gimbalInfo->roll, gimbal_pitch = state->gimbalInfo->pitch, gimbal_yaw = 0;
	
			double target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
	
			double t_lat_1, t_lat_2, t_lon_1, t_lon_2;

			double metersPerXPixel, metersPerYPixel;

			target_x = 0; target_y = 100;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
			
			target_x = 200; target_y = 100;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);
			metersPerXPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
			
			target_x = 100; target_y = 0;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_1, t_lon_1, t_alt);
			//System::Diagnostics::Trace::WriteLine("Saliency: lat1:" + t_lat_1 + " t_lon_1:" + t_lon_1 + " t_lat_2:" + t_lat_2 + " t_lon_2:" + t_lon_2);

			target_x = 100; target_y = 200;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt, plane_roll, plane_pitch, plane_heading, gimbal_roll, gimbal_pitch, gimbal_yaw, target_x, target_y, zoom, t_lat_2, t_lon_2, t_alt);
			metersPerYPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
			//System::Diagnostics::Trace::WriteLine("Saliency: lat1:" + (double)t_lat_1 + " t_lon_1:" + (double)t_lon_1 + " t_lat_2:" + (double)t_lat_2 + " t_lon_2:" + (double)t_lon_2);

			//System::Diagnostics::Trace::WriteLine("Saliency: metersPerX:" + metersPerXPixel + " metersPerY:" + metersPerYPixel);
			
			// loop through each bounding box
			for (int i = 0; i < numBoxes; i++) {
				box currentBox = boundingBoxes[i]; // get bounding box

				// check for correct size ... if wrong size, skip this result
				if (!(validSize(((double)currentBox.width)*metersPerXPixel) && validSize(((double)currentBox.height)*metersPerYPixel))) {
					//System::Diagnostics::Trace::WriteLine("Saliency: size of target disregarded w:" + currentBox.width*metersPerXPixel + " h:" + currentBox.height*metersPerYPixel );
					continue;
				}
				else
					System::Diagnostics::Trace::WriteLine("Saliency: size of target included w:" + currentBox.width*metersPerXPixel + " h:" + currentBox.height*metersPerYPixel  );


				// save image to file
				
				int leftCorner = (int) (currentBox.x > BORDER_PADDING ? currentBox.x - BORDER_PADDING : 0);
				int topCorner = (int) (currentBox.y > BORDER_PADDING ? currentBox.y - BORDER_PADDING : 0);
				int rightCorner = (int) (currentBox.x + currentBox.width + BORDER_PADDING < width ? currentBox.x + currentBox.width + BORDER_PADDING : width);
				int bottomCorner = (int) (currentBox.y + currentBox.height + BORDER_PADDING < height? currentBox.y + currentBox.height + BORDER_PADDING : height);

				int littleImageWidth = (rightCorner - leftCorner); 
				int littleImageHeight = (bottomCorner - topCorner);

				float * tempBuffer = new float[(bottomCorner - topCorner) * (rightCorner - leftCorner) * 4];
				int counter = 0;

				for (int y = topCorner; y < bottomCorner; y++) {
					for (int x =  leftCorner; x < rightCorner; x++) {

						for (int c = 0; c < 4; c++) {
							tempBuffer[counter] = savingBuffer[(y * width + x)*4 + c];
							counter++;

						}

					}
				}
				

				delete tempBuffer;

			}
			*/
			delete state;

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

	// (re) allocate all buffers
	if (postSaliency)
	{
		delete postSaliency;
		postSaliency = NULL;
	}

	//postSaliency = new float[w*h];

	if (savingBuffer)
	{
		delete savingBuffer;
		savingBuffer = NULL;
	}
	
	savingBuffer = new float[w*h*4];

	if (inputBuffer)
	{
		delete inputBuffer;
		inputBuffer = NULL;
	}

	inputBuffer = new float[w*h*4];

	if (newDelegate != nullptr)
		parent = newDelegate;

	tempPause = false;
}

// copies new frame into buffer, sets flag
void 
Saliency::analyzeFrame(float * buffer, FrameData ^ theData)	
{
	if (canAcceptFrame()) {
		memcpy(inputBuffer, buffer, width*height*4*sizeof(float));
		currentFrameData = theData;
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
	}

	if( saveImagesThread != nullptr  )
		saveImagesThread->Abort();

}
