#pragma once

#include <cv.h>

using namespace System;
using namespace System::Collections;

#define VIDEO_LATENCY 0.150 // seconds

namespace Skynet
{
	ref class SkynetController;
}
namespace OpenGLForm 
{
	ref class COpenGL;
}

namespace Decklink
{
	class Callback;
}

namespace Communications
{
	ref struct PlaneState;
}

namespace Vision
{
	ref class Saliency;
	ref class DuplicateResolver;

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
		float *buffer;
		cv::Mat *img;
		DateTime timestamp;
		Collections::Generic::List<Box ^>^ saliencyBlobs;
		Communications::PlaneState ^ planeState;

		Frame(float *buf, int w, int h, DateTime time)
		{
			allocImg(buf,w,h);
			init(time);
		}

		Frame(float *buf, int w, int h)
		{
			allocImg(buf,w,h);
			init(DateTime::Now.AddSeconds(-VIDEO_LATENCY));
		}

		Frame(cv::Mat inputImg)
		{
			buffer = NULL;
			img = new cv::Mat(inputImg);
			
			init(DateTime::Now.AddSeconds(-VIDEO_LATENCY));
		}

		void allocImg(float *buf, int w, int h)
		{
			buffer = buf;
			img = new cv::Mat(h, w, CV_32FC3, buf);
		}

		void init(DateTime time)
		{
			timestamp = time;
			planeState = nullptr;
			saliencyBlobs = gcnew Collections::Generic::List<Box ^>();
		}

		~Frame()
		{
		}

		!Frame()
		{
			delete img; 
			if (buffer != NULL)
				delete buffer;
		}

		bool isReady()
		{
			// if frame is more than 2 seconds old
			if (DateTime::Now.Subtract(timestamp).TotalSeconds > 2.0)
				return true;
			else
				return false;
		}
	};

    public ref class VisionController
    {
	public:
		VisionController(OpenGLForm::COpenGL ^ openGL, Skynet::SkynetController ^ skynetCtrl);
		~VisionController();

		void receiveFrame(float *buffer);
		void gotFirstFrame(int imgWidth, int imgHeight);

		Decklink::Callback *getDecklinkCallback() { return callback; }

	protected:
		void runLoop();
		void initImagingPathway(); // TODO
		void receiveOCRResults(); // TODO: pass in a struct
		void analyzeFrame(Frame ^ frame);

		bool endRunLoop;
        
	private:
        OpenGLForm::COpenGL			^ openGLForm;
        Skynet::SkynetController 	^ skynetController;
		Queue 						^ frameQueue;
        DuplicateResolver			^ duplicateResolver;
        Saliency					^ saliency;
        Decklink::Callback 			* callback;
        System::Threading::Thread	^ runLoopThread;
        int width, height;
    };
}