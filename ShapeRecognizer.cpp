#include "StdAfx.h"
#include "ShapeRecognizer.h"
#include "BlobResult.h"
#include "Util.h"
#include "VisionUtil.h"
#include "Auvsi_Ocr.h"
#include <math.h>

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::Collections::Generic;

ShapeRecognizer::ShapeRecognizer(void)
{
}


String ^ 
ShapeRecognizer::recognizeShape(String ^ testShapeFilename)
{
	ManagedIPL ^image = gcnew ManagedIPL(testShapeFilename, CV_LOAD_IMAGE_GRAYSCALE);
	
	cv::Mat binaryImg = Auvsi_Ocr::prepareTrainingImage(image->img);

	return recognizeShape(binaryImg, testShapeFilename);
}

String ^ 
ShapeRecognizer::recognizeShape(cv::Mat binaryShapeImg, String ^ filename)
{
	if (binaryShapeImg.size().width != binaryShapeImg.size().height)
		throw gcnew Exception("ShapeRecognizer requires Square image - pad to square");

	setImageWithPreprocessing(binaryShapeImg);
	List<Line ^> ^ lines = findLines(filename);
	List<Circle ^> ^ circles = findCircles(filename);
	String ^ classification = classifyShape(lines, circles);

	if (filename != nullptr)
		saveImage(mImage->o(), filename + "_a_start.jpg");

	return classification;
}

void 
ShapeRecognizer::setImageWithPreprocessing(cv::Mat img)
{
	Canny(img, img, 5, 200, 3);
	mImage = gcnew MRef<cv::Mat>(img);
}

List<Line ^> ^ 
ShapeRecognizer::findLines(String ^ filename)
{
	std::vector<cv::Vec2f> lines = runHoughLines();
	lines = combineCloseLines(lines);
	lines = combineCloseLines(lines); // do two passes to merge all lines
	lines = filterSmallLines(lines);

	List<Line ^> ^ lineList = gcnew List<Line ^>(lines.size());
	for (int i = 0; i < lines.size(); ++i)
		lineList->Add(gcnew Line(lines[i]));

	cv::Mat output = printLinesToImage(lines, 128);
	
	if (filename != nullptr)
		saveImage(output, filename + "_b_lines.jpg");

	return lineList;
}

std::vector<cv::Vec2f>  
ShapeRecognizer::runHoughLines()
{
	std::vector<cv::Vec2f> lines;
	cv::Mat input = mImage->o().clone();
	double distResolution = 1;
	double angleResolution = CV_PI/180; // 180 is best
	cv::HoughLines(input, lines, distResolution, angleResolution, (int)HOUGH_LINE_THRESHOLD);

	return lines;
}


std::vector<cv::Vec2f>  
ShapeRecognizer::combineCloseLines(std::vector<cv::Vec2f> lines)
{
	std::vector<cv::Vec2f> newLines;
	for (int i = 0; i < lines.size(); ++i)
	{
		cv::Vec2f lineToAdd = lines[i];
		bool foundCloseLine = false;
		for (int j = 0; j < newLines.size(); ++j)
		{
			cv::Vec2f lineToBeat = newLines[j];

			try {
				if (!linesAreClose(lineToAdd, lineToBeat))
					continue;

				foundCloseLine = true;
				if (leftLineIsBetter(lineToAdd, lineToBeat))
				{
					//PRINT("replaced a line(d,theta):" + VEC2STR(lineToAdd) + "old:" + VEC2STR(lineToBeat));
					newLines[j] = lineToAdd;
				}
			}
			catch (BadLineException ^) { PRINT("BADLINE AHHH"); }
		}

		if (!foundCloseLine)
		{
			//PRINT("added a line(d,theta):" + lineToAdd[0] + "," + lineToAdd[1]);
			newLines.push_back(lineToAdd);
		}
		//else
			//PRINT("ignored");
	}

	// debug
	/*for (int i = 0; i < newLines.size(); ++i)
	{
		cv::Vec2f line = newLines[i];
		PRINT("final line(d,theta):" + VEC2STR(line));
	}*/

	return newLines;
}


bool 
ShapeRecognizer::linesAreClose(cv::Vec2f leftLine, cv::Vec2f rightLine)
{	
	float locationDistance = calcDistanceBetweenLines(leftLine, rightLine);
	float angleDistance = distanceBetweenAngles(leftLine[1], rightLine[1], PI);

	bool angleIsClose = angleDistance < radians<float>(MIN_DISTANCE_BETWEEN_ANGLES_DEGREES);
	bool locationIsClose = locationDistance < MIN_DISTANCE_BETWEEN_LINES;
	
	/*PRINT("linesAreClose left(d,theta):" + VEC2STR(leftLine) + 
						" right(d,theta):" + VEC2STR(rightLine) + 
						" diff(d,theta):" + locationDistance + "," + angleDistance +
						" result:" + (angleIsClose && locationIsClose));*/
	
	return angleIsClose && locationIsClose;
}

float 
ShapeRecognizer::calcDistanceBetweenLines(cv::Vec2f leftLine, cv::Vec2f rightLine)
{
	if (linesIntersect(leftLine, rightLine))
		return 0.0f;
	
	cv::Point l1, l2, r1, r2;
	determineStartAndEnd(leftLine, l1, l2);
	determineStartAndEnd(rightLine, r1, r2);

	float minDistBetweenSamePts = std::min(distanceBetweenPoints(l1,r1),distanceBetweenPoints(l2,r2));
	float minDistBetweenOppositePts = std::min(distanceBetweenPoints(l2,r1),distanceBetweenPoints(l1,r2));

	float minDist = std::min(minDistBetweenSamePts, minDistBetweenOppositePts);

	return minDist;
}

bool 
ShapeRecognizer::linesIntersect(cv::Vec2f leftLine, cv::Vec2f rightLine)
{
	cv::Mat mainImage = mImage->o();
	cv::Mat leftImage = cv::Mat::zeros(mainImage.size(), CV_8UC1);
	cv::Mat rightImage = cv::Mat::zeros(mainImage.size(), CV_8UC1);
	
	drawLineInImage(leftLine, leftImage, 1);
	drawLineInImage(rightLine, rightImage, 1);

	cv::Mat overlap;
	cv::bitwise_and(leftImage, rightImage, overlap);
	int numOverlappingPixels = cv::countNonZero(overlap);
	bool linesHaveOverlap = numOverlappingPixels > 0;

	return linesHaveOverlap;
}

float 
ShapeRecognizer::distanceBetweenPoints(cv::Point a, cv::Point b)
{
	return sqrt(pow((float)a.x - b.x, 2) + pow((float)a.y - b.y, 2));
}

bool 
ShapeRecognizer::leftLineIsBetter(cv::Vec2f leftLine, cv::Vec2f rightLine)
{
	float leftStrength = calcLineStrength(leftLine);
	float rightStrength = calcLineStrength(rightLine);
	
	//PRINT("\t\t" + VEC2STR(leftLine) + "," + VEC2STR(rightLine));

	
	return leftStrength > rightStrength;
}

cv::Mat 
ShapeRecognizer::printLinesToImage(std::vector<cv::Vec2f> lines, uchar color)
{
	cv::Mat input = mImage->o();
	cv::Mat output = input.clone();//cv::Mat::zeros(mImage->o().size(), CV_8UC1);
	// taken from OpenCV hough tutorial
	for( size_t i = 0; i < lines.size(); i++ )
	{
		try {
			drawLineInImage(lines[i], output, color);
			
		}
		catch (BadLineException ^) { PRINT("BADLINE AHHH"); }
	}
	return output;
}

void 
ShapeRecognizer::drawLineInImage(cv::Vec2f line, cv::Mat image, uchar color)
{
	cv::Point start, end;
	determineStartAndEnd(line, start, end);
	cv::line(image, start, end, cv::Scalar(color), 1, 4);
}

float 
ShapeRecognizer::calcLineStrength(cv::Vec2f line)
{
	cv::Point start, end;
	determineStartAndEnd(line, start, end);
	return calcLineStrength(start, end);
}

void 
ShapeRecognizer::determineStartAndEnd(cv::Vec2f line, cv::Point & start, cv::Point & end)
{
	float rho = line[0];
	float theta = line[1];
	float a = cos(theta);
	float b = sin(theta);
	float x0, y0;
	//float x0 = a*rho;
	//float y0 = b*rho;
	
	stepAlongLineToFindCenterpoint(rho, theta, a, b, x0, y0);
	stepAlongLineToFindEndpoints(x0, y0, a, b, start, end);


	//PRINT("start:" + PTSTR(start) + ", end:" + PTSTR(end) + "x0,y0,a,b:" SCOM x0 SCOM y0 SCOM a SCOM b);
}

void 
ShapeRecognizer::stepAlongLineToFindCenterpoint(float rho, float theta, float a, float b, float & x0, float & y0)
{
	float dist = -1000.0f;
	float xi = a*rho;
	float yi = b*rho;

	while (!coordinateIsWithinImage(x0, y0) && dist < 1000.0f)
	{
		dist += 4;
		calcLineCoordinate(xi, yi, a, b, dist, x0, y0);

		if (dist > 1000.0f)
			throw gcnew BadLineException();
	}
}

void 
ShapeRecognizer::stepAlongLineToFindEndpoints(float x0, float y0, float a, float b, cv::Point & start, cv::Point & end)
{
	float dist = 0;
	float x=x0;
	float y=y0;

	while (coordinateIsWithinImage(x, y))
	{
		dist += 1;
		calcLineCoordinate(x0, y0, a, b, dist, x, y);
	}
	start = cv::Point((int)x, (int)y);

	x=x0;
	y=y0;
	dist = 0;
	while (coordinateIsWithinImage(x, y))
	{
		dist -= 1;
		calcLineCoordinate(x0, y0, a, b, dist, x, y);
	}
	end = cv::Point((int)x, (int)y);
}

void 
ShapeRecognizer::calcLineCoordinate(float x0, float y0, float a, float b, float dist, float & x, float & y)
{
	x = x0 + dist*(-b);
	y = y0 + dist*(a);
}

bool 
ShapeRecognizer::coordinateIsWithinImage(float x, float y)
{
	int imageSize = mImage->o().size().width;
	int borderSize = 2;
	return x > borderSize && x < imageSize-borderSize && y > borderSize && y < imageSize-borderSize;
}

float 
ShapeRecognizer::calcLineStrength(cv::Point start, cv::Point end)
{
	cv::Mat input = mImage->o();
	cv::Mat mask = cv::Mat::zeros(input.size(), CV_8UC1);
	int lineThickness = 1; // set to 2,8 if we are missing a lot of lines
	int lineType = 4;
	cv::line(mask, start, end, cv::Scalar(1), lineThickness, lineType);
	cv::Mat overlap = mask.mul(input);
	overlap = overlap > 0;
	//int strength = getSizeOfBiggestBlobs(overlap);
	int strength = cv::countNonZero(overlap);
	//PRINT("\t\t" + PTSTR(start) + "," + PTSTR(end) + " - strength:" + strength);

	return (float)strength;
}

int 
ShapeRecognizer::getSizeOfBiggestBlobs(cv::Mat mask)
{
	throw gcnew Exception("I think this doesn't work right");
	CBlobResult blobs;
	CBlob * currentBlob;
	IplImage binaryIpl = mask;
	blobs = CBlobResult( &binaryIpl, NULL, 0 );

	int minBlobSize = 3;
	blobs.Filter( blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobSize );

	int sumOfBlobArea = 0;
	for (int i = 0; i < blobs.GetNumBlobs(); i++ )
	{
    	currentBlob = blobs.GetBlob(i);
		sumOfBlobArea += (int)currentBlob->Area();
		PRINT("area:" + currentBlob->Area());
	}
	PRINT("sumOfBlobArea:" + sumOfBlobArea);
	
	return sumOfBlobArea;
}

std::vector<cv::Vec2f>  
ShapeRecognizer::filterSmallLines(std::vector<cv::Vec2f> lines)
{
	float minLineStrength = HOUGH_LINE_THRESHOLD;
	std::vector<cv::Vec2f> newLines;
	for (int i = 0; i < lines.size(); ++i)
	{
		try {
			float lineStrength = calcLineStrength(lines[i]);
			if (lineStrength > minLineStrength)
			{	
				//PRINT("finalline(d,theta):" + VEC2STR(lines[i]));
				newLines.push_back(lines[i]);
			}
		}
		catch (BadLineException ^) { PRINT("BADLINE AHHH"); }
	}
	return newLines;
}

List<Circle ^> ^ 
ShapeRecognizer::findCircles(String ^ filename)
{
	
	return nullptr;
}

String ^ 
ShapeRecognizer::classifyShape(List<Line ^> ^ lines, List<Circle ^> ^ circles)
{
	// TODO: check for circles

	int numLines = lines->Count;
	PRINT("numLines:" + numLines);
	switch (numLines)
	{
	case 3:
		return "tria";
	case 4:
		return classifyQuadrilateral(lines);
	case 5:
		return classifyPentShape(lines);
	case 6:
		return classifyHexaShape(lines);
	case 8:
		return "octa";
	}
	return "----";
}

float
ShapeRecognizer::maxDistanceForDrawLinesAndCenterFill(List<Line ^> ^ lines)
{
	std::vector<cv::Vec2f> stdLines = standardLinesFromManaged(lines);

	uchar grayColor = 128;
	char whiteColor = 255;

	cv::Mat image = printLinesToImage(stdLines, grayColor);
	image = centerFill(image, whiteColor);
	float maxDistance = farthestPixelFromCenter(image, whiteColor);

	return maxDistance;
}

sdt::vector<cv::Vec2f>
ShapeRecognizer::standardLinesFromManaged(List<Line ^> ^ lines)
{
	sdt::vector<cv::Vec2f> stdLines;
	for each (Line ^ line in lines)
		stdLines.push_back(line->standardLine());
	return stdLines;
}


