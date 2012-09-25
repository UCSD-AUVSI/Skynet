#pragma once

struct cudaArray;
struct textureReference;
class Auvsi_Matrix;
enum AuvsiMatrixDataType;

// The different types of filters we can represent
enum auvsiFilterType
{
	auvsiFilterGaussian,
	auvsiFilterGabor
};

class Auvsi_Filter : public Auvsi_Matrix
{
public:
	Auvsi_Filter(void);
	Auvsi_Filter(int width, int height, int fftWidth, int fftHeight, int dimension, int dataType); 
	virtual ~Auvsi_Filter(void);

	void bindToTexture2DImag( textureReference & texRef ); // defined in .cu file

	void createGaussian( float sigma );
	void createGabor( float sigma, float scale, int orientation );
	void createAverage( int size );

	void prepareFFT( int fftWidth, int fftHeight );
	
	inline int getCenterX(void) { return _centerX; }
	inline int getCenterY(void) { return _centerY; }
	
	friend class Auvsi_Saliency;
protected:
	void allocOnGpu( void );

private:
	// Filter specific variables
	int _filterType;	
	int _centerX;
	int _centerY;
	float _sigma;
	int _orientation;

	// host data
	void * _filterData;
};
