#pragma once
#include "SkynetController.h"
#include "DatabaseStructures.h"

namespace Skynet {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for TargetsForm
	/// </summary>
	public ref class TargetsForm : public System::Windows::Forms::Form
	{
	public:
		TargetsForm(SkynetController^ skynetController);
		System::Void insertCandidate(Database::CandidateRowData^ data);
		System::Void insertUnverified(Database::UnverifiedRowData^ data);
		System::Void insertVerified(Database::VerifiedRowData^ data);
		System::Void setCandidateTableContents( array<Database::CandidateRowData ^> ^ contents );
		System::Void setUnverifiedTableContents( array<Database::UnverifiedRowData ^> ^ contents );
		System::Void setVerifiedTableContents( array<Database::VerifiedRowData ^> ^ contents );

		// TODO
		System::Void removeCandidate(Database::CandidateRowData^ data){};
		System::Void removeUnverified(Database::UnverifiedRowData^ data){};
		System::Void removeVerified(Database::VerifiedRowData^ data){};
		System::Void removeAllTargets();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TargetsForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: 
		SkynetController^ skynetController;
	private: System::Windows::Forms::TabControl^  metadataTabControl;
	private: System::Windows::Forms::TabPage^  candidatesTab;
	protected: 

	private: System::Windows::Forms::DataGridView^  candidatesDataGridView;

	private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_IDColumn;
	private: System::Windows::Forms::DataGridViewImageColumn^  candidate_ThumbnailColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_LatitudeColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  canidate_LongitudeColumn;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  candidate_VerifiedColumn;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  processed;
	private: System::Windows::Forms::TabPage^  unverifiedTab;

	private: System::Windows::Forms::DataGridView^  unverifiedDataGridView;

	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_IDColumn;
	private: System::Windows::Forms::DataGridViewImageColumn^  unverified_ThumbnailColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LatitudeColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LongitudeColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_HeadingColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_CenterPixel;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverfied_LetterColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_LetterColor;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_Shape;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  unverified_ShapeColor;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  unverified_ProcessedColumn;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  unverified_TargetLock;
	private: System::Windows::Forms::TabPage^  verifiedTab;
	private: System::Windows::Forms::DataGridView^  verifiedDataGridView;


	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_IDColumn;
	private: System::Windows::Forms::DataGridViewImageColumn^  verified_ThumbnailColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_LatitudeColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_LongitudeColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_HeadingColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_Shape;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_ShapeColor;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_LetterColumn;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  verified_LetterColor;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  targetid_column;
	private: System::Windows::Forms::DataGridViewCheckBoxColumn^  verified_lockon;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->metadataTabControl = (gcnew System::Windows::Forms::TabControl());
			this->candidatesTab = (gcnew System::Windows::Forms::TabPage());
			this->candidatesDataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->candidate_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->candidate_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->canidate_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_VerifiedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->processed = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->unverifiedTab = (gcnew System::Windows::Forms::TabPage());
			this->unverifiedDataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->unverified_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->unverified_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_CenterPixel = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverfied_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->unverified_ProcessedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->unverified_TargetLock = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->verifiedTab = (gcnew System::Windows::Forms::TabPage());
			this->verifiedDataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->verified_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->verified_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->targetid_column = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_lockon = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->metadataTabControl->SuspendLayout();
			this->candidatesTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->candidatesDataGridView))->BeginInit();
			this->unverifiedTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->unverifiedDataGridView))->BeginInit();
			this->verifiedTab->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->verifiedDataGridView))->BeginInit();
			this->SuspendLayout();
			// 
			// metadataTabControl
			// 
			this->metadataTabControl->Controls->Add(this->candidatesTab);
			this->metadataTabControl->Controls->Add(this->unverifiedTab);
			this->metadataTabControl->Controls->Add(this->verifiedTab);
			this->metadataTabControl->Dock = System::Windows::Forms::DockStyle::Fill;
			this->metadataTabControl->Location = System::Drawing::Point(0, 0);
			this->metadataTabControl->Name = L"metadataTabControl";
			this->metadataTabControl->SelectedIndex = 0;
			this->metadataTabControl->Size = System::Drawing::Size(1031, 395);
			this->metadataTabControl->TabIndex = 50;
			// 
			// candidatesTab
			// 
			this->candidatesTab->Controls->Add(this->candidatesDataGridView);
			this->candidatesTab->Location = System::Drawing::Point(4, 22);
			this->candidatesTab->Name = L"candidatesTab";
			this->candidatesTab->Padding = System::Windows::Forms::Padding(3);
			this->candidatesTab->Size = System::Drawing::Size(1023, 369);
			this->candidatesTab->TabIndex = 0;
			this->candidatesTab->Text = L"Candidate Regions";
			// 
			// candidatesDataGridView
			// 
			this->candidatesDataGridView->AllowUserToAddRows = false;
			this->candidatesDataGridView->AllowUserToDeleteRows = false;
			this->candidatesDataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->candidatesDataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(6) {this->candidate_IDColumn, 
				this->candidate_ThumbnailColumn, this->candidate_LatitudeColumn, this->canidate_LongitudeColumn, this->candidate_VerifiedColumn, 
				this->processed});
			this->candidatesDataGridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->candidatesDataGridView->Location = System::Drawing::Point(3, 3);
			this->candidatesDataGridView->Name = L"candidatesDataGridView";
			this->candidatesDataGridView->RowTemplate->Height = 64;
			this->candidatesDataGridView->Size = System::Drawing::Size(1017, 363);
			this->candidatesDataGridView->TabIndex = 0;
			this->candidatesDataGridView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &TargetsForm::candidatesDataGridView_CellContentClick);
			// 
			// candidate_IDColumn
			// 
			this->candidate_IDColumn->HeaderText = L"ID";
			this->candidate_IDColumn->Name = L"candidate_IDColumn";
			this->candidate_IDColumn->ReadOnly = true;
			this->candidate_IDColumn->Width = 70;
			// 
			// candidate_ThumbnailColumn
			// 
			this->candidate_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->candidate_ThumbnailColumn->Name = L"candidate_ThumbnailColumn";
			this->candidate_ThumbnailColumn->ReadOnly = true;
			this->candidate_ThumbnailColumn->Width = 64;
			// 
			// candidate_LatitudeColumn
			// 
			this->candidate_LatitudeColumn->HeaderText = L"Latitude";
			this->candidate_LatitudeColumn->Name = L"candidate_LatitudeColumn";
			this->candidate_LatitudeColumn->ReadOnly = true;
			// 
			// canidate_LongitudeColumn
			// 
			this->canidate_LongitudeColumn->HeaderText = L"Longitude";
			this->canidate_LongitudeColumn->Name = L"canidate_LongitudeColumn";
			this->canidate_LongitudeColumn->ReadOnly = true;
			// 
			// candidate_VerifiedColumn
			// 
			this->candidate_VerifiedColumn->HeaderText = L"Verified";
			this->candidate_VerifiedColumn->Name = L"candidate_VerifiedColumn";
			this->candidate_VerifiedColumn->ReadOnly = true;
			this->candidate_VerifiedColumn->Visible = false;
			// 
			// processed
			// 
			this->processed->HeaderText = L"Processed";
			this->processed->Name = L"processed";
			// 
			// unverifiedTab
			// 
			this->unverifiedTab->Controls->Add(this->unverifiedDataGridView);
			this->unverifiedTab->Location = System::Drawing::Point(4, 22);
			this->unverifiedTab->Name = L"unverifiedTab";
			this->unverifiedTab->Padding = System::Windows::Forms::Padding(3);
			this->unverifiedTab->Size = System::Drawing::Size(1023, 369);
			this->unverifiedTab->TabIndex = 1;
			this->unverifiedTab->Text = L"Unverified Targets";
			this->unverifiedTab->UseVisualStyleBackColor = true;
			// 
			// unverifiedDataGridView
			// 
			this->unverifiedDataGridView->AllowUserToAddRows = false;
			this->unverifiedDataGridView->AllowUserToDeleteRows = false;
			this->unverifiedDataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->unverifiedDataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(12) {this->unverified_IDColumn, 
				this->unverified_ThumbnailColumn, this->unverified_LatitudeColumn, this->unverified_LongitudeColumn, this->unverified_HeadingColumn, 
				this->unverified_CenterPixel, this->unverfied_LetterColumn, this->unverified_LetterColor, this->unverified_Shape, this->unverified_ShapeColor, 
				this->unverified_ProcessedColumn, this->unverified_TargetLock});
			this->unverifiedDataGridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->unverifiedDataGridView->Location = System::Drawing::Point(3, 3);
			this->unverifiedDataGridView->Name = L"unverifiedDataGridView";
			this->unverifiedDataGridView->RowTemplate->Height = 64;
			this->unverifiedDataGridView->Size = System::Drawing::Size(1017, 363);
			this->unverifiedDataGridView->TabIndex = 0;
			this->unverifiedDataGridView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &TargetsForm::unverifiedDataGridView_CellContentClick);
			// 
			// unverified_IDColumn
			// 
			this->unverified_IDColumn->HeaderText = L"ID";
			this->unverified_IDColumn->Name = L"unverified_IDColumn";
			this->unverified_IDColumn->ReadOnly = true;
			this->unverified_IDColumn->Width = 70;
			// 
			// unverified_ThumbnailColumn
			// 
			this->unverified_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->unverified_ThumbnailColumn->Name = L"unverified_ThumbnailColumn";
			this->unverified_ThumbnailColumn->ReadOnly = true;
			this->unverified_ThumbnailColumn->Width = 64;
			// 
			// unverified_LatitudeColumn
			// 
			this->unverified_LatitudeColumn->HeaderText = L"Latitude";
			this->unverified_LatitudeColumn->Name = L"unverified_LatitudeColumn";
			this->unverified_LatitudeColumn->ReadOnly = true;
			// 
			// unverified_LongitudeColumn
			// 
			this->unverified_LongitudeColumn->HeaderText = L"Longitude";
			this->unverified_LongitudeColumn->Name = L"unverified_LongitudeColumn";
			this->unverified_LongitudeColumn->ReadOnly = true;
			// 
			// unverified_HeadingColumn
			// 
			this->unverified_HeadingColumn->HeaderText = L"Heading";
			this->unverified_HeadingColumn->Name = L"unverified_HeadingColumn";
			this->unverified_HeadingColumn->ReadOnly = true;
			// 
			// unverified_CenterPixel
			// 
			this->unverified_CenterPixel->HeaderText = L"Center Pixel";
			this->unverified_CenterPixel->Name = L"unverified_CenterPixel";
			this->unverified_CenterPixel->ReadOnly = true;
			// 
			// unverfied_LetterColumn
			// 
			this->unverfied_LetterColumn->HeaderText = L"Letter";
			this->unverfied_LetterColumn->Name = L"unverfied_LetterColumn";
			this->unverfied_LetterColumn->ReadOnly = true;
			// 
			// unverified_LetterColor
			// 
			this->unverified_LetterColor->HeaderText = L"Letter Color";
			this->unverified_LetterColor->Name = L"unverified_LetterColor";
			this->unverified_LetterColor->ReadOnly = true;
			// 
			// unverified_Shape
			// 
			this->unverified_Shape->HeaderText = L"Shape";
			this->unverified_Shape->Name = L"unverified_Shape";
			this->unverified_Shape->ReadOnly = true;
			// 
			// unverified_ShapeColor
			// 
			this->unverified_ShapeColor->HeaderText = L"Shape Color";
			this->unverified_ShapeColor->Name = L"unverified_ShapeColor";
			this->unverified_ShapeColor->ReadOnly = true;
			// 
			// unverified_ProcessedColumn
			// 
			this->unverified_ProcessedColumn->HeaderText = L"Processed";
			this->unverified_ProcessedColumn->Name = L"unverified_ProcessedColumn";
			this->unverified_ProcessedColumn->ReadOnly = true;
			// 
			// unverified_TargetLock
			// 
			this->unverified_TargetLock->HeaderText = L"Lock-On";
			this->unverified_TargetLock->Name = L"unverified_TargetLock";
			this->unverified_TargetLock->ReadOnly = true;
			// 
			// verifiedTab
			// 
			this->verifiedTab->Controls->Add(this->verifiedDataGridView);
			this->verifiedTab->Location = System::Drawing::Point(4, 22);
			this->verifiedTab->Name = L"verifiedTab";
			this->verifiedTab->Padding = System::Windows::Forms::Padding(3);
			this->verifiedTab->Size = System::Drawing::Size(1023, 369);
			this->verifiedTab->TabIndex = 2;
			this->verifiedTab->Text = L"VerifiedTargets";
			this->verifiedTab->UseVisualStyleBackColor = true;
			// 
			// verifiedDataGridView
			// 
			this->verifiedDataGridView->AllowUserToAddRows = false;
			this->verifiedDataGridView->AllowUserToDeleteRows = false;
			this->verifiedDataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->verifiedDataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(11) {this->verified_IDColumn, 
				this->verified_ThumbnailColumn, this->verified_LatitudeColumn, this->verified_LongitudeColumn, this->verified_HeadingColumn, 
				this->verified_Shape, this->verified_ShapeColor, this->verified_LetterColumn, this->verified_LetterColor, this->targetid_column, 
				this->verified_lockon});
			this->verifiedDataGridView->Dock = System::Windows::Forms::DockStyle::Fill;
			this->verifiedDataGridView->Location = System::Drawing::Point(3, 3);
			this->verifiedDataGridView->MultiSelect = false;
			this->verifiedDataGridView->Name = L"verifiedDataGridView";
			this->verifiedDataGridView->RowTemplate->Height = 64;
			this->verifiedDataGridView->Size = System::Drawing::Size(1017, 363);
			this->verifiedDataGridView->TabIndex = 0;
			this->verifiedDataGridView->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &TargetsForm::verifiedDataGridView_CellContentClick);
			// 
			// verified_IDColumn
			// 
			this->verified_IDColumn->HeaderText = L"ID";
			this->verified_IDColumn->Name = L"verified_IDColumn";
			this->verified_IDColumn->ReadOnly = true;
			this->verified_IDColumn->Width = 70;
			// 
			// verified_ThumbnailColumn
			// 
			this->verified_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->verified_ThumbnailColumn->Name = L"verified_ThumbnailColumn";
			this->verified_ThumbnailColumn->ReadOnly = true;
			this->verified_ThumbnailColumn->Width = 64;
			// 
			// verified_LatitudeColumn
			// 
			this->verified_LatitudeColumn->HeaderText = L"Latitude";
			this->verified_LatitudeColumn->Name = L"verified_LatitudeColumn";
			this->verified_LatitudeColumn->ReadOnly = true;
			// 
			// verified_LongitudeColumn
			// 
			this->verified_LongitudeColumn->HeaderText = L"Longitude";
			this->verified_LongitudeColumn->Name = L"verified_LongitudeColumn";
			this->verified_LongitudeColumn->ReadOnly = true;
			// 
			// verified_HeadingColumn
			// 
			this->verified_HeadingColumn->HeaderText = L"Orientation";
			this->verified_HeadingColumn->Name = L"verified_HeadingColumn";
			this->verified_HeadingColumn->ReadOnly = true;
			// 
			// verified_Shape
			// 
			this->verified_Shape->HeaderText = L"Shape";
			this->verified_Shape->Name = L"verified_Shape";
			this->verified_Shape->ReadOnly = true;
			// 
			// verified_ShapeColor
			// 
			this->verified_ShapeColor->HeaderText = L"Shape Color";
			this->verified_ShapeColor->Name = L"verified_ShapeColor";
			this->verified_ShapeColor->ReadOnly = true;
			// 
			// verified_LetterColumn
			// 
			this->verified_LetterColumn->HeaderText = L"Letter";
			this->verified_LetterColumn->Name = L"verified_LetterColumn";
			this->verified_LetterColumn->ReadOnly = true;
			// 
			// verified_LetterColor
			// 
			this->verified_LetterColor->HeaderText = L"Letter Color";
			this->verified_LetterColor->Name = L"verified_LetterColor";
			this->verified_LetterColor->ReadOnly = true;
			// 
			// targetid_column
			// 
			this->targetid_column->HeaderText = L"Candidate ID";
			this->targetid_column->Name = L"targetid_column";
			this->targetid_column->ReadOnly = true;
			// 
			// verified_lockon
			// 
			this->verified_lockon->HeaderText = L"Lock On";
			this->verified_lockon->Name = L"verified_lockon";
			this->verified_lockon->ReadOnly = true;
			// 
			// TargetsForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(1031, 395);
			this->Controls->Add(this->metadataTabControl);
			this->Name = L"TargetsForm";
			this->Text = L"Targets";
			this->metadataTabControl->ResumeLayout(false);
			this->candidatesTab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->candidatesDataGridView))->EndInit();
			this->unverifiedTab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->unverifiedDataGridView))->EndInit();
			this->verifiedTab->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->verifiedDataGridView))->EndInit();
			this->ResumeLayout(false);

		}
#pragma endregion
	private: System::Void candidatesDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
	private: System::Void unverifiedDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
	private: System::Void verifiedDataGridView_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
};
}
