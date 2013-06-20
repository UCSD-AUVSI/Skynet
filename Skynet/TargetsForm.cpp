#include "TargetsForm.h"
#include "SkynetController.h"
#include "DatabaseStructures.h"
#include "TargetDialog.h" 
#include "Delegates.h"
#include "GeoReference.h"

using namespace Skynet;
using namespace Database;
TargetsForm::TargetsForm(SkynetController^ skynetController): skynetController(skynetController) {
	InitializeComponent();
}

System::Void TargetsForm::candidatesDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
	try {
		String ^ id = Convert::ToString(candidatesDataGridView->Rows[e->RowIndex]->Cells[0]->Value);
		CandidateRowData^ candidate = skynetController->candidateWithID(id);
		(gcnew TargetDialog(skynetController, candidate))->Show();
	} catch (System::ArgumentOutOfRangeException^){
		// Silently fail
	}
}

		 
System::Void TargetsForm::unverifiedDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
	String ^ id = Convert::ToString(unverifiedDataGridView->Rows[e->RowIndex]->Cells[0]->Value);
	UnverifiedRowData^ unverified = skynetController->unverifiedWithID(id);
	(gcnew TargetDialog(skynetController, unverified))->Show();
}

System::Void TargetsForm::verifiedDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
	try {
		String ^ id = Convert::ToString(verifiedDataGridView->Rows[e->RowIndex]->Cells[0]->Value);
		VerifiedRowData^ verified = skynetController->verifiedWithID(id);
		(gcnew TargetDialog(skynetController, verified))->Show();
	} catch (Exception^ ){
		// Fail silently;
	}
}

System::Void TargetsForm::setCandidateTableContents( array<Database::CandidateRowData ^> ^ contents ) {
	PRINT("IN setCandidateTableContents");
	if ( !candidatesDataGridView->InvokeRequired ) {
		candidatesDataGridView->Rows->Clear();
		for each(Database::CandidateRowData ^ data in contents) {
			insertCandidate(data);
		}
	}
	else {
		auto candidateTableContentsDelegate = gcnew Delegates::candidateRowDataArrayToVoid(this,&TargetsForm::setCandidateTableContents);
		array<Object ^>^ arr = {contents}; 
		this->Invoke(candidateTableContentsDelegate,arr);
	}
}

System::Void TargetsForm::insertCandidate( Database::CandidateRowData ^ data) {
	int num = data->candidateid;
	int rowNum;
	DataGridView ^ grid = this->candidatesDataGridView;
	if ( !grid->InvokeRequired )
	{
		rowNum = grid->Rows->Add();
		grid->Rows[rowNum]->Cells[0]->Value = "" + data->candidateid;

		Image ^ thumbnail;
		try
		{
			String ^ path = data->imageName;
			thumbnail = Image::FromFile( path ) ; 
			thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
		}
		catch( Exception ^ e)
		{
			System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
		}


		grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
		grid->Rows[rowNum]->Cells[4]->Value = "0*";
		grid->Rows[rowNum]->Cells[5]->Value = data->processed;
		double lat = data->telemetry->planeLocation->lat;
		double lon = data->telemetry->planeLocation->lon;
		grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
		grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
	}
	else {
		auto candidateTableUpdateDelegate = gcnew Delegates::candidateRowDataToVoid(this,&TargetsForm::insertCandidate);
		this->Invoke(candidateTableUpdateDelegate,data);
	}
}

System::Void TargetsForm::insertUnverified( Database::UnverifiedRowData ^ data) 
{

	int rowNum;

	DataGridView ^ grid = this->unverifiedDataGridView;
	
	if ( !grid->InvokeRequired )
	{
		rowNum = grid->Rows->Add();
		grid->Rows[rowNum]->Cells[0]->Value = "" + data->targetid;
		Image ^ thumbnail;
		try
		{
			String ^ path = data->candidate->imageName;
			thumbnail = Image::FromFile( path );
			thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
		}
		catch( Exception ^ e)
		{
			System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
		}


		grid->Rows[rowNum]->Cells[1]->Value = thumbnail;

		// TODO: Code Duplication
		double lat, lon, alt;
		try{
			Vision::GeoReference::getTargetGPS(data, lat, lon, alt);
		} catch (Vision::GeoReferenceException^){
			lat = data->candidate->telemetry->planeLocation->lat;
			lon = data->candidate->telemetry->planeLocation->lon;
		} finally {
			grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
			grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
		}
		grid->Rows[rowNum]->Cells[4]->Value = "0*";
		grid->Rows[rowNum]->Cells[5]->Value = "("+ data->description->targetX + "," + data->description->targetY+")";
		grid->Rows[rowNum]->Cells[6]->Value = data->description->letter;
		grid->Rows[rowNum]->Cells[7]->Value = data->description->letterColor;
		grid->Rows[rowNum]->Cells[8]->Value = data->description->shape;
		grid->Rows[rowNum]->Cells[9]->Value = data->description->shapeColor;
		grid->Rows[rowNum]->Cells[10]->Value = true;
	}
	else
	{
		auto unverifiedTableUpdateDelegate = gcnew Delegates::unverifiedRowDataToVoid(this,&TargetsForm::insertUnverified);
		this->Invoke(unverifiedTableUpdateDelegate,data);
	}

}

System::Void TargetsForm::insertVerified( Database::VerifiedRowData ^ data) 
{
	
	int rowNum;

	DataGridView ^ grid = this->verifiedDataGridView;
	
	if ( !grid->InvokeRequired )
	{
		rowNum = grid->Rows->Add();
		grid->Rows[rowNum]->Cells[0]->Value = "" + data->submitid;

		Image ^ thumbnail;
		try
		{
			if (data->target->candidate->imageName != nullptr) 
			{
				String ^ path = data->target->candidate->imageName;
				thumbnail = Image::FromFile( path );
				thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
			}
		}
		catch( Exception ^ e)
		{
			System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
		}


		grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
		grid->Rows[rowNum]->Cells[2]->Value = data->centerGPS->lat;
		grid->Rows[rowNum]->Cells[3]->Value = data->centerGPS->lon;
		grid->Rows[rowNum]->Cells[4]->Value = data->target->description->heading;
		grid->Rows[rowNum]->Cells[5]->Value = data->target->description->shape;
		grid->Rows[rowNum]->Cells[6]->Value = data->target->description->shapeColor;
		grid->Rows[rowNum]->Cells[7]->Value = data->target->description->letter;
		grid->Rows[rowNum]->Cells[8]->Value = data->target->description->letterColor;
		grid->Rows[rowNum]->Cells[9]->Value = "" + data->target->targetid;
	}
	else
	{
		auto verifiedTableUpdateDelegate = gcnew Delegates::verifiedRowDataToVoid(this,&TargetsForm::insertVerified);
		this->Invoke(verifiedTableUpdateDelegate,data);
	}
}

System::Void TargetsForm::setUnverifiedTableContents( array<Database::UnverifiedRowData ^> ^ contents )
		{
			if (contents == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::setUnverifiedTableContents(): data is null");
				return;
			}

			if ( !this->unverifiedDataGridView->InvokeRequired )
			{
				this->unverifiedDataGridView->Rows->Clear();

				for each(Database::UnverifiedRowData ^ data in contents)
				{
					insertUnverified(data);
				}
			}
			else
			{
				auto unverifiedTableContentsDelegate = gcnew Delegates::unverifiedRowDataArrayToVoid(this,&TargetsForm::setUnverifiedTableContents);
				array<Object ^>^ arr = {contents}; 
				this->Invoke(unverifiedTableContentsDelegate,arr);
			}
		}

System::Void TargetsForm::setVerifiedTableContents( array<Database::VerifiedRowData ^> ^ contents )
		{
			if (contents == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::setVerifiedTableContents(): data is null");
				return;
			}
			if ( !this->verifiedDataGridView->InvokeRequired )
			{
				this->verifiedDataGridView->Rows->Clear();

				for each(Database::VerifiedRowData ^ data in contents)
				{
					insertVerified(data);
				}
			}
			else
			{
				auto verifiedTableContentsDelegate = gcnew Delegates::verifiedRowDataArrayToVoid(this,&TargetsForm::setVerifiedTableContents);
				array<Object ^>^ arr = {contents}; 
				this->Invoke(verifiedTableContentsDelegate,arr);
			}
		}

System::Void TargetsForm::removeAllTargets(){
	this->candidatesDataGridView->Rows->Clear();
	this->unverifiedDataGridView->Rows->Clear();
	this->verifiedDataGridView->Rows->Clear();
}