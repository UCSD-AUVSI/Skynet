#include <cufft.h>

#pragma once

struct cudaArray;
struct textureReference;
struct float2;

// BMP information for loading and saving images
#pragma pack(1)
typedef struct{
    short type;
    int size;
    short reserved1;
    short reserved2;
    int offset;
} BMPHeader;

#pragma pack()
typedef struct{
    int size;
    int width;
    int height;
    short planes;
    short bitsPerPixel;
    unsigned compression;
    unsigned imageSize;
    int xPelsPerMeter;
    int yPelsPerMeter;
    int clrUsed;
    int clrImportant;
} BMPInfoHeader;

// The different types of data we can represent
enum AuvsiMatrixDataType
{
	auvsiMatrixFloat = 1,
	auvsiMatrixInt = 2
};

class Auvsi_Matrix
{
public:
	Auvsi_Matrix( void );
	Auvsi_Matrix( int width, int height, int dimension, int dataType );
	Auvsi_Matrix( int width, int height, int fftWidth, int fftHeight, int dimension, int dataType );
	Auvsi_Matrix( int width, int height, int numChannels, int dataType, void * data );

	virtual ~Auvsi_Matrix(void);

	void bindToTexture2D( textureReference & texRef ); // defined in .cu file
	void bindPaddedToTexture2D( textureReference & texRef ); // defined in .cu file

	void saveImage( const char * path );

	// Various get methods
	inline void * getData(void) { return _data; }
	inline void * getPadded(void) { return _fftPaddedData; }
	inline int getWidth(void){ return _width; }
	inline int getHeight(void){ return _height; }
	inline int getDimension(void){ return _dimension; }
	inline int getNumBytes(void){ return _numBytes; }
	inline int getDataType(void){ return _dataType; }
	inline int getFFTWidth(void){ return _fftWidth; }
	inline int getFFTHeight(void){ return _fftHeight; }

	//void padTheData(int fftWidth, int fftHeight);
	void prepareFFT( void );

	void * dataOnHost();
	void allocComplexData();
	void filterFFT( Auvsi_Matrix * kernel );
	void inverseFFT();
	void runForwardFFT();
	void unpadData(int kernelW, int kernelH);


	friend class Auvsi_Saliency;
private:
protected:


	size_t _pitch;
	size_t _pitchPadded;

	int _dimension;
	int _numBytes;
	int _widthBytes;
	int _dataType;

	cudaChannelFormatDesc _chanDesc;
	cudaChannelFormatDesc _chanDescPadded;

	// FFT variables
	int _fftWidth;				
	int _fftHeight;
public:
	// Device side variables
	void * _data;
	void * _fftPaddedData;		
	void * _fftPaddedDataComplex;
	int _width;		// matrix contents width
	int _height;	
protected:
	cufftHandle _fftPlanForward;	// fft plan
	cufftHandle _fftPlanReverse;


};
