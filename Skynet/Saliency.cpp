#include "StdAfx.h"
#include "Saliency.h"
#include "SaveImage.h"
#include "DuplicateResolver.h"
#include "DatabaseStructures.h"
#include "ImageWithPlaneData.h"
#include "VisionController.h"
#include "GeoReference.h"
#include "Util.h"
#include "MasterHeader.h"
#include "SystemSettings.h"


#ifdef SALIENCY_ENABLED
#include "../SalientGreenCUDA/SalientGreenGPU.H"
#endif
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "../CvBlob/cvblob.h"

// Saliency update frequency (in miliseconds)
#define UPDATE_FREQUENCY 5

#define BORDER_PADDING	10 // in pixels


using namespace Vision;
using namespace Database;
using namespace System::Collections::Generic;
using namespace Communications;
using namespace System::Threading;
using namespace Util;
using namespace System;

#ifdef SALIENCY_ENABLED
sg::SalientGreenGPU green; // global variable, suck it
#endif

cv::Mat testImage;
Saliency::Saliency(VisionController^ visionController): 
	visionController(visionController),
	width(0), height(0), threshold(0.7f), frameCount(0)
{
}

void Saliency::runTest()
{
	runTestOnImageNamed("C:\\Users\\ucsd_auvsi\\Desktop\\saliency_test2.jpg");
}

void Saliency::runTestOnImageNamed(System::String ^ filename)
{
	using namespace Util;

	cv::Mat image = cv::imread(managedToSTL(filename), 1);
	cv::imwrite(  managedToSTL("C:\\Users\\ucsd_auvsi\\Desktop\\saliencytest_preconvert.bmp"), image);

	Frame ^ localFrame = nullptr; // TODO: Fix thisgcnew Frame(image);
	
#ifdef SALIENCY_ENABLED
	sg::SalientGreenGPU green;
#endif

	computeSaliencyForFrame(localFrame,true);
	analyzeResults(localFrame, image.rows, image.cols, true );

}


cv::Mat Saliency::convertImageForSaliency(cv::Mat image) {
	const double widthRatio = Saliency::MAX_IMAGE_WIDTH / (double)image.cols;
	const double heightRatio = Saliency::MAX_IMAGE_HEIGHT / (double)image.rows;
	const double ratio = System::Math::Min(widthRatio, heightRatio);
	// TODO: Fix
	if ( widthRatio != 1.0 || heightRatio != 1.0) {
		cv::Mat result;
		cv::resize(image,result,cv::Size(0,0),ratio,ratio);
		setValues(result.cols, result.rows);
		return result;
	} else {
		setValues(image.cols, image.rows);
		return image;
	}
}
	

void
Saliency::computeSaliencyForFrame(Frame ^ frame, bool debug)
{
#ifdef SALIENCY_ENABLED
	frameCount++;

	sg::SalientGreenGPU::labWeights lw;
	sg::SalientGreenGPU::Results results;

	cv::Mat frameImage = *(frame->planeState->image);
	cv::Mat inputImage = convertImageForSaliency(frameImage);

	results = green.computeSaliencyGPU( inputImage, &lw );

	// run connected components
	cvb::CvBlobs blobs;

	double threshold = 0.7;

	cv::Mat labSaliency8bit(results.labSaliency.size(), CV_8UC1);
	results.labSaliency.convertTo(labSaliency8bit,CV_8UC1,1.0);
	cv::threshold(labSaliency8bit,labSaliency8bit,threshold*255,255.0,CV_THRESH_BINARY);

	// do labelling
	IplImage saliencyOutputImgIpl = labSaliency8bit;
	IplImage *bloblsOutputImgIpl = cvCreateImage(results.labSaliency.size(), IPL_DEPTH_LABEL, 1);
	unsigned int result = cvb::cvLabel(&saliencyOutputImgIpl, bloblsOutputImgIpl, blobs);

	/**
	 * Output image for rendering the blobs - debug only
	 */
	cv::Mat saliencyBlobImage;
	if (debug){
		cv::cvtColor(results.labSaliency,saliencyBlobImage,CV_GRAY2RGB);
	}

	// store blob results in Frame object
	for (cvb::CvBlobs::const_iterator it=blobs.begin(); it!=blobs.end(); ++it)
	{
		cvb::CvBlob * blob = (*it).second;
		frame->saliencyBlobs->Add(gcnew Box(blob->minx,blob->miny,blob->maxx,blob->maxy));
		if (debug){
			cv::rectangle(saliencyBlobImage,cv::Point(blob->minx,blob->miny),cv::Point(blob->maxx,blob->maxy),cv::Scalar(0,0,255), 5);
			PRINT("box: " + blob->minx+", "+blob->miny+", "+blob->maxx+", "+blob->maxy);
		}
	}


	if (debug){ 
		cv::imwrite( managedToSTL("C:\\Saliency_Test_Output\\SaliencyInput" + frame + ".jpg"),inputImage);
		cv::imwrite( managedToSTL("C:\\Saliency_Test_Output\\SaliencyThreshold" + frame + ".jpg"),labSaliency8bit);
		cv::imwrite( managedToSTL("C:\\Saliency_Test_Output\\SaliencyBlobs" + frame + ".jpg"),saliencyBlobImage);
		PRINT("Saliency: " + frame->saliencyBlobs->Count + " objects found");
	}
	cvReleaseBlobs(blobs);
	cvReleaseImage(&bloblsOutputImgIpl);
#endif
}


void 
Saliency::analyzeResults ( Frame ^ frame, int saliencyImageWidth, int saliencyImageHeight, bool debug )
{
	ImageWithPlaneData^ state = frame->planeState;
	double metersPerXPixel, metersPerYPixel;
	{ // georef calculations
		double plane_lat, plane_lon, plane_alt;
		double plane_roll, plane_pitch, plane_heading;
		double gimbal_roll, gimbal_pitch, gimbal_yaw;
		plane_lat = state->latitude;
		plane_lon = state->longitude;
		plane_alt = state->altitude;
		plane_roll = state->roll * 3.14159 / 180.0;
		plane_pitch = state->pitch * 3.14159 / 180.0;
		plane_heading = state->yaw;
		gimbal_roll = state->gimbalRoll;
		gimbal_pitch = state->gimbalPitch;
		gimbal_yaw = 0;

		double target_x = 0, target_y = 0, zoom = 1, t_lat = -1, t_lon = -1, t_alt = -1;
		double t_lat_1 , t_lat_2, t_lon_1, t_lon_2;

		target_x = 0; target_y = 100;
		try{ 
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt,
								 plane_roll, plane_pitch, plane_heading,
								 gimbal_roll, gimbal_pitch, gimbal_yaw,
								 target_x, target_y, zoom,
								 saliencyImageWidth, saliencyImageHeight,
								 t_lat_1, t_lon_1, t_alt);
			target_x = 200; target_y = 100;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt,
								 plane_roll, plane_pitch, plane_heading,
								 gimbal_roll, gimbal_pitch, gimbal_yaw,
								 target_x, target_y, zoom,
								 saliencyImageWidth, saliencyImageHeight,
								 t_lat_2, t_lon_2, t_alt);

			metersPerXPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
					
			target_x = 100; target_y = 0;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt, 
									 plane_roll, plane_pitch, plane_heading,
									 gimbal_roll, gimbal_pitch, gimbal_yaw, 
									 target_x, target_y, zoom,
								 saliencyImageWidth, saliencyImageHeight,
									 t_lat_1, t_lon_1, t_alt);
			target_x = 100; target_y = 200;
			GeoReference::getGPS(plane_lat, plane_lon, plane_alt,
								plane_roll, plane_pitch, plane_heading,
								gimbal_roll, gimbal_pitch, gimbal_yaw,
								target_x, target_y, zoom, 
								saliencyImageWidth, saliencyImageHeight,
								t_lat_2, t_lon_2, t_alt);

			metersPerYPixel = GeoReference::distanceBetweenGPS(t_lat_1, t_lon_1, t_lat_2, t_lon_2) / 200.0f;
		} catch(Vision::GeoReferenceException^ e){
			PRINT("SALIENCY: GEOREF ERROR OCCURED: "+e);
			return;
		}
	} // end georef calcs

	double xRatio = (double)state->image->cols / saliencyImageWidth;
	double yRatio = (double)state->image->rows / saliencyImageHeight;


	cv::Mat image;

	if (debug){
		// TODO: Fix leak, if present
		image = state->image->clone();
		cv::rectangle(image,
					  cv::Point(5,5),
					  cv::Point(5 + (int)(Saliency::MAX_TARGET_SIDE_LENGTH_METERS / metersPerXPixel * xRatio),
								5 + (int)(Saliency::MAX_TARGET_SIDE_LENGTH_METERS / metersPerYPixel * yRatio)),
					  cv::Scalar(0,0,0),2);

		cv::rectangle(image,
					  cv::Point(5,5),
					  cv::Point(5 + (int)(Saliency::MIN_TARGET_SIDE_LENGTH_METERS / metersPerXPixel * xRatio),
								5 + (int)(Saliency::MIN_TARGET_SIDE_LENGTH_METERS / metersPerYPixel * yRatio)),
					  cv::Scalar(0,0,0),2);
	}

	// save blobs
	int i = 0;
	for each (Box ^box in frame->saliencyBlobs) 
	{
		// skip candidates of the wrong size
		if (!(validSize(((double)box->width())*metersPerXPixel) && validSize(((double)box->height())*metersPerYPixel))) {
			continue;
		}
		else {
			System::Diagnostics::Trace::WriteLine("Saliency: size of target included w:" + box->width()*metersPerXPixel + " h:" + box->height()*metersPerYPixel  );
		}

		// copy and save candidate image, with padding
		int leftCorner = (int)((box->minx > BORDER_PADDING ? box->minx - BORDER_PADDING : 0) * xRatio);
		int topCorner = (int)((box->miny > BORDER_PADDING ? box->miny - BORDER_PADDING : 0) * yRatio);
		int rightCorner = (int)((box->maxx + BORDER_PADDING < (int)saliencyImageWidth ? box->maxx + BORDER_PADDING : saliencyImageWidth) * xRatio);
		int bottomCorner = (int)((box->maxy + BORDER_PADDING < (int)saliencyImageHeight ? box->maxy + BORDER_PADDING : saliencyImageHeight) * yRatio);

		if (debug) {
			cv::rectangle(image,cv::Point(leftCorner, bottomCorner),cv::Point(rightCorner, topCorner),cv::Scalar(0,0,255), 5);
		}

		int blobWidth = (rightCorner - leftCorner); 
		int blobHeight = (bottomCorner - topCorner);


		cv::Mat blobImg = cv::Mat(*state->image, cv::Rect(leftCorner, topCorner, blobWidth, blobHeight));

		if (debug){
			// TODO: Put this back in
			// System::String ^ path = "C:\\Saliency_Test_Output\\Image"+analyzeframe+"_"+i+".bmp";
			// cv::imwrite(managedToSTL(path),blobImg);
		}
		else
		{
			System::String ^imageName =  frameCount + "_" + i + ".bmp";
			System::String ^path = HTTP_SERVER_TARGET_PATH + imageName;

			CandidateRowData ^ candidateData = gcnew CandidateRowData(state);
			candidateData->imageName = path;
				
			cv::imwrite(managedToSTL(path),blobImg);
			
			try {
				visionController->processSaliencyCandidate(candidateData);
			}
			catch (System::Exception ^ e) {		
				System::Diagnostics::Trace::WriteLine( "WARNING: Saliency exception: " + e);
			}

		}
		
		i += 1;
	}
	if (debug){
		// TODO: Put back in
		// cv::imwrite( managedToSTL("C:\\Saliency_Test_Output\\SaliencyBlobsPostGeoref" + analyzeframe + ".jpg"),image);
	}
}

bool Saliency::validSize(double size)
{
	return size < Saliency::MAX_TARGET_SIDE_LENGTH_METERS && size > Saliency::MIN_TARGET_SIDE_LENGTH_METERS;
}

// set new width, height, delegate
void 
Saliency::setValues(int w, int h) 
{
	width = w;
	height = h;
}

// copies new frame into buffer, sets flag
void 
Saliency::analyzeFrame(Frame ^ frame)	
{
	if (canAcceptFrame()){
		busy = true;
		computeSaliencyForFrame(frame, false);
		analyzeResults(frame, width, height, false );
		busy = false;
	}
}