#pragma once

#include "DatabaseStructures.h"

#define PI 3.14159265358979323846

namespace Skynet {

	enum DialogEditingMode {
		DialogEditingCandidate,
		DialogEditingUnverified,
		DialogEditingVerified
	};

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	ref class SkynetController;

	/// <summary>
	/// Summary for TargetDialog
	/// </summary>
	public ref class TargetDialog : public System::Windows::Forms::Form
	{
	public:
		TargetDialog( Object ^ parent, SkynetController ^ newAppController);

		void showDialogForData(Database::CandidateRowData ^ theData);
		void showDialogForData(Database::UnverifiedRowData ^ theData);
		void showDialogForData(Database::VerifiedRowData ^ theData);

		void reloadData();
		void setImage();
		void getDataFromUI();
		bool isOpen() { return open; }

		property Database::CandidateRowData ^ Candidate
		{
			Database::CandidateRowData ^ get()
			{
				return candidate;
			}
			void set( Database::CandidateRowData ^ newData)
			{
				candidate = newData;
				// TODO set type

				//data = gcnew Database::DialogEditingData(newData);
				reloadData();
			}

		}

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

		void buildVotingText();
		void clearVotingText();

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

		float centerX, centerY;
		float topOfTargetX, topOfTargetY;
		bool _markLat, _markHeading;
		bool open, imageOpen;
		Image ^ _targetImage;
		Object ^ _parent;
		Database::CandidateRowData ^ candidate;
		Database::UnverifiedRowData ^ target;
		Database::DialogEditingData ^ data;
		SkynetController ^ appController;
		DialogEditingMode mode;


	private: System::Windows::Forms::PictureBox^  imageBox;

	private: System::Windows::Forms::Button^  latlonButton;

	private: System::Windows::Forms::Button^  headingButton;

	private: System::Windows::Forms::Button^  okButton;

private: System::Windows::Forms::Label^  instructionLabel;
private: System::Windows::Forms::TextBox^  textBox1;
private: System::Windows::Forms::TextBox^  textBox2;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::Label^  label2;
private: System::Windows::Forms::Label^  label3;
private: System::Windows::Forms::Label^  label4;
private: System::Windows::Forms::Label^  label5;
private: System::Windows::Forms::Label^  label6;
private: System::Windows::Forms::TextBox^  textBox3;
private: System::Windows::Forms::TextBox^  textBox4;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Label^  centerVoteLabel;
private: System::Windows::Forms::Label^  headingVoteLabel;
private: System::Windows::Forms::Label^  shapeVoteResults;
private: System::Windows::Forms::Label^  letterVoteResults;


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
			this->latlonButton = (gcnew System::Windows::Forms::Button());
			this->headingButton = (gcnew System::Windows::Forms::Button());
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->instructionLabel = (gcnew System::Windows::Forms::Label());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->centerVoteLabel = (gcnew System::Windows::Forms::Label());
			this->headingVoteLabel = (gcnew System::Windows::Forms::Label());
			this->shapeVoteResults = (gcnew System::Windows::Forms::Label());
			this->letterVoteResults = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageBox))->BeginInit();
			this->SuspendLayout();
			// 
			// imageBox
			// 
			this->imageBox->Location = System::Drawing::Point(13, 12);
			this->imageBox->Name = L"imageBox";
			this->imageBox->Size = System::Drawing::Size(726, 540);
			this->imageBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->imageBox->TabIndex = 0;
			this->imageBox->TabStop = false;
			this->imageBox->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &TargetDialog::imageBox_MouseDown);
			this->imageBox->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &TargetDialog::imageBox_MouseUp);
			// 
			// latlonButton
			// 
			this->latlonButton->Location = System::Drawing::Point(12, 597);
			this->latlonButton->Name = L"latlonButton";
			this->latlonButton->Size = System::Drawing::Size(97, 23);
			this->latlonButton->TabIndex = 1;
			this->latlonButton->Text = L"Mark Center";
			this->latlonButton->UseVisualStyleBackColor = true;
			this->latlonButton->Click += gcnew System::EventHandler(this, &TargetDialog::latlonButton_Click);
			// 
			// headingButton
			// 
			this->headingButton->Location = System::Drawing::Point(115, 597);
			this->headingButton->Name = L"headingButton";
			this->headingButton->Size = System::Drawing::Size(96, 23);
			this->headingButton->TabIndex = 2;
			this->headingButton->Text = L"Mark Heading";
			this->headingButton->UseVisualStyleBackColor = true;
			this->headingButton->Click += gcnew System::EventHandler(this, &TargetDialog::headingButton_Click);
			// 
			// okButton
			// 
			this->okButton->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 11.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->okButton->Location = System::Drawing::Point(549, 568);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(109, 52);
			this->okButton->TabIndex = 3;
			this->okButton->Text = L"Move to Verified";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &TargetDialog::okButton_Click);
			// 
			// instructionLabel
			// 
			this->instructionLabel->AutoSize = true;
			this->instructionLabel->Location = System::Drawing::Point(12, 558);
			this->instructionLabel->Name = L"instructionLabel";
			this->instructionLabel->Size = System::Drawing::Size(61, 13);
			this->instructionLabel->TabIndex = 5;
			this->instructionLabel->Text = L"Instructions";
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(259, 574);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(100, 20);
			this->textBox1->TabIndex = 6;
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(259, 600);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(100, 20);
			this->textBox2->TabIndex = 7;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(281, 555);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(53, 16);
			this->label1->TabIndex = 10;
			this->label1->Text = L"Shape";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(224, 580);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(31, 13);
			this->label2->TabIndex = 11;
			this->label2->Text = L"Color";
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(224, 603);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(31, 13);
			this->label3->TabIndex = 12;
			this->label3->Text = L"Type";
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(388, 603);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(31, 13);
			this->label4->TabIndex = 17;
			this->label4->Text = L"Type";
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(388, 580);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(31, 13);
			this->label5->TabIndex = 16;
			this->label5->Text = L"Color";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label6->Location = System::Drawing::Point(421, 555);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(102, 16);
			this->label6->TabIndex = 15;
			this->label6->Text = L"Alphanumeric";
			// 
			// textBox3
			// 
			this->textBox3->Location = System::Drawing::Point(423, 600);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(100, 20);
			this->textBox3->TabIndex = 14;
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(423, 574);
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(100, 20);
			this->textBox4->TabIndex = 13;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(665, 568);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 52);
			this->button1->TabIndex = 18;
			this->button1->Text = L"Delete";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &TargetDialog::button1_Click);
			// 
			// centerVoteLabel
			// 
			this->centerVoteLabel->AutoSize = true;
			this->centerVoteLabel->Location = System::Drawing::Point(12, 623);
			this->centerVoteLabel->Name = L"centerVoteLabel";
			this->centerVoteLabel->Size = System::Drawing::Size(95, 13);
			this->centerVoteLabel->TabIndex = 19;
			this->centerVoteLabel->Text = L"Center vote results";
			// 
			// headingVoteLabel
			// 
			this->headingVoteLabel->AutoSize = true;
			this->headingVoteLabel->Location = System::Drawing::Point(113, 623);
			this->headingVoteLabel->Name = L"headingVoteLabel";
			this->headingVoteLabel->Size = System::Drawing::Size(104, 13);
			this->headingVoteLabel->TabIndex = 20;
			this->headingVoteLabel->Text = L"Heading vote results";
			// 
			// shapeVoteResults
			// 
			this->shapeVoteResults->AutoSize = true;
			this->shapeVoteResults->Location = System::Drawing::Point(256, 623);
			this->shapeVoteResults->Name = L"shapeVoteResults";
			this->shapeVoteResults->Size = System::Drawing::Size(101, 13);
			this->shapeVoteResults->TabIndex = 21;
			this->shapeVoteResults->Text = L"Shape Vote Results";
			// 
			// letterVoteResults
			// 
			this->letterVoteResults->AutoSize = true;
			this->letterVoteResults->Location = System::Drawing::Point(421, 623);
			this->letterVoteResults->Name = L"letterVoteResults";
			this->letterVoteResults->Size = System::Drawing::Size(97, 13);
			this->letterVoteResults->TabIndex = 22;
			this->letterVoteResults->Text = L"Letter Vote Results";
			// 
			// TargetDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->BackColor = System::Drawing::SystemColors::ControlDarkDark;
			this->ClientSize = System::Drawing::Size(743, 737);
			this->Controls->Add(this->letterVoteResults);
			this->Controls->Add(this->shapeVoteResults);
			this->Controls->Add(this->headingVoteLabel);
			this->Controls->Add(this->centerVoteLabel);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->textBox3);
			this->Controls->Add(this->textBox4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->instructionLabel);
			this->Controls->Add(this->okButton);
			this->Controls->Add(this->headingButton);
			this->Controls->Add(this->latlonButton);
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
	

private: System::Void cancelButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 candidate = nullptr;
			 target = nullptr;
			 data = nullptr;
			 open = false;
			 this->Close();
		 }
private: System::Void headingButton_Click(System::Object^  sender, System::EventArgs^  e) {

			_markHeading = true;
			_markLat = false;
			instructionLabel->Text = "Click on top of target";
		 }
private: System::Void latlonButton_Click(System::Object^  sender, System::EventArgs^  e) {
			_markHeading = false;
			_markLat = true;
			instructionLabel->Text = "Click on center of target";
		 }
private: System::Void imageBox_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {			 			 
			 if (!imageOpen)
				 setImage();

			 if( _markHeading ) {
				 
				 Point location = e->Location;

				 float x = (float)location.X;
				 float y = (float)location.Y;

				 float boxWidth = (float)this->imageBox->Size.Width;
				 float boxHeight = (float)this->imageBox->Size.Width;

				 float imWidth = (float)data->dataWidth;
				 float imHeight = (float)data->dataHeight;

				 float newPointX = x/boxWidth*imWidth;
				 float newPointY = y/boxHeight*imHeight;

				 topOfTargetX = newPointX;
				 topOfTargetY = newPointY;

			 }
			 else if( _markLat )
			 {
				 
				 Point location = e->Location;

				 float x = (float)location.X;
				 float y = (float)location.Y;

				 float boxWidth = (float)this->imageBox->Size.Width;
				 float boxHeight = (float)this->imageBox->Size.Width;

				 float imWidth = (float)data->dataWidth;
				 float imHeight = (float)data->dataHeight;

				 float newCenterX = x/boxWidth*imWidth;
				 float newCenterY = y/boxHeight*imHeight;

				 centerX = newCenterX;
				 centerY = newCenterY;

			 }
		 }
private: System::Void imageBox_MouseUp(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 return;
		 }

private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e);

};

	/*
	public ref class CandidateTargetDialog: public TargetDialog {

	};

	public ref class UnverifiedTargetDialog: public TargetDialog {
	
	};

	public ref class VerifiedTargetDialog: public TargetDialog {

	};
	*/

}
