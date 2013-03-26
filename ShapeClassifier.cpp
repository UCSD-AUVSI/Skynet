#include "StdAfx.h"
#include "ShapeClassifier.h"
#include "Auvsi_Ocr.h"
#include "Util.h"
#include "VisionUtil.h"

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;

ShapeClassifier::ShapeClassifier(ShapeRecognizer ^ parentRecognizer)
{
	shapeRecognizer = parentRecognizer;
}

String ^ 
ShapeClassifier::classifyShape(List<Line ^> ^ imgLines, List<Circle ^> ^ imgCircles, int imgSize)
{
	imageSize = imgSize;
	lines = imgLines;
	circles = imgCircles;

	String ^ result = doClassification();


	return result;
}


String ^ 
ShapeClassifier::doClassification()
{
	if (hasCurves())
		return classifyCurvyShape();

	if (isCross())
		return "cros";

	if (isTriangle())
		return "tria";

	if (isQuadrilateral())
		return classifyQuadrilateral();

	if (hasFiveLines())
		return classifyFiveLineShape();

	if (hasSixLines())
		return classifySixLineShape();
}

bool 
ShapeClassifier::hasCurves()
{
	return circles->Count > 0;
}

bool 
ShapeClassifier::isCross()
{
	List<ParallelLineSet ^> ^ lineSets = ParallelLineSet::GroupLines(lines);

	// two sets of parallel lines encompassing all lines
	bool hasTwoSetsOfParallelLines = lineSets->Count == 2;
	if (!hasTwoSetsOfParallelLines)
		return false;

	// each set is perpendicular to the other
	bool setsArePerpendicular = lineSets[0]->isPerpendicularTo(lineSets[1]);
	if (!setsArePerpendicular)
		return false;
	
	// center fill = no pixel more than 20 from center
	float maxDistanceFromCenter = parentRecognizer->maxDistanceForDrawLinesAndCenterFill();
	bool pixelGoesCloseToEdge = maxDistanceFromCenter > MAX_CROSS_CENTER_DISTANCE_FROM_CENTER;
	if (pixelGoesCloseToEdge)
		return false;

	return true;
}

bool 
ShapeClassifier::isTriangle()
{
	return lines->Count == 3;
}

bool 
ShapeClassifier::isQuadrilateral()
{
	return lines->Count == 4;
}

bool 
ShapeClassifier::hasFiveLines()
{
	return lines->Count == 5;
}

bool 
ShapeClassifier::hasSixLines()
{
	return lines->Count == 6;
}


String ^ 
ShapeClassifier::classifyCurvyShape()
{
	// TODO
	return "circ";
}

String ^ 
ShapeClassifier::classifyQuadrilateral()
{
	// TODO

	return "rect";
}

String ^ 
ShapeClassifier::classifyFiveLineShape()
{
	// TODO
	return "pent";
}

String ^ 
ShapeClassifier::classifySixLineShape()
{
	// TODO
	return "hexa";
}

List<ParallelLineSet ^> ^ 
ParallelLineSet::GroupLines(List<Line ^ > ^ lines)
{
	List<ParallelLineSet ^> ^ setList = gcnew List<ParallelLineSet ^>();
	for each (Line ^ line in lines)
	{
		AddLineToSetList(line, setList);
	}
	return setList;
}

void 
ParallelLineSet::AddLineToSetList(Line ^ line, List<ParallelLineSet ^> ^ setList)
{
	bool matchedLine = false;
	for each (ParallelLineSet ^ set in setList)
	{
		matchedLine = set->isParallelToLine(line);
		if (matchedLine)
		{
			set->addLine(line);
			break;
		}
	}
	if (!matchedLine)
		setList->Add(gcnew ParallelLineSet(line));
}

ParallelLineSet::ParallelLineSet(Line ^ line)
{
	angle = line->angle;

	lines = gcnew List<Line ^ >();
	addLine(line);
}

bool 
ParallelLineSet::isParallelToLine(Line ^ line)
{
	return isParallelTo(line->angle);
}

void 
ParallelLineSet::addLine(Line ^ line)
{
	lines->Add(line);
}

bool 
ParallelLineSet::isParallelTo(ParallelLineSet ^ set)
{
	return isParallelTo(set->angleRadians);
}

bool 
ParallelLineSet::isPerpendicularTo(Line ^ line)
{
	return isPerpendicularTo(line->angle);
}

bool 
ParallelLineSet::isPerpendicularTo(ParallelLineSet ^ set)
{
	return isPerpendicularTo(set->angleRadians);
}

bool 
ParallelLineSet::isParallelTo(float otherAngleRadians)
{
	float angleDistance = distanceBetweenAngles(angleRadians, otherAngleRadians, PI);
	return angleDistance < radians(PARALLEL_ANGLE_THRESHOLD_DEGREES);
}

bool 
ParallelLineSet::isPerpendicularTo(float otherAngleRadians)
{
	float angleDistance = distanceBetweenAngles(angleRadians+PI/2, otherAngleRadians, PI);
	return angleDifferenceRadians < radians(PARALLEL_ANGLE_THRESHOLD_DEGREES);
}

