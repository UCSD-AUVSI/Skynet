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


TargetDialog::TargetDialog(SkynetController ^ newAppController)
{
	appController = newAppController;
	_markLat = false;
	_markHeading = false;
	

	centerX = -1.0f;
	centerY = -1.0f;
	
	topOfTargetX = -1.0f;
	topOfTargetY = -1.0f;
	
	open = false;
	imageOpen = false;
	_targetImage = nullptr;

	InitializeComponent();

}

/*
void CandidateTargetDialog::showDialog(){}
void VerifiedTargetDialog::showDialog(){}
void UnverifiedTargetDialog::showDialog(){}
*/

/*
void TargetDialog::showDialogForData(Database::CandidateRowData ^ theData)
{
	if (theData == nullptr) {
		PRINT("TargetDialog::showDialogForData() ERROR theData == nullptr");
		return;
	}

	candidate = theData;
	target = nullptr;
	open = true;
	_markLat = false;
	_markHeading = false;
	//reloadData();
}

void TargetDialog::showDialogForData(Database::UnverifiedRowData ^ theData)
{
	if (theData == nullptr) {
		PRINT("TargetDialog::showDialogForData() ERROR theData == nullptr");
		return;
	}

	mode = DialogEditingCandidate;
	data = gcnew DialogEditingData(theData);
	candidate = theData->candidate;
	data->id = theData->targetid;
	target = theData;
	open = true;
	_markLat = false;
	_markHeading = false;
	this->setImage();
	this->Show();
}


void TargetDialog::showDialogForData(Database::VerifiedRowData ^ theData)
{
	if (theData == nullptr) {
		PRINT("TargetDialog::showDialogForData() ERROR theData == nullptr");
		return;
	}

	mode = DialogEditingVerified;

	data = gcnew DialogEditingData(theData);
	open = true;
	_markLat = false;
	_markHeading = false;

	reloadData();
	this->Show();
	
}
*/

void TargetDialog::reloadData()
{

	//data->imageName = appController->imageNameForID("" + data->id);


	// reload text fields
	/*
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

	/*
	// change UI elements
	switch (mode) {
		case DialogEditingCandidate:
			okButton->Text = "Vote";
			clearVotingText();
			break;
		case DialogEditingUnverified:
			okButton->Text = "Approve for Export";
			buildVotingText();
			break;
		case DialogEditingVerified:
			okButton->Text = "STOP: DO NOT MODIFY";
			clearVotingText();
			break;
		default:
			okButton->Text = "Shit, bro";
			break;
	}
	*/
	/*
	centerX = (float)data->targetX;
	centerY = (float)data->targetY;

	topOfTargetX = (float)( data->topOfTargetX );
	topOfTargetY = (float)( data->topOfTargetY );

	setImage();
	*/

}

CandidateTargetDialog::CandidateTargetDialog(Database::CandidateRowData^ candidate, Skynet::SkynetController^ skynetController):
	TargetDialog(skynetController){}

UnverifiedTargetDialog::UnverifiedTargetDialog(Database::UnverifiedRowData^ unverified, Skynet::SkynetController^ skynetController):
	TargetDialog(skynetController){}

VerifiedTargetDialog::VerifiedTargetDialog(Database::VerifiedRowData^ verified, Skynet::SkynetController^ skynetController):
	TargetDialog(skynetController){}

void TargetDialog::buildVotingText() 
{
	// TODO: implement
}

void TargetDialog::clearVotingText() 
{
	// TODO: implement
}

void TargetDialog::setImage()
{
	/*
	//System::Diagnostics::Trace::WriteLine("TargetDialog::setImage(): setting image: " + HTTP_SERVER_TARGET_PATH + data->imageName->Remove(0, 8));
	try
	{
		String ^ path;
		switch (mode) {
			case DialogEditingCandidate:
				path = HTTP_SERVER_TARGET_PATH + data->imageName;
				break;
			case DialogEditingUnverified:
				path = HTTP_SERVER_TARGET_PATH + data->imageName;
				buildVotingText();
				break;
			case DialogEditingVerified:
				path = HTTP_SERVER_TARGET_PATH + data->imageName;
				clearVotingText();
				break;
			default:
				okButton->Text = "Shit, bro";
				break;
		}
		_targetImage = Image::FromFile( path );
		imageBox->Image = _targetImage;
	}
	catch( Exception ^ e)
	{
		System::Diagnostics::Trace::WriteLine("ERROR: TargetDialog::setImage(): failed to set image: " + e);
	}
	*/

}


DialogEditingData^ TargetDialog::getDataFromUI()
{
	DialogEditingData^ data;
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
	saveAsVerified();

	open = false;
	_markLat = false;
	_markHeading = false;
	this->Close();
}

System::Void 
TargetDialog::button1_Click(System::Object^  sender, System::EventArgs^  e) 
{
	// TODO: Delete

	open = false;
	_markLat = false;
	_markHeading = false;
	this->Close();

}