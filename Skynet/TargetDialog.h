#pragma once

#include "DatabaseStructures.h"
#include "SkynetController.h"

// TODO: This shouldn't be defined here
#define PI 3.14159265358979323846


namespace Vision {
	ref struct TargetResult;
}
namespace Skynet {


	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;


	public ref class TargetDialog : public System::Windows::Forms::Form
	{

		enum class TargetEditingMode {
			MarkingTopLeft,
			MarkingBottomRight,
			NotMarking,
			TargetMarked
		};
	public:
		TargetDialog(Skynet::SkynetController ^ skynetController, Database::TargetRowData^ candidate);

		void loadUIWithData(Database::DialogEditingData^ data);
		void setImage(String^ imageFilename);

		Database::DialogEditingData^ getDataFromUI();

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~TargetDialog()
		{
			if (components)
			{
				delete components;
			}
		}

		double atan3( double x, double y )
		{
			double angle;

			if(x> 0 && y>0)
				angle = -Math::Atan(y/x);
			else if(x>0 && y<0)
			    angle = Math::Atan(-y/x);
			else if(x<0 && y<0)
				angle = PI/2.0 +Math::Atan(-x/-y);
			else if(x<0 && y>0)
			    angle = -PI/2 -Math::Atan(-x/y);
			else if(x> 0 && y == 0)
			    angle = 0;
			else if(x<0 && y==0)
			    angle = PI;
			else if(x==0 && y >0)
			    angle = -PI/2.0;
			else if(x==0 && y<0)
			    angle = -PI/2.0;
			else if(x==0 && y==0)
			    angle =0;

			return -angle;
		}
		

	protected:

		Tuple<int, int>^ topLeft;
		Tuple<int, int>^ bottomRight;
		Database::TargetRowData^ rowData;
		Image ^ _targetImage;
		Object ^ _parent;
		Database::DialogEditingData^ dialogData;
		Skynet::SkynetController ^ appController;
	private: System::Windows::Forms::Button^  ocrButton;
	protected: 
		TargetEditingMode currentMode;

		System::Void createVerifiedTargetFromMarkedTarget();
		Tuple<int, int>^ convertPixelsFromDisplayToOriginal(Tuple<int, int>^ picturePixels);

	private: System::Windows::Forms::PictureBox^  imageBox;
	private: System::Windows::Forms::Button^  markTargetButton;





	private: System::Windows::Forms::Button^  okButton;

private: System::Windows::Forms::Label^  instructionLabel;
	private: System::Windows::Forms::TextBox^  shapeColorTextBox;
	private: System::Windows::Forms::TextBox^  shapeTextBox;


	private: System::Windows::Forms::Label^  shapeLabel;
	private: System::Windows::Forms::Label^  shapeColorLabel;
	private: System::Windows::Forms::Label^  shapeEntryLabel;
	private: System::Windows::Forms::Label^  letterLabel;
	private: System::Windows::Forms::Label^  letterColor;






	private: System::Windows::Forms::Label^  alphanumericLabel;
	private: System::Windows::Forms::TextBox^  letterTextBox;


	private: System::Windows::Forms::TextBox^  letterColorTextBox;

	private: System::Windows::Forms::Button^  deleteButton;


	protected: 

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
			this->imageBox = (gcnew System::Windows::Forms::PictureBox());
			this->markTargetButton = (gcnew System::Windows::Forms::Button());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->instructionLabel = (gcnew System::Windows::Forms::Label());
			this->shapeColorTextBox = (gcnew System::Windows::Forms::TextBox());
			this->shapeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->shapeLabel = (gcnew System::Windows::Forms::Label());
			this->shapeColorLabel = (gcnew System::Windows::Forms::Label());
			this->shapeEntryLabel = (gcnew System::Windows::Forms::Label());
			this->letterLabel = (gcnew System::Windows::Forms::Label());
			this->letterColor = (gcnew System::Windows::Forms::Label());
			this->alphanumericLabel = (gcnew System::Windows::Forms::Label());
			this->letterTextBox = (gcnew System::Windows::Forms::TextBox());
			this->letterColorTextBox = (gcnew System::Windows::Forms::TextBox());
			this->deleteButton = (gcnew System::Windows::Forms::Button());
			this->ocrButton = (gcnew System::Windows::Forms::Button());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageBox))->BeginInit();
			this->SuspendLayout();
			// 
			// imageBox
			// 
			this->imageBox->Location = System::Drawing::Point(13, 12);
			this->imageBox->MaximumSize = System::Drawing::Size(2048, 2048);
			this->imageBox->MinimumSize = System::Drawing::Size(640, 480);
			this->imageBox->Name = L"imageBox";
			this->imageBox->Size = System::Drawing::Size(726, 480);
			this->imageBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->imageBox->TabIndex = 0;
			this->imageBox->TabStop = false;
			this->imageBox->Click += gcnew System::EventHandler(this, &TargetDialog::imageBox_Click);
			this->imageBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &TargetDialog::imageBox_MouseDown);
			// 
			// markTargetButton
			// 
			this->markTargetButton->Location = System::Drawing::Point(15, 580);
			this->markTargetButton->Name = L"markTargetButton";
			this->markTargetButton->Size = System::Drawing::Size(196, 53);
			this->markTargetButton->TabIndex = 2;
			this->markTargetButton->Text = L"Mark Target";
			this->markTargetButton->UseVisualStyleBackColor = true;
			this->markTargetButton->Click += gcnew System::EventHandler(this, &TargetDialog::markTargetButton_Click);
			// 
			// okButton
			// 
			this->okButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->okButton->Location = System::Drawing::Point(540, 579);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(191, 52);
			this->okButton->TabIndex = 3;
			this->okButton->Text = L"Move to Verified";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &TargetDialog::okButton_Click);
			// 
			// instructionLabel
			// 
			this->instructionLabel->AutoSize = true;
			this->instructionLabel->Location = System::Drawing::Point(12, 647);
			this->instructionLabel->Name = L"instructionLabel";
			this->instructionLabel->Size = System::Drawing::Size(61, 13);
			this->instructionLabel->TabIndex = 5;
			this->instructionLabel->Text = L"Instructions";
			// 
			// shapeColorTextBox
			// 
			this->shapeColorTextBox->Location = System::Drawing::Point(259, 574);
			this->shapeColorTextBox->Name = L"shapeColorTextBox";
			this->shapeColorTextBox->Size = System::Drawing::Size(100, 20);
			this->shapeColorTextBox->TabIndex = 6;
			// 
			// shapeTextBox
			// 
			this->shapeTextBox->Location = System::Drawing::Point(259, 613);
			this->shapeTextBox->Name = L"shapeTextBox";
			this->shapeTextBox->Size = System::Drawing::Size(100, 20);
			this->shapeTextBox->TabIndex = 7;
			// 
			// shapeLabel
			// 
			this->shapeLabel->AutoSize = true;
			this->shapeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->shapeLabel->Location = System::Drawing::Point(281, 555);
			this->shapeLabel->Name = L"shapeLabel";
			this->shapeLabel->Size = System::Drawing::Size(53, 16);
			this->shapeLabel->TabIndex = 10;
			this->shapeLabel->Text = L"Shape";
			// 
			// shapeColorLabel
			// 
			this->shapeColorLabel->AutoSize = true;
			this->shapeColorLabel->Location = System::Drawing::Point(224, 580);
			this->shapeColorLabel->Name = L"shapeColorLabel";
			this->shapeColorLabel->Size = System::Drawing::Size(31, 13);
			this->shapeColorLabel->TabIndex = 11;
			this->shapeColorLabel->Text = L"Color";
			// 
			// shapeEntryLabel
			// 
			this->shapeEntryLabel->AutoSize = true;
			this->shapeEntryLabel->Location = System::Drawing::Point(217, 620);
			this->shapeEntryLabel->Name = L"shapeEntryLabel";
			this->shapeEntryLabel->Size = System::Drawing::Size(38, 13);
			this->shapeEntryLabel->TabIndex = 12;
			this->shapeEntryLabel->Text = L"Shape";
			// 
			// letterLabel
			// 
			this->letterLabel->AutoSize = true;
			this->letterLabel->Location = System::Drawing::Point(388, 620);
			this->letterLabel->Name = L"letterLabel";
			this->letterLabel->Size = System::Drawing::Size(34, 13);
			this->letterLabel->TabIndex = 17;
			this->letterLabel->Text = L"Letter";
			// 
			// letterColor
			// 
			this->letterColor->AutoSize = true;
			this->letterColor->Location = System::Drawing::Point(388, 580);
			this->letterColor->Name = L"letterColor";
			this->letterColor->Size = System::Drawing::Size(31, 13);
			this->letterColor->TabIndex = 16;
			this->letterColor->Text = L"Color";
			// 
			// alphanumericLabel
			// 
			this->alphanumericLabel->AutoSize = true;
			this->alphanumericLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->alphanumericLabel->Location = System::Drawing::Point(421, 555);
			this->alphanumericLabel->Name = L"alphanumericLabel";
			this->alphanumericLabel->Size = System::Drawing::Size(102, 16);
			this->alphanumericLabel->TabIndex = 15;
			this->alphanumericLabel->Text = L"Alphanumeric";
			// 
			// letterTextBox
			// 
			this->letterTextBox->Location = System::Drawing::Point(424, 613);
			this->letterTextBox->Name = L"letterTextBox";
			this->letterTextBox->Size = System::Drawing::Size(100, 20);
			this->letterTextBox->TabIndex = 14;
			// 
			// letterColorTextBox
			// 
			this->letterColorTextBox->Location = System::Drawing::Point(423, 574);
			this->letterColorTextBox->Name = L"letterColorTextBox";
			this->letterColorTextBox->Size = System::Drawing::Size(100, 20);
			this->letterColorTextBox->TabIndex = 13;
			// 
			// deleteButton
			// 
			this->deleteButton->Location = System::Drawing::Point(599, 673);
			this->deleteButton->Name = L"deleteButton";
			this->deleteButton->Size = System::Drawing::Size(75, 52);
			this->deleteButton->TabIndex = 18;
			this->deleteButton->Text = L"Delete";
			this->deleteButton->UseVisualStyleBackColor = true;
			this->deleteButton->Click += gcnew System::EventHandler(this, &TargetDialog::deleteButton_Click);
			// 
			// ocrButton
			// 
			this->ocrButton->Location = System::Drawing::Point(486, 673);
			this->ocrButton->Name = L"ocrButton";
			this->ocrButton->Size = System::Drawing::Size(79, 52);
			this->ocrButton->TabIndex = 19;
			this->ocrButton->Text = L"OCR";
			this->ocrButton->UseVisualStyleBackColor = true;
			this->ocrButton->Click += gcnew System::EventHandler(this, &TargetDialog::ocrButton_Click);
			// 
			// TargetDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->ClientSize = System::Drawing::Size(743, 737);
			this->Controls->Add(this->ocrButton);
			this->Controls->Add(this->deleteButton);
			this->Controls->Add(this->letterLabel);
			this->Controls->Add(this->letterColor);
			this->Controls->Add(this->alphanumericLabel);
			this->Controls->Add(this->letterTextBox);
			this->Controls->Add(this->letterColorTextBox);
			this->Controls->Add(this->shapeEntryLabel);
			this->Controls->Add(this->shapeColorLabel);
			this->Controls->Add(this->shapeLabel);
			this->Controls->Add(this->shapeTextBox);
			this->Controls->Add(this->shapeColorTextBox);
			this->Controls->Add(this->instructionLabel);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->markTargetButton);
			this->Controls->Add(this->imageBox);
			this->MaximizeBox = false;
			this->Name = L"TargetDialog";
			this->Text = L"TargetDialog";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void deleteButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void markTargetButton_Click(System::Object^  sender, System::EventArgs^  e);
private: System::Void imageBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
private: System::Void TargetDialog::switchToMode(TargetEditingMode mode);


private: System::Void imageBox_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void ocrButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 Vision::TargetResult^ result = appController->recognizeTarget(rowData->toDialogData()->imageName);
		 }
};

};
