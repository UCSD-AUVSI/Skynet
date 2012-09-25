#pragma once

struct cudaArray;
struct textureReference;
struct float4;
struct float2;

class Auvsi_Matrix;

enum AuvsiMatrixDataType;

class Auvsi_Image : public Auvsi_Matrix
{
public:
	Auvsi_Image( int width, int height, int numChannels, int dataType );
	Auvsi_Image( int width, int height, int numChannels, int dataType, void * data );
	Auvsi_Image( int width, int height, int fftWidth, int fftHeight, int numChannels, int dataType);

	Auvsi_Image( const char * path );

	Auvsi_Image( const char * path, bool bla );

	void assignImage( const void * buf, int width, int height, int numChannels );

	friend class Auvsi_Saliency;
private:
protected:
};
