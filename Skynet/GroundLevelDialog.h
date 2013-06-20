#pragma once

namespace Skynet {

	ref class SkynetController;
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GroundLevelDialog
	/// </summary>
	public ref class GroundLevelDialog : public System::Windows::Forms::Form
	{
	public:
		GroundLevelDialog(SkynetController^ skynetController): skynetController(skynetController)
		{
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		SkynetController^ skynetController;
		~GroundLevelDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  okButton;
	public: System::Windows::Forms::TextBox^  altitudeTextBox;

	private: System::Windows::Forms::Label^  label1;
	protected: 

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
			this->okButton = (gcnew System::Windows::Forms::Button());
			this->altitudeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// okButton
			// 
			this->okButton->Location = System::Drawing::Point(223, 28);
			this->okButton->Name = L"okButton";
			this->okButton->Size = System::Drawing::Size(49, 23);
			this->okButton->TabIndex = 0;
			this->okButton->Text = L"OK";
			this->okButton->UseVisualStyleBackColor = true;
			this->okButton->Click += gcnew System::EventHandler(this, &GroundLevelDialog::okButton_Click);
			// 
			// altitudeTextBox
			// 
			this->altitudeTextBox->Location = System::Drawing::Point(116, 30);
			this->altitudeTextBox->Name = L"altitudeTextBox";
			this->altitudeTextBox->Size = System::Drawing::Size(74, 20);
			this->altitudeTextBox->TabIndex = 1;
			this->altitudeTextBox->Text = L"30";
			this->altitudeTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(80, 4);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(61, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Set Altitude";
			this->label1->Click += gcnew System::EventHandler(this, &GroundLevelDialog::label1_Click);
			// 
			// GroundLevelDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(284, 172);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->altitudeTextBox);
			this->Controls->Add(this->okButton);
			this->Name = L"GroundLevelDialog";
			this->Text = L"GroundLevelDialog";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
			 }
	private: System::Void okButton_Click(System::Object^  sender, System::EventArgs^  e);
};
	
}