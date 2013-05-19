#pragma once

namespace Skynet {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for GPSLocationForm
	/// </summary>
	ref class SkynetController;
	public ref class GPSLocationForm : public System::Windows::Forms::Form
	{
	public:
		GPSLocationForm(Skynet::SkynetController^ controller)
		{
			skynetController = controller;
			InitializeComponent();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		SkynetController^ skynetController;
		~GPSLocationForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label^  latitudeLabel;
	protected: 
	private: System::Windows::Forms::TextBox^  latitudeTextBox;
	private: System::Windows::Forms::Label^  longitudeLabel;
	private: System::Windows::Forms::Label^  altitudeLabel;
	private: System::Windows::Forms::Label^  headerLabel;
	private: System::Windows::Forms::TextBox^  altitudeTextBox;
	private: System::Windows::Forms::TextBox^  longitudeTextBox;
	private: System::Windows::Forms::Button^  submitButton;

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
			this->latitudeLabel = (gcnew System::Windows::Forms::Label());
			this->latitudeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->longitudeLabel = (gcnew System::Windows::Forms::Label());
			this->altitudeLabel = (gcnew System::Windows::Forms::Label());
			this->headerLabel = (gcnew System::Windows::Forms::Label());
			this->altitudeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->longitudeTextBox = (gcnew System::Windows::Forms::TextBox());
			this->submitButton = (gcnew System::Windows::Forms::Button());
			this->SuspendLayout();
			// 
			// latitudeLabel
			// 
			this->latitudeLabel->AutoSize = true;
			this->latitudeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->latitudeLabel->Location = System::Drawing::Point(29, 76);
			this->latitudeLabel->Name = L"latitudeLabel";
			this->latitudeLabel->Size = System::Drawing::Size(71, 20);
			this->latitudeLabel->TabIndex = 0;
			this->latitudeLabel->Text = L"Latitude:";
			// 
			// latitudeTextBox
			// 
			this->latitudeTextBox->Location = System::Drawing::Point(127, 76);
			this->latitudeTextBox->Name = L"latitudeTextBox";
			this->latitudeTextBox->Size = System::Drawing::Size(100, 20);
			this->latitudeTextBox->TabIndex = 1;
			// 
			// longitudeLabel
			// 
			this->longitudeLabel->AutoSize = true;
			this->longitudeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->longitudeLabel->Location = System::Drawing::Point(26, 110);
			this->longitudeLabel->Name = L"longitudeLabel";
			this->longitudeLabel->Size = System::Drawing::Size(84, 20);
			this->longitudeLabel->TabIndex = 2;
			this->longitudeLabel->Text = L"Longitude:";
			// 
			// altitudeLabel
			// 
			this->altitudeLabel->AutoSize = true;
			this->altitudeLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 12, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->altitudeLabel->Location = System::Drawing::Point(27, 145);
			this->altitudeLabel->Name = L"altitudeLabel";
			this->altitudeLabel->Size = System::Drawing::Size(94, 20);
			this->altitudeLabel->TabIndex = 3;
			this->altitudeLabel->Text = L"Altitude (m):";
			// 
			// headerLabel
			// 
			this->headerLabel->AutoSize = true;
			this->headerLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->headerLabel->Location = System::Drawing::Point(24, 24);
			this->headerLabel->Name = L"headerLabel";
			this->headerLabel->Size = System::Drawing::Size(203, 24);
			this->headerLabel->TabIndex = 4;
			this->headerLabel->Text = L"Emulate Plane Position";
			// 
			// altitudeTextBox
			// 
			this->altitudeTextBox->Location = System::Drawing::Point(127, 145);
			this->altitudeTextBox->Name = L"altitudeTextBox";
			this->altitudeTextBox->Size = System::Drawing::Size(100, 20);
			this->altitudeTextBox->TabIndex = 5;
			// 
			// longitudeTextBox
			// 
			this->longitudeTextBox->Location = System::Drawing::Point(127, 112);
			this->longitudeTextBox->Name = L"longitudeTextBox";
			this->longitudeTextBox->Size = System::Drawing::Size(100, 20);
			this->longitudeTextBox->TabIndex = 6;
			// 
			// submitButton
			// 
			this->submitButton->Location = System::Drawing::Point(140, 184);
			this->submitButton->Name = L"submitButton";
			this->submitButton->Size = System::Drawing::Size(75, 23);
			this->submitButton->TabIndex = 7;
			this->submitButton->Text = L"OK";
			this->submitButton->UseVisualStyleBackColor = true;
			this->submitButton->Click += gcnew System::EventHandler(this, &GPSLocationForm::submitButton_Click);
			// 
			// GPSLocationForm
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(263, 219);
			this->Controls->Add(this->submitButton);
			this->Controls->Add(this->longitudeTextBox);
			this->Controls->Add(this->altitudeTextBox);
			this->Controls->Add(this->headerLabel);
			this->Controls->Add(this->altitudeLabel);
			this->Controls->Add(this->longitudeLabel);
			this->Controls->Add(this->latitudeTextBox);
			this->Controls->Add(this->latitudeLabel);
			this->Name = L"GPSLocationForm";
			this->Text = L"GPSLocationForm";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void submitButton_Click(System::Object^  sender, System::EventArgs^  e);
};
}
