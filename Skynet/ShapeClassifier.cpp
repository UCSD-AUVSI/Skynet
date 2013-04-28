#include "StdAfx.h"
#include "ShapeClassifier.h"
#include "Auvsi_Ocr.h"
#include "Util.h"
#include "VisionUtil.h"
#include "ShapeRecognizer.h"
#include <algorithm>
#include <cmath>

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;

ShapeClassifier::ShapeClassifier(ShapeRecognizer ^ parentRecognizer)
{
	shapeRecognizer = parentRecognizer;
}

String ^ 
ShapeClassifier::classifyShape(List<Line ^> ^ imgLines, int imgSize)
{
	imageSize = imgSize;
	lines = imgLines;

	String ^ result = doClassification();


	return result;
}


String ^ 
ShapeClassifier::doClassification()
{
	lineSets = ParallelLineSet::GroupLines(lines);

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

	return "----";
}

bool 
ShapeClassifier::hasCurves()
{
	// check more than 6 lines
	bool hasALotOfLines = lines->Count > 6 && lineSets->Count > 2;
	if (hasALotOfLines)
		return true;

	float maxDistanceFromCenter = shapeRecognizer->maxDistanceForDrawLinesAndCenterFill(lines);
	float maxAllowedDistance = MAX_CLOSED_SHAPE_DIST_FROM_CENTER*imageSize/2.0f;
	bool shapeIsUnclosed = maxDistanceFromCenter > maxAllowedDistance;
	if (shapeIsUnclosed)
		return true;

	// TODO check less than 75% coverage
	
	if (isSemi(false))
		return true;
	
	if (isQuar(false))
		return true;

	return false;
}

bool 
ShapeClassifier::isSemi(bool definitelyCurved)
{
	bool useLowerThreshold = definitelyCurved;
	if (hasOnlyOneStrongLine(useLowerThreshold))
		return true;

	// TODO check weird shaped hexagons and such?

	return false;
}

bool 
ShapeClassifier::hasOnlyOneStrongLine(bool useLowerThreshold)
{
	float maxLineStrength = highestLineStrength();

	float lineStrengthThreshold = (useLowerThreshold ? maxLineStrength*.6f : maxLineStrength/3.0f);
	int numLinesAboveThreshold = 0;
	for each (Line ^ line in lines)
		if (line->strength > lineStrengthThreshold)
			numLinesAboveThreshold += 1;
	bool onlyOneStrongLine = numLinesAboveThreshold == 1;
	return onlyOneStrongLine;
}

float 
ShapeClassifier::highestLineStrength()
{
	float maxLineStrength = 0.0f;
	for each (Line ^ line in lines)
	{
		float thisStrength = line->strength;
		maxLineStrength = std::max(maxLineStrength, thisStrength);
	}
	return maxLineStrength;
}
bool 
ShapeClassifier::isQuar(bool definitelyCurved)
{
	if (lines->Count < 3)
		return false;

	if (isStar())
		return false; // 5 line star with missing line could look like quar

	if (lines->Count == 4 && lineSets->Count == 4)
		return hasPerpendicularLines();

	bool dontAggresivelyClassifyQuar = !definitelyCurved;
	if (dontAggresivelyClassifyQuar)
		return false;


	List<Line ^> ^ strongestAscending = findTwoStrongestLines();

	bool strongestLinesNotPerpendicular = !ParallelLineSet::linesArePerpendicular(strongestAscending[0], strongestAscending[1]);
	if (strongestLinesNotPerpendicular)
		return false;

	float strengthDifferenceThreshold = strongestAscending[1]->strength *.4f;
	float strengthDifference = abs(strongestAscending[1]->strength - strongestAscending[0]->strength);
	bool strongestLinesHaveUnevenStrength = strengthDifference > strengthDifferenceThreshold;
	if (strongestLinesHaveUnevenStrength)
		return false;

	float thirdHighestStrength = 0.0f;
	for each (Line ^ line in lines)
	{
		bool lineIsNotInStrongest = !strongestAscending->Contains(line);
		if (lineIsNotInStrongest)
		{
			float thisStrength = line->strength;
			thirdHighestStrength = std::max(thirdHighestStrength, thisStrength);
		}
	}
	
	float lowThreshold = strongestAscending[0]->strength*.6f;
	bool thirdBestIsMuchWeaker = thirdHighestStrength < lowThreshold;
	if (thirdBestIsMuchWeaker)
		return true;

	return false;
}

bool 
ShapeClassifier::hasPerpendicularLines()
{
	for each (Line ^ line in lines)
		for each (Line ^ possibleMatch in lines)
			if (line != possibleMatch && ParallelLineSet::linesArePerpendicular(line, possibleMatch))
				return true;
	return false;
}

List<Line ^> ^ 
ShapeClassifier::findTwoStrongestLines()
{
	int numStrongLines = 2;
	List<Line ^> ^ strongestAscending = gcnew List<Line ^>(numStrongLines);
	for each (Line ^ line in lines)
	{
		bool listNotFull = strongestAscending->Count < numStrongLines;
		if (listNotFull)
			strongestAscending->Add(line);
		else
		{
			bool thisLineIsBetter = line->strength > strongestAscending[0]->strength;
			if (thisLineIsBetter)
				strongestAscending[0] = line;
		}

		bool listIsFull = strongestAscending->Count > 1;
		if (listIsFull)
		{
			bool listIsNotSortedInAscendingOrder = strongestAscending[0]->strength > strongestAscending[1]->strength;
			if (listIsNotSortedInAscendingOrder)
				strongestAscending->Reverse();
		}
	}
	
	return strongestAscending;
}

bool 
ShapeClassifier::isCross()
{
	// two sets of parallel lines encompassing all lines
	bool hasTwoSetsOfParallelLines = lineSets->Count == 2;
	if (!hasTwoSetsOfParallelLines)
	{

		return false;
	}
	// each set is perpendicular to the other
	bool setsArePerpendicular = lineSets[0]->isPerpendicularTo(lineSets[1]);
	if (!setsArePerpendicular)
		return false;
	
	// center fill = no pixel more than 20 from center
	float maxDistanceFromCenter = shapeRecognizer->maxDistanceForDrawLinesAndCenterFill(lines);
	float maxAllowedDistance = MAX_CENTERFILL_DISTANCE_FROM_CENTER*imageSize/2.0f;
	bool pixelGoesCloseToEdge = maxDistanceFromCenter > maxAllowedDistance;
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
	if (isQuar(true))
		return "quar";

	if (isSemi(true)) // lines->Count < 4 || 
		return "semi";
	
	return "circ";
}

String ^ 
ShapeClassifier::classifyQuadrilateral()
{
	bool isParallelogram = lineSets->Count == 2;
	if (isParallelogram)
		return classifyParallelogram();

	if (isTrapezoid())
		return "trap";

	return classifyFailedQuadrilateral();
}

String ^ 
ShapeClassifier::classifyParallelogram()
{
	if (lineSets[0]->isPerpendicularTo(lineSets[1]))
		return classifyRectangle();

	return "para";
}

String ^ 
ShapeClassifier::classifyRectangle()
{
	float distOne = lineSets[0]->distanceBetweenLines();
	float distTwo = lineSets[1]->distanceBetweenLines();

	float aspectRatio = distOne/distTwo;
	float idealSquareAspectRatio = 1.0f;
	float aspectRatioError = std::abs(aspectRatio - idealSquareAspectRatio);
	bool isSquare = aspectRatioError < SQUARE_ASPECT_RATIO_THRESHOLD;
	if (isSquare)
		return "squa";
	else 
		return "rect";
}

bool 
ShapeClassifier::isTrapezoid()
{
	bool foundSetOfParallelLines = false;
	for each (ParallelLineSet ^ lineSet in lineSets)
		if (lineSet->numLines() > 1)
			foundSetOfParallelLines = true;
	return foundSetOfParallelLines;
}

String ^ 
ShapeClassifier::classifyFailedQuadrilateral()
{
	
	if (isStar())
		return "star"; // fallback if a star's line got missed

	return "----";
}

String ^ 
ShapeClassifier::classifyFiveLineShape()
{
	if (isStar())
		return "star";
	if (isFiveLinedQuar())
		return "quar";
	if (isFiveLinedSemi())
		return "semi";
	return "pent";
}

bool 
ShapeClassifier::isFiveLinedQuar()
{
	List<Line ^> ^ twoStrongest = findTwoStrongestLines();
	bool linesArentPerpendicular = !ParallelLineSet::linesArePerpendicular(twoStrongest[0], twoStrongest[1]);
	if (linesArentPerpendicular)
	{
		return false;
	}

	float sumOfStrongest = Line::SumOfLines(twoStrongest);
	float sumOfAll = Line::SumOfLines(lines);
	bool twoBestAreMoreThanHalf = sumOfStrongest > QUAR_STRAIGHT_DIST_TO_TOTAL_RATIO*sumOfAll;
	if (twoBestAreMoreThanHalf)
	{
		return true;
	}
	return false;
}

bool 
ShapeClassifier::isFiveLinedSemi()
{
	int lineThickness = 2;
	float percentCoverage = shapeRecognizer->calcPercentageLineCoverage(lines, lineThickness);
	bool significantGapsInCoverge = percentCoverage < PENT_COVERAGE_THRESH;
	if (significantGapsInCoverge)
		return true;

	return false;
}

String ^ 
ShapeClassifier::classifySixLineShape()
{
	if (isStar())
		return "star";
	return "hexa";
}

bool 
ShapeClassifier::isStar()
{
	// center fill = no pixel more than 20 from center
	float maxDistanceFromCenter = shapeRecognizer->maxDistanceForDrawLinesAndCenterFill(lines);
	float maxAllowedDistance = MAX_CENTERFILL_DISTANCE_FROM_CENTER*imageSize/2.0f;
	bool pixelGoesCloseToEdge = maxDistanceFromCenter > maxAllowedDistance;
	if (pixelGoesCloseToEdge)
		return false;

	return true;
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
		matchedLine = set->isParallelTo(line);
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
	angleRadians = line->angle;

	lines = gcnew List<Line ^ >();
	addLine(line);
}

bool 
ParallelLineSet::isParallelTo(Line ^ line)
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
	float angleDistance = distanceBetweenAngles(angleRadians, otherAngleRadians, (float)PI);
	return angleDistance < radians(PARALLEL_ANGLE_THRESHOLD_DEGREES);
}

bool 
ParallelLineSet::isPerpendicularTo(float otherAngleRadians)
{
	return anglesArePerpendicular(angleRadians, otherAngleRadians);
}

float 
ParallelLineSet::distanceBetweenLines()
{
	float smallestDistanceFromOrigin = 1000.0f;
	float largestDistanceFromOrigin = -1000.0f;
	for each (Line ^ line in lines)
	{
		float thisDistanceFromOrigin = line->distFromOrigin;
		smallestDistanceFromOrigin = std::min(thisDistanceFromOrigin, smallestDistanceFromOrigin);
		largestDistanceFromOrigin = std::max(thisDistanceFromOrigin, largestDistanceFromOrigin);
	}
	float distanceBetween = largestDistanceFromOrigin - smallestDistanceFromOrigin;
	return distanceBetween;
}


bool 
ParallelLineSet::linesArePerpendicular(Line ^ lineA, Line ^ lineB)
{
	return anglesArePerpendicular(lineA->angle, lineB->angle); 
}


bool 
ParallelLineSet::anglesArePerpendicular(float angleARadians, float angleBRadians)
{
	float angleDistance = distanceBetweenAngles(angleARadians+(float)(PI/2.0), angleBRadians, (float)PI);
	return angleDistance < radians(PARALLEL_ANGLE_THRESHOLD_DEGREES);
}

