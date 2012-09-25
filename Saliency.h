#pragma once
#include <string>
// #include "Auvsi_Saliency.h"
#include "Database.h"

using namespace System::Threading;
using namespace System;
using namespace System::Windows::Forms;

#define MIN_SIZE	0.42
#define MAX_SIZE	3.5

namespace sg
{
	class SalientGreenGPU;
}

namespace Communications
{
	ref class PlaneWatcher;
}

namespace Vision
{
	ref class DuplicateResolver;
	ref struct Frame;

	public ref class Saliency
	{
	public:
		Saliency( Communications::PlaneWatcher ^ watcher, DuplicateResolver ^ resolver ); 	// constructor
		Saliency();
		virtual ~Saliency();	// destructor

		void setValues(int w, int h, Object ^ newDelegate);		// set initial values
		bool canAcceptFrame() { return !currentlyAnalyzing;}	// call to see if ready to accept new frame
		void analyzeFrame(Frame ^ frame);						// pass in new frame to analyze
		// float * postSaliency;		// results of saliency (black and white)

		void runTest();
		void runTestOnImageNamed(String ^ filename);

	protected:
		bool currentlyAnalyzing;	// saliency is running
		bool newFrameReady;			// saliency is ready for new frame
		Thread ^ saliencyThread;	// thread for running saliency
		Thread ^ saveImagesThread;	// thread for saving images
		// float * inputBuffer;		// frame being analyzed by saliency
		int width;					// frame width
		int height;					// frame height
		float threshold;			// threshold for saliency
		DuplicateResolver ^ parent;			// delegate to send info to
		Frame ^ currentFrameData;	// current saliency frame
		Communications::PlaneWatcher ^ planeWatcher;
		DuplicateResolver ^ duplicateResolver;
		
		void computeSaliencyForFrame(Frame ^ frame, bool isTest);//, sg::SalientGreenGPU green);

		
		int frameCount;

		// for saving images thread
		Frame ^ savingFrameData;
		// box * boundingBoxes;
		// int numBoxes;
		// float * savingBuffer;		// color image for saving to file
		bool newFrameForSaving;		// new frame ready for saveThread to save
		bool savingData;			// images are being saved

		bool tempPause;

		void analyzeResults ( Frame ^ frame, int imageHeight, int imageWidth, bool isTest );
		void analyzeResults ( Frame ^ frame, int imageHeight, int imageWidth, Communications::PlaneState ^ state, bool isTest ); 
		bool validSize(double size);
		
		void saliencyThreadFunction(); // main run loop for saliency
		void saveImagesThreadFunction();	// main run loop for saving images
	};
}
