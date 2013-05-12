#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#define SIDE 134.0f
#define SCALE (SIDE/180.0f)
#define DEFAULT_IMAGE_WIDTH 34.0f
#define DEFAULT_IMAGE_HEIGHT 25.0f

#include "Delegates.h"
#include "MasterHeader.h"
#include "SystemSettings.h"

namespace Skynet {

	/// <summary>
	/// Summary for HUDControl
	/// </summary>
	public ref class HUDControl : public System::Windows::Forms::UserControl
	{
	public:
		HUDControl(void)
		{
			InitializeComponent();
			gimbalX = SIDE/2.0f;
			gimbalY = SIDE/2;
			intendedGimbalX = SIDE/2.0f;
			intendedGimbalY = SIDE/2.0f;
			setCameraZoom(1.0f, false);
			setIntendedCameraZoom(1.0f, false);
			
			_redrawHUDdelegate = gcnew Delegates::voidToVoid( this, &HUDControl::redraw );
			bkgd = Image::FromFile( DROPBOX_DIR + "Skynet\\Skynet\\HUDbg.png" );
			dot = Image::FromFile( DROPBOX_DIR + "Skynet\\Skynet\\HUDdot.png" );
			outline = Image::FromFile( DROPBOX_DIR + "Skynet\\Skynet\\HUDoutline.png" );

			this->BackgroundImage = bkgd;
			this->Paint += gcnew System::Windows::Forms::PaintEventHandler( this, &HUDControl::paintHUD );
		}

		
		System::Void paintHUD(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
			 this->DrawHUD( e->Graphics );
		}

		void redraw()
		{
			this->Refresh();
		}

		//newX and newY are gotten by gimbal's center, no need to center it.
		bool setGimbalPosition( float newX, float newY, bool redraw)
		{
			//PRINT("raw newX: " + newX);
			newX = -newX;
			gimbalX = (newX + 90.0f)*SCALE;
			gimbalY = (newY + 90.0f)*SCALE;

			if (redraw)
				this->Invoke( this->_redrawHUDdelegate );

			return true;
		}

		bool setIntendedGimbalPosition( float newX, float newY, bool redraw )
		{
			PRINT("raw intendedGimbalX: " + newX);
			//intendedGimbalX = -intendedGimbalX;
			newX = -newX;
			intendedGimbalX = (newX + 90.0f)*SCALE;
			intendedGimbalY = (newY + 90.0f)*SCALE;
			
			if (redraw)
				this->Invoke( this->_redrawHUDdelegate );

			return true;

		}

		bool setCameraZoom( float zoom, bool redraw  )
		{
			if (zoom <= 1.0f || zoom > 10.1f) {
				cameraSizeX = DEFAULT_IMAGE_WIDTH;
				cameraSizeY = DEFAULT_IMAGE_HEIGHT;
			} else {
				cameraSizeX = DEFAULT_IMAGE_WIDTH/zoom;
				cameraSizeY = DEFAULT_IMAGE_HEIGHT/zoom;
			}
			
			if (redraw)
				this->Invoke( this->_redrawHUDdelegate );

			return true;
		}

		bool setIntendedCameraZoom( float zoom, bool redraw  )
		{
			if (zoom <= 1.0f || zoom > 10.1f) {
				intendedCameraSizeX = DEFAULT_IMAGE_WIDTH;
				intendedCameraSizeY = DEFAULT_IMAGE_HEIGHT;
			} else {
				intendedCameraSizeX = DEFAULT_IMAGE_WIDTH/zoom;
				intendedCameraSizeY = DEFAULT_IMAGE_HEIGHT/zoom;
			}
			
			if (redraw)
				this->Invoke( this->_redrawHUDdelegate );

			return true;
		}

	protected:
		float gimbalX;
		float gimbalY;
		float intendedGimbalX;
		float intendedGimbalY;
		float cameraSizeX;
		float cameraSizeY;
		float intendedCameraSizeX;
		float intendedCameraSizeY;

		Image ^ bkgd;
		Image ^ dot;
		Image ^ outline;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~HUDControl()
		{
			if (components)
			{
				delete components;
			}
		}

		bool DrawHUD( Graphics ^ g )
		{
			bool retVal = false;
			int height = (int)SIDE;
			int width = (int)SIDE;

			//Next lets scale down the measure tile width to a most significant digit unit
			try
			{
				g->DrawImage(dot, gimbalX-cameraSizeX/2.0f, gimbalY-cameraSizeY/2.0f, cameraSizeX, cameraSizeY );
				g->DrawImage(outline, intendedGimbalX-intendedCameraSizeX/2.0f, intendedGimbalY-intendedCameraSizeY/2.0f, intendedCameraSizeX, intendedCameraSizeY );
			}
			catch (Exception ^ e)
			{
				System::Diagnostics::Trace::WriteLine("Could not draw your background :(");
				e = nullptr;
				return retVal;
			}
			return true;
		}



	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;
		Delegates::voidToVoid ^ _redrawHUDdelegate; // WAS PUBLIC, MADE WARNINGS

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};
}
