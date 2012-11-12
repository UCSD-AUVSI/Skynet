#pragma once

/*

INSTRUCTIONS for making OpenCV work. This fixes the error:
1>  Generating Code...
1>c:\opencv\modules\core\include\opencv2\core\mat.hpp(2135): fatal error C1001: An internal error has occurred in the compiler.
1>  (compiler file 'f:\dd\vctools\compiler\utc\src\p2\wvm\mdmiscw.c', line 2704)
1>   To work around this problem, try simplifying or changing the program near the locations listed above.
1>  Please choose the Technical Support command on the Visual C++ 
1>   Help menu, or open the Technical Support help file for more information
1>
1>Build FAILED.

 Add the following to the beginning of include/opencv2/core/mat.hpp + core.hpp
 // and maybe include/opencv/cxcore.h + cv.h + cvaux.h + include/opencv2/opencv.hpp


#pragma managed (push, off) 

Add the following to the end of those files

#pragma managed (pop) 

*/

#include "MasterHeader.h"

#include "OpenGLForm.h"
#include "Comport.h"
#include "Joystick.h"
//#include "Auvsi_Saliency.h"
#include "Database.h"
#include "Delegates.h"
// #include "MapControl.h" DELETE MAP

#ifndef OPENCV_DISABLED
#include "GeoReference.h"
#endif

#include "TargetDialog.h"
#include "HUDControl.h"
#include "Saliency.h"
#include "OCR.h"
//#include "VideoSimulator.h"
#include "SimHandler.h"
#include "Comms.h"
#include "PlaneWatcher.h"
#include "AutopilotComport.h"
#include "SkynetController.h"
#include "TargetLock.h"
#include "VisionController.h"
#include "..\ImageDownloader\ImageDownloader\ImageReceiver.h"

#include <math.h>
#include "Tester.h"

#define GREEN_IMAGE_PATH "C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\Skynet\\greenled.png"
#define RED_IMAGE_PATH "C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\Skynet\\redled.png"
#define YELLOW_IMAGE_PATH "C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\Skynet\\yellowled.png"

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

	//[DllImport("AUVSI_SALIENCY", CharSet=CharSet::Ansi)]
	//extern "C" void Auvsi_Saliency();	

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
//		Delegates::rowDataToVoid ^ saliencyAddTarget;
		Delegates::verifiedRowDataToVoid ^ verifiedTableUpdateDelegate;
		Delegates::unverifiedRowDataToVoid ^ unverifiedTableUpdateDelegate;
		Delegates::waypointArraytoVoid ^ checkboxListDelegate;
		Delegates::candidateRowDataToVoid ^ candidateTableUpdateDelegate;
		Delegates::verifiedRowDataArrayToVoid ^ verifiedTableContentsDelegate;
		Delegates::unverifiedRowDataArrayToVoid ^ unverifiedTableContentsDelegate;
		Delegates::candidateRowDataArrayToVoid ^ candidateTableContentsDelegate;
		Delegates::dataGridViewRowToVoid ^ ocrDelegate;
		Delegates::voidToVoid ^ saveImageDelegate;
	protected:
		Vision::VisionController ^		visionController;
		OpenGLForm::COpenGL ^			openGLView;  	// video viewport
		SkynetController ^				appController;
		Communications::TargetLock ^	targetLock;
		Communications::Comms ^			theComms;
		Joystick  ^						m_joystick; 
		Database::DatabaseConnection ^	db;
		TargetDialog ^					imageDialog;
		Simulator::VideoSimulator ^		theVideoSimulator;
		Simulator::SimHandler ^			theSimHandler;
		Communications::PlaneWatcher ^	thePlaneWatcher;


		String ^ fileExtension;
		String ^ defaultMapCache;

		bool isconnected;
		bool isConnecting;
		//OpenGLForm::COpenGL ^ openGLView2; // airplane virtual cockpit viewport
		bool vidOptFolderDialogOpen;		// For video options directory

		Thread ^ comReadThread;
		int imageNumber; // for saving images

		// Temporary logging stuff
		FileStream ^ logFile;

		// Video saving stuff
		int splitLength; // in seconds
		bool recording;
		DateTime recordStart;
		//String ^ fileExtensionVideo;

		//Database ID
		int incrId;

		//Map point
		Point mouseGPS;

		// Image details


		Image ^redImage;
		Image ^yellowImage;
		Image ^greenImage;

	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage1;
	private: System::Windows::Forms::TabPage^  tabPage2;


	private: System::Windows::Forms::Button^  startRecordButton;

	private: System::Windows::Forms::Button^  vidOptChangeDirButton;

	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  encodingComboBox;

	private: System::Windows::Forms::TextBox^  splitLengthTextBox;

	private: System::Windows::Forms::TextBox^  vidOptOutputDirText;

	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::CheckedListBox^  checkedListBox1;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::Label^  label6;

	private: System::Windows::Forms::FolderBrowserDialog^  vidOptChangeDirBrowserDialog;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Property;
	private: System::Windows::Forms::DataGridViewTextBoxColumn^  Value;
	private: System::Windows::Forms::Timer^  joystickTimer;

	private: System::Windows::Forms::TabControl^  metadataTabControl;
	private: System::Windows::Forms::TabPage^  tabPage3;
	private: System::Windows::Forms::DataGridView^  dataGridView1;
	private: System::Windows::Forms::TabPage^  tabPage4;
	private: System::Windows::Forms::TabPage^  tabPage5;
	private: System::Windows::Forms::DataGridView^  dataGridView2;
	private: System::Windows::Forms::DataGridView^  dataGridView3;

	private: System::Windows::Forms::CheckBox^  checkBox1;
	private: System::Windows::Forms::Timer^  videoSaveTimer;
	// private: MapControl ^ mapView; DELETE MAP

	private: System::Windows::Forms::ToolStripMenuItem^  mapToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  providerToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  mapnikToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  osmarenderToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  cyclemapToolStripMenuItem;



	private: System::Windows::Forms::TableLayoutPanel^  tableLayoutPanel1;


private: System::Windows::Forms::ToolStripMenuItem^  exportDataToolStripMenuItem;
private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
private: System::Windows::Forms::ToolStripMenuItem^  databaseToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  resetToolStripMenuItem;
private: System::Windows::Forms::Timer^  mapUpdateTimer;
private: System::Windows::Forms::ToolStripMenuItem^  lockToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  downloadToolStripMenuItem;
private: System::Windows::Forms::ContextMenuStrip^  mapMenuStrip;
private: System::Windows::Forms::ToolStripMenuItem^  mapLookGPSToolStripMenuItem;



private: System::Windows::Forms::ToolStripMenuItem^  simulatorToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  choosePathToolStripMenuItem;

private: System::Windows::Forms::ToolStripMenuItem^  pauseToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  stopToolStripMenuItem;
private: System::Windows::Forms::OpenFileDialog^  simReadVidDialog;
private: System::Windows::Forms::Button^  button1;
private: System::Windows::Forms::Button^  button2;
private: System::Windows::Forms::Button^  button3;
private: System::Windows::Forms::Button^  button4;
private: System::Windows::Forms::Label^  label8;
private: System::Windows::Forms::Label^  label11;
private: System::Windows::Forms::Label^  label12;
private: System::Windows::Forms::PictureBox^  pictureBox1;
private: System::Windows::Forms::PictureBox^  pictureBox2;
private: System::Windows::Forms::PictureBox^  pictureBox3;
private: System::Windows::Forms::Label^  label9;
private: System::Windows::Forms::Label^  label10;
private: System::Windows::Forms::Label^  label13;
private: System::Windows::Forms::Label^  label14;
private: System::Windows::Forms::Label^  label15;
private: System::Windows::Forms::Label^  label16;
private: System::Windows::Forms::Panel^  panel1;
private: System::Diagnostics::Process^  process1;
private: System::Windows::Forms::Panel^  gimbalHUDPanel;
private: HUDControl^ gimbalHUDView;


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


private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_IDColumn;
private: System::Windows::Forms::DataGridViewImageColumn^  confirmed_ThumbnailColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LatitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LongitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_HeadingColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_Shape;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_ShapeColor;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LetterColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  confirmed_LetterColor;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  targetid_column;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  verified_lockon;
public: System::Windows::Forms::PictureBox^  autosearchBox;
public: System::Windows::Forms::PictureBox^  pathfinderBox;
private: System::Windows::Forms::PictureBox^  Border;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_IDColumn;
private: System::Windows::Forms::DataGridViewImageColumn^  candidate_ThumbnailColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  candidate_LatitudeColumn;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  canidate_LongitudeColumn;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  candidate_VerifiedColumn;
private: System::Windows::Forms::DataGridViewCheckBoxColumn^  processed;
private: System::Windows::Forms::CheckedListBox^  gpsCheckboxList;
private: System::Windows::Forms::Label^  waypointsLabel;
private: System::Windows::Forms::Button^  sendWaypointsButton;
private: System::Windows::Forms::ToolStripMenuItem^  wifiToolStripMenuItem;
private: System::Windows::Forms::ToolStripMenuItem^  stopToolStripMenuItem1;
private: System::Windows::Forms::Button^  generateMapButton;

private: System::Windows::Forms::TextBox^  mapBoundariesTextBox;

private: System::Windows::Forms::Label^  label17;
private: System::Windows::Forms::Button^  loadBoundariesFromFileButton;









public: 

public: 

private: System::Windows::Forms::Label^  label7;
			 

	public:
		Form1(void)
		{
			InitializeComponent();

			Tester::runTests();


			// Debug 
			
			//GeoReference::computeHomography( 0, 0, 400, 0, Math::PI/4, 0, 0, -1.5708, 1 );

			appController = gcnew SkynetController(this);

			// Set up Map
			/*this->mapView = gcnew MapControl();
			this->mapView->Location = this->mapPanel->Location;
			this->mapView->Name = L"mapView";
			this->mapView->Size = this->mapPanel->Size;			
			this->mapView->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::mapView_MouseMove);
			this->mapView->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &Form1::mapView_MouseDown);*/

			// Set up HUD
			this->gimbalHUDView = gcnew HUDControl();
			this->gimbalHUDView->Location = this->gimbalHUDPanel->Location;
			this->gimbalHUDView->Name = L"gimbalHUDView";
			this->gimbalHUDView->Size = this->gimbalHUDPanel->Size;
			this->gimbalHUDView->Refresh();
			this->gimbalHUDView->BackColor = System::Drawing::Color::Transparent;
			
			//this->mapView->ContextMenuStrip = mapMenuStrip;
			
			// this->Controls->Add(this->mapView); DELETE MAP
			this->Controls->Add(this->gimbalHUDView);

			//hide map
			//this->mapPanel->Location = System::Drawing::Point(0, 0);
			//this->mapPanel->Name = L"mapPanel";
			//this->mapPanel->Size = System::Drawing::Size(0, 0);

			//hide HUD
			this->gimbalHUDPanel->Location = System::Drawing::Point(0, 0);
			this->gimbalHUDPanel->Name = L"gimbalHUDPanel";
			this->gimbalHUDPanel->Size = System::Drawing::Size(0, 0);

			this->ResumeLayout(false);
			this->PerformLayout();

			// Set default map provider
			/*defaultMapCache = "D:\\Skynet Files\\Map Cache";			
			mapView->SetCacheDirectory( defaultMapCache + "\\Mapnik", false );
			mapView->SetTileProvider( Mapnik ); DELETE MAP */
			mapnikToolStripMenuItem->Checked = true;
			osmarenderToolStripMenuItem->Checked = false;
			cyclemapToolStripMenuItem->Checked = false;
			// end set up Map
			
			thePlaneWatcher = gcnew Communications::PlaneWatcher(this);
			

			// Set up openGL						
			// old todo: in order to have two openGL windows, you really only get one OpenGL scene and give each one a different viewport
			wglMakeCurrent( NULL, NULL ); // this causees a delay which allows wglCreateContext to work properly
			//openGLView2 = gcnew OpenGLForm::COpenGL( this->openGLPanel2, this->openGLPanel2->Width, this->openGLPanel2->Height );
			openGLView = gcnew OpenGLForm::COpenGL( this->openGLPanel, this->openGLPanel->Width, this->openGLPanel->Height, this, thePlaneWatcher );
			appController->setPlaneWatcher( thePlaneWatcher );
			appController->setCameraView( openGLView );

			
			//set up database
			db = gcnew Database::DatabaseConnection();
			bool connec = db->connect();
			incrId = 0;
			appController->setDatabase( db );

			this->visionController = gcnew Vision::VisionController(openGLView, appController);


			PRINT("Starting ImageReceiver");
			if ( thePlaneWatcher == nullptr) {
				PRINT("WTF");
			}
			ImageReceiver ^ receiver = gcnew ImageReceiver("C:\\Users\\ucsd_auvsi\\Dropbox\\Skynet\\flight_images",visionController, thePlaneWatcher, true);
			
			theVideoSimulator = gcnew Simulator::VideoSimulator( this->visionController );

			// Comport Stuff
			theSimHandler = gcnew Simulator::SimHandler(theVideoSimulator, openGLView, this->visionController);

			//set up Joystick
			m_joystick = gcnew Joystick( this );
			m_joystick->init( NULL );
			m_joystick->theWatcher = thePlaneWatcher;
			m_joystick->setDelegate( appController );


			// Video options
			vidOptFolderDialogOpen = false;
			recording = false;
			//fileExtensionVideo = ".avi";

			// Image options
			imageNumber = 0;
			fileExtension = ".jpg";

			// Logging			
			DateTime timeTemp = DateTime::Now;
			logFile = File::Create( "D:\\Skynet Files\\Flight Logs\\" + timeTemp.ToString("o")->Replace(":", "-") + ".txt" );

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
		//	this->metadataTable->Rows->Add("    Format");
		//	this->metadataTable[1, V_FORMAT]->Value = "NTSC";

			// Delegates
			comportErrorDelegate = gcnew Delegates::voidToVoid( this, &Form1::noComportData );
			comportEstablishDelegate = gcnew Delegates::voidToVoid( this, &Form1::comportEstablish );
			saveImageDelegate = gcnew Delegates::voidToVoid( this, &Form1::saveImage );
			imageDialogDelegate = gcnew Delegates::twointThreedoubleToVoid( this, &Form1::imageDialogCallback );


			// Delegates::rowDataToVoid ^ saliencyAddTarget = gcnew Delegates::rowDataToVoid( this, &Form1::addRawTarget );
			// saliencyAddTarget = gcnew Delegates::rowDataToVoid( this, &Form1::addRawTarget );	
			// Delegates::rowDataToVoid == delegate void rowDataToVoid( Database::RowData ^ data );
			// delegate void rowDataToVoid( Database::RowData ^ data );


			ocrDelegate = gcnew Delegates::dataGridViewRowToVoid( this, &Form1::ocrUpdateData );

			theComms = gcnew Communications::Comms(theSimHandler->theTelSimulator, this);
			isconnected = false;
			isConnecting = false;

			m_joystick->comm = theComms;
			theSimHandler->theComms = theComms;
			((TelemetrySimulator ^)theSimHandler->theTelSimulator)->theAutopilotComport = theComms->autopilot;
			appController->setSimHandler(theSimHandler);
			redImage = Image::FromFile( RED_IMAGE_PATH );
			yellowImage = Image::FromFile( YELLOW_IMAGE_PATH );
			greenImage = Image::FromFile( GREEN_IMAGE_PATH );

			pictureBox1->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox2->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox3->SizeMode = PictureBoxSizeMode::StretchImage;
			pictureBox1->Image = dynamic_cast<Image^>(redImage);
			pictureBox2->Image = dynamic_cast<Image^>(redImage);
			pictureBox3->Image = dynamic_cast<Image^>(redImage);

			
			//theComms->connectAll();
			//theComms->gotoLatLon(534.0f, 2878.0f);

			consoleMessage("... Skynet online", Color::Orange);
			
			gimbalHUDView->BringToFront();

			appController->loadAllTablesFromDisk();
			targetLock = gcnew Communications::TargetLock( theComms, thePlaneWatcher );
			appController->setTargetLock(targetLock);





			appController->comeAlive();
			ParameterizedThreadStart^ threadDelegate = gcnew ParameterizedThreadStart(appController, &SkynetController::updateVerifiedTableFromDatabaseAtInterval);
			Thread^ verifiedTableUpdaterThread = gcnew Thread (threadDelegate);
			verifiedTableUpdaterThread->Name = "Verified Table Updater Thread";
			verifiedTableUpdaterThread->Start(5000);

			StreamReader^ gpsListStream = gcnew StreamReader("D:\\Skynet Files\\Field Boundaries.txt");
			String ^ gpsCoords = gpsListStream->ReadToEnd();
			mapBoundariesTextBox->Text = gpsCoords;
		}

	public:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1()
		{
			if ( logFile )
            	delete (IDisposable^)logFile;

			if (theComms)
			{
				//if( comReadThread )
					//comReadThread->Abort();
				//delete theComms;
				System::Diagnostics::Trace::WriteLine("Comport Deleted via Garbage Collecting");
			}
			if( m_joystick )
			{
				delete m_joystick;
				System::Diagnostics::Trace::WriteLine("Joystick Deleted");
			}

			if (components)
			{
				delete components;
				System::Diagnostics::Trace::WriteLine("Components Deleted");
			}

		}
	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  toolStripMenuItem1;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;
	private: System::Windows::Forms::Timer^  openGLTimer;
	private: System::Windows::Forms::Panel^  openGLPanel;
	private: System::Windows::Forms::RichTextBox^  errorLogTextBox;
	public: System::Windows::Forms::DataGridView^  metadataTable;


	private: System::Windows::Forms::ToolStripMenuItem^  serialCommunicationsToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  connectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;

	
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  disconnectToolStripMenuItem;
	private: System::Windows::Forms::ToolStripComboBox^  comPortStripComboBox;
	private: System::Windows::Forms::ToolStripComboBox^  baudRateStripComboBox;

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
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle5 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->toolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->exportDataToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->serialCommunicationsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->connectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->comPortStripComboBox = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->baudRateStripComboBox = (gcnew System::Windows::Forms::ToolStripComboBox());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->disconnectToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mapToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->providerToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->mapnikToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->osmarenderToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->cyclemapToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->lockToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->downloadToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->databaseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->resetToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simulatorToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->choosePathToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->pauseToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stopToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->wifiToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stopToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openGLTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->openGLPanel = (gcnew System::Windows::Forms::Panel());
			this->gimbalHUDPanel = (gcnew System::Windows::Forms::Panel());
			this->errorLogTextBox = (gcnew System::Windows::Forms::RichTextBox());
			this->metadataTable = (gcnew System::Windows::Forms::DataGridView());
			this->Property = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->Value = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage1 = (gcnew System::Windows::Forms::TabPage());
			this->checkBox1 = (gcnew System::Windows::Forms::CheckBox());
			this->tabPage2 = (gcnew System::Windows::Forms::TabPage());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->checkedListBox1 = (gcnew System::Windows::Forms::CheckedListBox());
			this->vidOptChangeDirButton = (gcnew System::Windows::Forms::Button());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->encodingComboBox = (gcnew System::Windows::Forms::ComboBox());
			this->splitLengthTextBox = (gcnew System::Windows::Forms::TextBox());
			this->vidOptOutputDirText = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->startRecordButton = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->vidOptChangeDirBrowserDialog = (gcnew System::Windows::Forms::FolderBrowserDialog());
			this->joystickTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->metadataTabControl = (gcnew System::Windows::Forms::TabControl());
			this->tabPage3 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView1 = (gcnew System::Windows::Forms::DataGridView());
			this->candidate_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->candidate_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->canidate_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->candidate_VerifiedColumn = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->processed = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->tabPage4 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView2 = (gcnew System::Windows::Forms::DataGridView());
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
			this->tabPage5 = (gcnew System::Windows::Forms::TabPage());
			this->dataGridView3 = (gcnew System::Windows::Forms::DataGridView());
			this->confirmed_IDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_ThumbnailColumn = (gcnew System::Windows::Forms::DataGridViewImageColumn());
			this->confirmed_LatitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LongitudeColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_HeadingColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_Shape = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_ShapeColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LetterColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->confirmed_LetterColor = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->targetid_column = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->verified_lockon = (gcnew System::Windows::Forms::DataGridViewCheckBoxColumn());
			this->videoSaveTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->tableLayoutPanel1 = (gcnew System::Windows::Forms::TableLayoutPanel());
			this->mapUpdateTimer = (gcnew System::Windows::Forms::Timer(this->components));
			this->mapMenuStrip = (gcnew System::Windows::Forms::ContextMenuStrip(this->components));
			this->mapLookGPSToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->simReadVidDialog = (gcnew System::Windows::Forms::OpenFileDialog());
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->label8 = (gcnew System::Windows::Forms::Label());
			this->label11 = (gcnew System::Windows::Forms::Label());
			this->label12 = (gcnew System::Windows::Forms::Label());
			this->pictureBox1 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox2 = (gcnew System::Windows::Forms::PictureBox());
			this->pictureBox3 = (gcnew System::Windows::Forms::PictureBox());
			this->label9 = (gcnew System::Windows::Forms::Label());
			this->label10 = (gcnew System::Windows::Forms::Label());
			this->label13 = (gcnew System::Windows::Forms::Label());
			this->label14 = (gcnew System::Windows::Forms::Label());
			this->label15 = (gcnew System::Windows::Forms::Label());
			this->label16 = (gcnew System::Windows::Forms::Label());
			this->panel1 = (gcnew System::Windows::Forms::Panel());
			this->process1 = (gcnew System::Diagnostics::Process());
			this->autosearchBox = (gcnew System::Windows::Forms::PictureBox());
			this->Border = (gcnew System::Windows::Forms::PictureBox());
			this->pathfinderBox = (gcnew System::Windows::Forms::PictureBox());
			this->gpsCheckboxList = (gcnew System::Windows::Forms::CheckedListBox());
			this->waypointsLabel = (gcnew System::Windows::Forms::Label());
			this->sendWaypointsButton = (gcnew System::Windows::Forms::Button());
			this->mapBoundariesTextBox = (gcnew System::Windows::Forms::TextBox());
			this->generateMapButton = (gcnew System::Windows::Forms::Button());
			this->label17 = (gcnew System::Windows::Forms::Label());
			this->loadBoundariesFromFileButton = (gcnew System::Windows::Forms::Button());
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->BeginInit();
			this->tabControl1->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->tabPage2->SuspendLayout();
			this->metadataTabControl->SuspendLayout();
			this->tabPage3->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->BeginInit();
			this->tabPage4->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView2))->BeginInit();
			this->tabPage5->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView3))->BeginInit();
			this->mapMenuStrip->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->BeginInit();
			this->panel1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->autosearchBox))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Border))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathfinderBox))->BeginInit();
			this->SuspendLayout();
			// 
			// menuStrip1
			// 
			this->menuStrip1->BackColor = System::Drawing::Color::Gray;
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->toolStripMenuItem1, 
				this->serialCommunicationsToolStripMenuItem, this->mapToolStripMenuItem, this->databaseToolStripMenuItem, this->simulatorToolStripMenuItem, 
				this->wifiToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(3587, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// toolStripMenuItem1
			// 
			this->toolStripMenuItem1->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->exportDataToolStripMenuItem, 
				this->toolStripSeparator3, this->exitToolStripMenuItem});
			this->toolStripMenuItem1->Name = L"toolStripMenuItem1";
			this->toolStripMenuItem1->Size = System::Drawing::Size(37, 20);
			this->toolStripMenuItem1->Text = L"&File";
			// 
			// exportDataToolStripMenuItem
			// 
			this->exportDataToolStripMenuItem->Name = L"exportDataToolStripMenuItem";
			this->exportDataToolStripMenuItem->Size = System::Drawing::Size(143, 22);
			this->exportDataToolStripMenuItem->Text = L"Export &Data...";
			this->exportDataToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exportDataToolStripMenuItem_Click);
			// 
			// toolStripSeparator3
			// 
			this->toolStripSeparator3->Name = L"toolStripSeparator3";
			this->toolStripSeparator3->Size = System::Drawing::Size(140, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->ShortcutKeys = static_cast<System::Windows::Forms::Keys>((System::Windows::Forms::Keys::Alt | System::Windows::Forms::Keys::F4));
			this->exitToolStripMenuItem->Size = System::Drawing::Size(143, 22);
			this->exitToolStripMenuItem->Text = L"&Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::exitToolStripMenuItem_Click);
			// 
			// serialCommunicationsToolStripMenuItem
			// 
			this->serialCommunicationsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->connectToolStripMenuItem, 
				this->toolStripSeparator1, this->comPortStripComboBox, this->baudRateStripComboBox, this->toolStripSeparator2, this->disconnectToolStripMenuItem});
			this->serialCommunicationsToolStripMenuItem->Name = L"serialCommunicationsToolStripMenuItem";
			this->serialCommunicationsToolStripMenuItem->Size = System::Drawing::Size(24, 20);
			this->serialCommunicationsToolStripMenuItem->Text = L"-";
			this->serialCommunicationsToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::serialCommunicationsToolStripMenuItem_Click);
			// 
			// connectToolStripMenuItem
			// 
			this->connectToolStripMenuItem->Name = L"connectToolStripMenuItem";
			this->connectToolStripMenuItem->Size = System::Drawing::Size(181, 22);
			this->connectToolStripMenuItem->Text = L"Connect";
			this->connectToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::connectToolStripMenuItem_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(178, 6);
			// 
			// comPortStripComboBox
			// 
			this->comPortStripComboBox->Name = L"comPortStripComboBox";
			this->comPortStripComboBox->Size = System::Drawing::Size(121, 23);
			this->comPortStripComboBox->Text = L"Com Port";
			this->comPortStripComboBox->TextChanged += gcnew System::EventHandler(this, &Form1::comPortStripComboBox_TextChanged);
			// 
			// baudRateStripComboBox
			// 
			this->baudRateStripComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(8) {L"2400", L"4800", L"9600", L"19200", 
				L"38400", L"57600", L"115200", L"31250"});
			this->baudRateStripComboBox->Name = L"baudRateStripComboBox";
			this->baudRateStripComboBox->Size = System::Drawing::Size(121, 23);
			this->baudRateStripComboBox->Text = L"Baud Rate";
			this->baudRateStripComboBox->TextChanged += gcnew System::EventHandler(this, &Form1::baudRateStripComboBox_TextChanged);
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(178, 6);
			// 
			// disconnectToolStripMenuItem
			// 
			this->disconnectToolStripMenuItem->Name = L"disconnectToolStripMenuItem";
			this->disconnectToolStripMenuItem->Size = System::Drawing::Size(181, 22);
			this->disconnectToolStripMenuItem->Text = L"Disconnect";
			this->disconnectToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::disconnectToolStripMenuItem_Click);
			// 
			// mapToolStripMenuItem
			// 
			this->mapToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->providerToolStripMenuItem, 
				this->lockToolStripMenuItem, this->downloadToolStripMenuItem});
			this->mapToolStripMenuItem->Name = L"mapToolStripMenuItem";
			this->mapToolStripMenuItem->Size = System::Drawing::Size(43, 20);
			this->mapToolStripMenuItem->Text = L"Map";
			// 
			// providerToolStripMenuItem
			// 
			this->providerToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->mapnikToolStripMenuItem, 
				this->osmarenderToolStripMenuItem, this->cyclemapToolStripMenuItem});
			this->providerToolStripMenuItem->Name = L"providerToolStripMenuItem";
			this->providerToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->providerToolStripMenuItem->Text = L"Provider";
			// 
			// mapnikToolStripMenuItem
			// 
			this->mapnikToolStripMenuItem->Name = L"mapnikToolStripMenuItem";
			this->mapnikToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->mapnikToolStripMenuItem->Text = L"Mapnik";
			this->mapnikToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::mapnikToolStripMenuItem_Click);
			// 
			// osmarenderToolStripMenuItem
			// 
			this->osmarenderToolStripMenuItem->Name = L"osmarenderToolStripMenuItem";
			this->osmarenderToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->osmarenderToolStripMenuItem->Text = L"Osmarender";
			this->osmarenderToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::osmarenderToolStripMenuItem_Click);
			// 
			// cyclemapToolStripMenuItem
			// 
			this->cyclemapToolStripMenuItem->Name = L"cyclemapToolStripMenuItem";
			this->cyclemapToolStripMenuItem->Size = System::Drawing::Size(139, 22);
			this->cyclemapToolStripMenuItem->Text = L"Cyclemap";
			this->cyclemapToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::cyclemapToolStripMenuItem_Click);
			// 
			// lockToolStripMenuItem
			// 
			this->lockToolStripMenuItem->Name = L"lockToolStripMenuItem";
			this->lockToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->lockToolStripMenuItem->Text = L"Lock";
			this->lockToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::lockToolStripMenuItem_Click);
			// 
			// downloadToolStripMenuItem
			// 
			this->downloadToolStripMenuItem->Name = L"downloadToolStripMenuItem";
			this->downloadToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->downloadToolStripMenuItem->Text = L"Download";
			this->downloadToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::downloadToolStripMenuItem_Click);
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
			this->resetToolStripMenuItem->Size = System::Drawing::Size(102, 22);
			this->resetToolStripMenuItem->Text = L"Reset";
			this->resetToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::resetToolStripMenuItem_Click);
			// 
			// simulatorToolStripMenuItem
			// 
			this->simulatorToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->choosePathToolStripMenuItem, 
				this->pauseToolStripMenuItem, this->stopToolStripMenuItem});
			this->simulatorToolStripMenuItem->Name = L"simulatorToolStripMenuItem";
			this->simulatorToolStripMenuItem->Size = System::Drawing::Size(70, 20);
			this->simulatorToolStripMenuItem->Text = L"Simulator";
			// 
			// choosePathToolStripMenuItem
			// 
			this->choosePathToolStripMenuItem->Name = L"choosePathToolStripMenuItem";
			this->choosePathToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->choosePathToolStripMenuItem->Text = L"Choose Path";
			this->choosePathToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::choosePathToolStripMenuItem_Click);
			// 
			// pauseToolStripMenuItem
			// 
			this->pauseToolStripMenuItem->Name = L"pauseToolStripMenuItem";
			this->pauseToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->pauseToolStripMenuItem->Text = L"Pause";
			this->pauseToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::pauseToolStripMenuItem_Click);
			// 
			// stopToolStripMenuItem
			// 
			this->stopToolStripMenuItem->Name = L"stopToolStripMenuItem";
			this->stopToolStripMenuItem->Size = System::Drawing::Size(141, 22);
			this->stopToolStripMenuItem->Text = L"Stop";
			this->stopToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::stopToolStripMenuItem_Click);
			// 
			// wifiToolStripMenuItem
			// 
			this->wifiToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->stopToolStripMenuItem1});
			this->wifiToolStripMenuItem->Name = L"wifiToolStripMenuItem";
			this->wifiToolStripMenuItem->Size = System::Drawing::Size(40, 20);
			this->wifiToolStripMenuItem->Text = L"Wifi";
			// 
			// stopToolStripMenuItem1
			// 
			this->stopToolStripMenuItem1->Name = L"stopToolStripMenuItem1";
			this->stopToolStripMenuItem1->Size = System::Drawing::Size(98, 22);
			this->stopToolStripMenuItem1->Text = L"Stop";
			this->stopToolStripMenuItem1->Click += gcnew System::EventHandler(this, &Form1::stopToolStripMenuItem1_Click);
			// 
			// openGLTimer
			// 
			this->openGLTimer->Enabled = true;
			this->openGLTimer->Interval = 30;
			this->openGLTimer->Tick += gcnew System::EventHandler(this, &Form1::openGLTimer_Tick);
			// 
			// openGLPanel
			// 
			this->openGLPanel->Location = System::Drawing::Point(385, 55);
			this->openGLPanel->Name = L"openGLPanel";
			this->openGLPanel->Size = System::Drawing::Size(1280, 720);
			this->openGLPanel->TabIndex = 1;
			this->openGLPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &Form1::openGLPanel_Paint);
			// 
			// gimbalHUDPanel
			// 
			this->gimbalHUDPanel->BackColor = System::Drawing::Color::Transparent;
			this->gimbalHUDPanel->Location = System::Drawing::Point(550, 790);
			this->gimbalHUDPanel->Name = L"gimbalHUDPanel";
			this->gimbalHUDPanel->Size = System::Drawing::Size(134, 134);
			this->gimbalHUDPanel->TabIndex = 34;
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
			this->errorLogTextBox->Size = System::Drawing::Size(292, 283);
			this->errorLogTextBox->TabIndex = 2;
			this->errorLogTextBox->Text = L"";
			this->errorLogTextBox->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::errorLogTextBox_KeyPress);
			// 
			// metadataTable
			// 
			this->metadataTable->AllowUserToAddRows = false;
			this->metadataTable->AllowUserToDeleteRows = false;
			this->metadataTable->AllowUserToResizeColumns = false;
			this->metadataTable->AllowUserToResizeRows = false;
			dataGridViewCellStyle4->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle4->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle4->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle4->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->AlternatingRowsDefaultCellStyle = dataGridViewCellStyle4;
			this->metadataTable->BackgroundColor = System::Drawing::Color::Black;
			dataGridViewCellStyle5->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle5->BackColor = System::Drawing::Color::DimGray;
			dataGridViewCellStyle5->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle5->ForeColor = System::Drawing::Color::White;
			dataGridViewCellStyle5->SelectionBackColor = System::Drawing::Color::White;
			dataGridViewCellStyle5->SelectionForeColor = System::Drawing::Color::Black;
			dataGridViewCellStyle5->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->ColumnHeadersDefaultCellStyle = dataGridViewCellStyle5;
			this->metadataTable->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->metadataTable->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->Property, 
				this->Value});
			this->metadataTable->EditMode = System::Windows::Forms::DataGridViewEditMode::EditProgrammatically;
			this->metadataTable->GridColor = System::Drawing::Color::Black;
			this->metadataTable->Location = System::Drawing::Point(12, 55);
			this->metadataTable->Name = L"metadataTable";
			this->metadataTable->ReadOnly = true;
			dataGridViewCellStyle6->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle6->BackColor = System::Drawing::Color::Black;
			dataGridViewCellStyle6->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, 
				System::Drawing::GraphicsUnit::Point, static_cast<System::Byte>(0)));
			dataGridViewCellStyle6->ForeColor = System::Drawing::SystemColors::WindowText;
			dataGridViewCellStyle6->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle6->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle6->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->metadataTable->RowHeadersDefaultCellStyle = dataGridViewCellStyle6;
			this->metadataTable->RowHeadersWidthSizeMode = System::Windows::Forms::DataGridViewRowHeadersWidthSizeMode::DisableResizing;
			this->metadataTable->RowTemplate->DefaultCellStyle->BackColor = System::Drawing::Color::Black;
			this->metadataTable->RowTemplate->DefaultCellStyle->ForeColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->NullValue = L"none";
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionBackColor = System::Drawing::Color::White;
			this->metadataTable->RowTemplate->DefaultCellStyle->SelectionForeColor = System::Drawing::Color::Black;
			this->metadataTable->Size = System::Drawing::Size(292, 406);
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
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Location = System::Drawing::Point(1172, 781);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(493, 153);
			this->tabControl1->TabIndex = 5;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->checkBox1);
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = L"tabPage1";
			this->tabPage1->Padding = System::Windows::Forms::Padding(3);
			this->tabPage1->Size = System::Drawing::Size(485, 127);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = L"Filters";
			this->tabPage1->UseVisualStyleBackColor = true;
			// 
			// checkBox1
			// 
			this->checkBox1->AutoSize = true;
			this->checkBox1->Location = System::Drawing::Point(6, 6);
			this->checkBox1->Name = L"checkBox1";
			this->checkBox1->Size = System::Drawing::Size(103, 17);
			this->checkBox1->TabIndex = 0;
			this->checkBox1->Text = L"Display Saliency";
			this->checkBox1->UseVisualStyleBackColor = true;
			this->checkBox1->CheckedChanged += gcnew System::EventHandler(this, &Form1::checkBox1_CheckedChanged);
			// 
			// tabPage2
			// 
			this->tabPage2->Controls->Add(this->label4);
			this->tabPage2->Controls->Add(this->checkedListBox1);
			this->tabPage2->Controls->Add(this->vidOptChangeDirButton);
			this->tabPage2->Controls->Add(this->label3);
			this->tabPage2->Controls->Add(this->label2);
			this->tabPage2->Controls->Add(this->encodingComboBox);
			this->tabPage2->Controls->Add(this->splitLengthTextBox);
			this->tabPage2->Controls->Add(this->vidOptOutputDirText);
			this->tabPage2->Controls->Add(this->label1);
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = L"tabPage2";
			this->tabPage2->Padding = System::Windows::Forms::Padding(3);
			this->tabPage2->Size = System::Drawing::Size(485, 127);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = L"DONT USE THIS";
			this->tabPage2->UseVisualStyleBackColor = true;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(352, 6);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(78, 13);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Output Options";
			// 
			// checkedListBox1
			// 
			this->checkedListBox1->CheckOnClick = true;
			this->checkedListBox1->FormattingEnabled = true;
			this->checkedListBox1->Items->AddRange(gcnew cli::array< System::Object^  >(4) {L"Raw Input", L"Filtered Output", L"Text Overlay", 
				L"Disp Saliency"});
			this->checkedListBox1->Location = System::Drawing::Point(355, 22);
			this->checkedListBox1->Name = L"checkedListBox1";
			this->checkedListBox1->Size = System::Drawing::Size(120, 64);
			this->checkedListBox1->TabIndex = 9;
			this->checkedListBox1->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::checkedListBox1_SelectedIndexChanged);
			// 
			// vidOptChangeDirButton
			// 
			this->vidOptChangeDirButton->Location = System::Drawing::Point(204, 3);
			this->vidOptChangeDirButton->Name = L"vidOptChangeDirButton";
			this->vidOptChangeDirButton->Size = System::Drawing::Size(100, 23);
			this->vidOptChangeDirButton->TabIndex = 6;
			this->vidOptChangeDirButton->Text = L"Change Directory";
			this->vidOptChangeDirButton->UseVisualStyleBackColor = true;
			this->vidOptChangeDirButton->Click += gcnew System::EventHandler(this, &Form1::vidOptChangeDirButton_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(8, 63);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(52, 13);
			this->label3->TabIndex = 5;
			this->label3->Text = L"Encoding";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(8, 36);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(63, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Split Length";
			// 
			// encodingComboBox
			// 
			this->encodingComboBox->FormattingEnabled = true;
			this->encodingComboBox->Items->AddRange(gcnew cli::array< System::Object^  >(3) {L"None", L"XviD", L"MJPG"});
			this->encodingComboBox->Location = System::Drawing::Point(98, 60);
			this->encodingComboBox->Name = L"encodingComboBox";
			this->encodingComboBox->Size = System::Drawing::Size(100, 21);
			this->encodingComboBox->TabIndex = 3;
			this->encodingComboBox->Text = L"None";
			this->encodingComboBox->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::encodingComboBox_SelectedIndexChanged);
			// 
			// splitLengthTextBox
			// 
			this->splitLengthTextBox->Location = System::Drawing::Point(98, 33);
			this->splitLengthTextBox->Name = L"splitLengthTextBox";
			this->splitLengthTextBox->Size = System::Drawing::Size(100, 20);
			this->splitLengthTextBox->TabIndex = 2;
			this->splitLengthTextBox->Text = L"300";
			this->splitLengthTextBox->TextAlign = System::Windows::Forms::HorizontalAlignment::Right;
			this->splitLengthTextBox->Validated += gcnew System::EventHandler(this, &Form1::splitLengthTextBox_Validated);
			// 
			// vidOptOutputDirText
			// 
			this->vidOptOutputDirText->Location = System::Drawing::Point(98, 6);
			this->vidOptOutputDirText->Name = L"vidOptOutputDirText";
			this->vidOptOutputDirText->ReadOnly = true;
			this->vidOptOutputDirText->Size = System::Drawing::Size(100, 20);
			this->vidOptOutputDirText->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(8, 10);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(84, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Output Directory";
			// 
			// startRecordButton
			// 
			this->startRecordButton->Location = System::Drawing::Point(997, 31);
			this->startRecordButton->Name = L"startRecordButton";
			this->startRecordButton->Size = System::Drawing::Size(100, 23);
			this->startRecordButton->TabIndex = 7;
			this->startRecordButton->Text = L"Start Recording";
			this->startRecordButton->UseVisualStyleBackColor = true;
			this->startRecordButton->Click += gcnew System::EventHandler(this, &Form1::startRecordButton_Click);
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
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(385, 35);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(34, 13);
			this->label7->TabIndex = 8;
			this->label7->Text = L"Video";
			// 
			// vidOptChangeDirBrowserDialog
			// 
			this->vidOptChangeDirBrowserDialog->Description = L"Select Video Output Directory";
			// 
			// joystickTimer
			// 
			this->joystickTimer->Enabled = true;
			this->joystickTimer->Interval = 125;
			this->joystickTimer->Tick += gcnew System::EventHandler(this, &Form1::joystickTimer_Tick);
			// 
			// metadataTabControl
			// 
			this->metadataTabControl->Controls->Add(this->tabPage3);
			this->metadataTabControl->Controls->Add(this->tabPage4);
			this->metadataTabControl->Controls->Add(this->tabPage5);
			this->metadataTabControl->Location = System::Drawing::Point(1698, 627);
			this->metadataTabControl->Name = L"metadataTabControl";
			this->metadataTabControl->SelectedIndex = 0;
			this->metadataTabControl->Size = System::Drawing::Size(1274, 307);
			this->metadataTabControl->TabIndex = 10;
			// 
			// tabPage3
			// 
			this->tabPage3->Controls->Add(this->dataGridView1);
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = L"tabPage3";
			this->tabPage3->Padding = System::Windows::Forms::Padding(3);
			this->tabPage3->Size = System::Drawing::Size(1266, 281);
			this->tabPage3->TabIndex = 0;
			this->tabPage3->Text = L"Candidate Regions";
			this->tabPage3->UseVisualStyleBackColor = true;
			// 
			// dataGridView1
			// 
			this->dataGridView1->AllowUserToAddRows = false;
			this->dataGridView1->AllowUserToDeleteRows = false;
			this->dataGridView1->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView1->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(6) {this->candidate_IDColumn, 
				this->candidate_ThumbnailColumn, this->candidate_LatitudeColumn, this->canidate_LongitudeColumn, this->candidate_VerifiedColumn, 
				this->processed});
			this->dataGridView1->Location = System::Drawing::Point(6, 6);
			this->dataGridView1->Name = L"dataGridView1";
			this->dataGridView1->RowTemplate->Height = 64;
			this->dataGridView1->Size = System::Drawing::Size(1254, 269);
			this->dataGridView1->TabIndex = 0;
			this->dataGridView1->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView1_CellDoubleClick);
			this->dataGridView1->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView1_DataError);
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
			// tabPage4
			// 
			this->tabPage4->Controls->Add(this->dataGridView2);
			this->tabPage4->Location = System::Drawing::Point(4, 22);
			this->tabPage4->Name = L"tabPage4";
			this->tabPage4->Padding = System::Windows::Forms::Padding(3);
			this->tabPage4->Size = System::Drawing::Size(1266, 281);
			this->tabPage4->TabIndex = 1;
			this->tabPage4->Text = L"Unconfirmed Targets";
			this->tabPage4->UseVisualStyleBackColor = true;
			// 
			// dataGridView2
			// 
			this->dataGridView2->AllowUserToAddRows = false;
			this->dataGridView2->AllowUserToDeleteRows = false;
			this->dataGridView2->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView2->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(12) {this->unverified_IDColumn, 
				this->unverified_ThumbnailColumn, this->unverified_LatitudeColumn, this->unverified_LongitudeColumn, this->unverified_HeadingColumn, 
				this->unverified_CenterPixel, this->unverfied_LetterColumn, this->unverified_LetterColor, this->unverified_Shape, this->unverified_ShapeColor, 
				this->unverified_ProcessedColumn, this->unverified_TargetLock});
			this->dataGridView2->Location = System::Drawing::Point(6, 6);
			this->dataGridView2->Name = L"dataGridView2";
			this->dataGridView2->RowTemplate->Height = 64;
			this->dataGridView2->Size = System::Drawing::Size(1254, 269);
			this->dataGridView2->TabIndex = 0;
			this->dataGridView2->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView2_CellDoubleClick);
			this->dataGridView2->CurrentCellDirtyStateChanged += gcnew System::EventHandler(this, &Form1::dataGridView2_CurrentCellDirtyStateChanged);
			this->dataGridView2->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView2_DataError);
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
			// tabPage5
			// 
			this->tabPage5->Controls->Add(this->dataGridView3);
			this->tabPage5->Location = System::Drawing::Point(4, 22);
			this->tabPage5->Name = L"tabPage5";
			this->tabPage5->Padding = System::Windows::Forms::Padding(3);
			this->tabPage5->Size = System::Drawing::Size(1266, 281);
			this->tabPage5->TabIndex = 2;
			this->tabPage5->Text = L"Confirmed Targets";
			this->tabPage5->UseVisualStyleBackColor = true;
			// 
			// dataGridView3
			// 
			this->dataGridView3->AllowUserToAddRows = false;
			this->dataGridView3->AllowUserToDeleteRows = false;
			this->dataGridView3->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->dataGridView3->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(11) {this->confirmed_IDColumn, 
				this->confirmed_ThumbnailColumn, this->confirmed_LatitudeColumn, this->confirmed_LongitudeColumn, this->confirmed_HeadingColumn, 
				this->confirmed_Shape, this->confirmed_ShapeColor, this->confirmed_LetterColumn, this->confirmed_LetterColor, this->targetid_column, 
				this->verified_lockon});
			this->dataGridView3->Location = System::Drawing::Point(6, 6);
			this->dataGridView3->MultiSelect = false;
			this->dataGridView3->Name = L"dataGridView3";
			this->dataGridView3->RowTemplate->Height = 64;
			this->dataGridView3->Size = System::Drawing::Size(1254, 269);
			this->dataGridView3->TabIndex = 0;
			this->dataGridView3->CellDoubleClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &Form1::dataGridView3_CellDoubleClick);
			this->dataGridView3->CurrentCellDirtyStateChanged += gcnew System::EventHandler(this, &Form1::dataGridView2_CurrentCellDirtyStateChanged);
			this->dataGridView3->DataError += gcnew System::Windows::Forms::DataGridViewDataErrorEventHandler(this, &Form1::dataGridView3_DataError);
			// 
			// confirmed_IDColumn
			// 
			this->confirmed_IDColumn->HeaderText = L"ID";
			this->confirmed_IDColumn->Name = L"confirmed_IDColumn";
			this->confirmed_IDColumn->ReadOnly = true;
			this->confirmed_IDColumn->Width = 70;
			// 
			// confirmed_ThumbnailColumn
			// 
			this->confirmed_ThumbnailColumn->HeaderText = L"Thumbnail";
			this->confirmed_ThumbnailColumn->Name = L"confirmed_ThumbnailColumn";
			this->confirmed_ThumbnailColumn->ReadOnly = true;
			this->confirmed_ThumbnailColumn->Width = 64;
			// 
			// confirmed_LatitudeColumn
			// 
			this->confirmed_LatitudeColumn->HeaderText = L"Latitude";
			this->confirmed_LatitudeColumn->Name = L"confirmed_LatitudeColumn";
			this->confirmed_LatitudeColumn->ReadOnly = true;
			// 
			// confirmed_LongitudeColumn
			// 
			this->confirmed_LongitudeColumn->HeaderText = L"Longitude";
			this->confirmed_LongitudeColumn->Name = L"confirmed_LongitudeColumn";
			this->confirmed_LongitudeColumn->ReadOnly = true;
			// 
			// confirmed_HeadingColumn
			// 
			this->confirmed_HeadingColumn->HeaderText = L"Orientation";
			this->confirmed_HeadingColumn->Name = L"confirmed_HeadingColumn";
			this->confirmed_HeadingColumn->ReadOnly = true;
			// 
			// confirmed_Shape
			// 
			this->confirmed_Shape->HeaderText = L"Shape";
			this->confirmed_Shape->Name = L"confirmed_Shape";
			this->confirmed_Shape->ReadOnly = true;
			// 
			// confirmed_ShapeColor
			// 
			this->confirmed_ShapeColor->HeaderText = L"Shape Color";
			this->confirmed_ShapeColor->Name = L"confirmed_ShapeColor";
			this->confirmed_ShapeColor->ReadOnly = true;
			// 
			// confirmed_LetterColumn
			// 
			this->confirmed_LetterColumn->HeaderText = L"Letter";
			this->confirmed_LetterColumn->Name = L"confirmed_LetterColumn";
			this->confirmed_LetterColumn->ReadOnly = true;
			// 
			// confirmed_LetterColor
			// 
			this->confirmed_LetterColor->HeaderText = L"Letter Color";
			this->confirmed_LetterColor->Name = L"confirmed_LetterColor";
			this->confirmed_LetterColor->ReadOnly = true;
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
			// videoSaveTimer
			// 
			this->videoSaveTimer->Enabled = true;
			this->videoSaveTimer->Interval = 1000;
			this->videoSaveTimer->Tick += gcnew System::EventHandler(this, &Form1::videoSaveTimer_Tick);
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
			this->tableLayoutPanel1->Location = System::Drawing::Point(3571, 586);
			this->tableLayoutPanel1->Name = L"tableLayoutPanel1";
			this->tableLayoutPanel1->RowCount = 1;
			this->tableLayoutPanel1->RowStyles->Add((gcnew System::Windows::Forms::RowStyle(System::Windows::Forms::SizeType::Percent, 50)));
			this->tableLayoutPanel1->Size = System::Drawing::Size(0, 0);
			this->tableLayoutPanel1->TabIndex = 14;
			// 
			// mapUpdateTimer
			// 
			this->mapUpdateTimer->Enabled = true;
			this->mapUpdateTimer->Interval = 250;
			this->mapUpdateTimer->Tick += gcnew System::EventHandler(this, &Form1::mapUpdateTimer_Tick);
			// 
			// mapMenuStrip
			// 
			this->mapMenuStrip->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->mapLookGPSToolStripMenuItem});
			this->mapMenuStrip->Name = L"mapMenuStrip";
			this->mapMenuStrip->Size = System::Drawing::Size(129, 26);
			// 
			// mapLookGPSToolStripMenuItem
			// 
			this->mapLookGPSToolStripMenuItem->Name = L"mapLookGPSToolStripMenuItem";
			this->mapLookGPSToolStripMenuItem->Size = System::Drawing::Size(128, 22);
			this->mapLookGPSToolStripMenuItem->Text = L"Look Here";
			this->mapLookGPSToolStripMenuItem->Click += gcnew System::EventHandler(this, &Form1::mapLookGPSToolStripMenuItem_Click);
			// 
			// simReadVidDialog
			// 
			this->simReadVidDialog->FileName = L"simReadVidDialog";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(550, 31);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(75, 23);
			this->button1->TabIndex = 15;
			this->button1->Text = L"Zoom in";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &Form1::button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(649, 31);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(75, 23);
			this->button2->TabIndex = 16;
			this->button2->Text = L"Stop Zoom";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &Form1::button2_Click);
			// 
			// button3
			// 
			this->button3->Location = System::Drawing::Point(749, 31);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(75, 23);
			this->button3->TabIndex = 17;
			this->button3->Text = L"Zoom Out";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &Form1::button3_Click);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(1266, 30);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(100, 23);
			this->button4->TabIndex = 18;
			this->button4->Text = L"Connect";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &Form1::connectButton_Click);
			// 
			// label8
			// 
			this->label8->AutoSize = true;
			this->label8->BackColor = System::Drawing::Color::DimGray;
			this->label8->Enabled = false;
			this->label8->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label8->Location = System::Drawing::Point(305, 76);
			this->label8->Name = L"label8";
			this->label8->Size = System::Drawing::Size(80, 16);
			this->label8->TabIndex = 19;
			this->label8->Text = L"Recording";
			// 
			// label11
			// 
			this->label11->AutoSize = true;
			this->label11->Enabled = false;
			this->label11->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label11->Location = System::Drawing::Point(310, 176);
			this->label11->Name = L"label11";
			this->label11->Size = System::Drawing::Size(69, 16);
			this->label11->TabIndex = 22;
			this->label11->Text = L"Autopilot";
			// 
			// label12
			// 
			this->label12->AutoSize = true;
			this->label12->Enabled = false;
			this->label12->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label12->Location = System::Drawing::Point(312, 276);
			this->label12->Name = L"label12";
			this->label12->Size = System::Drawing::Size(57, 16);
			this->label12->TabIndex = 23;
			this->label12->Text = L"Gimbal";
			// 
			// pictureBox1
			// 
			this->pictureBox1->Enabled = false;
			this->pictureBox1->Location = System::Drawing::Point(310, 95);
			this->pictureBox1->Name = L"pictureBox1";
			this->pictureBox1->Size = System::Drawing::Size(69, 69);
			this->pictureBox1->TabIndex = 24;
			this->pictureBox1->TabStop = false;
			// 
			// pictureBox2
			// 
			this->pictureBox2->Enabled = false;
			this->pictureBox2->Location = System::Drawing::Point(310, 195);
			this->pictureBox2->Name = L"pictureBox2";
			this->pictureBox2->Size = System::Drawing::Size(69, 69);
			this->pictureBox2->TabIndex = 25;
			this->pictureBox2->TabStop = false;
			// 
			// pictureBox3
			// 
			this->pictureBox3->Enabled = false;
			this->pictureBox3->Location = System::Drawing::Point(310, 295);
			this->pictureBox3->Name = L"pictureBox3";
			this->pictureBox3->Size = System::Drawing::Size(69, 69);
			this->pictureBox3->TabIndex = 26;
			this->pictureBox3->TabStop = false;
			// 
			// label9
			// 
			this->label9->AutoSize = true;
			this->label9->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label9->ForeColor = System::Drawing::Color::Chartreuse;
			this->label9->Location = System::Drawing::Point(4, 5);
			this->label9->Name = L"label9";
			this->label9->Size = System::Drawing::Size(151, 13);
			this->label9->TabIndex = 27;
			this->label9->Text = L"Green = Running / Connected";
			// 
			// label10
			// 
			this->label10->AutoSize = true;
			this->label10->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label10->ForeColor = System::Drawing::Color::Yellow;
			this->label10->Location = System::Drawing::Point(4, 26);
			this->label10->Name = L"label10";
			this->label10->Size = System::Drawing::Size(109, 13);
			this->label10->TabIndex = 28;
			this->label10->Text = L"Yellow = Packet Loss";
			// 
			// label13
			// 
			this->label13->AutoSize = true;
			this->label13->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label13->ForeColor = System::Drawing::Color::DarkRed;
			this->label13->Location = System::Drawing::Point(4, 49);
			this->label13->Name = L"label13";
			this->label13->Size = System::Drawing::Size(176, 13);
			this->label13->TabIndex = 29;
			this->label13->Text = L"Red = Not Running / Disconnected";
			// 
			// label14
			// 
			this->label14->AutoSize = true;
			this->label14->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label14->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label14->Location = System::Drawing::Point(4, 96);
			this->label14->Name = L"label14";
			this->label14->Size = System::Drawing::Size(180, 13);
			this->label14->TabIndex = 30;
			this->label14->Text = L"Autopilot = Telemetry Data from UAV";
			// 
			// label15
			// 
			this->label15->AutoSize = true;
			this->label15->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label15->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label15->Location = System::Drawing::Point(4, 119);
			this->label15->Name = L"label15";
			this->label15->Size = System::Drawing::Size(223, 13);
			this->label15->TabIndex = 31;
			this->label15->Text = L"Gimbal = Communications for Pointing Camera";
			// 
			// label16
			// 
			this->label16->AutoSize = true;
			this->label16->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label16->ForeColor = System::Drawing::SystemColors::ControlLightLight;
			this->label16->Location = System::Drawing::Point(4, 72);
			this->label16->Name = L"label16";
			this->label16->Size = System::Drawing::Size(217, 13);
			this->label16->TabIndex = 32;
			this->label16->Text = L"Recording = Recording Telemetry and Video";
			// 
			// panel1
			// 
			this->panel1->BackColor = System::Drawing::Color::Black;
			this->panel1->Controls->Add(this->label13);
			this->panel1->Controls->Add(this->label9);
			this->panel1->Controls->Add(this->label10);
			this->panel1->Controls->Add(this->label16);
			this->panel1->Controls->Add(this->label14);
			this->panel1->Controls->Add(this->label15);
			this->panel1->Location = System::Drawing::Point(12, 790);
			this->panel1->Name = L"panel1";
			this->panel1->Size = System::Drawing::Size(292, 140);
			this->panel1->TabIndex = 33;
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
			// autosearchBox
			// 
			this->autosearchBox->Location = System::Drawing::Point(384, 790);
			this->autosearchBox->Name = L"autosearchBox";
			this->autosearchBox->Size = System::Drawing::Size(134, 134);
			this->autosearchBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->autosearchBox->TabIndex = 35;
			this->autosearchBox->TabStop = false;
			this->autosearchBox->LoadCompleted += gcnew System::ComponentModel::AsyncCompletedEventHandler(this, &Form1::autosearchBox_LoadCompleted);
			// 
			// Border
			// 
			this->Border->BackgroundImage = (cli::safe_cast<System::Drawing::Image^  >(resources->GetObject(L"Border.BackgroundImage")));
			this->Border->Location = System::Drawing::Point(382, 788);
			this->Border->Name = L"Border";
			this->Border->Size = System::Drawing::Size(138, 138);
			this->Border->TabIndex = 36;
			this->Border->TabStop = false;
			// 
			// pathfinderBox
			// 
			this->pathfinderBox->BackgroundImageLayout = System::Windows::Forms::ImageLayout::Zoom;
			this->pathfinderBox->Location = System::Drawing::Point(1698, 31);
			this->pathfinderBox->Name = L"pathfinderBox";
			this->pathfinderBox->Size = System::Drawing::Size(587, 592);
			this->pathfinderBox->SizeMode = System::Windows::Forms::PictureBoxSizeMode::Zoom;
			this->pathfinderBox->TabIndex = 37;
			this->pathfinderBox->TabStop = false;
			// 
			// gpsCheckboxList
			// 
			this->gpsCheckboxList->FormattingEnabled = true;
			this->gpsCheckboxList->Location = System::Drawing::Point(2724, 77);
			this->gpsCheckboxList->Name = L"gpsCheckboxList";
			this->gpsCheckboxList->Size = System::Drawing::Size(299, 409);
			this->gpsCheckboxList->TabIndex = 38;
			this->gpsCheckboxList->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::gpsCheckboxList_SelectedIndexChanged);
			// 
			// waypointsLabel
			// 
			this->waypointsLabel->AutoSize = true;
			this->waypointsLabel->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->waypointsLabel->Location = System::Drawing::Point(2812, 36);
			this->waypointsLabel->Name = L"waypointsLabel";
			this->waypointsLabel->Size = System::Drawing::Size(135, 24);
			this->waypointsLabel->TabIndex = 39;
			this->waypointsLabel->Text = L"Path waypoints";
			// 
			// sendWaypointsButton
			// 
			this->sendWaypointsButton->Location = System::Drawing::Point(2793, 517);
			this->sendWaypointsButton->Name = L"sendWaypointsButton";
			this->sendWaypointsButton->Size = System::Drawing::Size(154, 69);
			this->sendWaypointsButton->TabIndex = 40;
			this->sendWaypointsButton->Text = L"Send checked waypoints to virtual cockpit";
			this->sendWaypointsButton->UseVisualStyleBackColor = true;
			this->sendWaypointsButton->Click += gcnew System::EventHandler(this, &Form1::sendWaypointsButton_Click);
			// 
			// mapBoundariesTextBox
			// 
			this->mapBoundariesTextBox->Location = System::Drawing::Point(2346, 77);
			this->mapBoundariesTextBox->Multiline = true;
			this->mapBoundariesTextBox->Name = L"mapBoundariesTextBox";
			this->mapBoundariesTextBox->Size = System::Drawing::Size(317, 409);
			this->mapBoundariesTextBox->TabIndex = 41;
			// 
			// generateMapButton
			// 
			this->generateMapButton->Location = System::Drawing::Point(2437, 539);
			this->generateMapButton->Name = L"generateMapButton";
			this->generateMapButton->Size = System::Drawing::Size(149, 52);
			this->generateMapButton->TabIndex = 42;
			this->generateMapButton->Text = L"Generate Map";
			this->generateMapButton->UseVisualStyleBackColor = true;
			this->generateMapButton->Click += gcnew System::EventHandler(this, &Form1::button5_Click);
			// 
			// label17
			// 
			this->label17->AutoSize = true;
			this->label17->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(0)));
			this->label17->Location = System::Drawing::Point(2433, 36);
			this->label17->Name = L"label17";
			this->label17->Size = System::Drawing::Size(148, 24);
			this->label17->TabIndex = 43;
			this->label17->Text = L"Map Boundaries";
			this->label17->Click += gcnew System::EventHandler(this, &Form1::label17_Click);
			// 
			// loadBoundariesFromFileButton
			// 
			this->loadBoundariesFromFileButton->Location = System::Drawing::Point(2437, 492);
			this->loadBoundariesFromFileButton->Name = L"loadBoundariesFromFileButton";
			this->loadBoundariesFromFileButton->Size = System::Drawing::Size(149, 41);
			this->loadBoundariesFromFileButton->TabIndex = 44;
			this->loadBoundariesFromFileButton->Text = L"Load map boundaries from file";
			this->loadBoundariesFromFileButton->UseVisualStyleBackColor = true;
			this->loadBoundariesFromFileButton->Click += gcnew System::EventHandler(this, &Form1::loadWaypointsFromFileButton_Click);
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->AutoValidate = System::Windows::Forms::AutoValidate::EnableAllowFocusChange;
			this->BackColor = System::Drawing::Color::DimGray;
			this->ClientSize = System::Drawing::Size(3587, 946);
			this->Controls->Add(this->loadBoundariesFromFileButton);
			this->Controls->Add(this->label17);
			this->Controls->Add(this->mapBoundariesTextBox);
			this->Controls->Add(this->sendWaypointsButton);
			this->Controls->Add(this->generateMapButton);
			this->Controls->Add(this->gpsCheckboxList);
			this->Controls->Add(this->waypointsLabel);
			this->Controls->Add(this->autosearchBox);
			this->Controls->Add(this->gimbalHUDPanel);
			this->Controls->Add(this->panel1);
			this->Controls->Add(this->pictureBox3);
			this->Controls->Add(this->pathfinderBox);
			this->Controls->Add(this->pictureBox2);
			this->Controls->Add(this->pictureBox1);
			this->Controls->Add(this->label12);
			this->Controls->Add(this->label11);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label8);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->tableLayoutPanel1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->metadataTabControl);
			this->Controls->Add(this->label7);
			this->Controls->Add(this->label6);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->metadataTable);
			this->Controls->Add(this->startRecordButton);
			this->Controls->Add(this->errorLogTextBox);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->openGLPanel);
			this->Controls->Add(this->menuStrip1);
			this->Controls->Add(this->Border);
			this->Location = System::Drawing::Point(250, 0);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"Form1";
			this->Text = L"UCSD Skynet";
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->Click += gcnew System::EventHandler(this, &Form1::Form1_Click);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyDown);
			this->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &Form1::Form1_KeyPress);
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->metadataTable))->EndInit();
			this->tabControl1->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->tabPage1->PerformLayout();
			this->tabPage2->ResumeLayout(false);
			this->tabPage2->PerformLayout();
			this->metadataTabControl->ResumeLayout(false);
			this->tabPage3->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView1))->EndInit();
			this->tabPage4->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView2))->EndInit();
			this->tabPage5->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->dataGridView3))->EndInit();
			this->mapMenuStrip->ResumeLayout(false);
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox1))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox2))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pictureBox3))->EndInit();
			this->panel1->ResumeLayout(false);
			this->panel1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->autosearchBox))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->Border))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->pathfinderBox))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e) 
			 {
			 }

	/// <summary>
	/// Timer that handles rendering of OpenGL - what is actually rendered is handled in
	/// OpenGLForm.h
	/// </summary>
	private: System::Void openGLTimer_Tick(System::Object^  sender, System::EventArgs^  e) 
			 {
				 UNREFERENCED_PARAMETER(sender);
				 UNREFERENCED_PARAMETER(e);

				 if( openGLView == nullptr )
					 return;

				 openGLView->Render();
				 openGLView->SwapOpenGLBuffers();
			 }
private: System::Void metadataTable_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
		 }
private: System::Void serialCommunicationsToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void exitToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 Application::Exit();
		 }

private: System::Void vidOptChangeDirButton_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 String ^ temp;
		 
		 	 System::Windows::Forms::DialogResult result = vidOptChangeDirBrowserDialog->ShowDialog();

			 if ( result == ::DialogResult::OK )
			 {
			 	temp = vidOptChangeDirBrowserDialog->SelectedPath;

				vidOptOutputDirText->Text = temp;
				consoleMessage( "Video output directory changed to: " + temp, Color::Gray );
			 }

			 return;
		 
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
public:  System::Void doNothing() {
			 System::Diagnostics::Trace::WriteLine("Doing nothing");
		 }

public:	 System::Void saveCurrentImage() {
			 appController->saveCurrentFrameAsUnverifiedOnMainThread();
		 }

public:	 System::Void printToConsole( String ^ message, Color ^ color)
		 {
			 array<Object ^> ^ retArr = gcnew array< Object^ >{message, color};
	
			 printToConsole( retArr );
		 }
public:  System::Void printToConsole( array<Object ^> ^ retArr ) 
		 {
			 
			//System::Diagnostics::Trace::WriteLine("Form1::printToConsole");
			//consoleMessage( (String ^)retArr[0] + ((Int32)retArr[1]), Color::Blue);
			consoleMessage( (String ^)retArr[0], ((Color)retArr[1]));//((ColorRef ^)retArr[1])->theColor );
		 }

public:	 System::Void updateCommsStatus( array<Object ^> ^ retArr )
		 {
			 if (!isconnected)
				 return;

			 Image ^theImage;
			 // get result (yellow vs green)
			 if ((Int32)retArr[1] == GREEN_STATUS)
				 theImage = greenImage;
			 else if ((Int32)retArr[1] == YELLOW_STATUS)
				 theImage = yellowImage;
			 else 
				 theImage = redImage;

			 // get type (rabbit vs autopilot) and update image
			 if (((String ^)retArr[0])->Equals("Autopilot"))
				 pictureBox2->Image = theImage;
			 else if (((String ^)retArr[0])->Equals("Rabbit"))
				 pictureBox3->Image = theImage;


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

private: System::Void comPortStripComboBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {

			 // dont run this function, autoconnect makes this obsolete

			/* String ^ port = comPortStripComboBox->Text;

			 bool result = theComport->setPortName( port );

			 if( result )
			 {
			 	consoleMessage( "Comport changed to: " + port, Color::Gray );
			 }
			 else
			 {
			 	errorMessage( "Failed changing comport to: " + port );
			 }*/
		 }

private: System::Void baudRateStripComboBox_TextChanged(System::Object^  sender, System::EventArgs^  e) {

			 
			 // dont run this function, autoconnect makes this obsolete


			 /*
			 String ^ baudRateString = baudRateStripComboBox->Text;
			 // convert string to int
			 int baudRate = Convert::ToInt32( baudRateString, 10 );

			 bool result = theComport->setBaudRate( baudRate );

			 if( result )
			 {
			 	consoleMessage( "Baud rate changed to: " + baudRate, Color::Gray );
			 }
			 else
			 {
			 	errorMessage( "Failed changing baud rate to: " + baudRate );
			 }*/

		 }

private: System::Void joystickTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
			m_joystick->UpdateInputState( NULL );
		 }


private: System::Void connectToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
		 {		
			 
			 // dont run this function, autoconnect makes this obsolete
			 
			 /*bool result = theComport->connect();

			 if (result)
			 {
			 	consoleMessage( "Connected to comport " + comPortStripComboBox->Text + " with baud rate " + baudRateStripComboBox->Text, Color::Green );

				comReadThread = gcnew Thread(gcnew ThreadStart(theComport, &Communications::Comport::readThread));
				comReadThread->Name = "Comport Read Thread";
				comReadThread->Start();

			 	isReadingData = false;
			 }
			 else
			 	errorMessage( "Failed to connect" );*/
		 }

private: System::Void disconnectToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 
			// dont run this function, autoconnect makes this obsolete


			 /*
			bool result = theComport->disconnect();

			if (result)
			{
				consoleMessage( "Disconnected from comport " + comPortStripComboBox->Text, Color::Green );
				comReadThread->Abort();			
			}
			else
				errorMessage( "Failed to disconnect" );
				*/

		 }

private: bool isReadingData;

public: System::Void setUnverifiedTableContents( array<Database::UnverifiedRowData ^> ^ contents )
		{
			if (contents == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::setUnverifiedTableContents(): data is null");
				return;
			}

			if ( !this->dataGridView2->InvokeRequired )
			{
				this->dataGridView2->Rows->Clear();

				for each(Database::UnverifiedRowData ^ data in contents)
				{
					insertUnverifiedData(data);
				}
			}
			else
			{
				unverifiedTableContentsDelegate = gcnew Delegates::unverifiedRowDataArrayToVoid(this,&Form1::setUnverifiedTableContents);
				array<Object ^>^ arr = {contents}; 
				this->Invoke(unverifiedTableContentsDelegate,arr);
			}
		}

public: System::Void setVerifiedTableContents( array<Database::VerifiedRowData ^> ^ contents )
		{
			if (contents == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::setVerifiedTableContents(): data is null");
				return;
			}
			if ( !this->dataGridView3->InvokeRequired )
			{
				this->dataGridView3->Rows->Clear();

				for each(Database::VerifiedRowData ^ data in contents)
				{
					if (data == nullptr)
						continue;
					insertVerifiedTargetData(data);
				}
			}
			else
			{
				verifiedTableContentsDelegate = gcnew Delegates::verifiedRowDataArrayToVoid(this,&Form1::setVerifiedTableContents);
				array<Object ^>^ arr = {contents}; 
				this->Invoke(verifiedTableContentsDelegate,arr);
			}
		}

public: System::Void setCandidateTableContents( array<Database::CandidateRowData ^> ^ contents )
		{
			if (contents == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::setCandidateTableContents: data is null");
				return;
			}
			if ( !this->dataGridView1->InvokeRequired )
			{

				this->dataGridView1->Rows->Clear();

				for each(Database::CandidateRowData ^ data in contents)
				{
					if (data == nullptr)
						continue;
					insertCandidateData(data);
				}
			}
			else
			{
				candidateTableContentsDelegate = gcnew Delegates::candidateRowDataArrayToVoid(this,&Form1::setCandidateTableContents);
				array<Object ^>^ arr = {contents}; 
				this->Invoke(candidateTableContentsDelegate,arr);
			}
		}

private: bool findIntInArray( int num, array<int> ^ arr)
		 {
			 for each(int i in arr)
				 if (num == i)
					 return true;
			 return false;
		 }

public: System::Void setProcessedForCandidates( array<int> ^ idsToSet )
		{
			DataGridView ^ grid = this->dataGridView1;
			
			int size = grid->Rows->Count;

			for (int i = 0; i < size; i++)
			{
				int id = Int32::Parse(   (String ^)(grid->Rows[i]->Cells[0]->Value)    );
				if (findIntInArray(id, idsToSet))
					grid->Rows[i]->Cells[9]->Value = true;
				else
					grid->Rows[i]->Cells[9]->Value = false;
			}

		}

public: System::Void insertCandidateData( Database::CandidateRowData ^ data) 
		{
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertCandidateData(): data is null");
				return;
			}

			try {
				int num = data->candidateid;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertCandidateData(): data is null");
				return;
			}
			int rowNum;
			DataGridView ^ grid = this->dataGridView1;
			if ( !grid-> InvokeRequired )
			{
				rowNum = grid->Rows->Add();
				grid->Rows[rowNum]->Cells[0]->Value = "" + data->candidateid;

				Image ^ thumbnail;
				try
				{
					String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName;
					thumbnail = Image::FromFile( path ) ; 
					thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
				}
				catch( Exception ^ e)
				{
					System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
				}

				double lat, lon, alt;
				Vision::GeoReference::getTargetGPS(data, lat, lon, alt);

				grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
				grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
				grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
				grid->Rows[rowNum]->Cells[4]->Value = "0*";
				grid->Rows[rowNum]->Cells[5]->Value = data->processed;
				//grid->Rows[rowNum]->Cells[5]->Value = data->letter;
				//grid->Rows[rowNum]->Cells[6]->Value = data->letterColor;
				//grid->Rows[rowNum]->Cells[7]->Value = data->shape;
				//grid->Rows[rowNum]->Cells[8]->Value = data->shapeColor;
			}
			else
			{
				candidateTableUpdateDelegate = gcnew Delegates::candidateRowDataToVoid(this,&Form1::insertCandidateData);
				this->Invoke(unverifiedTableUpdateDelegate,data);
			}
		}
public: System::Void insertUnverifiedData( Database::UnverifiedRowData ^ data) 
		{
			if (data == nullptr) {
				
				//System::Diagnostics::Trace::WriteLine("ERROR in Form1::insertTargetData(): data is null");
				return;
			}

			int rowNum;

			DataGridView ^ grid = this->dataGridView2;
			
			if ( !grid-> InvokeRequired )
			{
				rowNum = grid->Rows->Add();
				grid->Rows[rowNum]->Cells[0]->Value = "" + data->targetid;
				Image ^ thumbnail;
				try
				{
					String ^ path = HTTP_SERVER_TARGET_PATH + data->candidate->imageName;
					// thumbnail = Image::FromFile( path );
					thumbnail = Image::FromFile( path );
					thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
				}
				catch( Exception ^ e)
				{
					System::Diagnostics::Trace::WriteLine("Failure to add image to table: " + e);
				}

				double lat, lon, alt;
				Vision::GeoReference::getTargetGPS(data, lat, lon, alt);

				grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
				grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
				grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
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
				unverifiedTableUpdateDelegate = gcnew Delegates::unverifiedRowDataToVoid(this,&Form1::insertUnverifiedData);
				this->Invoke(unverifiedTableUpdateDelegate,data);
			}

		}

public: System::Void fillGpsCheckboxList ( array<Waypoint ^>^ waypoints )
		{
			auto gpsCheckboxList = this->gpsCheckboxList;
			if ( gpsCheckboxList->InvokeRequired ){
				checkboxListDelegate = gcnew Delegates::waypointArraytoVoid(this,&Form1::fillGpsCheckboxList);
				this->Invoke(checkboxListDelegate,(Object ^)waypoints);
			}else{
				auto gpsListItems = this->gpsCheckboxList->Items;
				gpsListItems->Clear();
				for ( int i = 0; i < waypoints->Length; i++){
					gpsListItems->Add(waypoints[i],true);
				}
			}
		}

public: System::Void insertVerifiedTargetData( Database::VerifiedRowData ^ data) 
		{
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::VerifiedRowData(): data is null");
				return;
			}

			
			int rowNum;

			DataGridView ^ grid = this->dataGridView3;
			
			if ( !grid->InvokeRequired )
			{
				rowNum = grid->Rows->Add();
				grid->Rows[rowNum]->Cells[0]->Value = "" + data->submitid;

				Image ^ thumbnail;
				try
				{
					if (data->target->candidate->imageName != nullptr) 
					{
						String ^ path = HTTP_SERVER_TARGET_PATH + data->target->candidate->imageName;
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
				verifiedTableUpdateDelegate = gcnew Delegates::verifiedRowDataToVoid(this,&Form1::insertVerifiedTargetData);
				this->Invoke(verifiedTableUpdateDelegate,data);
			}
		}

public: System::Void modifyCandidateInTable( Database::CandidateRowData ^ data )
		{
			bool foundTarget = false;
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyCandidateInTable(): data is null");
				return;
			}

			try {
				int num = data->candidateid;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyCandidateInTable(): data is null");
				return;
			}

			DataGridView ^ grid = this->dataGridView1;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals("" + data->candidateid)) {
					foundTarget = true;
					// modify row
					int rowNum = i;
					grid->Rows[rowNum]->Cells[0]->Value = "" + data->candidateid;

					Image ^ thumbnail;
					try
					{
						String ^ path = HTTP_SERVER_TARGET_PATH + data->imageName;
						thumbnail = Image::FromFile( path );
						thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
					}
					catch( Exception ^ e)
					{
						System::Diagnostics::Trace::WriteLine("Form1::modifyCandidateInTable(): Failed to add image to table: " + e);
					}

					double lat, lon, alt;
					Vision::GeoReference::getTargetGPS(data, lat, lon, alt);

					grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
					grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[4]->Value = "0*";
					//grid->Rows[rowNum]->Cells[5]->Value = data->letter;
					//grid->Rows[rowNum]->Cells[6]->Value = data->letterColor;
					//grid->Rows[rowNum]->Cells[7]->Value = data->shape;
					//grid->Rows[rowNum]->Cells[8]->Value = data->shapeColor;
					grid->Rows[rowNum]->Cells[9]->Value = true;


				}
			}

			if (!foundTarget) 
				System::Diagnostics::Trace::WriteLine("Form1::modifyCandidateInTable() ERROR couldn't find corresponding row");

		} 

public: System::Void modifyUnverifiedInTable( Database::UnverifiedRowData ^ data )
		{
			bool foundTarget = false;
			if (data == nullptr) {
				
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyTargetInTable(): data is null");
				return;
			}

			try {
				int num = data->targetid;
			} 
			catch (Exception ^ ) {
				System::Diagnostics::Trace::WriteLine("ERROR in Form1::modifyTargetInTable(): data is null");
				return;
			}

			DataGridView ^ grid = this->dataGridView2;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals("" + data->targetid)) {
					foundTarget = true;
					System::Diagnostics::Trace::WriteLine("Form1::modifyTargetInTable() found target: " + data->description->letterColor);

					// modify row
					int rowNum = i;
					grid->Rows[rowNum]->Cells[0]->Value = "" + data->targetid;

					Image ^ thumbnail;
					try
					{
						String ^ path = HTTP_SERVER_TARGET_PATH + data->candidate->imageName;
						thumbnail = Image::FromFile( path );
						thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
					}
					catch( Exception ^ e)
					{
						System::Diagnostics::Trace::WriteLine("Form1::modifyTargetInTable(): Failed to add image to table: " + e);
					}

					double lat, lon, alt;
					Vision::GeoReference::getTargetGPS(data, lat, lon, alt);

					grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
					grid->Rows[rowNum]->Cells[2]->Value = "" + Single(lat).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[3]->Value = "" + Single(lon).ToString("######.#######") + "*";
					grid->Rows[rowNum]->Cells[4]->Value = "0*";
					grid->Rows[rowNum]->Cells[5]->Value = "("+ data->description->targetX + "," + data->description->targetY + ")";
					grid->Rows[rowNum]->Cells[6]->Value = data->description->letter;
					grid->Rows[rowNum]->Cells[7]->Value = data->description->letterColor;
					grid->Rows[rowNum]->Cells[8]->Value = data->description->shape;
					grid->Rows[rowNum]->Cells[9]->Value = data->description->shapeColor;
					grid->Rows[rowNum]->Cells[10]->Value = true;


				}
			}

			if (!foundTarget) 
				System::Diagnostics::Trace::WriteLine("Form1::modifyTargetInTable() ERROR couldn't find corresponding row");

		}

public: System::Void removeUnverifiedFromTable(String ^ id)
		{
			DataGridView ^ grid = this->dataGridView1;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals(id)) {
					// remove row
					grid->Rows->RemoveAt(i);
				}
			}

		}
public: System::Void removeUnverifiedFromTable(Database::CandidateRowData ^ data)
		{
			removeUnverifiedFromTable("" + data->candidateid);
		}

public: System::Void removeTargetFromTable(String ^ id)
		{
			DataGridView ^ grid = this->dataGridView2;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[0]->Value->Equals(id)) {
					// remove row
					grid->Rows->RemoveAt(i);
				}
			}

		}
public: System::Void removeTargetFromTable(Database::UnverifiedRowData ^ data)
		{
			removeTargetFromTable("" + data->targetid);
		}
#define targetid_VERIFIED_COL 9
public: System::Void removeVerifiedTargetFromTable(String ^ id)
		{
			if (BADSTRING(id))
				return;

			DataGridView ^ grid = this->dataGridView3;

			for (int i = 0; i < grid->Rows->Count; i++) {
				if (grid->Rows[i]->Cells[targetid_VERIFIED_COL]->Value->Equals(id)) {
					// remove row
					grid->Rows->RemoveAt(i);
				}
			}

		}

/*
public: System::Void insertTable( Database::TableName table, Database::RowData ^ data )
		{
			int rowNum;
			DataGridView ^ grid;
			switch( table )
			{
				case Database::tableCandidateRegions:
					grid = this->dataGridView1;
					break;
				case Database::tableUnverifiedTargets:
					grid = this->dataGridView2;
					break;
				case Database::tableVerifiedTargets:
					grid = this->dataGridView3;
					break;
			}

			rowNum = grid->Rows->Add();
			grid->Rows[rowNum]->Cells[0]->Value = "" + data->id;

			Image ^ thumbnail;
			/*try //DEBUG: I removed this to test
			{
				String ^ path = data->path->Replace("\\\\", "\\");
				thumbnail = Image::FromFile( path );
				thumbnail = thumbnail->GetThumbnailImage( 64, 64, nullptr, IntPtr::Zero );
			}
			catch( Exception ^ )
			{
		System::Diagnostics::Trace::WriteLine("catch in form1");
			}*/
/*
			grid->Rows[rowNum]->Cells[1]->Value = thumbnail;
			grid->Rows[rowNum]->Cells[2]->Value = "" + data->target_latitude;
			grid->Rows[rowNum]->Cells[3]->Value = "" + data->target_longitude;
			grid->Rows[rowNum]->Cells[4]->Value = "" + data->heading;
			grid->Rows[rowNum]->Cells[5]->Value = data->letter;
			grid->Rows[rowNum]->Cells[6]->Value = data->shape;
			grid->Rows[rowNum]->Cells[7]->Value = data->fg_color;
			grid->Rows[rowNum]->Cells[8]->Value = data->bg_color;
			grid->Rows[rowNum]->Cells[9]->Value = data->processed;

		}
*/
public: System::Void saveImage(){
			/*DateTime time = DateTime::Now;

			String ^ path = vidOptOutputDirText->Text + "\\image_" + time.ToString("o")->Replace(":", "-") + fileExtension;
			String ^ pathBase = vidOptOutputDirText->Text + "\\image_" + time.ToString("o")->Replace(":", "-");

			Database::RowData ^ data = gcnew Database::RowData();
			data->id = this->incrId++;
			data->path = path->Replace("\\", "\\\\");
			data->target_latitude = 0;			// Latitude of point selected as target
			data->target_longitude = 0;			// Longitude of point selected as target
			data->target_X = 0;					// pixel X of target
			data->target_Y = 0;					// pixel Y of target
			data->heading = 0;					// heading of target
			data->letter = "A";				
			data->shape = "Circle";					
			data->fg_color = "White";				// foreground color
			data->bg_color = "Yellow";				// background color
			data->processed = FALSE;					// whether this has been processed by OpenCV
			//data->verified = FALSE;					// human verified
			data->center_latitude = 0;			// Latitude of center pixel
			data->center_longitude = 0;			// Longitude of center pixel
			data->mapping_latitude = (float)Convert::ToDouble( this->metadataTable[1, A_LAT]->Value );			// pixel to meter translation for latitude
			data->mapping_longitude = (float)Convert::ToDouble( this->metadataTable[1, A_LON]->Value );		// pixel to meter translation for longitude
			

			//openGLView->saveImage( path );
			openGLView->saveImage( path, pathBase, data->homography, Convert::ToDouble( this->metadataTable[1, A_HEAD]->Value ) );

			this->insertTable( Database::tableUnverifiedTargets, data);
			db->insertData( Database::tableUnverifiedTargets, data);*/
		}

/*public: System::Void addRawTarget(Database::RowData ^ data) {
			
			//System::Diagnostics::Trace::WriteLine( "FORM1 RAN addRawTarget");
			//System::Diagnostics::Trace::WriteLine( "data: ");
			data->id = this->incrId++;
			this->insertTable( Database::tableCandidateRegions, data);
			//db->insertData( Database::tableCandidateRegions, data);
			//System::Diagnostics::Trace::WriteLine( "FORM1 FINISHED addRawTarget");
		}
*/

public: System::Void noComportData( ) {
			if( isReadingData )
			{
				errorMessage( "No COM link to airplane" );
				isReadingData = false;
			}
		 }

public: System::Void comportEstablish( ) {
			if( !isReadingData )
			{
				consoleMessage( "COM link established", Color::Green );
				isReadingData = true;
			}
		}

private: System::Void checkedListBox1_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
			  IEnumerator^ myEnum2 = checkedListBox1->CheckedItems->GetEnumerator();
		      while ( myEnum2->MoveNext() )
		      {
		         Object^ itemChecked = safe_cast<Object^>(myEnum2->Current);
				//if (Compare(itemChecked
		         // Use the IndexOf method to get the index of an item.
		         MessageBox::Show( String::Concat( "Item with title: \"", itemChecked, "\", is checked. Checked state is: ", checkedListBox1->GetItemCheckState( checkedListBox1->Items->IndexOf( itemChecked ) ), "." ) );
		      }
		 }
private: System::Void checkBox1_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			
			 /*if (checkBox1->Checked) {
				 openGLView->switchSaliencyState( true );
				 
				 consoleMessage( "Displaying video and saliency", Color::Green );
			
			 } else {
				 openGLView->switchSaliencyState( false );
				 consoleMessage( "Displaying video but not saliency", Color::Green );
			
			 }*/
			 
		     

}

private: void AddText( Stream^ fs, String^ value )
{
   array<Byte>^info = (gcnew UTF8Encoding( true ))->GetBytes( value );
   fs->Write( info, 0, info->Length );
}

public: System::Void updateIntendedGimbalPosition( float rollDegrees, float pitchDegrees )
		{
			if (rollDegrees > -360.0f && rollDegrees < 360.0f && pitchDegrees > -360.0f && pitchDegrees < 360.0f)
				gimbalHUDView->setIntendedGimbalPosition( rollDegrees, pitchDegrees, true);
			
		}

public: System::Void updateIntendedCameraZoom( float zoom )
		{
			if (zoom > -0.01f)
				gimbalHUDView->setIntendedCameraZoom( zoom, true);
		}
		 
public: System::Void reloadTable(ImageWithPlaneData^ state) 
		{
			this->metadataTable[1, G_ROLL]->Value = "" + state->gimbalRoll + "*";
			this->metadataTable[1, G_PITCH]->Value = "" + state->gimbalPitch + "*"; //elevation;
			this->metadataTable[1, A_ALT]->Value = "" + state->altitude + " (m)"; // altitude;
			this->metadataTable[1, A_LAT]->Value = "" + state->latitude + "*"; // latitude;
			this->metadataTable[1, A_LON]->Value = "" + state->longitude + "*"; // longitude; // ######.####
			this->metadataTable[1, A_HEAD]->Value = "" + state->yaw + "*"; //heading;
			this->metadataTable[1, A_ROLL]->Value = "" + state->roll + "*"; // roll;
			this->metadataTable[1, A_PITCH]->Value = "" + state->pitch + "*"; //pitch;
			this->metadataTable[1, V_ZOOM]->Value = "LOL"; // zoom;
			
			 gimbalHUDView->setGimbalPosition( (float)state->gimbalRoll, (float)state->gimbalPitch, false );
		}

public: System::Void updateGimbalInfo( Communications::GimbalInfo ^ data ) {
			//System::Diagnostics::Trace::WriteLine("Form1::updateGimbalInfo() BLAHBLAHBLAH");
			// save data for later
			thePlaneWatcher->updateGimbalInfo(data);



		}

public: System::Void updatePlaneGPSInfo( Communications::PlaneGPSPacket ^ data ) {
			thePlaneWatcher->updatePlaneGPSInfo(data);

			//TODO: update the table the user sees in el GUI
}

public: System::Void updatePlaneTelemInfo( Communications::PlaneTelemPacket ^ data ) {
			thePlaneWatcher->updatePlaneTelemInfo(data);

			//TODO: update the table the user sees in el GUI
}

public: System::Void updateComData( Communications::ComportDownstream * data ) {
			// TODO: split this function into many others

			/* if( !isReadingData )
			 {
			 	this->Invoke( this->comportEstablishDelegate );
			 }

			 // Airplane metadata
			 this->metadataTable[1, A_ALT]->Value = data->gps_alt;
			 this->metadataTable[1, A_LAT]->Value = data->gps_lat;
			 this->metadataTable[1, A_LON]->Value = data->gps_lon;
			 this->metadataTable[1, A_HEAD]->Value = data->airplane_heading * 180.0 / 3.14159;
			 this->metadataTable[1, A_ROLL]->Value = data->airplane_roll  * 180.0 / 3.14159;
			 this->metadataTable[1, A_PITCH]->Value = data->airplane_pitch  * 180.0 / 3.14159;

			 // Gimbal stuff
			 float azimuth = data->gimbal_azimuth % 1600;
			 azimuth *= 360.0f / 1600.0f;
			 azimuth *= -1.0f;
			 if( azimuth > 180.0f )
				 azimuth = (azimuth - 360.0f);
			 if( azimuth < -180.0f )
				 azimuth = (azimuth + 360.0f);

			 float elevation = data->gimbal_elevation % 1600;
			 elevation *= 360.0f / 1600.0f;
			 //elevation -= 180.0f;

			 this->metadataTable[1, G_AZIM]->Value = data->gimbal_roll; //azimuth;
			 this->metadataTable[1, G_ELEV]->Value = data->gimbal_pitch; //elevation;

			 // Camera Stuff
			 this->metadataTable[1, V_ZOOM]->Value = Communications::decodeZoomFloat( data->camera_zoom );			 

			 // Update local zoom
			 m_joystick->setZoom( Communications::decodeZoomInt( data->camera_zoom ) );
	
			 // Log data
			 try
			 {				 
				DateTime timeTemp = DateTime::Now;
				String ^ text = timeTemp.ToString("o")->Replace(":", "-") + " " + 
					this->metadataTable[1, A_ALT]->Value + " " +
					this->metadataTable[1, A_LAT]->Value + " " +
					this->metadataTable[1, A_LON]->Value + " " +
					this->metadataTable[1, A_HEAD]->Value + " " +
					this->metadataTable[1, A_ROLL]->Value + " " +
					this->metadataTable[1, A_PITCH]->Value + " " +
					azimuth + " " +
					elevation + " " +
					this->metadataTable[1, V_ZOOM]->Value + "\n";
				 AddText( logFile, text );			 	
			 }
			 catch( Exception ^ )
			 {
				System::Diagnostics::Trace::WriteLine("catch in comport");
			 }

			 delete data;*/
		 }
private: System::Void splitLengthTextBox_Validated(System::Object^  sender, System::EventArgs^  e) {
			 try
			 {
				 int temp = Convert::ToInt32( this->splitLengthTextBox->Text );

				 if( temp < 0 )
					 throw gcnew Exception();

				 splitLength = temp;
				 
				 consoleMessage( "Video split length changed to " + splitLength, Color::Gray );			
			 }
			 catch( Exception ^ )
			 {
				 this->splitLengthTextBox->Text =  Convert::ToString( splitLength );
			 }
		 }

private: System::Void encodingComboBox_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
		 
			// openGLView->changeEncoding( this->encodingComboBox->Text );
			// consoleMessage( "Video encoding changed to " + this->encodingComboBox->Text, Color::Gray );			
		 
		 }
private: System::Void startRecordButton_Click(System::Object^  sender, System::EventArgs^  e) {
		 	
			 // stop recording
			 if( recording ) {

				theSimHandler->endRecording();
				consoleMessage( "Stopped recording video and telemetry", Color::Green );	
				recording = false;
				System::Diagnostics::Trace::WriteLine("stopped recording in Form1");

				// update button
				((Windows::Forms::ButtonBase ^)sender)->Text = "Start Recording";
				
				pictureBox1->Image = dynamic_cast<Image^>(redImage);
			}

			// start recording
			else {
				DateTime time = DateTime::Now;
				String ^ path;//vidOptOutputDirText->Text + "\\video_" + time.ToString("o")->Replace(":", "-") + fileExtensionVideo;
			
				path = "D:\\Skynet Files\\video\\video_" + time.ToString("o")->Replace(":", "-");// + fileExtensionVideo;

				//recordStart = DateTime::Now;
				//openGLView->enableVideoRecording( path );
				// START TELEMETRY HERE
				if (!theSimHandler->beginRecording()) {
					// something went wrong
					consoleMessage( "Failed to Start Recording", Color::Red );

				}

				else {
					// recording began
					consoleMessage( "Started recording-telemetry", Color::Green );	
					recording = true;
					System::Diagnostics::Trace::WriteLine("started recording in Form1 " + path);
			
					
					// update button
					((Windows::Forms::ButtonBase ^)sender)->Text = "Stop Recording";
					pictureBox1->Image = dynamic_cast<Image^>(greenImage);
				}
			}
		 }
private: System::Void stopRecordButton_Click(System::Object^  sender, System::EventArgs^  e) {
		 	if( !recording )
				return;

			// no longer used (moved to startRecording)
		 }


private: System::Void videoSaveTimer_Tick(System::Object^  sender, System::EventArgs^  e) {
		// REMOVED
		return;
		


		if( !recording )
			return;

		DateTime time = DateTime::Now;
		time = time.AddSeconds( -Convert::ToInt32( SPLIT_LENGTH ) );

		if( DateTime::Compare( recordStart, time ) < 0 )
		{
			//recordStart = DateTime::Now;
			
			theSimHandler->beginRecording();
			consoleMessage( "Splitting video file", Color::Gray );	
			System::Diagnostics::Trace::WriteLine("split recording in Form1 ");
		}
	}
private: System::Void openGLPanel_Paint(System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e) {
		 }



private: System::Void mapnikToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //mapView->SetTileProvider( Mapnik );
			 //mapView->SetCacheDirectory( defaultMapCache + "\\Mapnik", false ); DELETE MAP
			 mapnikToolStripMenuItem->Checked = true;
			 osmarenderToolStripMenuItem->Checked = false;
			 cyclemapToolStripMenuItem->Checked = false;
		 }

private: System::Void osmarenderToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //mapView->SetTileProvider( Osmarender );
			 //mapView->SetCacheDirectory( defaultMapCache + "\\Osmarender", false );
			 mapnikToolStripMenuItem->Checked = false;
			 osmarenderToolStripMenuItem->Checked = true;
			 cyclemapToolStripMenuItem->Checked = false;
		 }

private: System::Void cyclemapToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //mapView->SetTileProvider( Cyclemap );
			 //mapView->SetCacheDirectory( defaultMapCache + "\\Cyclemap", false );
			 mapnikToolStripMenuItem->Checked = false;
			 osmarenderToolStripMenuItem->Checked = false;
			 cyclemapToolStripMenuItem->Checked = true;
		 }

private: System::Void mapView_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			//String ^ gps = mapView->GetPositionString( e->X, e->Y );
//			mapMouseGPSLabel->Text = "Mouse GPS: " + gps;
		 }
private: System::Void dataGridView2_CurrentCellDirtyStateChanged(System::Object^  sender, System::EventArgs^  e) {
			/*static bool letterChanging = false;
			static bool boxChanges = false;
			String ^ field;
			String ^ value;
			String ^ rowID;

			DataGridView ^ table = dynamic_cast<DataGridView ^>(sender);
			DataGridViewCell ^ curCell = table->CurrentCell;
			System::Diagnostics::Trace::WriteLine(table->CurrentCell);

			if( letterChanging && curCell->ColumnIndex == 5 ) // handle letter
			{
				letterChanging = false;

				// Commit changes
				table->CommitEdit( DataGridViewDataErrorContexts::Commit );
				field = "letter";
				value = "" + curCell->Value;
				rowID = "" + curCell->OwningRow->Cells[0]->Value;

				//candidate
				if(table == dataGridView1)
				{
					//db->updateValue( Database::tableCandidateRegions, field, value, rowID );
				}

				//unconfirmed
				else if (table == dataGridView2)
				{
					//db->updateValue( Database::tableUnverifiedTargets, field, value, rowID );
				}

				//confirmed
				else if (table == dataGridView3)
				{
					//db->updateValue( Database::tableVerifiedTargets, field, value, rowID );
				}
			}
			else if( curCell->ColumnIndex > 5 && curCell->ColumnIndex < 11  && !boxChanges ) // combo boxes and check boxes
			{
				
				switch( curCell->ColumnIndex )
				{
				case 6: // shape combo box
					field = "shape";
					break;
				case 7: // fg combo box
					field = "fg_color";
					break;
				case 8: // bg combo box
					field = "bg_color";
					break;
				case 9: // do nothing for this one
					break;
				case 10: // verified check box
					field = "verified";
					break;

				}

				boxChanges = true;

				// Commit changes
				table->CommitEdit( DataGridViewDataErrorContexts::Commit );
				value = "" + curCell->Value;
				rowID = "" + curCell->OwningRow->Cells[0]->Value;

				//table is candidate regions
				if (table == dataGridView1)
				{
					//db->updateValue( Database::tableCandidateRegions, field, value, rowID );

					//move from candidate to confirmed if 'verified' is checked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;
						table->Rows->Remove(row);
					 	dataGridView3->Rows->Add(row);
						//db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );					
					}
				}

				//table is unverified targets
				else if (table == dataGridView2)
				{
					db->updateValue( Database::tableUnverifiedTargets, field, value, rowID );

					//move from unconfirmed to confirmed if 'verified' is checked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;

						// Add to map
						mapView->AddTarget( Convert::ToDouble( row->Cells[2]->Value ), // lat
											Convert::ToDouble( row->Cells[3]->Value ), // lon
											Convert::ToInt32( row->Cells[0]->Value ) ); // id

						table->Rows->Remove(row);
				 		dataGridView3->Rows->Add(row);
						db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );					
					}
				}

				//table is verified targets
				else if (table == dataGridView3)
				{
					db->updateValue( Database::tableVerifiedTargets, field, value, rowID );

					//move from confirmed to unconfirmed if 'verified' is unchecked
					if(field == "verified")
					{
						DataGridViewRow ^ row = curCell->OwningRow;

						// Remove from map
						mapView->RemoveTarget( Convert::ToInt32( row->Cells[0]->Value ) ); // id

						table->Rows->Remove(row);
				 		dataGridView2->Rows->Add(row);
						db->move( Database::tableVerifiedTargets, Database::tableUnverifiedTargets, value );						
					}
				}
			}
			else
			{
				letterChanging = true;
				boxChanges = false;
			}*/
		 }
private: System::Void resetToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 appController->clearAllTables();

			 this->dataGridView1->Rows->Clear();
			 this->dataGridView2->Rows->Clear();
			 this->dataGridView3->Rows->Clear();
		 }

private: System::Void exportDataToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) 
			{
				FolderBrowserDialog^ fileDialog = gcnew FolderBrowserDialog;

				if ( fileDialog->ShowDialog() == ::DialogResult::OK )
				{
					appController->exportData(fileDialog->SelectedPath + "\\");
				}

			}

private: System::Void mapUpdateTimer_Tick(System::Object^  sender, System::EventArgs^  e) {	

			 // REMOVED

			 return;

			 // Update Map position of airplane
			 if( isReadingData )
			 {
				
				System::Diagnostics::Trace::WriteLine("Set new airplane location: " + Convert::ToDouble(this->metadataTable[1, A_LAT]->Value) + ", " + Convert::ToDouble(this->metadataTable[1, A_LON]->Value));
				 
				/* array<float> ^ homography = GeoReference::computeHomography( Convert::ToDouble(this->metadataTable[1, A_LAT]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_LON]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_LAT]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_ROLL]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_PITCH]->Value),
					 Convert::ToDouble(this->metadataTable[1, A_HEAD]->Value),
					 Convert::ToDouble(this->metadataTable[1, G_AZIM]->Value),
					 Convert::ToDouble(this->metadataTable[1, G_ELEV]->Value),
					 Convert::ToDouble(this->metadataTable[1, V_ZOOM]->Value) );*/

				 /*mapView->SetCameraLookLocation( homography, 
					 Convert::ToDouble(this->metadataTable[1, A_LAT]->Value), 
					 Convert::ToDouble(this->metadataTable[1, A_LON]->Value) );*/
			 }
		 }

private: System::Void lockToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( lockToolStripMenuItem->Checked )
			 {
//				 mapView->SetUILocked( false ); DELETE MAP
				 lockToolStripMenuItem->Checked = false;
			 }
			 else
			 {
//				mapView->SetUILocked( true ); DELETE MAP
			 	lockToolStripMenuItem->Checked = true;
			 }
				

		 }
private: System::Void downloadToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 if( downloadToolStripMenuItem->Checked )
			 {
//				 mapView->SetDownload( false ); DELETE MAP
				 downloadToolStripMenuItem->Checked = false;
			 }
			 else
			 {
//				mapView->SetDownload( true ); DELETE MAP
			 	downloadToolStripMenuItem->Checked = true;
			 }
		 }
private: System::Void mapLookGPSToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
//				double lat, lon;
				double north = 0.0f, east = 0.0f;
				//char zone[4];
//				mapView->GetPosition(mouseGPS.X, mouseGPS.Y, lon, lat); DELETE MAP


				// fix georeferencing
				//GeoReference::LLtoUTM(23, lat, lon, north, east, zone);


				Communications::ComportUpstream * packet = new Communications::ComportUpstream();
				packet->gps_lat_gimbal_x.f = (float)north;
				packet->gps_lon_gimbal_y.f = (float)east;
				packet->update_type = 0x0B;

				//for( int i = 0; i < 100; ++i ) //theComport->writeData(packet); // TODO: send data correctly

				delete packet;

		 }
private: System::Void mapView_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {
			 	if( e->Button != ::MouseButtons::Right )
					return;

				mouseGPS = e->Location;
//				mapMenuStrip->Show( mapView, mouseGPS ); DELETE MAP


		 }
		 
private: System::Void dataGridView1_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 // if( e->ColumnIndex < 1 || e->ColumnIndex > 4 )
			//	 return;

			 if( e->RowIndex < 0 )
				 return;

			 String ^ id = Convert::ToString(dataGridView1->Rows[e->RowIndex]->Cells[0]->Value);
			 
			 viewCandidate(appController->candidateWithID(id));
		 }

		 
private: System::Void dataGridView2_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 //if( e->ColumnIndex < 1 || e->ColumnIndex > 4 )
			//	 return;

			 if( e->RowIndex < 0 )
				 return;

			 if ( e->ColumnIndex == 11 ) {

				 // get data
				 String ^ id = Convert::ToString(dataGridView2->Rows[e->RowIndex]->Cells[0]->Value);
				 Database::UnverifiedRowData ^ data = appController->unverifiedWithID(id);

				 // initiate target lock
				 double lat, lon, alt;
				 Vision::GeoReference::getTargetGPS(data, lat, lon, alt);
				 targetLock->lockOntoGPS( (float)lat, (float)lon );
			 }

			 else {
				 // show dialog
				 String ^ id = Convert::ToString(dataGridView2->Rows[e->RowIndex]->Cells[0]->Value);
			 
				 //editUnverifiedTargetUsingDialog(appController->unverifiedWithID(id));
				 viewCandidate(appController->unverifiedWithID(id)->candidate);
			 }

		 }
#define VERIFIED_LOCK_ON_COLUMN 10
private: System::Void dataGridView3_CellDoubleClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e) {
			 if( e->RowIndex < 0 )
				 return;

			 if (e->ColumnIndex == VERIFIED_LOCK_ON_COLUMN) {
				 
				 String ^ id = Convert::ToString(dataGridView3->Rows[e->RowIndex]->Cells[targetid_VERIFIED_COL]->Value);
				 Database::UnverifiedRowData ^ data = appController->unverifiedWithID(id);

				 if (data == nullptr) {
					 this->errorMessageUniversal("Could not find corresponding target to lock on to");
					 return;
				 }
				 // initiate target lock
				 double lat, lon, alt;
				 Vision::GeoReference::getTargetGPS(data, lat, lon, alt);

				 if (lat > 180 || lon > 180) {
					 this->errorMessageUniversal("Could not calculate GPS to lock on to");
					 return;
				 }

				 targetLock->lockOntoGPS( (float)lat, (float)lon );

			 }
			 else {
				 String ^ id = Convert::ToString(dataGridView3->Rows[e->RowIndex]->Cells[0]->Value);
			 
				 //editVerifiedTargetUsingDialog(appController->verifiedWithID(id));
				 viewCandidate(appController->verifiedWithID(id)->target->candidate);
			 }
			 //System::Diagnostics::Trace::WriteLine("ERROR: dataGridView3_CellDoubleClick(): no methods in place to edit verified targets. tell developers to get back to work.");
		 }

public: System::Void editUnverifiedTargetUsingDialog(Database::UnverifiedRowData ^ data) 
		{
			if (data == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR: editUnverifiedTargetUsingDialog(): data == nullptr.");
				return;
			}
			imageDialog = gcnew TargetDialog( this, appController );
			 
			imageDialog->showDialogForData(data);
			//imageDialog->Show();

			//imageDialog->targetid = Convert::ToInt32( dataGridView3->Rows[e->RowIndex]->Cells[0]->Value );
			//imageDialog->RowID = e->RowIndex;
			//imageDialog->ImagePath = db->getField(Database::tableVerifiedTargets, 1, id);
			//imageDialog->Latitude = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[2]->Value );
			//imageDialog->Longitude = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[3]->Value );
			//imageDialog->Heading = Convert::ToDouble( dataGridView3->Rows[e->RowIndex]->Cells[4]->Value );
			 
		}


public: System::Void viewCandidate(Database::CandidateRowData ^ data)
{
	if (data == nullptr) {
		System::Diagnostics::Trace::WriteLine("ERROR: editUnverifiedTargetUsingDialog(): data == nullptr.");
		return;
	}
	imageDialog = gcnew TargetDialog( this, appController );
			 
	imageDialog->showDialogForData(data);
	//imageDialog->Show();

}


public: System::Void editVerifiedTargetUsingDialog(Database::VerifiedRowData ^ theData) 
		{
			if (theData == nullptr) {
				System::Diagnostics::Trace::WriteLine("ERROR: editVerifiedTargetUsingDialog(): data == nullptr.");
				return;
			}
			imageDialog = gcnew TargetDialog( this, appController );
			 
			imageDialog->showDialogForData(theData);
		}

		 //public: Vision::ocrItem ^ getCandidateData( void )
		 //{
		 //	DataGridViewCellCollection ^ row;
			//try
			//{
			//	row = dataGridView1->Rows[0]->Cells;
			//}
			//catch( Exception ^ )
			//{
			//	// no data to take
			//	return nullptr;
			//}

			//// Set up return value
			//String ^ id = Convert::ToString( row[0]->Value );
			//Vision::ocrItem ^ retVal = gcnew Vision::ocrItem();			
			//retVal->_homography = db->getHomography( Database::tableCandidateRegions, id );
			//retVal->_row = dataGridView1->Rows[0];
			//retVal->_heading = Convert::ToDouble( row[4]->Value );
			//retVal->_path = db->getField(Database::tableCandidateRegions, 1, id);

			//// Remove row from this table
			//dataGridView1->Rows->Remove( dataGridView1->Rows[0] );

			////dataGridView3->Rows->Add(row);
			////db->move( Database::tableUnverifiedTargets, Database::tableVerifiedTargets, value );

			//return retVal;
		 //}

		 public: void ocrUpdateData( DataGridViewRow ^ row )
		 {
		 	/*DataGridViewCellCollection ^ cells = row->Cells;
			String ^ id = Convert::ToString( cells[0]->Value );
			
			// Update all values in old entry in database
			db->updateValue( Database::tableCandidateRegions, "shape", Convert::ToString( cells[COL_SHAPE]->Value ), id );
			db->updateValue( Database::tableCandidateRegions, "letter", Convert::ToString( cells[COL_LETTER]->Value ), id );
			db->updateValue( Database::tableCandidateRegions, "processed", "TRUE", id );

			// Move into unverified
			db->moveProcessed( Database::tableCandidateRegions, Database::tableUnverifiedTargets, "TRUE" );
			dataGridView2->Rows->Add(row);*/
		 }

		
		 public: void imageDialogCallback( int rowID, int tarID, double lat, double lon, double heading )
		 {
		 	/*DataGridViewCellCollection ^ row = dataGridView3->Rows[rowID]->Cells;

			// Update table
			row[2]->Value = Convert::ToString( lat );
			row[3]->Value = Convert::ToString( lon );
			row[4]->Value = Convert::ToString( heading );

			// Update database
			db->updateValue( Database::tableVerifiedTargets, "target_lat", Convert::ToString( lat ), Convert::ToString( tarID ) );
			db->updateValue( Database::tableVerifiedTargets, "target_lon", Convert::ToString( lon ), Convert::ToString( tarID ) );
			db->updateValue( Database::tableVerifiedTargets, "heading", Convert::ToString( heading ), Convert::ToString( tarID ) );

			// Update target in map
			mapView->AddTarget( lat, lon, tarID );*/
		 }
private: System::Void choosePathToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {

			 // ask user for filename
			String ^ temp;
		 
			System::Windows::Forms::DialogResult result = simReadVidDialog->ShowDialog();

			if ( result == ::DialogResult::OK )
			{
				temp = simReadVidDialog->FileName;
				
				theSimHandler->startPlayback(temp);

				//vidOptOutputDirText->Text = temp;
				System::Diagnostics::Trace::WriteLine( "Simulator input path changed to: " + temp);
			}

			// if user presses cancel, then DONT START VIDEO!
		 }
private: System::Void startToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 //theVideoSimulator->startVideo();
			 //callback->dontShow();

			 //THIS IS NEVER CALLED
		 }
private: System::Void pauseToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 
			 if (theSimHandler->isPaused()) {

				theSimHandler->resumePlayback();
				pauseToolStripMenuItem->Text = "Pause";

				//System::Diagnostics::Trace::WriteLine( "PAUSing AHHHHHH" );

			 } else {
				pauseToolStripMenuItem->Text = "Play";
				theSimHandler->pausePlayback();

			 }

		 }
private: System::Void stopToolStripMenuItem_Click(System::Object^  sender, System::EventArgs^  e) {
			 theSimHandler->endPlayback();
		 }
private: System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e) {
			
			 System::Diagnostics::Trace::WriteLine( "form1 - keydown" );

			array<System::Byte> ^ buffer = nullptr;


			if (e->KeyCode == Keys::Q) {
				// send zoom in packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x24;
				buffer[0] = 0xFF;
				System::Diagnostics::Trace::WriteLine( "zoom in" );
			}

			else if (e->KeyCode == Keys::A) {
				// send stop zoom packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x00;
				buffer[0] = 0xFF;

			}

			else if (e->KeyCode == Keys::Z) {
				// send zoom out packet
				buffer = gcnew array<System::Byte>(6);
				buffer[0] = 0x81;
				buffer[0] = 0x01;
				buffer[0] = 0x04;
				buffer[0] = 0x07;
				buffer[0] = 0x34;
				buffer[0] = 0xFF;

			}

			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this

		 }
private: System::Void Form1_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 System::Diagnostics::Trace::WriteLine( "form1 - keypress" );

		 }
private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {
			/*array<System::Byte> ^ buffer = nullptr;


			// send zoom in packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x24;
			buffer[5] = 0xFF;
			System::Diagnostics::Trace::WriteLine( "zoom in" );
			

			if (buffer != nullptr)
				;//theComport->writeRawData(buffer); // TODO: fix this*/

			 //theComms->gotoLatLon(0,0);
			 theComms->sendHelloToRabbit();
		 }
private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {
			array<System::Byte> ^ buffer = nullptr;


			// send stop zoom packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x00;
			buffer[5] = 0xFF;


			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this
		 }
private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
			array<System::Byte> ^ buffer = nullptr;


			// send zoom out packet
			buffer = gcnew array<System::Byte>(6);
			buffer[0] = 0x81;
			buffer[1] = 0x01;
			buffer[2] = 0x04;
			buffer[3] = 0x07;
			buffer[4] = 0x34;
			buffer[5] = 0xFF;

			

			//if (buffer != nullptr) //theComport->writeRawData(buffer); // TODO: fix this
		 }
private: System::Void Form1_Click(System::Object^  sender, System::EventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine( "click\nKEYPRESS\nKEYPRESS\nKEYPRESS\nKEYPRESS\nKEYPRESS\NKEYPRESS\nKEYPRESS\n" );
		 }
private: System::Void errorLogTextBox_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) {
			 System::Diagnostics::Trace::WriteLine( "errorlog - keypress" );
		 }
public:	 System::Void handleConnectionResult(array<Int32> ^ retArr) {

			int result = (int)retArr[0];
			isConnecting = false;

			/*if (result == RABBIT_CONNECTED) {
					consoleMessage( "Connected to Rabbit", Color::Green );
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
			} else if (result == AUTOPILOT_CONNECTED) {
					consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);
			}*/

			if (result == BOTH_FAILED) {
				// both failed to connect
					//consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Failed to Connect to Autopilot", Color::Red );
					pictureBox2->Image = dynamic_cast<Image^>(redImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);

			}

			else {
				// recording began
				if (result == BOTH_CONNECTED) {
				//	consoleMessage( "Connected to Rabbit", Color::Green );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
				}/* else if (result == RABBIT_CONNECTED) {
					consoleMessage( "Connected to Rabbit", Color::Green );
					consoleMessage( "Failed to Connect to Autopilot", Color::Red );
					pictureBox2->Image = dynamic_cast<Image^>(redImage);
					pictureBox3->Image = dynamic_cast<Image^>(greenImage);
				}*/ else if (result == AUTOPILOT_CONNECTED) {
					//consoleMessage( "Failed to Connect to Rabbit", Color::Red );
					consoleMessage( "Connected to Autopilot", Color::Green );
					pictureBox2->Image = dynamic_cast<Image^>(greenImage);
					pictureBox3->Image = dynamic_cast<Image^>(redImage);
				}

				isconnected = true;
				System::Diagnostics::Trace::WriteLine("Connected");
			
					
				// update button
				//this->button3 = (gcnew System::Windows::Forms::Button());
				this->button4->Text = "Disconnect";
				
			}



		 }
private: System::Void connectButton_Click(System::Object^  sender, System::EventArgs^  e) {
			 // disconnect
			 if( isconnected ) {

				 
				theComms->disconnectAll();
				consoleMessage( "Disconnected from Comms", Color::Green );	
				isconnected = false;

				System::Diagnostics::Trace::WriteLine("Disconnected from Comms in Form1");
				
				// update button
				((Windows::Forms::ButtonBase ^)sender)->Text = "Connect";
				pictureBox2->Image = dynamic_cast<Image^>(redImage);
				pictureBox3->Image = dynamic_cast<Image^>(redImage);
			}

			// connect
			else {

				if (isConnecting)
					return;
				isConnecting = true;

				consoleMessage( "Connecting...", Color::Green );

				// spawn thread
				Thread ^ comConnectThread = gcnew Thread(gcnew ThreadStart(theComms, &Communications::Comms::connectAll));
				comConnectThread->Name = "Comms Connect Thread";
				comConnectThread->Start();

			}

		 }


private: System::Void dataGridView1_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView1_DataError: " + e);
		 }
private: System::Void dataGridView2_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView2_DataError: " + e);
		 }
private: System::Void dataGridView3_DataError(System::Object^  sender, System::Windows::Forms::DataGridViewDataErrorEventArgs^  e) {
			 //System::Diagnostics::Trace::WriteLine("Form1::dataGridView3_DataError: " + e);
		 }

private: System::Void autosearchBox_LoadCompleted(System::Object^  sender, System::ComponentModel::AsyncCompletedEventArgs^  e) {
			PRINT("AutosearchBox Load Completed");
		 }
private: System::Void sendWaypointsButton_Click(System::Object^  sender, System::EventArgs^  e) {
			PRINT("Waypoints button is clicked");
			PRINT("GPS Coordinates to send: ");//
			//auto commandArray = gpsCheckboxList->CheckedItems->Cast<UAVCommand ^>();
			//auto commandArray = Enumerable::ToArray(Enumerable::Cast<UAVCommand ^>(gpsCheckboxList->CheckedItems));
			
			array<Waypoint ^>^ commandArray = gcnew array<Waypoint ^>(gpsCheckboxList->CheckedItems->Count);
			IEnumerator^ cmdEnum = gpsCheckboxList->CheckedItems->GetEnumerator(); 
			for (int i = 0; cmdEnum->MoveNext(); i++)
			{
				commandArray[i] = (Waypoint ^)cmdEnum->Current;
			}
			
			theComms->sendAutopilotWaypoints(commandArray); // DEBUG: send actual waypoints
		 }
private: System::Void stopToolStripMenuItem1_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 theComms->sendWifiAbortCommand();
		 }
private: System::Void button5_Click(System::Object^  sender, System::EventArgs^  e) {
		if ( appController->getIntelligenceController() == nullptr){
			appController->createIntelligenceController(mapBoundariesTextBox->Lines);		 
		}else{
			appController->restartIntelligenceController(mapBoundariesTextBox->Lines);
		}
}
private: System::Void label17_Click(System::Object^  sender, System::EventArgs^  e) {
		 }
private: System::Void gpsCheckboxList_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) {
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
					 mapBoundariesTextBox->Text = boundaries;
				 }
			 }
		 }
};
}

