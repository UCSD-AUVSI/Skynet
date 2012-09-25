/*
 *  Auvsi_DualTree.cpp
 *  OpenCV21btest
 *
 *  Created by Lewis Anderson on 5/29/10.
 *  Copyright 2010 UCSD AUVSI. All rights reserved.
 *
 */

#include "Auvsi_DualTree.h"
#include "Resampler.h"
#include <new>

#define LENGTH_OF_VECTOR 64
#define LENGTH_OF_FILTER 10 // also amount to pad by

/* Constructor:
 *		allocate each pass, set default values
 */
Auvsi_DualTree::Auvsi_DualTree()
{
	stages = 4;
		
	inputVector = new float[64];
		
	isRow = true;

	
	// make Faf: (for first stage in each tree)
	float Faf[][10][2] = { { {			0,				   0},	// Tree 1: {low, high}
					{	-0.08838834764832, -0.01122679215254}, 
					{	 0.08838834764832,  0.01122679215254},
					{	 0.69587998903400,  0.08838834764832},
					{	 0.69587998903400,  0.08838834764832},
					{	 0.08838834764832, -0.69587998903400},
					{	-0.08838834764832,  0.69587998903400},
					{	 0.01122679215254, -0.08838834764832},
					{	 0.01122679215254, -0.08838834764832},
					{					0,				   0}},
					
				{	{	0.01122679215254,                  0},	// Tree 2: {low, high}
					{   0.01122679215254,                  0},
					{  -0.08838834764832,  -0.08838834764832},
					{   0.08838834764832,  -0.08838834764832},
					{   0.69587998903400,   0.69587998903400},
					{   0.69587998903400,  -0.69587998903400},
					{   0.08838834764832,   0.08838834764832},
					{  -0.08838834764832,   0.08838834764832},
					{                  0,   0.01122679215254},
					{                  0,  -0.01122679215254} }};
					
	// copy to instance variable
	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 10; j++) {
			filterFafLo[i][j] = Faf[i][j][0]; // low-pass
			filterFafHi[i][j] = Faf[i][j][1]; // high-pass
		}

	// make af: (for stages 2,3,4 in each tree)
	float af[][10][2] = 
		{ { {	 0.0351638400,				   0},		// Tree 1: {low, high}
			{				0,				   0},
			{-0.0883294200000, -0.11430184000000},
			{0.23389032000000,				   0},
			{0.76027237000000,  0.58751830000000},
			{0.58751830000000, -0.76027237000000},
			{				0,  0.23389032000000},
			{-0.1143018400000,  0.08832942000000},
			{				0,				   0},
			{				0, -0.03516384000000} },
			
		  { {                 0,  -0.03516384000000},	// Tree 2: {low, high}
			{                 0,                  0},
			{ -0.11430184000000,   0.08832942000000},
			{                 0,   0.23389032000000},
			{  0.58751830000000,  -0.76027237000000},
			{  0.76027237000000,   0.58751830000000},
			{  0.23389032000000,                  0},
			{ -0.08832942000000,  -0.11430184000000},
			{                 0,                  0},
			{  0.03516384000000,                  0} } };
	// copy to instance variable
	for (int i = 0; i < 2; i++) 
		for (int j = 0; j < 10; j++) {
			filterafLo[i][j] = af[i][j][0]; // low-pass
			filterafHi[i][j] = af[i][j][1]; // high-pass
		}
}

/*	Fill inputVector with a row or column from the image, in order to process another line
 *		(called once per line)
 */
void
Auvsi_DualTree::fillInputVector(int lineNumber)
{
	if (isRow) {
		// fill current row into inputVector
		float* ptrBegin = inputImage.ptr<float>(lineNumber);
		for (int i = 0; i < inputImage.size().width; i++) {
			inputVector[i] = ptrBegin[i];
			//printf("%f\t", inputVector[i]);
		}
	} else {
		// fill current column into inputVector
		float* ptrBegin;
		for (int i = 0; i < inputImage.size().height; i++) {
			ptrBegin = inputImage.ptr<float>(i);
			inputVector[i] = ptrBegin[lineNumber];
		}	

	}
}

/*	Circular shift
 *		shifts input by "shift" amount
 *	Memory Considerations:
 *		returns shifted copy of input
 *		(user must free input and result)
 */
float *
Auvsi_DualTree::cshift(float * input, int length, int shift)
{
	float * result = new float[length];
	
	shift = -shift; // otherwise its backwards

	// loop through, shifting and copying input into results
	for (int i = 0, j = shift; i < length; i ++, j++) {
		if (j < 0)
			result[i] = input[j + length];
		else if (j >= length)
			result[i] = input[j - length];
		else 
			result[i] = input[j];
	}
	return result;
}

/*	Runs upfirdn analysis
 *		runs FIR on input array using filter, and downsamples by a factor of 2
 *	returns results
 *	Memory Considerations:
 *		allocates the result
 *		does NOT deallocate input
 *
 *	new results
 *	return results
 *		(pads input by ten elements to get the rest of the values)
 *		see https://opensource.motorola.com/sf/discussion/do/listPosts/projects.upfirdn/discussion.general_comments.topc2009?_pagenum=2
 *
 */
float *
Auvsi_DualTree::runUpfirdn(float *input, int inLength, float *filter, int filterLength)
{
	// pad input by ten to flush all values out
	float *inputPadded = new float[inLength + filterLength];
	for (int i = 0; i < inLength + filterLength; i++) {
		if (i < inLength)
			inputPadded[i] = input[i];
		else 
			inputPadded[i] = 0.0f;
	}
	
	// begin resampling/filtering
	Resampler<float, float, float> theResampler(1, 2, filter, filterLength);
	int resultsCount = theResampler.neededOutCount(inLength + filterLength);		// calc size of output
	
	float * results = new float[resultsCount]; // allocate results

	int numSamplesComputed = theResampler.apply(inputPadded, inLength + filterLength, results, resultsCount); // run filtering
	delete[] inputPadded;
	
	return results;
}

/*	Runs analysis filter bank
 *		based of afb.m: [lo, hi] = afb(x, af)
 *	returns lo
 *	saves hi in instance variable (passX), length = 32, 16, 8, 4 of floats
 *	if ( tree = 1), save in .x of Complex else in .y of Complex
 *	
 *	allocates return, nothing else
 */
float *
Auvsi_DualTree::afb(float * input, float filterLo[10], float filterHi[10], int pass, int tree)
{
	// check for correct trees
	if (tree != 1 && tree != 2) {
		printf("ERROR: tree %d does not equal 1 or 2\n", tree);
		return NULL;
	}
	// choose length of current input and pass
	int length, inputCount; 
	switch (pass) {
		case 1:
			length = 32;
			inputCount = 64;
			break;
		case 2:
			length = 16;
			inputCount = 32;
			break;
		case 3:
			length = 8;
			inputCount = 16;
			break;
		case 4:
			length = 4;
			inputCount = 8;
			break;
		default:
			printf("ERROR: pass %d does not equal 1,2,3, or 4\n", pass);
			return NULL;
			break;
	}
	// circular shift
	float * inputShifted = cshift(input, inputCount, -5); // -length(af)/2
	
	// lowpass filter
	float * loResults = runUpfirdn(inputShifted, inputCount, filterLo, LENGTH_OF_FILTER);
	
	// process results
	for (int i = 0; i < LENGTH_OF_FILTER / 2; i++ )
		loResults[i] = loResults[inputCount/2 + i] + loResults[i];
	
	float * loFinal = new float[inputCount/2];
	for (int i = 0; i < inputCount/2; i++)
		loFinal[i] = loResults[i];
	delete[] loResults;

	// highpass filter
	float * hiResults = runUpfirdn(inputShifted, inputCount, filterHi, LENGTH_OF_FILTER);
	
	// process results
	for (int i = 0; i < LENGTH_OF_FILTER / 2; i++ )
		hiResults[i] = hiResults[inputCount/2 + i] + hiResults[i];
	
	float * hiFinal = new float[inputCount/2];
	for (int i = 0; i < inputCount/2; i++)
		hiFinal[i] = hiResults[i];
	delete[] hiResults;

	
	// copy hi into instance variables:
	//	tree 1 -> .x
	//	tree 2 -> .y
	//	pass 1/2/3/4 -> passOne/Two/Three/Four
	for (int i = 0; i < length; i++) {
		// check whether .x or .y
		if (tree == 1)	// .x
			switch (pass) { // decide which array 
				case 1:
					passOne[i].x = hiFinal[i];
					break;
				case 2:
					passTwo[i].x = hiFinal[i];
					break;
				case 3:
					passThree[i].x = hiFinal[i];
					break;
				case 4:
					passFour[i].x = hiFinal[i];
					break;
			}
		else			// .y
			switch (pass) { // decide which array
				case 1:
					passOne[i].y = hiFinal[i];
					break;
				case 2:
					passTwo[i].y = hiFinal[i];
					break;
				case 3:
					passThree[i].y = hiFinal[i];
					break;
				case 4:
					passFour[i].y = hiFinal[i];
					break;
			}
	}
	delete[] hiFinal; // free memory
	delete[] inputShifted;
	return loFinal;		// return low-pass results
}

/*	Compute 1D dualtree complex wavelet transform of inputVector
 *		brunt of computation is HERE
 */
void
Auvsi_DualTree::computeLine()
{	
	// normalize input vector
	for (int i = 0; i < LENGTH_OF_VECTOR; i++)
		inputVector[i] = inputVector[i] / sqrt(2.0);

	//	Tree 1
	float * resulta = afb(inputVector, filterFafLo[0], filterFafHi[0], 1, 1);
	float * resultb = afb(resulta, filterafLo[0], filterafHi[0], 2, 1); 
	delete[] resulta;
	float * resultc = afb(resultb, filterafLo[0], filterafHi[0], 3, 1); 
	delete[] resultb;
	float * resultd = afb(resultc, filterafLo[0], filterafHi[0], 4, 1); 
	delete[] resultc;
	// copy low-pass Tree 1 results into passFive.x
	for (int i = 0; i < 4; i++) 
		passFive[i].x = resultd[i];
	
	delete[] resultd;
	
	//	Tree 2
	resulta = afb(inputVector, filterFafLo[1], filterFafHi[1], 1, 2);
	resultb = afb(resulta, filterafLo[1], filterafHi[1], 2, 2); 
	delete[] resulta;
	resultc = afb(resultb, filterafLo[1], filterafHi[1],  3, 2); 
	delete[] resultb;
	resultd = afb(resultc, filterafLo[1], filterafHi[1],  4, 2); 
	delete[] resultc;
	
	//	copy low-pass Tree 2 results into passFive.y
	for (int i = 0; i < 4; i++) 
		passFive[i].y = resultd[i];
	
	delete[] resultd; // clear memory
	// at this point, passOne...Five is full of valid results
}


/*	Take results in each Complex array, and add them to the outputImage
 *		Do formatting to get rid of the ones we dont really care about:
 */
void
Auvsi_DualTree::addToResults(int lineNumber)
{
	// add current passes to outputResults
	
	typedef cv::Vec<float, 2> VT;
	
	VT *pointer = outputImage.ptr<VT>(lineNumber);
	
	// copy pass five into image (0 .. 3)
	for (int i = 0; i < 4; i++) {
		pointer[i][0] = passFive[i].x;
		pointer[i][1] = passFive[i].y;
	}
	// copy pass four (4 .. 7)
	for (int i = 0; i < 4; i++) {
		pointer[i + 4][0] = passFour[i].x;
		pointer[i + 4][1] = passFour[i].y;
	}
	// copy pass three (8 .. 15)
	for (int i = 0; i < 8; i++) {
		pointer[i + 8][0] = passThree[i].x;
		pointer[i + 8][1] = passThree[i].y;
	}
	if (lineNumber != 0)
		return;
	for (int i = 0; i < 16; i++)
		;//printf("%f %f\n", pointer[i][0], pointer[i][1]);
}

/*	Allocate/Clear outputResults
 *		class will compute multiple images, so get rid of previous results
 */
void
Auvsi_DualTree::clearResults()
{
	// clear outputResults, make a 16*128 2-channel cv::Mat
	outputImage = cv::Mat::zeros(128, 16, CV_32FC2);
}

/*	Run the dual tree complex wavelet transform on every line in the image, and keep relevant data
 */
void 
Auvsi_DualTree::runDualTree()
{
	clearResults();		// clear previous results
	int numLines;		// get number of rows/columns from inputImage
	if (isRow)
		numLines = inputImage.size().height;
	else
		numLines = inputImage.size().width;
	
	// iterate through each line
	for (int i = 0; i < numLines; i++) {
		fillInputVector(i);		// put current line into inputVector
		computeLine();			// compute inputVector
		addToResults(i);		// add results to outputImage;
	}

	// done!! :)
}

