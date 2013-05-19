#pragma once

namespace Skynet {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Summary for MapView
	/// </summary>
	public ref class MapView : public System::Windows::Forms::Form
	{
	public:
		MapView(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MapView()
		{
			if (components)
			{
				delete components;
			}
		}
	public: System::Windows::Forms::PictureBox^  autosearchImage;
	public: System::Windows::Forms::PictureBox^  pathfinderImage;
	protected: 

	protected: 

	private: System::Windows::Forms::Label^  autosearchLabel;
	private: System::Windows::Forms::Label^  label1;

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
			this->autosearchImage = (gcnew System::Windows::Forms::PictureBox());
			this->pathfinderImage = (gcnew System::Windows::Forms::PictureBox());
			this->autosearchLabel = (gcnew System::Windows::Forms::Label());
			this->label1 = (gcnew System::Windows::Forms::Label());
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->autosearchImage))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathfinderImage))->BeginInit();
			this->SuspendLayout();
			// 
			// autosearchImage
			// 
			this->autosearchImage->Location = System::Drawing::Point(12, 12);
			this->autosearchImage->Name = L"autosearchImage";
			this->autosearchImage->Size = System::Drawing::Size(326, 341);
			this->autosearchImage->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->autosearchImage->TabIndex = 0;
			this->autosearchImage->TabStop = false;
			// 
			// pathfinderImage
			// 
			this->pathfinderImage->Location = System::Drawing::Point(373, 12);
			this->pathfinderImage->Name = L"pathfinderImage";
			this->pathfinderImage->Size = System::Drawing::Size(326, 341);
			this->pathfinderImage->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pathfinderImage->TabIndex = 1;
			this->pathfinderImage->TabStop = false;
			// 
			// autosearchLabel
			// 
			this->autosearchLabel->AutoSize = true;
			this->autosearchLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->autosearchLabel->Location = System::Drawing::Point(12, 356);
			this->autosearchLabel->Name = L"autosearchLabel";
			this->autosearchLabel->Size = System::Drawing::Size(180, 37);
			this->autosearchLabel->TabIndex = 2;
			this->autosearchLabel->Text = L"Autosearch";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 24, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(384, 356);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(178, 37);
			this->label1->TabIndex = 3;
			this->label1->Text = L"Pathfinding";
			// 
			// MapView
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(712, 417);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->autosearchLabel);
			this->Controls->Add(this->pathfinderImage);
			this->Controls->Add(this->autosearchImage);
			this->Name = L"MapView";
			this->Text = L"MapView";
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->autosearchImage))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathfinderImage))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	};
}
