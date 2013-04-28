#pragma once

#include <cv.h>
#include <highgui.h>

namespace Vision {

	#define PARALLEL_ANGLE_THRESHOLD_DEGREES 10.0f
	#define MAX_CENTERFILL_DISTANCE_FROM_CENTER 0.75f // ratio of radius of image
	#define MAX_CLOSED_SHAPE_DIST_FROM_CENTER 1.0f // ratio of radius of image
	#define SQUARE_ASPECT_RATIO_THRESHOLD 0.15 // square aspect ratio is between 1.0-x and 1.0+x
	#define QUAR_STRAIGHT_DIST_TO_TOTAL_RATIO 0.58f
	#define PENT_COVERAGE_THRESH 84.0f
	
	using namespace System;
	using namespace System::Collections::Generic;

	ref class Line;
	ref class ShapeRecognizer;

	ref class ParallelLineSet
	{
	public:
		static List<ParallelLineSet ^> ^ GroupLines(List<Line ^ > ^ lines);
		ParallelLineSet(Line ^ line);
		bool isParallelTo(Line ^ line);
		bool isParallelTo(ParallelLineSet ^ set);
		void addLine(Line ^ line);
		bool isPerpendicularTo(Line ^ line);
		bool isPerpendicularTo(ParallelLineSet ^ set);
		static bool linesArePerpendicular(Line ^ lineA, Line ^ lineB);
		float distanceBetweenLines(); 
		int numLines() { return lines->Count; }
	private:
		List<Line ^ > ^ lines;
		float angleRadians;
		
		static void AddLineToSetList(Line ^ line, List<ParallelLineSet ^> ^ setList);
		bool isParallelTo(float otherAngleRadians);
		bool isPerpendicularTo(float otherAngleRadians);
		static bool anglesArePerpendicular(float angleARadians, float angleBRadians);
	};

	ref class ShapeClassifier
	{
	public:
		ShapeClassifier(ShapeRecognizer ^ parentRecognizer);
		
		String ^ classifyShape(List<Line ^> ^ imgLines, int imgSize);
	private:
		String ^ doClassification();
		bool hasCurves();
		bool isSemi(bool definitelyCurved);
		bool hasOnlyOneStrongLine(bool useLowerThreshold);
		float highestLineStrength();
		bool isQuar(bool definitelyCurved);
		bool hasPerpendicularLines();
		List<Line ^> ^ findTwoStrongestLines();
		bool isCross();
		bool isTriangle();
		bool isQuadrilateral();
		bool hasFiveLines();
		bool hasSixLines();
		String ^ classifyCurvyShape();
		String ^ classifyQuadrilateral();
		String ^ classifyParallelogram();
		String ^ classifyRectangle();
		bool isTrapezoid();
		String ^ classifyFailedQuadrilateral();
		String ^ classifyFiveLineShape();
		bool isFiveLinedQuar();
		bool isFiveLinedSemi();
		String ^ classifySixLineShape();
		bool isStar();
		//String ^ classifyQuadrilateral(List<Line ^> ^ lines);
		//String ^ classifyPentShape(List<Line ^> ^ lines);
		//String ^ classifyHexaShape(List<Line ^> ^ lines);

		int imageSize;
		List<Line ^> ^ lines;
		ShapeRecognizer ^ shapeRecognizer;
		List<ParallelLineSet ^> ^ lineSets;
	};

}