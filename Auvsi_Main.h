/*
 *  Auvsi_Main.h
 *  
 *
 *  Created by Lewis Anderson on 11/26/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

extern "C" void LoadBMPFile(uchar4 **dst, int *width, int *height, const char *name);

extern "C" void CUDA_MallocAndCopyArray(uchar4 **h_Src, int imageW, int imageH) ;

extern "C" void CUDA_Bind2TextureArray();

extern "C" void CUDA_UnbindTexture();

extern "C" float4 *cuda_rgb_2_lab(int imageW, int imageH);

#define BLOCKDIM_X 16
#define BLOCKDIM_Y 8