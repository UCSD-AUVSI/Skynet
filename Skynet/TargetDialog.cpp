#include "StdAfx.h"
#include "TargetDialog.h"

#include "SkynetController.h"
#include "DatabaseStructures.h"
#include "MasterHeader.h"

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace Database;
using namespace Skynet;


// TODO: Remove this kind of thing
#define SHAPE_COLOR		shapeColorTextBox
#define SHAPE			shapeTextBox
#define LETTER_COLOR	letterColorTextBox
#define LETTER			letterTextBox


TargetDialog::TargetDialog(SkynetController ^ appController, Database::TargetRowData^ row): 
	appController(appController),
	currentMode(TargetEditingMode::NotMarking),
	dialogData(row->toDialogData()),
	rowData(row)

{
	InitializeComponent();
	loadUIWithData(dialogData);
}


void TargetDialog::loadUIWithData(Database::DialogEditingData^ data)
{

	// reload text fields
	if (String::IsNullOrEmpty(data->shape) || data->shape->Equals("Unknown"))
		SHAPE->Text = "";
	else 
		SHAPE->Text = data->shape;

	if (String::IsNullOrEmpty(data->shapeColor) || data->shapeColor->Equals("Unknown"))
		SHAPE_COLOR->Text = "";
	else 
		SHAPE_COLOR->Text = data->shapeColor;

	if (String::IsNullOrEmpty(data->letter) || data->letter->Equals("Unknown"))
		LETTER->Text = "";
	else 
		LETTER->Text = data->letter;

	if (String::IsNullOrEmpty(data->letterColor) || data->letterColor->Equals("Unknown"))
		LETTER_COLOR->Text = "";
	else 
		LETTER_COLOR->Text = data->letterColor;

	setImage(data->imageName);

}

void TargetDialog::setImage(String^ imageFilename)
{
	_targetImage = Image::FromFile(imageFilename);
	imageBox->Image = _targetImage;
}


DialogEditingData^ TargetDialog::getDataFromUI()
{
	DialogEditingData^ data = gcnew DialogEditingData();
	// reload text fields
	if (!String::IsNullOrWhiteSpace(shapeTextBox->Text)){
		data->shape = shapeTextBox->Text;
	}

	if (!String::IsNullOrWhiteSpace(shapeColorTextBox->Text)){
		data->shapeColor = shapeColorTextBox->Text;
	}

	if (!String::IsNullOrWhiteSpace(letterTextBox->Text)){
		data->letter = letterTextBox->Text;
	}

	if (!String::IsNullOrWhiteSpace(letterColorTextBox->Text)){
		data->letterColor = letterColorTextBox->Text;
	}

	return data;
}


Tuple<int, int>^
TargetDialog::convertPixelsFromDisplayToOriginal(Tuple<int, int>^ picturePixels) {
	double xRatio = (double)_targetImage->Width / (double)imageBox->Width;
	double yRatio = (double)_targetImage->Height / (double)imageBox->Height;
	return gcnew Tuple<int, int>(picturePixels->Item1 * xRatio, picturePixels->Item2 * yRatio);
}

System::Void 
TargetDialog::okButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	VerifiedRowData ^ verified = rowData->asVerified(getDataFromUI());
	appController->upsertVerified(verified);
	this->Close();
}

System::Void
TargetDialog::createVerifiedTargetFromMarkedTarget()
{
	auto topLeftOriginal = convertPixelsFromDisplayToOriginal(topLeft);
	auto bottomRightOriginal = convertPixelsFromDisplayToOriginal(bottomRight);
	VerifiedRowData ^ verified = rowData->asVerified(getDataFromUI(), topLeftOriginal, bottomRightOriginal);	
	appController->upsertVerified(verified);
	this->Close();
}

System::Void
TargetDialog::deleteButton_Click(System::Object^ sender, System::EventArgs^ e)
{
	appController->removeTarget(rowData);
	this->Close();
}
System::Void 
TargetDialog::markTargetButton_Click(System::Object^  sender, System::EventArgs^  e) {
	switchToMode(TargetEditingMode::MarkingBottomRight);
 }
System::Void
TargetDialog::imageBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {			 			 
	Point location;
	 switch (this->currentMode){
	 case TargetEditingMode::MarkingBottomRight:
		 location = e->Location;
		 bottomRight = gcnew System::Tuple<int, int>(location.X, location.Y);
		 switchToMode(TargetEditingMode::MarkingTopLeft);
		 break;
	 case TargetEditingMode::MarkingTopLeft:
		 location = e->Location;
		 topLeft = gcnew System::Tuple<int, int>(location.X, location.Y);
		 createVerifiedTargetFromMarkedTarget();
		 switchToMode(TargetEditingMode::TargetMarked);
	 default:
		 break;
	 }
}
System::Void
TargetDialog::switchToMode(TargetEditingMode mode) {
	currentMode = mode;
	switch (mode){
	case TargetEditingMode::MarkingBottomRight:
		instructionLabel->Text = "Click on center of target";
		break;
	case TargetEditingMode::MarkingTopLeft:
		instructionLabel->Text = "Click on top of target";
		break;
	case TargetEditingMode::TargetMarked:
		instructionLabel->Text = "Target Identified";
		break;
	case TargetEditingMode::NotMarking:
		instructionLabel->Text = "";
		break;
	}
}