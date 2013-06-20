#pragma once

#using<System.Drawing.dll>
#include "MasterHeader.h"

#include "Database.h"
#include "Delegates.h"

#include "GeoReference.h"
#include "TargetDialog.h"
#include "TargetsForm.h"
#include "HUDControl.h"
#include "Saliency.h"
#include "OCRWrapper.h"
#include "SimHandler.h"
#include "PlaneWatcher.h"
#include "SkynetController.h"
#include "VisionController.h"
#include "Waypoint.h"
#include "PlaneDataReceiver.h"
#include "ImageWithPlaneData.h"
#include "MapView.h"
#include "GPSLocationForm.h"
#include "GPSCoord.h"

#include <math.h>

#define GREEN_IMAGE_PATH (DROPBOX_DIR + "Skynet\\Skynet\\greenled.png")
#define RED_IMAGE_PATH (DROPBOX_DIR + "Skynet\\Skynet\\redled.png")
#define YELLOW_IMAGE_PATH (DROPBOX_DIR + "Skynet\\Skynet\\yellowled.png")
#define VERIFIED_LOCK_ON_COLUMN 10

	// row indexes for comport data
	const int A_ALT			= 1;
	const int A_LAT			= 2;
	const int A_LON			= 3;
	const int A_HEAD		= 4;
	const int A_ROLL		= 5;
	const int A_PITCH		= 6;
	const int G_ROLL		= 8;
	const int G_PITCH		= 9;
	const int V_ZOOM		= 11;
	const int V_FORMAT		= 12;
	
namespace Skynet {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace System::Runtime::InteropServices;
	using namespace System::Threading;
	using namespace System::IO;
	using namespace System::Text;
	using namespace System::Linq;
	using namespace Intelligence;


	const int COL_ID = 0;
	const int COL_THUMB = 1;
	const int COL_LAT = 2;
	const int COL_LON = 3;
	const int COL_HEAD = 4;
	const int COL_LETTER = 5;
	const int COL_SHAPE = 6;
	const int COL_FG = 7;
	const int COL_BG = 8;
	const int COL_PROC = 9;
	const int COL_VERIFIED = 10;
	/// <summary>
	/// Summary for Form1
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
		///
		/// Stuff added manually
		///

	public:
		Delegates::voidToVoid ^ comportErrorDelegate;
		Delegates::voidToVoid ^ comportEstablishDelegate;
		Delegates::twointThreedoubleToVoid ^ imageDialogDelegate;
		Delegates::verifiedRowDataToVoid ^ verifiedTableUpdateDelegate;
		Delegates::unverifiedRowDataToVoid ^ unverifiedTableUpdateDelegate;
		Delegates::waypointArraytoVoid ^ checkboxListDelegate;
		Delegates::candidateRowDataToVoid ^ candidateTableUpdateDelegate;
		Delegates::verifiedRowDataArrayToVoid ^ verifiedTableContentsDelegate;
		Delegates::unverifiedRowDataArrayToVoid ^ unverifiedTableContentsDelegate;
		Delegates::candidateRowDataArrayToVoid ^ candidateTableContentsDelegate;
		Delegates::dataGridViewRowToVoid ^ ocrDelegate;
		Delegates::voidToVoid ^ saveImageDelegate;
		MapView^ mapView;
	protected:
		TargetsForm^ targetsForm;
		SkynetController ^				appController;
		// Database::DatabaseConnection ^	db;
		TargetDialog ^					imageDialog;

		//added
		//TargetsForm^ maptargetsForm;

		
		
		String ^ fileExtension;
		String ^ defaultMapCache;

		bool isconnected;
		bool isConnecting;
		bool vidOptFolderDialogOpen;		// For video options directory

		Thread ^ comReadThread;
		int imageNumber; // for saving images

		// Temporary logging stuff
		FileStream ^ logFile;

		// Video saving stuff
		int splitLength; // in seconds
		bool recording;
		DateTime recordStart;

		//Database ID
		int incrId;

		//Map point
		Point mouseGPS;

		// Image details


		Image ^redImage;
		Image ^yellowImage;
		Image ^greenImage;






	private: System::Windows::Forms::Button^  startRecordButton;














	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;

	private: System::Windows::Forms::FolderBrowserDialog^  vidOptChangeDirBrowserDialog;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Property;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Value;










	private: System::Windows::Forms::Timer^  videoSaveTimer;









	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;


private: System::Windows::Forms::ToolStripMenuItem^  exportDataToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
private: System::Windows::Forms::ToolStripMenuItem^  databaseToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  resetToolStripMenuItem;
private: System::Windows::Forms::Timer^  mapUpdateTimer;


private: System::Windows::Forms::ContextMenuStrip^  mapMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  mapLookGPSToolStripMenuItem;
private: System::Windows::Forms::OpenFileDialog^  simReadVidDialog;
private: System::Diagnostics::Process^  process1;
private: HUDControl^ gimbalHUDView;
private: System::Windows::Forms::CheckedListBox^  gpsCheckboxList;
private: System::Windows::Forms::Button^  generateMapButton;
private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::PictureBox^  imageView;
private: System::Windows::Forms::ToolStripMenuItem^  searchAreaToolStripMenuItem;
private: System::Windows::Forms::DataGridView^  mapBoundariesDataGridView;



private: System::Windows::Forms::ToolStripMenuItem^  loadMapFromTextFileToolStripMenuItem;
private: System::Windows::Forms::DataGridView^  pathDataGridView;

private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Label^  label1;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  dataGridViewTextBoxColumn2;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  dataGridViewTextBoxColumn3;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  boundary_latitude;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  boundary_longitude;
private: System::Windows::Forms::ToolStripMenuItem^  lockPlaneCoordinatesToolStripMenuItem;


private: System::Windows::Forms::ToolStripMenuItem^  unlockPlaneCoordinatesToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  startMissionToolStripMenuItem;
private: System::Windows::Forms::Button^  button5;


private: Thread^ verifiedTableUpdaterThread;
private: System::Windows::Forms::Label^  currentFrameLabel;
private: System::Windows::Forms::Button^  goToFrameButton;
private: System::Windows::Forms::TextBox^  frameTextBox;

private: System::Windows::Forms::Label^  currentFrameDescLabel;
private: System::Windows::Forms::Button^  nextButton;
private: System::Windows::Forms::Button^  stopButton;
private: System::Windows::Forms::Button^  playPauseButton;
private: System::Windows::Forms::Button^  previousButton;
public: bool threadsShouldQuit;
	public:
		Form1(void)
		{
			threadsShouldQuit = false;
			InitializeComponent();

			// Tester::runTests();
			appController = gcnew SkynetController(this);

			
			this->ResumeLayout(false);
			this->PerformLayout();


			// Image options
			imageNumber = 0;
			fileExtension = ".jpg";

			// Logging			
			DateTime timeTemp = DateTime::Now;
			logFile = File::Create( SKYNET_LOG_DIR + timeTemp.ToString("o")->Replace(":", "-") + ".txt" );

			// Set up metadata Table
			this->metadataTable->Rows->Add("Airplane Data", "---");
			this->metadataTable->Rows->Add("    Altitude");
			this->metadataTable->Rows->Add("    Latitude");
			this->metadataTable->Rows->Add("    Longitude");
			this->metadataTable->Rows->Add("    Heading");
			this->metadataTable->Rows->Add("    Roll");
			this->metadataTable->Rows->Add("    Pitch");
			this->metadataTable->Rows->Add("Gimbal Data", "---");
			this->metadataTable->Rows->Add("    Roll");
			this->metadataTable->Rows->Add("    Pitch");
			this->metadataTable->Rows->Add("Video Data", "---");
			this->metadataTable->Rows->Add("    Zoom");

			consoleMessage("... Skynet online", Color::Orange);
			
			targetsForm = gcnew TargetsForm(appController);
			targetsForm->Show();
			appController->loadAllTablesFromDisk();


			appController->comeAlive();
			ParameterizedThreadStart^ threadDelegate = gcnew ParameterizedThreadStart(appController, &SkynetController::updateVerifiedTableFromDatabaseAtInterval);
			verifiedTableUpdaterThread = gcnew Thread (threadDelegate);
			verifiedTableUpdaterThread->Name = "Verified Table Updater Thread";
			verifiedTableUpdaterThread->Start(5000);

		}

	public:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if ( logFile )
            	delete (IDisposable^)logFile;

			if (components)
			{
				delete components;
				System::Diagnostics::Trace::WriteLine("Components Deleted");
			}
			
			threadsShouldQuit = true;
			delete appController;
			Application::Exit();

		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::Timer^  openGLTimer;

	private: System::Windows::Forms::RichTextBox^  errorLogTextBox;
	public: System::Windows::Forms::DataGridView^  metadataTable;


	private: System::ComponentModel::IContainer^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = (gcnew System::ComponentModel::Container());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exportDataToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lockPlaneCoordinatesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->unlockPlaneCoordinatesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->startMissionToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->databaseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->resetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->searchAreaToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->loadMapFromTextFileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openGLTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->errorLogTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->metadataTable = (gcnew System::Windows::Forms::DataGridView());
			this->Property = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Value = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->startRecordButton = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->vidOptChangeDirBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->videoSaveTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->mapUpdateTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->mapMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->mapLookGPSToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simReadVidDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->process1 = (gcnew System::Diagnostics::Process());
			this->gpsCheckboxList = (gcnew System::Windows::Forms::CheckedListBox());
			this->generateMapButton = (gcnew System::Windows::Forms::Button());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->button5 = (gcnew System::Windows::Forms::Button());
			this->imageView = (gcnew System::Windows::Forms::PictureBox());
			this->mapBoundariesDataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->boundary_latitude = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->boundary_longitude = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->pathDataGridView = (gcnew System::Windows::Forms::DataGridView());
			this->dataGridViewTextBoxColumn2 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->dataGridViewTextBoxColumn3 = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->previousButton = (gcnew System::Windows::Forms::Button());
			this->playPauseButton = (gcnew System::Windows::Forms::Button());
			this->stopButton = (gcnew System::Windows::Forms::Button());
			this->nextButton = (gcnew System::Windows::Forms::Button());
			this->currentFrameDescLabel = (gcnew System::Windows::Forms::Label());
			this->frameTextBox = (gcnew System::Windows::Forms::TextBox());
			this->goToFrameButton = (gcnew System::Windows::Forms::Button());
			this->currentFrameLabel = (gcnew System::Windows::Forms::Label());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageView))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mapBoundariesDataGridView))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathDataGridView))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::Color::Gray;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->toolStripMenuItem1, 
				this->databaseToolStripMenuItem, this->searchAreaToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(1398, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->exportDataToolStripMenuItem, 
				this->lockPlaneCoordinatesToolStripMenuItem, this->unlockPlaneCoordinatesToolStripMenuItem, this->startMissionToolStripMenuItem, 
				this->toolStripSeparator3, this->exitToolStripMenuItem});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(37, 20);
			this->toolStripMenuItem1->Text = L"&File";
			// 
			// exportDataToolStripMenuItem
			// 
			this->exportDataToolStripMenuItem->Name = L"exportDataToolStripMenuItem";
			this->exportDataToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->exportDataToolStripMenuItem->Text = L"Export &Data...";
			this->exportDataToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exportDataToolStripMenuItem_Click);
			// 
			// lockPlaneCoordinatesToolStripMenuItem
			// 
			this->lockPlaneCoordinatesToolStripMenuItem->Name = L"lockPlaneCoordinatesToolStripMenuItem";
			this->lockPlaneCoordinatesToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->lockPlaneCoordinatesToolStripMenuItem->Text = L"Lock Plane Coordinates";
			this->lockPlaneCoordinatesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::lockPlaneCoordinatesToolStripMenuItem_Click);
			// 
			// unlockPlaneCoordinatesToolStripMenuItem
			// 
			this->unlockPlaneCoordinatesToolStripMenuItem->Name = L"unlockPlaneCoordinatesToolStripMenuItem";
			this->unlockPlaneCoordinatesToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->unlockPlaneCoordinatesToolStripMenuItem->Text = L"Unlock Plane Coordinates";
			this->unlockPlaneCoordinatesToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::unlockPlaneCoordinatesToolStripMenuItem_Click);
			// 
			// startMissionToolStripMenuItem
			// 
			this->startMissionToolStripMenuItem->Name = L"startMissionToolStripMenuItem";
			this->startMissionToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->startMissionToolStripMenuItem->Text = L"Start Mission";
			this->startMissionToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::startMissionToolStripMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(207, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->exitToolStripMenuItem->Size = System::Drawing::Size(210, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// databaseToolStripMenuItem
			// 
			this->databaseToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->resetToolStripMenuItem});
			this->databaseToolStripMenuItem->Name = L"databaseToolStripMenuItem";
			this->databaseToolStripMenuItem->Size = System::Drawing::Size(67, 20);
			this->databaseToolStripMenuItem->Text = L"Database";
			// 
			// resetToolStripMenuItem
			// 
			this->resetToolStripMenuItem->Name = L"resetToolStripMenuItem";
			this->resetToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->resetToolStripMenuItem->Text = L"Reset";
			this->resetToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::resetToolStripMenuItem_Click);
			// 
			// searchAreaToolStripMenuItem
			// 
			this->searchAreaToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->loadMapFromTextFileToolStripMenuItem});
			this->searchAreaToolStripMenuItem->Name = L"searchAreaToolStripMenuItem";
			this->searchAreaToolStripMenuItem->Size = System::Drawing::Size(81, 20);
			this->searchAreaToolStripMenuItem->Text = L"Search Area";
			// 
			// loadMapFromTextFileToolStripMenuItem
			// 
			this->loadMapFromTextFileToolStripMenuItem->Name = L"loadMapFromTextFileToolStripMenuItem";
			this->loadMapFromTextFileToolStripMenuItem->Size = System::Drawing::Size(204, 22);
			this->loadMapFromTextFileToolStripMenuItem->Text = L"Load Map From Text File";
			// 
			// errorLogTextBox
			// 
			this->errorLogTextBox->AutoWordSelection = true;
			this->errorLogTextBox->BackColor = System::Drawing::Color::Black;
			this->errorLogTextBox->BorderStyle = System::Windows::Forms::BorderStyle::None;
			this->errorLogTextBox->ForeColor = System::Drawing::SystemColors::Window;
			this->errorLogTextBox->Location = System::Drawing::Point(12, 492);
			this->errorLogTextBox->Name = L"errorLogTextBox";
			this->errorLogTextBox->ReadOnly = true;
			this->errorLogTextBox->Size = System::Drawing::Size(227, 377);
			this->errorLogTextBox->TabIndex = 2;
			this->errorLogTextBox->Text = L"";
			// 
			// metadataTable
			// 
			this->metadataTable->AllowUserToAddRows = false;
			this->metadataTable->AllowUserToDeleteRows = false;
			this->metadataTable->AllowUserToResizeColumns = false;
			this->metadataTable->AllowUserToResizeRows = false;
			dataGridViewCellStyle1->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle1->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->AlternatingRowsDefaultCellStyle = dataGridViewCellStyle1;
			this->metadataTable->BackgroundColor = System::Drawing::Color::Black;
			dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle2->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle2->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle2->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle2->SelectionForeColor = System::Drawing::Color::Black;
			dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle2;
			this->metadataTable->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->metadataTable->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->Property, 
				this->Value});
			this->metadataTable->EditMode = System::Windows::Forms::DataGridViewEditMode::EditProgrammatically;
			this->metadataTable->GridColor = System::Drawing::Color::Black;
			this->metadataTable->Location = System::Drawing::Point(12, 55);
			this->metadataTable->Name = L"metadataTable";
			this->metadataTable->ReadOnly = true;
			dataGridViewCellStyle3->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle3->BackColor = System::Drawing::Color::Black;
			dataGridViewCellStyle3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle3->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle3->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle3->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle3->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->RowHeadersDefaultCellStyle = dataGridViewCellStyle3;
			this->metadataTable->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->metadataTable->RowTemplate->DefaultCellStyle->BackColor = System::Drawing::Color::Black;
			this->metadataTable->RowTemplate->DefaultCellStyle->ForeColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->NullValue = L"none";
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionBackColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->Size = System::Drawing::Size(221, 391);
			this->metadataTable->TabIndex = 3;
			this->metadataTable->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::metadataTable_CellContentClick);
			// 
			// Property
			// 
			this->Property->HeaderText = L"Property";
			this->Property->Name = L"Property";
			this->Property->ReadOnly = true;
			this->Property->Resizable = System::Windows::Forms::DataGridViewTriState::False;
			this->Property->ToolTipText = L"Categories for metadata information.";
			// 
			// Value
			// 
			this->Value->HeaderText = L"Value";
			this->Value->Name = L"Value";
			this->Value->ReadOnly = true;
			this->Value->Width = 149;
			// 
			// startRecordButton
			// 
			this->startRecordButton->Location = System::Drawing::Point(376, 55);
			this->startRecordButton->Name = L"startRecordButton";
			this->startRecordButton->Size = System::Drawing::Size(75, 23);
			this->startRecordButton->TabIndex = 46;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(13, 473);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(58, 13);
			this->label5->TabIndex = 6;
			this->label5->Text = L"Status Log";
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(16, 36);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(52, 13);
			this->label6->TabIndex = 7;
			this->label6->Text = L"Metadata";
			// 
			// vidOptChangeDirBrowserDialog
			// 
			this->vidOptChangeDirBrowserDialog->Description = L"Select Video Output Directory";
			// 
			// tableLayoutPanel1
			// 
			this->tableLayoutPanel1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Right));
			this->tableLayoutPanel1->AutoSize = true;
			this->tableLayoutPanel1->AutoSizeMode = System::Windows::Forms::AutoSizeMode::GrowAndShrink;
			this->tableLayoutPanel1->ColumnCount = 1;
			this->tableLayoutPanel1->ColumnStyles->Add((gcnew System::Windows::Forms::ColumnStyle(System::Windows::Forms::SizeType::Percent, 
				50)));
			this->tableLayoutPanel1->GrowStyle = System::Windows::Forms::TableLayoutPanelGrowStyle::FixedSize;
			this->tableLayoutPanel1->Location = System::Drawing::Point(992, 586);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(0, 0);
			this->tableLayoutPanel1->TabIndex = 14;
			// 
			// mapMenuStrip
			// 
			this->mapMenuStrip->Name = L"mapMenuStrip";
			this->mapMenuStrip->Size = System::Drawing::Size(61, 4);
			// 
			// mapLookGPSToolStripMenuItem
			// 
			this->mapLookGPSToolStripMenuItem->Name = L"mapLookGPSToolStripMenuItem";
			this->mapLookGPSToolStripMenuItem->Size = System::Drawing::Size(32, 19);
			// 
			// simReadVidDialog
			// 
			this->simReadVidDialog->FileName = L"simReadVidDialog";
			// 
			// process1
			// 
			this->process1->StartInfo->Domain = L"";
			this->process1->StartInfo->LoadUserProfile = false;
			this->process1->StartInfo->Password = nullptr;
			this->process1->StartInfo->StandardErrorEncoding = nullptr;
			this->process1->StartInfo->StandardOutputEncoding = nullptr;
			this->process1->StartInfo->UserName = L"";
			this->process1->SynchronizingObject = this;
			// 
			// gpsCheckboxList
			// 
			this->gpsCheckboxList->FormattingEnabled = true;
			this->gpsCheckboxList->Location = System::Drawing::Point(1530, 55);
			this->gpsCheckboxList->Name = L"gpsCheckboxList";
			this->gpsCheckboxList->Size = System::Drawing::Size(183, 409);
			this->gpsCheckboxList->TabIndex = 38;
			// 
			// generateMapButton
			// 
			this->generateMapButton->Location = System::Drawing::Point(1145, 301);
			this->generateMapButton->Name = L"generateMapButton";
			this->generateMapButton->Size = System::Drawing::Size(245, 126);
			this->generateMapButton->TabIndex = 42;
			this->generateMapButton->Text = L"Generate Path";
			this->generateMapButton->UseVisualStyleBackColor = true;
			this->generateMapButton->Click += gcnew System::EventHandler(this, &Form1::generateMapButton_click);
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(1141, 36);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(216, 24);
			this->label17->TabIndex = 43;
			this->label17->Text = L"Search Area Boundaries";
			// 
			// button5
			// 
			this->button5->Location = System::Drawing::Point(252, 708);
			this->button5->Name = L"button5";
			this->button5->Size = System::Drawing::Size(184, 151);
			this->button5->TabIndex = 45;
			this->button5->Text = L"Target Visible";
			this->button5->UseVisualStyleBackColor = true;
			this->button5->Click += gcnew System::EventHandler(this, &Form1::targetVisibleButton_Click);
			// 
			// imageView
			// 
			this->imageView->Location = System::Drawing::Point(252, 55);
			this->imageView->Name = L"imageView";
			this->imageView->Size = System::Drawing::Size(818, 638);
			this->imageView->SizeMode = System::Windows::Forms::PictureBoxSizeMode::StretchImage;
			this->imageView->TabIndex = 47;
			this->imageView->TabStop = false;
			// 
			// mapBoundariesDataGridView
			// 
			this->mapBoundariesDataGridView->AllowUserToOrderColumns = true;
			this->mapBoundariesDataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->mapBoundariesDataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->boundary_latitude, 
				this->boundary_longitude});
			this->mapBoundariesDataGridView->Location = System::Drawing::Point(1145, 78);
			this->mapBoundariesDataGridView->Name = L"mapBoundariesDataGridView";
			this->mapBoundariesDataGridView->Size = System::Drawing::Size(245, 201);
			this->mapBoundariesDataGridView->TabIndex = 48;
			// 
			// boundary_latitude
			// 
			this->boundary_latitude->DataPropertyName = L"lat";
			this->boundary_latitude->HeaderText = L"Latitude";
			this->boundary_latitude->Name = L"boundary_latitude";
			// 
			// boundary_longitude
			// 
			this->boundary_longitude->DataPropertyName = L"lon";
			this->boundary_longitude->HeaderText = L"Longitude";
			this->boundary_longitude->Name = L"boundary_longitude";
			// 
			// pathDataGridView
			// 
			this->pathDataGridView->AllowUserToOrderColumns = true;
			this->pathDataGridView->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->pathDataGridView->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->dataGridViewTextBoxColumn2, 
				this->dataGridViewTextBoxColumn3});
			this->pathDataGridView->Location = System::Drawing::Point(1145, 492);
			this->pathDataGridView->Name = L"pathDataGridView";
			this->pathDataGridView->Size = System::Drawing::Size(245, 201);
			this->pathDataGridView->TabIndex = 49;
			// 
			// dataGridViewTextBoxColumn2
			// 
			this->dataGridViewTextBoxColumn2->HeaderText = L"Latitude";
			this->dataGridViewTextBoxColumn2->Name = L"dataGridViewTextBoxColumn2";
			// 
			// dataGridViewTextBoxColumn3
			// 
			this->dataGridViewTextBoxColumn3->HeaderText = L"Longitude";
			this->dataGridViewTextBoxColumn3->Name = L"dataGridViewTextBoxColumn3";
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(1165, 444);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(216, 24);
			this->label1->TabIndex = 50;
			this->label1->Text = L"Search Area Boundaries";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(1145, 703);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(245, 160);
			this->button1->TabIndex = 51;
			this->button1->Text = L"Send Path to VC";
			this->button1->UseVisualStyleBackColor = true;
			// 
			// previousButton
			// 
			this->previousButton->Location = System::Drawing::Point(483, 708);
			this->previousButton->Name = L"previousButton";
			this->previousButton->Size = System::Drawing::Size(143, 151);
			this->previousButton->TabIndex = 52;
			this->previousButton->Text = L"Previous";
			this->previousButton->UseVisualStyleBackColor = true;
			this->previousButton->Click += gcnew System::EventHandler(this, &Form1::previousButton_Click);
			// 
			// playPauseButton
			// 
			this->playPauseButton->Location = System::Drawing::Point(645, 708);
			this->playPauseButton->Name = L"playPauseButton";
			this->playPauseButton->Size = System::Drawing::Size(129, 71);
			this->playPauseButton->TabIndex = 53;
			this->playPauseButton->Text = L"Pause";
			this->playPauseButton->UseVisualStyleBackColor = true;
			this->playPauseButton->Click += gcnew System::EventHandler(this, &Form1::playPauseButton_Click);
			// 
			// stopButton
			// 
			this->stopButton->Location = System::Drawing::Point(645, 788);
			this->stopButton->Name = L"stopButton";
			this->stopButton->Size = System::Drawing::Size(129, 71);
			this->stopButton->TabIndex = 54;
			this->stopButton->Text = L"Stop";
			this->stopButton->UseVisualStyleBackColor = true;
			this->stopButton->Click += gcnew System::EventHandler(this, &Form1::stopButton_Click);
			// 
			// nextButton
			// 
			this->nextButton->Location = System::Drawing::Point(780, 708);
			this->nextButton->Name = L"nextButton";
			this->nextButton->Size = System::Drawing::Size(145, 151);
			this->nextButton->TabIndex = 55;
			this->nextButton->Text = L"Next";
			this->nextButton->UseVisualStyleBackColor = true;
			this->nextButton->Click += gcnew System::EventHandler(this, &Form1::nextButton_Click);
			// 
			// currentFrameDescLabel
			// 
			this->currentFrameDescLabel->AutoSize = true;
			this->currentFrameDescLabel->Location = System::Drawing::Point(931, 708);
			this->currentFrameDescLabel->Name = L"currentFrameDescLabel";
			this->currentFrameDescLabel->Size = System::Drawing::Size(76, 13);
			this->currentFrameDescLabel->TabIndex = 56;
			this->currentFrameDescLabel->Text = L"Current Frame:";
			// 
			// frameTextBox
			// 
			this->frameTextBox->Location = System::Drawing::Point(956, 802);
			this->frameTextBox->Name = L"frameTextBox";
			this->frameTextBox->Size = System::Drawing::Size(100, 20);
			this->frameTextBox->TabIndex = 57;
			this->frameTextBox->Text = L"0";
			this->frameTextBox->TextChanged += gcnew System::EventHandler(this, &Form1::textBox1_TextChanged);
			// 
			// goToFrameButton
			// 
			this->goToFrameButton->Location = System::Drawing::Point(956, 828);
			this->goToFrameButton->Name = L"goToFrameButton";
			this->goToFrameButton->Size = System::Drawing::Size(100, 31);
			this->goToFrameButton->TabIndex = 58;
			this->goToFrameButton->Text = L"Go To Frame";
			this->goToFrameButton->UseVisualStyleBackColor = true;
			this->goToFrameButton->Click += gcnew System::EventHandler(this, &Form1::goToFrameButton_Click);
			// 
			// currentFrameLabel
			// 
			this->currentFrameLabel->AutoSize = true;
			this->currentFrameLabel->Location = System::Drawing::Point(989, 737);
			this->currentFrameLabel->Name = L"currentFrameLabel";
			this->currentFrameLabel->Size = System::Drawing::Size(12, 13);
			this->currentFrameLabel->TabIndex = 59;
			this->currentFrameLabel->Text = L"/";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			this->BackColor = System::Drawing::Color::DimGray;
			this->ClientSize = System::Drawing::Size(1398, 881);
			this->Controls->Add(this->currentFrameLabel);
			this->Controls->Add(this->goToFrameButton);
			this->Controls->Add(this->frameTextBox);
			this->Controls->Add(this->currentFrameDescLabel);
			this->Controls->Add(this->nextButton);
			this->Controls->Add(this->stopButton);
			this->Controls->Add(this->playPauseButton);
			this->Controls->Add(this->previousButton);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->pathDataGridView);
			this->Controls->Add(this->mapBoundariesDataGridView);
			this->Controls->Add(this->imageView);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->button5);
			this->Controls->Add(this->generateMapButton);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->metadataTable);
			this->Controls->Add(this->startRecordButton);
			this->Controls->Add(this->errorLogTextBox);
			this->Controls->Add(this->menuStrip1);
			this->Location = System::Drawing::Point(250, 0);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"UCSD Skynet";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->Click += gcnew System::EventHandler(this, &Form1::sendWaypointsButton_Click);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->imageView))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->mapBoundariesDataGridView))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathDataGridView))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
			 {
			 }

private: System::Void metadataTable_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void serialCommunicationsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Application::Exit();
		 }

	/// <summary>
	/// Outputs a message to the console in the specified color.
	/// Prefixes the message with the current date and time, postfixes with a newline
	/// </summary>
private: System::Void consoleMessage( String ^ message, Color col )
		 {
			 DateTime time = DateTime::Now;
			 errorLogTextBox->SelectionColor = Color::White;
			 errorLogTextBox->AppendText( time.ToString() + "\n" );
			 errorLogTextBox->SelectionColor = col;
			 errorLogTextBox->AppendText( message );
			 errorLogTextBox->AppendText( "\n" );

			 // Always scroll to the bottom of the text field.
			 errorLogTextBox->Select(errorLogTextBox->Text->Length, 0);
			 errorLogTextBox->ScrollToCaret();

		 	 return;
		 }
public: System::Void setWayPointsText(String ^ text)
		{
			/**
			 TODO: Re-Implement
			 **/
			/*
			if (!pathWaypointsTextBox->InvokeRequired){
				pathWaypointsTextBox->Text=text;
			} else {
				this->Invoke(gcnew Delegates::stringToVoid(this,&Form1::setWayPointsText), (Object^)text);
			}
			*/
		}
public:  System::Void doNothing() {
			 System::Diagnostics::Trace::WriteLine("Doing nothing");
		 }

public:	 System::Void printToConsole( String ^ message, Color ^ color)
		 {
			 array<Object ^> ^ retArr = gcnew array< Object^ >{message, color};
	
			 printToConsole( retArr );
		 }
public:  System::Void printToConsole( array<Object ^> ^ retArr ) 
		 {
			consoleMessage( (String ^)retArr[0], ((Color)retArr[1]));//((ColorRef ^)retArr[1])->theColor );
		 }


public: System::Void displayPlaneData(ImageWithPlaneData^ imageWithPlaneData){
	try{
		this->imageView->Image = gcnew Bitmap(imageWithPlaneData->imageFilename);
		updateTable(imageWithPlaneData);
	} catch (System::ArgumentException^){
		// Silently fail
	}
}

public: System::Void updateAutosearchImage(Image^ image){
	if (!mapView){
		mapView = gcnew MapView();
		mapView->Show();
	}
	mapView->autosearchImage->Image = image;
}

public: System::Void printGreenMessage( String ^ message )
		{
			Delegates::stringToVoid ^ blahdelegate = gcnew Delegates::stringToVoid(this, &Form1::printGreenMessageMainThread );

			try {
				this->Invoke( blahdelegate, gcnew array<Object ^>{message} );
			}
			catch(Exception ^ e) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::printGreenMessage(): Failed to print error message: " + e);
			}


		}

private: System::Void printGreenMessageMainThread( String ^ message )
		 {
			 consoleMessage( message, Color::Green );

		 }
public:  System::Void errorMessageUniversal( String ^ message )
		 {
			Delegates::stringToVoid ^ blahdelegate = gcnew Delegates::stringToVoid(this, &Form1::errorMessage );

			try {
				this->Invoke( blahdelegate, gcnew array<Object ^>{message} );
			}
			catch(Exception ^ e) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::errorMessageUniversal(): Failed to print error message: " + e);
			}

		 }
private: System::Void errorMessage( String ^ message )
		 {
			 consoleMessage( message, Color::Red );
		 }



private: bool isReadingData;

public: System::Void setUnverifiedTableContents( array<Database::UnverifiedRowData ^> ^ contents )
		{
			targetsForm->setUnverifiedTableContents(contents);
		}

public: System::Void setVerifiedTableContents( array<Database::VerifiedRowData ^> ^ contents )
		{
			targetsForm->setVerifiedTableContents(contents);
		}

public: System::Void setCandidateTableContents( array<Database::CandidateRowData ^> ^ contents )
		{
			targetsForm->setCandidateTableContents(contents);
		}

private: bool findIntInArray( int num, array<int> ^ arr)
		 {
			 for each(int i in arr)
				 if (num == i)
					 return true;
			 return false;
		 }

public: System::Void insertCandidateData( Database::CandidateRowData ^ data) 
		{
			targetsForm->insertCandidate(data);
		}
public: System::Void insertUnverifiedData( Database::UnverifiedRowData ^ data) 
		{
			targetsForm->insertUnverified(data);
		}

public: System::Void insertVerifiedTargetData( Database::VerifiedRowData ^ data) 
		{
			targetsForm->insertVerified(data);
		}


public:		System::Void removeCandidate(Database::CandidateRowData^ data){};
public:		System::Void removeUnverified(Database::UnverifiedRowData^ data){};
public:		System::Void removeVerified(Database::VerifiedRowData^ data){};

private: void AddText( Stream^ fs, String^ value )
{
   array<Byte>^info = (gcnew UTF8Encoding( true ))->GetBytes( value );
   fs->Write( info, 0, info->Length );
}

public: System::Void updateTable(ImageWithPlaneData^ data) 
		{
			this->metadataTable[1, G_ROLL]->Value = "" + data->gimbalRoll + "*";
			this->metadataTable[1, G_PITCH]->Value = "" + data->gimbalPitch + "*"; //elevation;
			this->metadataTable[1, A_ALT]->Value = "" + data->altitude + " (m)"; // altitude;
			this->metadataTable[1, A_LAT]->Value = "" + data->latitude + "*"; // latitude;
			this->metadataTable[1, A_LON]->Value = "" + data->longitude + "*"; // longitude; // ######.####
			this->metadataTable[1, A_HEAD]->Value = "" + data->yaw + "*"; //heading;
			this->metadataTable[1, A_ROLL]->Value = "" + data->roll + "*"; // roll;
			this->metadataTable[1, A_PITCH]->Value = "" + data->pitch + "*"; //pitch;
		}



private: System::Void resetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (MessageBox::Show("Are you sure you want to reset the database?", "Really?", 
				 MessageBoxButtons::YesNo,MessageBoxIcon::Question) == System::Windows::Forms::DialogResult::Yes){
					 appController->clearAllTables();
					 targetsForm->removeAllTargets();
			 }
		 }

private: System::Void exportDataToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				FolderBrowserDialog^ fileDialog = gcnew FolderBrowserDialog;

				if ( fileDialog->ShowDialog() == ::DialogResult::OK )
				{
					appController->exportData(fileDialog->SelectedPath + "\\");
				}

			}

private: System::Void autosearchBox_LoadCompleted(System::Object^  sender, System::ComponentModel::AsyncCompletedEventArgs^  e) {
			PRINT("AutosearchBox Load Completed");
		 }
private: System::Void sendWaypointsButton_Click(System::Object^  sender, System::EventArgs^  e) {
			PRINT("Waypoints button is clicked");
			PRINT("GPS Coordinates to send: ");//
		 }

private: System::Void generateMapButton_click(System::Object^  sender, System::EventArgs^  e) {
	
	// TODO: This should map GPSCoord objects onto rows of this datagridView
	System::Collections::Generic::List<GPSCoord^> ^list = gcnew System::Collections::Generic::List<GPSCoord^>();

	for(int i = 0; i < mapBoundariesDataGridView->Rows->Count; i++){
		DataGridViewRow^ row = mapBoundariesDataGridView->Rows[i];
		String^ lat = (String^)row->Cells[0]->Value;
		String^ lon = (String^)row->Cells[1]->Value;
		if (String::IsNullOrWhiteSpace(lat) || String::IsNullOrWhiteSpace(lon)) continue;
		try {
			double latVal = Double::Parse(lat);
			double lonVal = Double::Parse(lon);
			list->Add(gcnew GPSCoord(latVal, lonVal));
		} catch ( FormatException^ ){}
	}

	appController->startIntelligenceController(list->ToArray());
}

private: System::Void loadWaypointsFromFileButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 Stream ^ boundariesFile;
			 OpenFileDialog^ fileDialog = gcnew OpenFileDialog;
			 fileDialog->InitialDirectory = "D:\\Skynet Files\\";
			 fileDialog->Filter = "txt files (*.txt)|*.txt|All files (*.*)|*.*";
			 fileDialog->FilterIndex = 2;
			 fileDialog->RestoreDirectory = true;
			 if ( fileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 if ((boundariesFile = fileDialog->OpenFile()) != nullptr )
				 {
					 StreamReader ^ fileReader = gcnew StreamReader(boundariesFile);
					 String^ boundaries = fileReader->ReadToEnd();
					 // TODO: Re-Implement
					 //mapBoundariesTextBox->Text = boundaries;
				 }
			 }
		 }
public: System::Void setPath(array<GPSCoord^>^ gpsCoords) {
	if (!pathDataGridView->InvokeRequired){
		pathDataGridView->Rows->Clear();
		for each(GPSCoord^ coord in gpsCoords){
			int index = pathDataGridView->Rows->Add();
			pathDataGridView->Rows[index]->Cells[0]->Value = coord->lat;
			pathDataGridView->Rows[index]->Cells[1]->Value = coord->lon;
		}
	} else {
		this->Invoke(gcnew Delegates::gpsCoordArrayToVoid(this,&Form1::setPath), (Object^)gpsCoords);
	}
}

public: System::Void displayPathfinderImage(String^ pathfinderImageFilename){
	if (!mapView){
		mapView = gcnew MapView();
		mapView->Show();
	}
	mapView->pathfinderImage->Image = gcnew Bitmap(pathfinderImageFilename);
}

private: System::Void targetVisibleButton_Click(System::Object^  sender, System::EventArgs^  e) {
			appController->saveCurrentFrameAsUnverified();		 
		 }
private: System::Void lockPlaneCoordinatesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			(gcnew GPSLocationForm(appController))->Show();
		 }
private: System::Void unlockPlaneCoordinatesToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->unlockPosition();
		 }
private: System::Void startMissionToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 FolderBrowserDialog^ fileDialog = gcnew FolderBrowserDialog();
			 if ( fileDialog->ShowDialog() == System::Windows::Forms::DialogResult::OK)
			 {
				 appController->startMission(fileDialog->SelectedPath);
			 }
		 }
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void previousButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->previousFrame();
		 }
private: System::Void playPauseButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->playOrPause();
		 }
private: System::Void stopButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->stopPlaying();
		 }
private: System::Void nextButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->nextFrame();
		 }
private: System::Void goToFrameButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 int frame = Int32::Parse(frameTextBox->Lines[0]);
			 appController->gotoFrame(frame);
		 }
public: void updateCurrentFrameString(String^ frameString){
			if (!currentFrameLabel->InvokeRequired){
				currentFrameLabel->Text = frameString;
			} else {
				this->Invoke(gcnew Delegates::stringToVoid(this,&Form1::updateCurrentFrameString), (Object^)frameString);
			}
		}

public: void setPlayPauseButtonTextToPaused(){
			if (!playPauseButton->InvokeRequired){
				playPauseButton->Text = "Play";
			} else {
				this->Invoke(gcnew Delegates::voidToVoid(this,&Form1::setPlayPauseButtonTextToPaused));
			}
		}

public: void setPlayPauseButtonTextToPlaying(){
			if (!playPauseButton->InvokeRequired){
				playPauseButton->Text = "Pause";
			} else {
				this->Invoke(gcnew Delegates::voidToVoid(this,&Form1::setPlayPauseButtonTextToPlaying));
			}
		}
};
}
