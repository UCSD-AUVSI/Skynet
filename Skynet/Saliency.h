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

		bool canAcceptFrame() { return !busy;}					// call to see if ready to accept new frame
		void analyzeFrame(Frame ^ frame);						// pass in new frame to analyze

		void setValues(int w, int h);
		void runTest();
		void runTestOnImageNamed(System::String ^ filename);

		const static int MAX_IMAGE_WIDTH = 720;
		const static int MAX_IMAGE_HEIGHT = 720;
		const static double MAX_TARGET_SIDE_LENGTH_METERS = 1.5;
		const static double MIN_TARGET_SIDE_LENGTH_METERS = 0.25;
		

	protected:
		bool busy;					// saliency is running
		int width;					// frame width
		int height;					// frame height
		float threshold;			// threshold for saliency
		VisionController^ visionController;
		
		void computeSaliencyForFrame(Frame ^ frame, bool isTest);

		
		int frameCount;

		void analyzeResults ( Frame ^ frame, int saliencyImageWidth, int saliencyImageHeight, bool isTest );
		bool validSize(double size);
		
	private:
		cv::Mat Saliency::convertImageForSaliency(cv::Mat image);
	};
}