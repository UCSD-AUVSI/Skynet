#pragma once

#include "TelemetryStructures.h"
#include "Database.h"
#include "SkynetControllerInterface.h"

using namespace System;
using namespace Communications;
using namespace System::Drawing;

namespace Intelligence
{
	ref class GPSCoord;
	ref class Autosearch;
	ref class IntelligenceController;
}

namespace OpenGLForm 
{
	ref class COpenGL;
}

namespace Communications 
{
	ref class PlaneWatcher;
	ref class TargetLock;
	ref class GEarthHandler;
}

namespace Database
{
	//ref class VoteCounter;
}

namespace Simulator
{
	ref class SimHandler;
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

		void setCameraView(OpenGLForm::COpenGL ^ cameraView);
		void setDatabase(Database::DatabaseConnection ^ newDatabase);
		void setPlaneWatcher(PlaneWatcher ^ newWatcher);
		void setTargetLock(TargetLock ^ newLock) { targetLock = newLock; }

		void exportData(String ^ basePath);

		virtual void intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees );
		virtual void intendedCameraZoomUpdated( float zoom );

		virtual void stopTargetLock();

		void loadAllTablesFromDisk();
		void loadCandidateTableFromDisk();
		void loadVerifiedTableFromDisk();
		void updateVerifiedTableFromDatabaseAtInterval(Object ^ interval);
		void loadUnverifiedTableFromDisk();
		void clearAllTables();
		void restartIntelligenceController(array<String ^>^ fieldBoundaries);
		void createIntelligenceController(array<String^>^ fieldBoundaries);

		String ^ saveCurrentFrameAsImage();
		String ^ saveCurrentFrameAsImage(String ^ basePath);

		virtual void saveCurrentFrameAsUnverified();	 // call this anywhere, any thread, any time
		void saveUnverified(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane);
		void saveCurrentFrameAsUnverifiedOnMainThread(); // call this only from main thread
		void addUnverifiedToGUITable(Object ^ theObject);

		void addUnverified(Database::UnverifiedRowData ^ data);
		//bool addVote(Database::VoteRowData ^ data);
		void addVerifiedTarget(Database::VerifiedRowData ^ data);
		void addVerifiedTargetWithDialogData(Database::DialogEditingData ^ data);
		void addVerifiedTargetToGUITable(Database::VerifiedRowData ^ data);
		
		void modifyUnverified(Database::UnverifiedRowData ^ data);

		void removeUnverified(Database::UnverifiedRowData ^ data);
		void removeUnverified(String ^ id);
		// void removeVotesForID(String ^ id);
		void removeVerifiedTargetForID(String ^ id);
		void displayAutosearchImage(Image ^ image);
		void displayPathfinderImage(Image ^ image);
		void pathfinderComplete(Image ^ image);

		Database::CandidateRowData ^ candidateWithID(String ^ id);
		Database::UnverifiedRowData ^ unverifiedWithID(String ^ id);
		Database::VerifiedRowData ^ verifiedWithID(String ^ id);
		// Database::VotesOnCandidate ^ votesForID(String ^ id);
		// Intelligence::Autosearch ^ getAutosearch();
		Intelligence::IntelligenceController ^ getIntelligenceController();
		void setSimHandler(Simulator::SimHandler ^ simHandler)
		{
			this->simHandler = simHandler;
		}
		Simulator::SimHandler ^ getSimHandler() { return simHandler; }
		String ^ imageNameForID(String ^ id);
		Database::DatabaseConnection ^ getDatabase();
		void printConsoleMessageInGreen(String ^ message);

		Form1 ^ getForm1() { return form1View; }
		PlaneWatcher ^ getPlaneWatcher() { return theWatcher; }

		void gotGPS();
		void gotVideo();
		bool guiHasData;
		bool appIsAlive;
		int frameCount;

		///////////// no longer used ////////////////
		//void loadTargetTableFromDisk();
		//void addTargetToGUITable(Object ^ theObject);
//		bool addTarget(Database::TargetRowData ^ data);
//		void modifyTarget(Database::TargetRowData ^ data);
//		void removeTarget(Database::TargetRowData ^ data);
		//void removeTarget(String ^ id);

	protected:
		//Intelligence::Autosearch ^ autosearch;
		Intelligence::IntelligenceController ^ intelligenceController;
		Form1 ^ form1View;
		OpenGLForm::COpenGL ^ openGLView;
		Database::DatabaseConnection ^ theDatabase;
		PlaneWatcher ^ theWatcher;
		TargetLock ^ targetLock;
		Communications::GEarthHandler ^ theGEarthHandler;
		Simulator::SimHandler ^ simHandler;
		
		//Database::VoteCounter ^ voteCounter;

		bool hasTelemetry;
		bool hasVideo;

	private:
		array<Intelligence::GPSCoord^>^ getSampleGPSCoords();
	};
}
