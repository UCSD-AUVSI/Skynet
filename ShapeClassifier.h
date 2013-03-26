#pragma once

#include <cv.h>
#include <highgui.h>

namespace Vision {

	#define PARALLEL_ANGLE_THRESHOLD_DEGREES 15.0f

	using namespace System;
	using namespace System::Collections::Generic;

	ref class Line;
	ref class Circle;
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

	private:
		List<Line ^ > ^ lines;
		float angleRadians;
		
		static void AddLineToSetList(Line ^ line, List<ParallelLineSet ^> ^ setList);
		bool isParallelTo(float otherAngleRadians);
		bool isPerpendicularTo(float otherAngleRadians);
	}

	ref class ShapeClassifier
	{
	public:
		ShapeClassifier(ShapeRecognizer ^ parentRecognizer);
		
		String ^ classifyShape(List<Line ^> ^ imgLines, List<Circle ^> ^ imgCircles, int imgSize);
	private:
		String ^ doClassification();
		bool hasCurves();
		bool isCross();
		bool isTriangle();
		bool isQuadrilateral();
		bool hasFiveLines();
		bool hasSixLines();
		String ^ classifyCurvyShape();
		String ^ classifyQuadrilateral();
		String ^ classifyFiveLineShape();
		String ^ classifySixLineShape();
		//String ^ classifyQuadrilateral(List<Line ^> ^ lines);
		//String ^ classifyPentShape(List<Line ^> ^ lines);
		//String ^ classifyHexaShape(List<Line ^> ^ lines);

		int imageSize;
		List<Line ^> ^ lines;
		List<Circle ^> ^ circles;
		ShapeRecognizer ^ shapeRecognizer;
	};

}