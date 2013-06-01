#pragma once

#include <cv.h>

#define VIDEO_LATENCY 0.150 // seconds

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	ref struct PlaneState;
}

namespace Database
{
	ref struct CandidateRowData;
}

ref struct ImageWithPlaneData;

namespace Vision
{
	ref class Saliency;
	ref class DuplicateResolver;
	ref class TargetRecognizer;

	public ref struct Box
	{
		int minx, miny, maxx, maxy;

		Box(int xmin, int ymin, int xmax, int ymax)
		{
			minx = xmin;
			miny = ymin;
			maxx = xmax;
			maxy = ymax;
		}

		int width() { return maxx - minx; }
		int height() { return maxy - miny; }
	};

	public ref struct Frame
	{
		System::Collections::Generic::List<Box ^>^ saliencyBlobs;
		ImageWithPlaneData ^ planeState;
		Frame(ImageWithPlaneData^ data): planeState(data), saliencyBlobs(gcnew System::Collections::Generic::List<Box ^>()) { }
	};

    public ref class VisionController
    {
	public:
		VisionController(Skynet::SkynetController ^ skynetCtrl);
		~VisionController();

		void receiveFrame(ImageWithPlaneData^ imageWithPlaneData);
		bool processSaliencyCandidate(Database::CandidateRowData^ candidate);
		void end(){endRunLoop = true;}
	protected:
		void runLoop();
		void initImagingPathway(); // TODO
		void receiveOCRResults(); // TODO: pass in a struct
		void analyzeFrame(Frame ^ frame);
		void updateSaliencyImageSize(int imgWidth, int imgHeight);
		bool endRunLoop;
        
	private:
        Skynet::SkynetController 	^ skynetController;
		System::Collections::Queue ^ frameQueue;
        DuplicateResolver			^ duplicateResolver;
        Saliency					^ saliency;
		TargetRecognizer			^ recognizer;
        System::Threading::Thread	^ runLoopThread;
        int width, height;
		bool paused;
    };
}