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
	ref class OCRWrapper;

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
		unsigned char *buffer;
		unsigned int saliencyImageHeight, saliencyImageWidth;
		cv::Mat *img;
		System::DateTime timestamp;
		System::Collections::Generic::List<Box ^>^ saliencyBlobs;
		ImageWithPlaneData ^ planeState;


		Frame(unsigned char *buf, int w, int h, System::DateTime time)
		{
			allocImg(buf,w,h);
			init(time);
		}

		Frame(unsigned char *buf, int w, int h)
		{
			allocImg(buf,w,h);
			init(System::DateTime::Now.AddSeconds(-VIDEO_LATENCY));
		}

		Frame(cv::Mat inputImg)
		{
			buffer = nullptr;
			img = new cv::Mat(inputImg);
			
			init(System::DateTime::Now.AddSeconds(-VIDEO_LATENCY));
		}

		void allocImg(unsigned char* buf, int w, int h)
		{
			buffer = buf;
			img = new cv::Mat(h, w, CV_8UC3, buf);
		}

		void init(System::DateTime time)
		{
			timestamp = time;
			planeState = nullptr;
			saliencyBlobs = gcnew System::Collections::Generic::List<Box ^>();
		}

		~Frame()
		{
			delete buffer;
			delete img;
		}

		bool isReady()
		{
			// if frame is more than 2 seconds old
			if (System::DateTime::Now.Subtract(timestamp).TotalSeconds > 2.0)
				return true;
			else
				return false;
		}
	};

    public ref class VisionController
    {
	public:
		VisionController(Skynet::SkynetController ^ skynetCtrl);
		~VisionController();

		void receiveFrame(ImageWithPlaneData^ imageWithPlaneData);
		bool processSaliencyCandidate(Database::CandidateRowData^ candidate);

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
		OCRWrapper					^ ocr;
        System::Threading::Thread	^ runLoopThread;
        int width, height;
		bool paused;
    };
}