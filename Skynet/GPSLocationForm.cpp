#include "stdafx.h"
#include "GPSLocationForm.h"
#include "MasterHeader.h"
#include "GPSCoord.h"
#include "SkynetController.h"

using namespace Intelligence;
namespace Skynet{
	//TODO: Handle error conditions better.
	System::Void GPSLocationForm::submitButton_Click(System::Object^  sender, System::EventArgs^  e) {
		if (String::IsNullOrEmpty(latitudeTextBox->Text) || 
		    String::IsNullOrEmpty(longitudeTextBox->Text) ||
			String::IsNullOrEmpty(altitudeTextBox->Text)) {
			PRINT("Invalid Inputs for text boxes");	
			return;
		}
		double lat, lon, altMeters;
		lat = Double::Parse(latitudeTextBox->Text);
		lon = Double::Parse(longitudeTextBox->Text);
		altMeters = Double::Parse(altitudeTextBox->Text);
		GPSCoord^ coordinate = gcnew GPSCoord(lat, lon, altMeters);
		skynetController->lockPosition(coordinate);
		Close();
	}
}