#pragma once

namespace Auvsi_Util
{
	int iDivUp(int a, int b);
	int iAlignUp(int a, int b);
	void transpose(float *d_src, float *d_dest, int width, int height);
	int calculateFFTsize( int dataSize );

	void printCudaArray( char * name, void * data, int size, int upTo, bool isFloat );

	void checkForCudaError(char * message);
};
