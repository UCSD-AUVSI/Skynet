#include "stdafx.h"
#include "GroundLevelDialog.h"
#include "SkynetController.h"

using namespace Skynet;

System::Void GroundLevelDialog::okButton_Click(System::Object^  sender, System::EventArgs^  e) {
	skynetController->setGroundLevel(Double::Parse(altitudeTextBox->Text));
	this->Close();
}