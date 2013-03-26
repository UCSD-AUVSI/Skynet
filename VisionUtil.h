
#pragma once
#include <cv.h>
#include <highgui.h>

#include "MasterHeader.h"

namespace Vision {
	
	#define MAX_RGB_COLOR_VALUE 256.0
	#define SIZE_OF_FEATURE_VECTOR 320

	#define PTSTR(pt) "(" + pt.x + "," + pt.y + ")"
	#define VEC2STR(pt) "(" + pt[0] + "," + pt[1] + ")"

	typedef unsigned char uchar;
	typedef cv::Vec<float, 3> PixelColor;
	
	using namespace System;

	// wraps an IplImage, so you don't have to worry about memory management
	ref class ManagedIPL
	{
	public:
		ManagedIPL(String ^ path, int options);
		!ManagedIPL();
		~ManagedIPL() { this->!ManagedIPL(); }

		IplImage *img;
	};

	// wraps any allocated native object, deleting it automatically when gc runs
	// this is here because native objects cannot be a part of managed classes
	// so, use this to wrap your native objects and don't worry about memory leaks!
	// :)
	template<typename T> ref class MRef
	{
	public:
		MRef(T o) { obj = new T(o); }
		MRef(T *o) { obj = o; }

		T o() { return *obj; } // returns raw object (not pointer to object)
		T *obj; // pointer to object
	protected:
		!MRef() { delete obj; }
		~MRef() { this->!MRef(); }
	};

	// cache statistics easily. subclass this and override updateStatistics
	ref class CachedStatistics
	{
	public:
		CachedStatistics()
		{
			isValid = false;
		}

		void invalidate()
		{
			isValid = false;
		}

		void updateIfNecessary()
		{
			if (needsUpdating())
				updateStatistics();
			setWasUpdated();
		}

		void updateStatistics() {}

	protected:
		bool needsUpdating() { return !isValid; }
		void setWasUpdated() { isValid = true; }
		bool isValid;
	};

	void saveImage(cv::Mat img, String ^ filename);

	System::String ^ shapeFloatToString(float input);
	int shapeStrToInt(String ^ const shape);

	wchar_t parseFilenameForLetter(String^ const filename);
	String^ parseFilenameForShape( String^ const filename);
	String^ parseFilenameForSeparateShape(String^ const filename);
	wchar_t disambiguateLetter(wchar_t input);

	PixelColor blackPixelColor();

	float calcColorDistance(PixelColor color1, PixelColor color2);

	cv::MatND calcHistogramOfImage(cv::Mat img);
	cv::MatND calcHistogramOfImage(cv::Mat img, int numBins);
	cv::MatND calcHistogramOfImage(cv::Mat img, int numBins, cv::Mat mask);

	cv::Mat magnitude(cv::Mat img);
	cv::Mat outsideFill(cv::Mat input, unsigned char color);
	cv::Mat centerFill(cv::Mat input, unsigned char color);
	float farthestPixelFromCenter(cv::Mat input, unsigned char color);

	float radians(float deg) { return deg*PI/180.0f; }
	float degrees(float rad) { return rad*180.0f/PI; }

	float distanceBetweenAngles(float angleARadians, float angleBRadians, float overflowLocationRadians);
	float distanceBetweenAngles(float angleARadians, float angleBRadians)
	{
		return distanceBetweenAngles(angleRadians, angleBRadians, PI);
	}
	float shiftAngleToPositive(float angleRadians, float overflowLocationRadians);

	float pythagoreanDistance(float x, float y);
}