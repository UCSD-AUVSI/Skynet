#include "StdAfx.h"
#include "TargetRecognizer.h"
#include "ShapeFinder.h"
#include "ShapeRecognizer.h"
#include "TessWrapper.h"
#include <msclr/lock.h>
#include "Auvsi_Recognize.h"
#include "VisionUtil.h"
#include "OCRWrapper.h"
//#include "Color_Util.h"

using namespace Vision;
using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace msclr;

/**
 * TODO: TargetRecognizer shouldn't have mutable state
 */
TargetRecognizer::TargetRecognizer(void)
{
	filename = nullptr;
	if (tessOCR == nullptr)
		tessOCR = gcnew TessWrapper();
}

TargetResult ^ 
TargetRecognizer::recognizeTarget(String ^ testShapeFilename)
{
	ManagedIPL ^image = gcnew ManagedIPL(testShapeFilename, CV_LOAD_IMAGE_COLOR);
	return recognizeTarget(image->img, testShapeFilename);
}

/**
 * TODO: nullptr should never be passed in as a parameter
 */
TargetResult ^ 
TargetRecognizer::recognizeTarget(cv::Mat img)
{
	return recognizeTarget(img, nullptr);
}

TargetResult ^ 
TargetRecognizer::recognizeTarget(cv::Mat img, String ^ filepath)
{
	filename = filepath;

	mColorImg = gcnew MRef<cv::Mat>(img);

	return doRecognition();
}

TargetResult ^ 
TargetRecognizer::doRecognition()
{
	segmentShape();
	String ^ shape = recognizeShape();

	segmentLetter();
	String ^ letter = recognizeLetter();

    String ^ color = "TODO";//recognizeColor(mColorImg->o());

	double targetOrientationDegrees = calculateTargetOrientationDegrees();
	
	return gcnew TargetResult(letter, shape, "", "", targetOrientationDegrees);
}

void 
TargetRecognizer::segmentShape()
{
	ShapeFinder ^ shapeFinder = gcnew ShapeFinder();
	cv::Mat shape = shapeFinder->findShape(mColorImg->o(), nullptr);
	mShapeImg = gcnew MRef<cv::Mat>(shape);
	
	if (!shapeIsValid())
		throw gcnew TargetNotFound();

	if (filename != nullptr)
		saveImage(shape, filename + "_a_shape.jpg");

}

bool 
TargetRecognizer::shapeIsValid()
{
	cv::Mat shape = mShapeImg->o();
	float shapeCoverageRatio = ratioOfBlobToWhole(shape); 
	bool shapeIsLarge = shapeCoverageRatio > 0.1f;
	
	return shapeIsLarge;
}

String ^ 
TargetRecognizer::recognizeShape()
{
	cv::Mat shape = mShapeImg->o();
	cv::Mat shapeSquare = Auvsi_Recognize::ProcessShape(shape);
	ShapeRecognizer ^ shapeRecognizer = gcnew ShapeRecognizer();
	return shapeRecognizer->recognizeShape(shapeSquare);
}

double
TargetRecognizer::calculateTargetOrientationDegrees(){
	return 0; // TODO: Implement
}

void 
TargetRecognizer::segmentLetter()
{
	cv::Mat letter = Auvsi_Recognize::SegmentLetter(mColorImg->o(), mShapeImg->o());
	mLetterImg = gcnew MRef<cv::Mat>(letter);

	if (filename != nullptr)
		saveImage(letter, filename + "_b_letter.jpg");
}

String ^ 
TargetRecognizer::recognizeLetter()
{
	if (!letterIsValid())
		return nullptr;
	return recognizeLetter(mLetterImg->o());
}

bool 
TargetRecognizer::letterIsValid()
{
	cv::Mat letter = mLetterImg->o();

	float coverageRatio = ratioOfBlobToWhole(letter); 
	bool letterIsLarge = coverageRatio > 0.0001f;
	
	return letterIsLarge;
}

String ^ 
TargetRecognizer::recognizeLetter(cv::Mat img)
{
	lock l(tessOCR);
	String ^ letter = tessOCR->computeImage(img)->ToLower();
	bool guessIsInvalid = letter == nullptr || letter->Length != 1 || letter->Equals("~");
	if (guessIsInvalid)
		return nullptr;
	else
		return "" + letter;
}

/*
String ^
TargetRecognizer::recognizeColor(cv::Mat img)
{
    return countColors(img);
}

String ^
TargetRecognizer::countColors(cv::Mat img) {
    std::vector<Color_Util::Pix> colors;
    bool found = false;
    for(int i = 0; i < img.size().height; i++) {
        for(int j = 0; j < img.size().width; j++) {
            cv::Vec3b current = img.at<cv::Vec3b>(i, j);

            for(int k = 0; k < colors.size(); k++) {
                if(colors[k].color == current) {
                    colors[k].count++;
                    found = true;
                }
            }

            if(!found)
                colors.push_back(Color_Util::Pix(current));

            found = false;
        }
    }

    return averageColors(colors);
}

String ^
TargetRecognizer::averageColors(const std::vector<Color_Util::Pix>& colors) {
    unsigned int bsum = 0;
    unsigned int rsum = 0;
    unsigned int gsum = 0;
    int count = 0;
    
    // skip the first element since the most common element is the background
    for(int i = 1; i < colors.size(); i++) {
        bsum += colors[i].color.val[0];
        gsum += colors[i].color.val[1];
        rsum += colors[i].color.val[2];
        ++count;
    }

    double average_red = rsum/count;
    double average_green = gsum/count;
    double average_blue = bsum/count;

    return mapToName(average_red, average_green, average_blue);
}

String ^
TargetRecognizer::mapToName(double avgRed, double avgGreen, double avgBlue) {
    double avgHue, avgSat, avgLight;
    Color_Util::toHSV(avgRed, avgGreen, avgBlue, avgHue, avgSat, avgLight);

    std::vector<Color_Util::Color> colorDict;
    Color_Util::setColorDict(colorDict);

    double temp = std::numeric_limits<double>::max();
    int index = 0;

    for(int i = 0; i < colorDict.size(); i++) {
        double hue, sat, light;
        int red = colorDict[i].red;
        int green = colorDict[i].green;
        int blue = colorDict[i].blue;
        Color_Util::toHSV(red, green, blue, hue, sat, light);

        double temp2 = abs(hue-ahue) + abs(sat-asat) + abs(light-alight);

        if(temp2 < temp) {
            temp = temp2;
            index = i;
        }
    }

    return colorDict[index].name;
}
*/
