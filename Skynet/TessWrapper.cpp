#include "StdAfx.h"
#include "TessWrapper.h"
#include "Util.h"
#include "VisionUtil.h"
#include "MasterHeader.h"
//#include "OCRTrainer.h"
#include "SystemSettings.h"
#include <cv.h>
#include <highgui.h>
#include <stdio.h>

using namespace Vision;
using namespace Util;
using namespace System;
using namespace System::IO;
using namespace System::Drawing;
using namespace System::Collections::Generic;

TessWrapper::TessWrapper(void)
{
	freopen ("C:\\myfile.txt","w",stderr);

	String ^ lang = "eng";

	ocr = gcnew tessnet2::Tesseract();
	ocr->SetVariable("tessedit_char_whitelist", "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ#");
	ocr->SetVariable("unlv_tilde_crunching", false);
	ocr->SetVariable("tessedit_zero_rejection", true);
	ocr->SetVariable("tessedit_zero_kelvin_rejection", true);
	ocr->Init(TESSDATA_DIRECTORY, lang, false);
}

String ^
TessWrapper::computeImage(cv::Mat blackWhiteImg) 
{
	if (blackWhiteImg.rows == 0 || blackWhiteImg.cols == 0)
		return "-";

	TessOCRAttempt ^bestResult = gcnew TessOCRAttempt("", -1);
	float maxRotation = 360.0f;
	float rotationStep = 20.0f;
	for (float rotation = 0.0f; rotation < maxRotation; rotation += rotationStep)
	{
		TessOCRAttempt ^attemptResult = attemptOCRForRotation(blackWhiteImg, rotation);
		bestResult->replaceIfBetter(attemptResult);

		//PRINT(rotation + ": " + attemptResult->str + " <-> " + attemptResult->conf);
	}

	return bestResult->str;
}

TessOCRAttempt ^TessWrapper::attemptOCRForRotation(cv::Mat img, float rotation)
{
	cv::Mat rotatedImage = rotateImage(img, rotation);
		
	Bitmap ^ bmp = matToBitmap(rotatedImage);
	
	List<tessnet2::Word ^> ^results = ocr->DoOCR(bmp, Rectangle::Empty);
	sortResultsByConfidence(results);

	return gcnew TessOCRAttempt(results[0]->Text, (float)results[0]->Confidence);
}

cv::Mat TessWrapper::rotateImage(const cv::Mat& source, double angle)
{
    cv::Point2f src_center(source.cols/2.0F, source.rows/2.0F);
    cv::Mat rot_mat = cv::getRotationMatrix2D(src_center, angle, 1.0);
    cv::Mat dst;
    warpAffine(source, dst, rot_mat, source.size());
    return dst;
}

public ref class WordComparer: IComparer<tessnet2::Word^>
{
public:
    virtual int Compare(tessnet2::Word^ x, tessnet2::Word^ y)
    {
        if (x == nullptr)
        {
            if (y == nullptr) return 0;
            else return -1;
        }
        else
        {
            if (y == nullptr) return 1;
            else return (int)(x->Confidence - y->Confidence);
        }
    }
};

void TessWrapper::sortResultsByConfidence(Collections::Generic::List<tessnet2::Word ^>^ words)
{
	words->Sort(gcnew WordComparer());
}

Bitmap ^
TessWrapper::matToBitmap(cv::Mat img)
{
	//img = thresholdImage(img);
	//http://stackoverflow.com/questions/15413688/grayscale-cvmat-to-system-bitmap
	Bitmap ^ bmp = gcnew Bitmap(img.size().width,
								img.size().height,
								img.step,
								Imaging::PixelFormat::Format8bppIndexed,
								(IntPtr)img.data);
	setGrayscalePalette(bmp);
	return bmp;
}

cv::Mat
TessWrapper::thresholdImage(cv::Mat input)
{
	double minVal, maxVal;
	cv::minMaxIdx(input, &minVal, &maxVal);

	return input < maxVal/2.0f;
}

void
TessWrapper::setGrayscalePalette(Bitmap ^ bmp)
{
	Imaging::ColorPalette^ pal = bmp->Palette;
	for(int i = 0; i <= 255; i++)
	{
		pal->Entries[i] = Color::FromArgb(i, i, i);
	}

	// The property returns a *copy* of the palette, so after modification, 
	// you have to set it back on the bitmap.
	bmp->Palette = pal;
}

String ^
TessWrapper::getBestWord(List<tessnet2::Word ^>^ words)
{
	bool listIsEmpty = words->Count == 0;
	if (listIsEmpty)
		throw gcnew TessCharNotMatchedException("Char not found");

	tessnet2::Word ^ bestWord = nullptr;
	for each (tessnet2::Word ^ word in words)
	{
		bool noBestWordYet = bestWord == nullptr;
		if (noBestWordYet)
		{
			bestWord = word;
		}
		else
		{
			bool thisWordIsBetter = bestWord->Confidence < word->Confidence;
			if (thisWordIsBetter)
				bestWord = word;
		}
	}

	return bestWord->Text;
}