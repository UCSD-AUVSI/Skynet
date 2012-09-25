/**
 * Auvsi_FFT.h
 *
 * Auvsi_FFT is a namespace containing various helper functions for performing
 * 2D R2C and C2R fourier transforms.
 *
 * The bulk of the code is taken from the convolutionFFT2D example in the CUDA SDK.
 *
 * authors:
 *		Shane Grant wgrant@ucsd.edu 2010
 *		Lewis Anderson lkanders@ucsd.edu 2010
 */

#pragma once

#ifdef __CUDACC__
    typedef float2 fComplex;
#else
    typedef struct{
        float x;
        float y;
    } fComplex;
#endif

typedef struct {
	float x;
	float y;
	float width;
	float height;
} box;

class Auvsi_Matrix;

namespace Auvsi_FFT
{

	////////////////////////////////////////////////////////////////////////////////
	// Helper functions
	////////////////////////////////////////////////////////////////////////////////
	//Round a / b to nearest higher integer value
	inline int iDivUp(int a, int b){
		return (a % b != 0) ? (a / b + 1) : (a / b);
	}

	//Align a to nearest higher multiple of b
	inline int iAlignUp(int a, int b){
		return (a % b != 0) ?  (a - a % b + b) : a;
	}

	void padMatrixFloat(
		Auvsi_Matrix * d_PaddedMatrix,
		Auvsi_Matrix * d_Source,
		int srcWidth,
		int srcHeight,
		int fftW,
		int fftH
	);

	extern "C" void addMatrix(
		Auvsi_Matrix * dst,
		Auvsi_Matrix * src,
		float normalizationFactor
	);

	extern "C" void subtractMatrix(
		Auvsi_Matrix * dst,
		Auvsi_Matrix * src,
		float normalizationFactor
	);

	extern "C" void padKernel(
		float *d_PaddedKernel,
		float *d_Kernel,
		int fftH,
		int fftW,
		int kernelH,
		int kernelW,
		int kernelY,
		int kernelX
	);

	extern "C" void padDataClampToBorder(
		float *d_PaddedData,
		float *d_Data,
		int fftH,
		int fftW,
		int dataH,
		int dataW,
		int kernelH,
		int kernelW,
		int kernelY,
		int kernelX
	);

	/*extern "C" void modulateAndNormalize(
		fComplex * d_Dst,
		fComplex * d_Data,
		fComplex * d_Kernel,
		int fftH,
		int fftW
	);*/
	extern "C" void modulateAndNormalize(
		Auvsi_Matrix * d_Dst,
		Auvsi_Matrix * d_Data,
		Auvsi_Matrix * d_Kernel
	);

	extern "C" void cutPadding(
		float *d_Dst,
		Auvsi_Matrix *d_Padded,
		int imageW,
		int imageH,
		int kernelW,
		int kernelH,
		int fftW,
		int fftH,
		int pitch
	);

	int calculateFFTsize( int dataSize );

	extern "C" void gpu_label_regions(int *pMarkers, int w, int h, int *pBuffer);
	extern "C" void findBoundingBoxes(int *input, int imageW, int imageH, box *& outputBoxes, int &numBoxes);
}
