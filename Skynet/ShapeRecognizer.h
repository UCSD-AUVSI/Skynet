#pragma once

#include <cv.h>
#include <highgui.h>
#include <vector>
#include "VisionUtil.h"

namespace Vision {

	#define MIN_DISTANCE_BETWEEN_ANGLES_DEGREES 25.0f // 25 is great
	#define MIN_DISTANCE_BETWEEN_LINES 6.0f		// 5 was good for small image
	#define HOUGH_LINE_THRESHOLD 15.0f				// was 10 for small image
	#define MIN_LINE_STRENGTH_RATIO 0.15f		// ratio between strongest and weakest line

	using namespace System;
	using namespace System::Collections::Generic;

	ref class Line
	{
	public:
		float angle;
		float distFromOrigin;
		float strength; 

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

		String ^ str()
		{
			return "Line(" + angle + ", " + distFromOrigin + ", " + strength + ")";
		}

		static float SumOfLines(List<Line ^> ^ lines)
		{
			float sum = 0;
			for each (Line ^ line in lines)
				sum += line->strength;
			return sum;
		}
	};

	ref class BadLineException : System::Exception {  };
	ref class ShapeClassifier;

	ref class ShapeRecognizer
	{
	public:
		ShapeRecognizer(void);

		// take image (or path to image), figure out what it is!!
		String ^ recognizeShape(String ^ testShapeFilename);
		String ^ recognizeShape(cv::Mat binaryShapeImg) { return recognizeShape(binaryShapeImg, nullptr); }
		String ^ recognizeShape(cv::Mat binaryShapeImg, String ^ theFilename);

		float maxDistanceForDrawLinesAndCenterFill(List<Line ^> ^ lines);
		float calcPercentageLineCoverage(List<Line ^> ^ lines, int lineThickness);
	private:
		void setImageWithPreprocessing(cv::Mat img);
		List<Line ^> ^ findLines();
		std::vector<cv::Vec2f> runHoughLines();
		std::vector<cv::Vec2f> combineCloseLines(std::vector<cv::Vec2f> lines);
		bool linesAreClose(cv::Vec2f leftLine, cv::Vec2f rightLine);
		float calcDistanceBetweenLines(cv::Vec2f leftLine, cv::Vec2f rightLine);
		bool linesIntersect(cv::Vec2f leftLine, cv::Vec2f rightLine);
		float distanceBetweenPoints(cv::Point a, cv::Point b);
		bool leftLineIsBetter(cv::Vec2f leftLine, cv::Vec2f rightLine);
		cv::Mat printLinesToImage(std::vector<cv::Vec2f> lines, uchar color, bool freshImage);
		cv::Mat printLinesToImage(std::vector<cv::Vec2f> lines, uchar color, bool freshImage, int lineThickness);
		void drawLineInImage(cv::Vec2f line, cv::Mat image, uchar color);
		void drawLineInImage(cv::Vec2f line, cv::Mat image, uchar color, int lineThickness);
		void determineStartAndEnd(cv::Vec2f line, cv::Point & start, cv::Point & end);
		void stepAlongLineToFindCenterpoint(float rho, float theta, float a, float b, float & x0, float & y0);
		void stepAlongLineToFindEndpoints(float x0, float y0, float a, float b, cv::Point & start, cv::Point & end);
		void calcLineCoordinate(float x0, float y0, float a, float b, float dist, float & x, float & y);
		bool coordinateIsWithinImage(float x, float y);
		float calcLineStrength(cv::Vec2f line);
		float calcLineStrength(cv::Point start, cv::Point end);
		int getSizeOfBiggestBlobs(cv::Mat mask);
		std::vector<cv::Vec2f> filterSmallLines(std::vector<cv::Vec2f> lines);
		std::vector<cv::Vec2f> standardLinesFromManaged(List<Line ^> ^ lines);
		List<Line ^> ^ managedLinesFromStandard(std::vector<cv::Vec2f> lines);

		MRef<cv::Mat> ^ mEdges;
		MRef<cv::Mat> ^ mOriginalImage;
		String ^ filename;
		ShapeClassifier ^ classifier;
	};
}

