#pragma once

#include <cv.h>
#include <highgui.h>
#include <vector>
#include "VisionUtil.h"

namespace Vision {

	#define MIN_DISTANCE_BETWEEN_ANGLES_DEGREES 25.0f // 25 is great
	#define MIN_DISTANCE_BETWEEN_LINES 3.0f		// was 10, but 5 works well
	#define HOUGH_LINE_THRESHOLD 10.0f				// was 10

	using namespace System;
	using namespace System::Collections::Generic;

	ref class Line
	{
	public:
		float angle;
		float distFromOrigin;

		Line(cv::Vec2f l)
		{
			distFromOrigin = l[0];
			angle = l[1];
		}

		Line(float a, float d)
		{
			angle = a;
			distFromOrigin = d;
		}

		cv::Vec2f standardLine()
		{
			return cv::Vec2f(distFromOrigin, angle);
		}
	};
	
	ref class Circle
	{
	public:
		int centerX, centerY;
		float radius;

		Circle(cv::Point center, float r)
		{
			centerX = center.x;
			centerY = center.y;
			radius = r;
		}
	};

	ref class BadLineException : System::Exception {  };

	ref class ShapeRecognizer
	{
	public:
		ShapeRecognizer(void);

		// take image (or path to image), figure out what it is!!
		String ^ recognizeShape(String ^ testShapeFilename);
		String ^ recognizeShape(cv::Mat binaryShapeImg) { return recognizeShape(binaryShapeImg, nullptr); }
		String ^ recognizeShape(cv::Mat binaryShapeImg, String ^ filename);

		float maxDistanceForDrawLinesAndCenterFill(List<Line ^> ^ lines);
	private:
		void setImageWithPreprocessing(cv::Mat img);
		List<Line ^> ^ findLines(String ^ filename);
		std::vector<cv::Vec2f> runHoughLines();
		std::vector<cv::Vec2f> combineCloseLines(std::vector<cv::Vec2f> lines);
		bool linesAreClose(cv::Vec2f leftLine, cv::Vec2f rightLine);
		float calcDistanceBetweenLines(cv::Vec2f leftLine, cv::Vec2f rightLine);
		bool linesIntersect(cv::Vec2f leftLine, cv::Vec2f rightLine);
		float distanceBetweenPoints(cv::Point a, cv::Point b);
		bool leftLineIsBetter(cv::Vec2f leftLine, cv::Vec2f rightLine);
		cv::Mat printLinesToImage(std::vector<cv::Vec2f> lines, uchar color);
		void drawLineInImage(cv::Vec2f line, cv::Mat image, uchar color);
		void determineStartAndEnd(cv::Vec2f line, cv::Point & start, cv::Point & end);
		void stepAlongLineToFindCenterpoint(float rho, float theta, float a, float b, float & x0, float & y0);
		void stepAlongLineToFindEndpoints(float x0, float y0, float a, float b, cv::Point & start, cv::Point & end);
		void calcLineCoordinate(float x0, float y0, float a, float b, float dist, float & x, float & y);
		bool coordinateIsWithinImage(float x, float y);
		float calcLineStrength(cv::Vec2f line);
		float calcLineStrength(cv::Point start, cv::Point end);
		int getSizeOfBiggestBlobs(cv::Mat mask);
		std::vector<cv::Vec2f> filterSmallLines(std::vector<cv::Vec2f> lines);
		List<Circle ^> ^ findCircles(String ^ filename);
		sdt::vector<cv::Vec2f> standardLinesFromManaged(List<Line ^> ^ lines);

		MRef<cv::Mat> ^ mImage;
		String ^ filename;
	};
}

