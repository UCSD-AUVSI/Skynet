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

#define SHAPE_COLOR		textBox1
#define SHAPE			textBox2
#define LETTER_COLOR	textBox4
#define LETTER			textBox3


TargetDialog::TargetDialog(SkynetController ^ appController, Database::TargetRowData^ row): 
	appController(appController),
	_markLat(false),
	_markHeading(false),
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

void TargetDialog::buildVotingText() 
{
	// TODO: implement
}

void TargetDialog::clearVotingText() 
{
	// TODO: implement
}

void TargetDialog::setImage(String^ imageFilename)
{
	imageBox->Image = Image::FromFile(imageFilename);
}


DialogEditingData^ TargetDialog::getDataFromUI()
{
	DialogEditingData^ data = gcnew DialogEditingData();
	// reload text fields
	if (!SHAPE->Text->Equals(""))
		data->shape = SHAPE->Text;

	if (!SHAPE_COLOR->Text->Equals(""))
		data->shapeColor = SHAPE_COLOR->Text;

	if (!LETTER->Text->Equals(""))
		data->letter = LETTER->Text;
	
	if (!LETTER_COLOR->Text->Equals(""))
		data->letterColor = LETTER_COLOR->Text;
	


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
	open = false;
	_markLat = false;
	_markHeading = false;
	this->Close();
}

System::Void 
TargetDialog::button1_Click(System::Object^  sender, System::EventArgs^  e) 
{
	// TODO

	open = false;
	_markLat = false;
	_markHeading = false;
	this->Close();

}