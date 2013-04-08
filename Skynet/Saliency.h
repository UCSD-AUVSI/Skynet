#pragma once
#include <string>
#include <cv.h>


namespace sg
{
	class SalientGreenGPU;
}

namespace Communications
{
	ref class PlaneWatcher;
}

ref struct ImageWithPlaneData;

namespace Vision
{
	ref struct Frame;
	ref class VisionController;

	public ref class Saliency
	{
	public:
		Saliency(VisionController^ visionController); 	// constructor
		virtual ~Saliency();	// destructor

		bool canAcceptFrame() { return !currentlyAnalyzing;}	// call to see if ready to accept new frame
		void analyzeFrame(Frame ^ frame);						// pass in new frame to analyze

		void setValues(int w, int h);
		void runTest();
		void runTestOnImageNamed(System::String ^ filename);

		const static int MAX_IMAGE_WIDTH = 720;
		const static int MAX_IMAGE_HEIGHT = 720;
		const static double MAX_TARGET_SIDE_LENGTH_METERS = 1.5;
		const static double MIN_TARGET_SIDE_LENGTH_METERS = 0.25;
		

	protected:
		bool currentlyAnalyzing;	// saliency is running
		bool newFrameReady;			// saliency is ready for new frame
		System::Threading::Thread ^ saliencyThread;	// thread for running saliency
		System::Threading::Thread ^ saveImagesThread;	// thread for saving images
		int width;					// frame width
		int height;					// frame height
		float threshold;			// threshold for saliency
		Frame ^ currentFrameData;	// current saliency frame
		VisionController^ visionController;
		
		void computeSaliencyForFrame(Frame ^ frame, bool isTest);//, sg::SalientGreenGPU green);

		
		int frameCount;

		// for saving images thread
		Frame ^ savingFrameData;
		bool newFrameForSaving;		// new frame ready for saveThread to save
		bool savingData;			// images are being saved

		bool tempPause;

		void analyzeResults ( Frame ^ frame, int imageHeight, int imageWidth, bool isTest );
		void analyzeResults ( Frame ^ frame, ImageWithPlaneData ^ state, bool isTest ); 
		bool validSize(double size);
		
		void saliencyThreadFunction(); // main run loop for saliency
		void saveImagesThreadFunction();	// main run loop for saving images
	private:
		cv::Mat Saliency::convertImageForSaliency(cv::Mat image);
	};
}