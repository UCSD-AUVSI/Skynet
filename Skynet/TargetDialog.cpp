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


	centerX = (float)data->targetX;
	centerY = (float)data->targetY;

	topOfTargetX = (float)( data->topOfTargetX );
	topOfTargetY = (float)( data->topOfTargetY );

	setImage(data->imageName);

}

void TargetDialog::setImage(String^ imageFilename)
{
	imageBox->Image = Image::FromFile(imageFilename);
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


	data->targetX = (int)centerX;
	data->targetY = (int)centerY;

	data->topOfTargetX = (int)topOfTargetX;
	data->topOfTargetY = (int)topOfTargetY; 

	return data;
}


System::Void 
TargetDialog::okButton_Click(System::Object^  sender, System::EventArgs^  e) 
{
	VerifiedRowData ^ verified = rowData->asVerified(getDataFromUI());
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
	switchToMode(TargetEditingMode::MarkingCenter);
 }
System::Void
TargetDialog::imageBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {			 			 
	Point location;
	 switch (this->currentMode){
	 case TargetEditingMode::MarkingCenter:
		 location = e->Location;
		 centerX = location.X;
		 centerY = location.Y;
		 switchToMode(TargetEditingMode::MarkingTop);
		 break;
	 case TargetEditingMode::MarkingTop:
		 location = e->Location;
		 topOfTargetX = location.X;
		 topOfTargetY = location.Y;
		 switchToMode(TargetEditingMode::TargetMarked);
	 default:
		 break;
	 }
}
System::Void
TargetDialog::switchToMode(TargetEditingMode mode) {
	currentMode = mode;
	switch (mode){
	case TargetEditingMode::MarkingCenter:
		instructionLabel->Text = "Click on center of target";
		break;
	case TargetEditingMode::MarkingTop:
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