#pragma once

#include "SkynetControllerInterface.h"
#include <cv.h>

ref struct ImageWithPlaneData;

namespace Intelligence
{
	ref class GPSCoord;
	ref class Autosearch;
	ref class IntelligenceController;
}

ref class PlaneDataReceiver;

namespace Communications 
{
	ref class PlaneWatcher;
}

namespace Simulator
{
	ref class SimHandler;
}

namespace Database
{
	ref class DatabaseConnection;
	ref struct CandidateRowData;
	ref struct DescriptionRowData;
	ref struct GPSPositionRowData;
	ref struct LocationRowData;
	ref struct TelemetryRowData;
	ref struct UnverifiedRowData;
	ref struct VerifiedRowData;
	ref struct DialogEditingData;
}

namespace Vision
{
	ref class VisionController;
}

namespace Skynet
{
	ref class Form1;
	
	public ref class SkynetController : public SkynetControllerInterface
	{
	public:
		SkynetController(Form1 ^ mainView);
		~SkynetController(void);

		void comeAlive(); // called when the GUI is all set up, and Skynet is ready to begin flight ops

		void setCameraView(System::Windows::Forms::PictureBox ^ cameraView);
		void setDatabase(Database::DatabaseConnection ^ newDatabase);
		void setPlaneWatcher(Communications::PlaneWatcher ^ newWatcher);

		void exportData(System::String ^ basePath);

		virtual void intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees );
		virtual void intendedCameraZoomUpdated( float zoom );

		virtual void stopTargetLock();

		void loadAllTablesFromDisk();
		void loadCandidateTableFromDisk();
		void loadVerifiedTableFromDisk();
		void updateVerifiedTableFromDatabaseAtInterval(Object ^ interval);
		void loadUnverifiedTableFromDisk();
		void clearAllTables();
		void restartIntelligenceController(array<System::String ^>^ fieldBoundaries);
		void createIntelligenceController(array<System::String^>^ fieldBoundaries);

		System::String ^ saveCurrentFrameAsImage();
		System::String ^ saveCurrentFrameAsImage(System::String ^ basePath);

		virtual void saveCurrentFrameAsUnverified();	 // call this anywhere, any thread, any time
		void saveUnverified(float * data, int width, int height, int numChannels, int originX, int originY, ImageWithPlaneData ^ stateOfPlane);
		void addUnverifiedToGUITable(Object ^ theObject);

		void addCandidate(Database::CandidateRowData ^ data);
		void addUnverified(Database::UnverifiedRowData ^ data);
		void addVerifiedTarget(Database::VerifiedRowData ^ data);
		void addVerifiedTargetWithDialogData(Database::DialogEditingData ^ data);
		void addVerifiedTargetToGUITable(Database::VerifiedRowData ^ data);
		
		void modifyUnverified(Database::UnverifiedRowData ^ data);

		void removeUnverified(Database::UnverifiedRowData ^ data);
		void removeUnverified(System::String ^ id);
		void removeVerifiedTargetForID(System::String ^ id);
		void displayAutosearchImage(System::Drawing::Image ^ image);
		void displayPathfinderImage(System::Drawing::Image ^ image);
		void pathfinderComplete(System::Drawing::Image ^ image);
		void processPlaneData(ImageWithPlaneData^ imageWithPlaneData);

		array<Database::UnverifiedRowData ^>^ getAllUnverified();
		Database::CandidateRowData ^ candidateWithID(System::String ^ id);
		Database::UnverifiedRowData ^ unverifiedWithID(System::String ^ id);
		Database::VerifiedRowData ^ verifiedWithID(System::String ^ id);
		// Intelligence::Autosearch ^ getAutosearch();
		Intelligence::IntelligenceController ^ getIntelligenceController();
		void setSimHandler(Simulator::SimHandler ^ simHandler)
		{
			this->simHandler = simHandler;
		}
		Simulator::SimHandler ^ getSimHandler() { return simHandler; }
		System::String ^ imageNameForID(System::String ^ id);
		Database::DatabaseConnection ^ getDatabase();
		void printConsoleMessageInGreen(System::String ^ message);

		Form1 ^ getForm1() { return form1View; }
		Communications::PlaneWatcher ^ getPlaneWatcher() { return theWatcher; }

		void gotGPS();
		void gotVideo();
		bool guiHasData;
		bool appIsAlive;
		int frameCount;


	protected:
		Intelligence::IntelligenceController ^ intelligenceController;
		Form1 ^ form1View;
		System::Windows::Forms::PictureBox ^ cameraView;
		Database::DatabaseConnection ^ theDatabase;
		Communications::PlaneWatcher ^ theWatcher;
		Simulator::SimHandler ^ simHandler;
		Vision::VisionController ^ visionController;
		PlaneDataReceiver ^ receiver;
		bool hasTelemetry;
		bool hasVideo;

	private:
		array<Intelligence::GPSCoord^>^ getSampleGPSCoords();
	};
}
