
#include <SalientGreenGPU/Cuda/dcAdjust.H>

static inline int divUp( int total, int grain )
{
	return ( total + grain - 1 ) / grain;
}

void addFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> const a, cv::gpu::DevMem2D_<myfloat2> const b,
		cv::gpu::DevMem2D_<myfloat2> c )
{

}

void addAndZeroFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> mat, float value )
{

}
void subFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> const a, cv::gpu::DevMem2D_<myfloat2> const b,
		cv::gpu::DevMem2D_<myfloat2> c )
{

}

void mulFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> const a, cv::gpu::DevMem2D_<myfloat2> const b,
		cv::gpu::DevMem2D_<myfloat2> c )
{

}

void mulValueFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> const a, float const b,
		cv::gpu::DevMem2D_<myfloat2> c )
{

}

void dcAdjustWrapper( cv::gpu::DevMem2D_<myfloat2> mat, float value )
{

}


void absFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> mat )
{

}

void maxFC2Wrapper( cv::gpu::DevMem2D_<myfloat2> mat, float value )
{

}

void minMaxHelperWrapper( cv::gpu::DevMem2D_<float> const mat, float thresh, float & sum, int & count )
{
	
}
