//#include <cv.h>
#include <opencv2/opencv.hpp>
// #include <highgui.h>
// #include <ml.h>
#include "Auvsi_Recognize.h"
#include "Auvsi_Radon.h"
#include "Auvsi_DualTree.h"
#include "Auvsi_Fft.h"
#include "Auvsi_Ocr.h"
#include <ctime>
#include <cmath>
#include <iostream>
#include <dirent.h>
#include <sys/types.h>

#define SIZE_OF_FEATURE_VECTOR 320
#define NUMBER_K_NEAREST_NEIGHBORS 2

Auvsi_Ocr *theOCR;

using namespace std;

void computeImage(IplImage *src, float **letterDescriptor, float **shapeDescriptor) 
{
	//Auvsi_Ocr *theOCR = new Auvsi_Ocr();
	if (src != NULL)
	{
		theOCR->processImage(src, letterDescriptor, shapeDescriptor);
		cvReleaseImage(&src);
	}
	else
		std::printf("\t\tERROR: null image\n");
	//delete theOCR;
	

}

// return ASCII value of letter
float letterStrToInt(string letter)
{
	char letterChar = letter.c_str()[0];
	return (float)letterChar;

}

string letterIntToStr(float input) 
{
	char letter = (char)(int)input;
	return string(1, letter);
	
}

// return an int. 100 == unknown
float shapeStrToInt(string shape)
{
	string str2;
	if (0 == shape.compare(0, 4, "squa")) {
		return 0.0f;
	}

	if (0 == shape.compare(0, 4, "diam"))
		return 1.0f;
	if (0 == shape.compare(0, 4, "rect"))
		return 2.0f;
	if (0 == shape.compare(0, 4, "para"))
		return 3.0f;	
	if (0 == shape.compare(0, 4, "hexa"))
		return 4.0f;
	return 100.0f;

}

void parseFilename(string filename, string &letter, string &shape)
{
	shape = "abcd";
	// int startIndex = -1, finalIndex = -1;
	// for (unsigned int i = 0; i < filename.length(); i++) {
	// 	if (startIndex == -1) {
	// 		if (filename[i] == '_') 
	// 			startIndex = i + 1;
	// 	} else {
	// 		if (filename[i] == '_')
	// 			finalIndex = i - 1;
	// 	}
	// }
	string shape1(filename, 2, 4);
	shape = shape1;
	string letter1(filename, 0, 1);
	letter = letter1;
}

void cvPrintMat( CvMat* arr, bool transpose CV_DEFAULT(false) )
{
	
    int row, col, ch;
    // int coi = 0;
    // CvMat matstub;
    CvMat *mat = arr;
    // int depth
    int nChannels;
    CvScalar value;
    int rows, cols;
	
    // depth = CV_MAT_DEPTH( mat->type );
    nChannels = CV_MAT_CN( mat->type );
    rows = !transpose ? mat->rows : mat->cols;
    cols = !transpose ? mat->cols : mat->rows;
    
    for( row = 0; row < rows; row++ )
    {
        for( col = 0; col < cols; col++ )
        {
            value = cvGet2D( mat, !transpose ? row : col, !transpose ? col : row );
            if( nChannels > 1 )
            {
                printf( "(%lf", value.val[0] );
                for( ch = 1; ch < nChannels; ch++ )
                {
                    printf( " %lf", value.val[ch] );
                }
                printf( ") " );
            }
            else
            {
                printf( "%lf ", value.val[0] );
            }
        }
        printf("\n");
    }
    fflush( stdout );
}


void printcvMat( cv::Mat input, string name ) 
{
	cout << "****** input: " << name << " ******" << endl;
	
	CvMat cvmat = input;
	cvPrintMat( &cvmat, false );
	
	cout << "************************" << endl;
	
}

void loadDatabase(CvMat **data, CvMat **classes, string filename)
{
	string dataFileName = "";
	dataFileName += filename;
	dataFileName += ".data.cv";

	string classFileName = "";
	classFileName += filename;
	classFileName += ".classes.cv";

	*data = (CvMat*)cvLoad( dataFileName.c_str() );
	*classes = (CvMat*)cvLoad( classFileName.c_str() );
}

void saveDatabase(cv::Mat data, cv::Mat classes, string filename)
{
	CvMat dataMat = data;
	CvMat classesMat = classes;

	string dataFileName = "";
	dataFileName += filename;
	dataFileName += ".data.cv";

	string classFileName = "";
	classFileName += filename;
	classFileName += ".classes.cv";

	cvSave(dataFileName.c_str(),&dataMat);
	cvSave(classFileName.c_str(),&classesMat);
}

cv::Mat computeDirectory( const char * directory )
{
	typedef cv::Vec<float, 1> VT;
	
	VT *rowPointer;
	
	// ** to build database **
	float *letterVec, *shapeVec;
	string letterStr, shapeStr, filePath;
	
	int trainSampleCount = 37;
	
	cv::Mat trainLetterData, trainLetterClasses, trainShapeData, trainShapeClasses;
	
	trainLetterData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	trainLetterClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents letter
	trainShapeData = cv::Mat(trainSampleCount, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	trainShapeClasses = cv::Mat(trainSampleCount, 1, CV_32FC1); // 1 number represents shape
	
	theOCR = new Auvsi_Ocr();

	// get contents of directory
	DIR *dp;
	struct dirent *ep;

	dp = opendir (directory );
	if (dp == NULL) {
		printf("Couldn't open the directory");
		return cv::Mat(1, 1, CV_32FC1);
	}	
	cout << "\n** listing contents of directory\n";
	int index = 0;
	while ( (ep = readdir (dp))) // loop through files
	{
		cout << ep->d_name << endl;		// print name
		if (ep->d_name[0] == '.' || ep->d_name[0] == '_') // skip . .. _*
			continue;
		
		// read and compute image
		parseFilename(ep->d_name, letterStr, shapeStr);				// read letter and shape from filename
		filePath = directory;			// build full path for file
		filePath.append(ep->d_name);
		computeImage(cvLoadImage(filePath.c_str(), 1), &letterVec, &shapeVec); // compute feature descriptors
		
		// add letter + descriptor to letter database
		rowPointer = trainLetterData.ptr<VT>(index);		// save feature vector
		for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
			rowPointer[j] = letterVec[j];
		rowPointer = trainLetterClasses.ptr<VT>(index);		// save letter name
		rowPointer[0] = letterStrToInt(letterStr);
		
		// add shape + descriptor to shape database
		rowPointer = trainShapeData.ptr<VT>(index);			// save feature vector
		for (int i = 0; i < SIZE_OF_FEATURE_VECTOR; i++)
			rowPointer[i] = shapeVec[i];		
		rowPointer = trainShapeClasses.ptr<VT>(index);		// save shape name
		rowPointer[0] = shapeStrToInt(shapeStr);
		
		
		cout << "\t" << letterStr << ", " << shapeStr << " = " << letterStrToInt(letterStr) << ", " << shapeStrToInt(shapeStr) << "\n";
	
		index++;
		if (index >= trainSampleCount)
			break;	// too many samples ...

	} // end loop
	cout << "** done listing contents\n";
	(void) closedir (dp); 

	CvKNearest knn(trainLetterData, trainLetterClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);
	
	//printcvMat( trainLetterData, "trainLetterData" );
	//printcvMat( trainLetterClasses, "trainLetterClasses" );
	//printcvMat( trainShapeData, "trainShapeData" );
	//printcvMat( trainShapeClasses, "trainShapeClasses" );
	
	cv::Mat sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	filePath = directory;			// build full path for file
	filePath.append("j_square.png"); //("j_square.png");
	computeImage(cvLoadImage(filePath.c_str(), 1), &letterVec, &shapeVec); // compute feature descriptors
	// build sample from feature vector
	rowPointer = sample.ptr<VT>(0);		// save feature vector
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		rowPointer[j] = letterVec[j];

	
	cv::Mat nearests = cvCreateMat( 1, 1, CV_32FC1);
	
	float response = knn.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS, &nearests);
	
	// printcvMat( sample, "sample" );
	cout << "Response: " << response << " = " << letterIntToStr(response) << endl;
	
	
	saveDatabase(trainLetterData, trainLetterClasses, "/Users/lewisanderson/Code/OCR/database/letterData");
	saveDatabase(trainShapeData, trainShapeClasses, "/Users/lewisanderson/Code/OCR/database/shapeData");
	
	
	// cvWaitKey(0);


	return sample;
}

void testDirectory(const char * directory)
{
	CvMat *letterData, *letterClass, *shapeData, *shapeClass;
	float *letterVec, *shapeVec;
	string letterStr, shapeStr, filePath;

	loadDatabase(&letterData, &letterClass, "/Users/lewisanderson/Code/OCR/database/letterData");
	loadDatabase(&shapeData, &shapeClass, "/Users/lewisanderson/Code/OCR/database/shapeData");

	IplImage *temp = letterData;
	cv::Mat trainLetterData = cv::Mat(temp);
	cv::Mat trainLetterClasses, trainShapeData, trainShapeClasses;

	

	// trainLetterData = temp;


	trainLetterClasses = *letterClass;
	trainShapeData = *shapeData;
	trainShapeClasses = *shapeClass;

	computeImage(cvLoadImage(, 1), &letterVec, &shapeVec);



	CvKNearest knn(trainLetterData, trainLetterClasses, cv::Mat(), false, NUMBER_K_NEAREST_NEIGHBORS);
		
	cv::Mat sample = cv::Mat(1, SIZE_OF_FEATURE_VECTOR, CV_32FC1);
	computeImage(cvLoadImage("/Users/lewisanderson/Code/OCR/data/j_square.png", 1), &letterVec, &shapeVec); // compute feature descriptors

	// build sample from feature vector
	rowPointer = sample.ptr<VT>(0);		// save feature vector
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		rowPointer[j] = letterVec[j];

	
	cv::Mat nearests = cvCreateMat( 1, 1, CV_32FC1);
	
	float response = knn.find_nearest(sample, NUMBER_K_NEAREST_NEIGHBORS, &nearests);
	
	// printcvMat( sample, "sample" );
	cout << "Response: " << response << " = " << letterIntToStr(response) << endl;


}

int main(int argc, char** argv )
{

	testDirectory("/Users/lewisanderson/Code/OCR/data/","/Users/lewisanderson/Code/OCR/database/");
	// computeDirectory("/Users/lewisanderson/Code/OCR/data/");

	exit(0);

	// typedef cv::Vec<float, 1> VT;
	// cv::Mat radonInput = cv::Mat::zeros(64,64,CV_32F);
	// radonInput.at<VT>(32,32) = 1.0;
	// radonInput.at<VT>(32,0) = 1.0;
	// radonInput.at<VT>(32,63) = 1.0;
	// radonInput.at<VT>(0,32) = 1.0;
	// radonInput.at<VT>(63,32) = 1.0;
	// radonInput.at<VT>(0,0) = 1.0;

	// cv::Mat radonInput = cv::imread("/Users/lewisanderson/Code/OCR/temp/letter.png");
	// cv::Mat rInFloat( radonInput.size(), CV_32F );
	// radonInput.convertTo( rInFloat, rInFloat.type(), 1.0/255.0f );

	// printcvMat(rInFloat,"radon");


	// Auvsi_Radon *testRadon = new Auvsi_Radon();

	// cv::Mat radonTest = testRadon->computeRadon(rInFloat).clone();
	// cv::Mat radonTestNorm = cv::abs(radonTest);
	// cv::normalize(radonTestNorm, radonTestNorm, 0.0, 1.0, cv::NORM_MINMAX);
	// cv::imshow("input", radonInput);
	// cv::imshow("radon", radonTestNorm);
	// cv::waitKey(0);







// 	return 0;



	theOCR = new Auvsi_Ocr();

	//float *letter, *shape;
	//IplImage *poSrc = cvLoadImage("/Users/lewisanderson/Xcode Projects/OCR/data/g_square.png", 1);

	printf("start:%f\n", (float)clock() / (float)CLOCKS_PER_SEC);
	
	//computeImage(poSrc, &letter, &shape);	// run segmentation, radon, dual-tree, fft, get feature descriptor
	
	// test OCR
	
	Auvsi_Radon *theRadon = new Auvsi_Radon();
	Auvsi_DualTree *theDualTree = new Auvsi_DualTree();
	Auvsi_Fft *theFft = new Auvsi_Fft();
	
	
	// //============// test segmentation
	Auvsi_Recognize *recognize = new Auvsi_Recognize(cvLoadImage("/Users/lewisanderson/Code/OCR/data/t_rectangle.png", 1));

	// printf("preparing to run computation");

	recognize->runComputation();


	// printf("getLetter:");

	// cv::Mat letter = recognize->getLetter();
	// printf("getSum");
	// float sum = cv::sum(recognize->getLetter())[0];
	// printf("sum of first letter:%f\n", sum);
	// cv::imshow("letter",letter);
	// cv::imwrite("/Users/lewisanderson/Code/OCR/temp/letter.png", letter);
	// cv::waitKey(0);
	// //============// done testing segmentation
	
	// //============// testing radon
	// cv::Mat radonOrig = theRadon->computeRadon(recognize->_letter).clone();
	// cv::Mat radonOrigNorm = radonOrig.clone();
	// cv::normalize(radonOrigNorm, radonOrigNorm, 0.0, 1.0, cv::NORM_MINMAX);
	// cv::imshow("input", recognize->_letter);
	// cv::imshow("radon", radonOrigNorm);
	// //============// done testing radon



	//============// testing wavelet
	cv::Mat treeOrig = theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter));
	double minVal, maxVal;
	cv::minMaxLoc(treeOrig,&minVal,&maxVal);

	// printcvMat(theRadon->getResults(),"radonOrig");
	printcvMat(treeOrig,"tree");
	// printf("max:%.4f,\tmin:%.4f\n", maxVal, minVal);
	// printf("chan:%d\n",CV_MAT_CN( letter.type() ));
	// cv::waitKey(0);
	// exit(0);
	//============// done testing wavelet


	//============// testing fft
	float *fftOrig = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter)));
	printf("fft: ");
	for (int i = 0; i < 320; i++)
		printf("%.4f\t",fftOrig[i]);	
	printf("\n");
	// printcvMat(treeOrig,"tree");
	// printf("chan:%d\n",CV_MAT_CN( letter.type() ));
	cv::waitKey(0);
	exit(0);
	//============// done testing fft
	cv::Mat letter;

	// cv::Mat radonOrigNorm = cv::abs(radonOrig);
	// cv::normalize(radonOrigNorm, radonOrigNorm, 0.0, 1.0, cv::NORM_MINMAX);
	// cv::imshow("radon", radonOrigNorm);
	// cv::waitKey(0);

	
	float total = 0;
	float *letterFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter)));
	float *safeFirstDesc = new float[SIZE_OF_FEATURE_VECTOR];
	for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++)
		safeFirstDesc[j] = letterFeatDesc[j];
	
	for (int i = 0; i < 10000; i++) {
		// printf("iteration %d\n",i);
		delete recognize;

		IplImage *inputImg = cvLoadImage("/Users/lewisanderson/Code/OCR/data/t_rectangle.png", 1);
		recognize = new Auvsi_Recognize(inputImg);
		cvReleaseImage(&inputImg);

		recognize->runComputation();
		if (!recognize->checkAll())
			printf("ERROR: bad image after recognize->runComputation()");


		// cv::imshow("letter",recognize->getLetter());
		// cv::waitKey(0);

		
		typedef cv::Vec<unsigned char, 1> VT;
		unsigned char value;
		cv::Mat letterDiff = recognize->getLetter() - letter;

		cv::Mat compOutput = theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter));
		// display
		// cv::Mat radonDiff = radonOutput - radonOrig;
		cv::normalize(compOutput, compOutput, 0.0, 1.0, cv::NORM_MINMAX);
		cv::imshow("letter",recognize->getLetter());
		cv::imshow("radon", compOutput);

		printf("\t%d: let:%.1f\tradon:%.1f\n",i,cv::sum(letterDiff)[0], cv::sum(compOutput)[0]);
		cv::waitKey(100);
		// end display

		// float letterDiffSum = 0;
		// for (int r = 0; r < difference.rows; r++) {
		// 	for (int c = 0; c < difference.cols; c++) {
		// 		VT pixel = difference.at<VT>(r, c);
		// 		diff += (float)(value = pixel[0]);
		// 	}
		// }
		float sum = cv::sum(recognize->getLetter())[0];
		
		// if (diff > 1.0f)
		// 	printf("WARNING: difference: %.1f\tsum:%.1f\n", diff, sum);
		// else 
		// 	printf("SUCCESS: no difference\n");
		//cvWaitKey(0);
		
		// letterFeatDesc = theFft->computeFeatureDescriptor(theDualTree->runDualTree(theRadon->computeRadon(recognize->_letter)));

		// float thisError = 0, featSum = 0;
		// for (int j = 0; j < SIZE_OF_FEATURE_VECTOR; j++) {
		// 	featSum += letterFeatDesc[j];
		// 	thisError += fabs(letterFeatDesc[j] - safeFirstDesc[j]);
		// }
		// if (diff > 1.0f)
		// 	printf("SECOND WARNING: difference: %f\tsum:%.2f\n", thisError,featSum);
		// else 
		// 	printf("SECOND SUCCESS: no difference\n");
		// total += thisError;
	}
	cout << "Total error: " << total << endl;
	
	delete safeFirstDesc;
	delete theFft;
	delete theDualTree;
	delete theRadon;
	/*
	// test segmentation
	for (int i = 0; i < 10; i++) {
		Auvsi_Recognize *recognize = new Auvsi_Recognize(cvLoadImage("/Users/lewisanderson/Xcode Projects/OCR/data/j_square.png", 1));
		recognize->runComputation();
		cvWaitKey(0);
	}
	*/
	
	
	/*
	// test everything 
	cv::Mat first = computeDirectory("/Users/lewisanderson/Xcode Projects/OCR/data/");
	
	for (int i = 0; i < 5; i++) {
		cv::Mat second = computeDirectory("/Users/lewisanderson/Xcode Projects/OCR/data/");
		cv::Mat diffOne = first - second;
		printcvMat(diffOne, "first - i");
	}*/
	
	printf("done computing:%f\n", (float)clock() / (float)CLOCKS_PER_SEC);

	//cvReleaseImage (&poSrc);
	delete theOCR;
	// ** to compute image **
	// compute feature descriptors
	// compare letterDescriptor to letter database
	// compare shapeDescriptor to shape database
	
	// output closest letter and shape (or none)


}