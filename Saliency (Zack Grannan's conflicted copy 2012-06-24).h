#pragma once
#include <string>
#include "Auvsi_Saliency.h"
//#include "Auvsi_FFT.h"
#include "Database.h"

using namespace System::Threading;
using namespace System;
using namespace System::Windows::Forms;

#define MIN_SIZE	0.8
#define MAX_SIZE	3.5

namespace Vision
{
	public ref struct FrameData
	{
		double latitude;
		double longitude;
		double altitude;
		double heading;
		double azimuth;
		double elevation;
		double zoomLevel;
		array<float> ^ homography;
	};

	
	delegate void saliencyUpdateDelegate( Database::RowData ^ data );

	public ref class Saliency
	{
	public:
		Saliency( Object ^ watcher ); 	// constructor
		Saliency();
		virtual ~Saliency();	// destructor

		void setValues(int w, int h, Object ^ newDelegate);		// set initial values
		bool canAcceptFrame() { return !currentlyAnalyzing;}	// call to see if ready to accept new frame
		void analyzeFrame(float * buffer, FrameData ^ theData);						// pass in new frame to analyze
		float * postSaliency;		// results of saliency (black and white)

		void runTestOnImageNamed(String ^ filename);

	protected:
		bool currentlyAnalyzing;	// saliency is running
		bool newFrameReady;			// saliency is ready for new frame
		Thread ^ saliencyThread;	// thread for running saliency
		Thread ^ saveImagesThread;	// thread for saving images
		float * inputBuffer;		// frame being analyzed by saliency
		int width;					// frame width
		int height;					// frame height
		float threshold;			// threshold for saliency
		Object ^ parent;			// delegate to send info to
		FrameData ^ currentFrameData;	// current saliency frame
		Object ^ planeWatcher;
		
		
		int frameCount;

		// for saving images thread
		FrameData ^ savingFrameData;
		box * boundingBoxes;
		int numBoxes;
		float * savingBuffer;		// color image for saving to file
		bool newFrameForSaving;		// new frame ready for saveThread to save
		bool savingData;			// images are being saved

		bool tempPause;

		void analyzeResults ( float * image, int imageHeight, int imageWidth, Communications:: PlaneState ^ state, box * results, int numResults, bool isTest );
		bool validSize(double size);
		
		void saliencyThreadFunction(); // main run loop for saliency
		void saveImagesThreadFunction();	// main run loop for saving images

	private:
		saliencyUpdateDelegate ^ saliencyDelegate;
	};
}
