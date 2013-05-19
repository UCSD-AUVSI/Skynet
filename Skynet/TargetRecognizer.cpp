#include "StdAfx.h"
#include "TargetRecognizer.h"
#include "ShapeFinder.h"
#include "ShapeRecognizer.h"
#include "TessWrapper.h"
#include <msclr/lock.h>
#include "Auvsi_Recognize.h"
#include "VisionUtil.h"
#include "OCRWrapper.h"
#include "ColorRecognition_Util.h"

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

	String ^ shapeColor = recognizeColor(mColorImg->o(), mShapeImg->o());
	String ^ letterColor = recognizeColor(mColorImg->o(), mLetterImg->o());

	double targetOrientationDegrees = calculateTargetOrientationDegrees();
	
	return gcnew TargetResult(letter, shape, shapeColor, letterColor, targetOrientationDegrees);
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

String ^
TargetRecognizer::recognizeColor(cv::Mat colorImg, cv::Mat img)
{
	float r_sum = 0, g_sum = 0, b_sum = 0;
	int count = 0;

	cv::Vec3b white(255, 255, 255);
	for (int i = 0; i < img.rows; i++)
	{
		for (int j = 0; j < img.cols; j++)
		{
			cv::Vec3b pixel = img.at<cv::Vec3b>(i,j);

			if (pixel == white)
			{
				cv::Vec3b coloredPixel = colorImg.at<cv::Vec3b>(i,j);

				r_sum += coloredPixel.val[2];
				g_sum += coloredPixel.val[1];
				b_sum += coloredPixel.val[0];

				++count;
			}
		}
	}

	int red = r_sum/count;
	int green = g_sum/count;
	int blue = b_sum/count;

	String ^ color = gcnew String(ColorRecognition_Util::mapToColorName(red, green, blue).c_str());
	return color;
}