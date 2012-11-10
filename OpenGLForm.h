#pragma once

#include <string>
#include "SaveImage.h"
#include "Saliency.h"
#include <msclr/lock.h>
#include "MasterHeader.h"
#include "SkynetController.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace msclr;

static GLuint texture[1];

namespace OpenGLForm 
{
	public ref class COpenGL: public System::Windows::Forms::NativeWindow
	{
	public:
		int benchmark;
		COpenGL(System::Windows::Forms::Panel ^ parentForm, GLsizei iWidth, GLsizei iHeight, Object ^ parent, Object ^ watcher)
		{
			CreateParams^ cp = gcnew CreateParams;
			
			// Set the position on the form
			cp->X = 0;
			cp->Y = 0;
			cp->Height = iHeight;
			cp->Width = iWidth;

			// Specify the form as the parent.
			cp->Parent = parentForm->Handle;

			// Create as a child of the specified parent and make OpenGL compliant (no clipping)
			cp->Style = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

			// Create the actual window
			this->CreateHandle(cp);

			m_hDC = GetDC((HWND)this->Handle.ToPointer());

			if(m_hDC)
			{
				MySetPixelFormat(m_hDC);
				ReSizeGLScene(iWidth, iHeight);
				InitGL();
			}

			//rtri = 0.0f;
			//rquad = 0.0f;

			callbackSetupDone = false;
			textureSetupDone = false;
			buffer = nullptr;
			frameW = frameH = -1;
			cubeZoom = 1.63f;
			xLocation = 1.0f;

			saveVideo = false;


			_parent = parent;
			newFrame = false;
		}
		
		virtual ~COpenGL(System::Void)
		{
			System::Diagnostics::Trace::WriteLine( "~COpenGL(System::Void())");
			
			this->DestroyHandle();
		}

		!COpenGL()
		{
			System::Diagnostics::Trace::WriteLine( "!COpenGL()");
			if( buffer != nullptr )
				delete buffer;

			if( imSaver != nullptr )
				delete imSaver;
		}

		System::Void UpdateBuffer( float * input )
		{
			theController->gotVideo();
			lock l(this);
			memcpy(buffer, input, frameW * frameH * sizeof(float) * 3);	
			
			newFrame = true;
		}

		System::Void CallbackSetup( GLsizei fWidth, GLsizei fHeight )
		{
			frameW = fWidth;
			frameH = fHeight;

			buffer = new float[fWidth * fHeight * 4];
			callbackSetupDone = true;		

			imSaver = new ImageUtil::SaveImage( frameH, frameW, 4 );	
//			theSaliency->setValues(frameW, frameH, _parent);
			PRINT("CallbackSetup ... shit will be borked if video was running during this");
		}

		System::Void saveVideoFrame()
		{
			benchmark++;
			imSaver->writeFrame( buffer );

			//System::Diagnostics::Trace::WriteLine("writeFrame in Render in OpenGLForm.h: " + benchmark);
		}

		System::Void Render(System::Void)
		{
			if( callbackSetupDone && !textureSetupDone )
			{
				SetupTexture();
				textureH = ((float)frameH)/((float)bufferH); 
				textureH = textureH / 2; // deinterlaced means that valid data takes up first half of buffer only
				textureW = ((float)frameW)/((float)bufferW);
				cubeW = 1.0f;
				cubeH = ((float)frameH)/((float)frameW); // aspect ratio
				cubeZoom = 1.63f; // this assumes the view is wider (in terms of aspect ratio) than the video. if the view is taller, zoom must change
				//cubeZoom = 1.00f;
			}
			if( textureSetupDone )
			{
				glBindTexture(GL_TEXTURE_2D, texture[0]);

				// only hose it if there is a new frame
				//if (newFrame)
					/*if (showSaliency)
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLint)frameW, (GLint)frameH, GL_LUMINANCE, GL_FLOAT, saliencyBuffer);
					else*/
						glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, (GLint)frameW, (GLint)frameH, GL_RGBA, GL_FLOAT, buffer);	
			}

			// never do this. instead, save video in SimHandler
			if( false && saveVideo )
			{
				benchmark++;
				imSaver->writeFrame( buffer );

				System::Diagnostics::Trace::WriteLine("writeFrame in Render in OpenGLForm.h: " + benchmark);
			}

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			
			glLoadIdentity(); 

			// display raw image in big rectangle
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, texture[0]);

			glTranslatef(xLocation, 0.0f, -cubeZoom); // UNCOMMENT

			glBegin(GL_QUADS);
				// Front Face
				//glColor3f(1.0f, 0.0f, 0.0f);
				glTexCoord2f(0.0f, textureH); glVertex3f(-cubeW - 1.0f, -cubeH,  0.0f);	// Bottom Left Of The Texture and Quad
				glTexCoord2f(textureW, textureH); glVertex3f( cubeW - 1.0f, -cubeH,  0.0f);	// Bottom Right Of The Texture and Quad
				glTexCoord2f(textureW, 0.0f); glVertex3f( cubeW - 1.0f,  cubeH,  0.0f);	// Top Right Of The Texture and Quad 
				glTexCoord2f(0.0f, 0.0f); glVertex3f(-cubeW - 1.0f,  cubeH,  0.0f);	// Top Left Of The Texture and Quad
			glEnd();
		}

		System::Void SwapOpenGLBuffers(System::Void)
		{
			SwapBuffers(m_hDC) ;
		}

		// save current frame to path
		System::Void saveImage( String ^ path )
		{
			if (imSaver != nullptr)
				imSaver->saveFrame( buffer, ManagedToSTL( path ) );
		}

		System::Void saveImage( String ^ path, String ^ pathBase, array<float> ^ homo, double heading )
		{
			float * homography = new float[9];
			for( int i = 0; i < 9; ++i )
				homography[i] = homo[i];

			//homography[0] = homography[4] = homography[8] = 1.0;
			//heading = 45;

			if (imSaver != nullptr)
				imSaver->saveFrame( buffer, ManagedToSTL( path ), ManagedToSTL( pathBase ), homography, heading );
		}

		bool enableVideoRecording( String ^ path )
		{
			System::Diagnostics::Trace::WriteLine("enableVideoRecording in OpenGLForm.h");
			if (imSaver != nullptr)
				imSaver->setupVideo( ManagedToSTL( path ) );
			else 
				return false;
			saveVideo = true;
			benchmark = 0;
			return true;
		}

		System::Void disableVideoRecording()
		{
			System::Diagnostics::Trace::WriteLine("disableVideoRecording in OpenGLForm.h");
			imSaver->stopVideo();
			saveVideo = false;
		}


		System::Void changeEncoding( String ^ encoding )
		{
			imSaver->changeEncoding( ManagedToSTL( encoding ) );
		}


	public:		
		float * buffer;
		GLsizei frameW, frameH;

		Skynet::SkynetController ^theController;
	private:
		HDC m_hDC;
		HGLRC m_hglrc;
		GLfloat	rtri;				// Angle for the triangle
		GLfloat	rquad;				// Angle for the quad

		int bufferW, bufferH;
		GLfloat textureW, textureH;
		GLfloat cubeW, cubeH;
		GLfloat cubeZoom;
		GLfloat xLocation;
		bool callbackSetupDone;
		bool textureSetupDone;	
		bool saveVideo;
//		bool showSaliency;
		bool newFrame;

		ImageUtil::SaveImage * imSaver;
//		Vision::Saliency ^ theSaliency;

		Object ^ _parent;

	protected:

		std::string ManagedToSTL(String ^ s) 
		{
		   using namespace Runtime::InteropServices;
		   const char* chars = 
		      (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		   std::string retVal = chars;
		   Marshal::FreeHGlobal(IntPtr((void*)chars));

		   return retVal;
		}

		GLint MySetPixelFormat(HDC hdc)
		{
			static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
				{
					sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
					1,											// Version Number
					PFD_DRAW_TO_WINDOW |						// Format Must Support Window
					PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
					PFD_DOUBLEBUFFER,							// Must Support Double Buffering
					PFD_TYPE_RGBA,								// Request An RGBA Format
					32,										// Select Our Color Depth
					0, 0, 0, 0, 0, 0,							// Color Bits Ignored
					0,											// No Alpha Buffer
					0,											// Shift Bit Ignored
					0,											// No Accumulation Buffer
					0, 0, 0, 0,									// Accumulation Bits Ignored
					16,											// 16Bit Z-Buffer (Depth Buffer)  
					0,											// No Stencil Buffer
					0,											// No Auxiliary Buffer
					PFD_MAIN_PLANE,								// Main Drawing Layer
					0,											// Reserved
					0, 0, 0										// Layer Masks Ignored
				};
			
			GLint  iPixelFormat; 
		 
			// get the device context's best, available pixel format match 
			if((iPixelFormat = ChoosePixelFormat(hdc, &pfd)) == 0)
			{
				MessageBox::Show("ChoosePixelFormat Failed");
				return 0;
			}
			 
			// make that match the device context's current pixel format 
			if(SetPixelFormat(hdc, iPixelFormat, &pfd) == FALSE)
			{
				MessageBox::Show("SetPixelFormat Failed");
				return 0;
			}

			if((m_hglrc = wglCreateContext(m_hDC)) == NULL)
			{
				MessageBox::Show("wglCreateContext Failed");
				return 0;
			}

			if((wglMakeCurrent(m_hDC, m_hglrc)) == NULL)
			{
				MessageBox::Show("wglMakeCurrent Failed");
				return 0;
			}


			return 1;
		}

		bool InitGL(GLvoid)										// All setup for opengl goes here
		{
			glShadeModel(GL_SMOOTH);							// Enable smooth shading
			glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black background
			glClearDepth(1.0f);									// Depth buffer setup
			glEnable(GL_DEPTH_TEST);							// Enables depth testing
			glDepthFunc(GL_LEQUAL);								// The type of depth testing to do
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really nice perspective calculations
			return TRUE;										// Initialisation went ok
		}

		GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize and initialise the gl window
		{
			if (height==0)										// Prevent A Divide By Zero By
			{
				height=1;										// Making Height Equal One
			}

			glViewport(0,0,width,height);						// Reset The Current Viewport

			glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
			glLoadIdentity();									// Reset The Projection Matrix

			// Calculate The Aspect Ratio Of The Window
			gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.1f,100.0f);

			glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
			glLoadIdentity();									// Reset The Modelview Matrix
		}

		int roundUpToPowerOfTwo(int dataSize) 
		{
		    int hiBit;
		    unsigned int lowPOT, hiPOT;

			dataSize = (dataSize % 16 != 0) ?  (dataSize - dataSize % 16 + 16) : dataSize;

		    //Find highest non-zero bit
		    for(hiBit = 31; hiBit >= 0; hiBit--)
		        if(dataSize & (1U << hiBit)) break;

		    //No need to align, if already power of two
		    lowPOT = 1U << hiBit;
		    if(lowPOT == dataSize) return dataSize;

		    //Align to a nearest higher power of two, if the size is small enough,
		    //else align only to a nearest higher multiple of 512,
		    //in order to save computation and memory bandwidth
		    hiPOT = 1U << (hiBit + 1);
		    if(hiPOT <= 1024)
		        return hiPOT;
		    else 
				return (dataSize % 512 != 0) ?  (dataSize - dataSize % 512 + 512) : dataSize;
		}

		GLint SetupTexture(System::Void)
		{					
			glGenTextures( 1, &texture[0] );
			
			glBindTexture( GL_TEXTURE_2D, texture[0] );

			bufferH = roundUpToPowerOfTwo( frameH );
			bufferW = roundUpToPowerOfTwo( frameW ); 
			
			float * imageData = new float[bufferH * bufferW * 4];

			for(int i = 0; i < (bufferH * bufferW ); i++) {
				int j = i * 3;
				imageData[j] = 1.0f;
				imageData[j+1] = 0;
				imageData[j+2] = 1.0f;
				imageData[j+3] = 1;
			}

			glTexImage2D(GL_TEXTURE_2D, 0, 3, (GLint)bufferW, (GLint)bufferH, 0, GL_RGBA, GL_FLOAT, imageData);

			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER | GL_TEXTURE_MAG_FILTER, GL_LINEAR );	// Linear Filtering
		
			System::Diagnostics::Trace::WriteLine( "SetupTexture for raw image buffer. texRef: " + texture[0] + " bufferW: " + bufferW + " bufferH: " + bufferH + " frameW: " + frameW + " frameH: " + frameH);
			
			delete[] imageData;
			textureSetupDone = true;

			return 1;
		}

	};
}
