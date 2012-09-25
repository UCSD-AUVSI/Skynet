#pragma once

#include "TelemetryStructures.h"
#include "Database.h"
#include "SkynetControllerInterface.h"


using namespace System;
using namespace Communications;
using namespace System::Drawing;

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
	ref class VoteCounter;
}

namespace Intelligence
{
	ref class GPSCoord;
	ref class Autosearch;
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

		void exportData();

		virtual void intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees );
		virtual void intendedCameraZoomUpdated( float zoom );

		virtual void stopTargetLock();

		void loadAllTablesFromDisk();
		void loadCandidateTableFromDisk();
		void loadVerifiedTargetsTableFromDisk();
		void clearAllTables();

		String ^ saveCurrentFrameAsImage();

		virtual void saveCurrentFrameAsCandidate();	 // call this anywhere, any thread, any time
		void saveCandidate(float * data, int width, int height, int numChannels, int originX, int originY, PlaneState ^ stateOfPlane);
		void saveCurrentFrameAsCandidateOnMainThread(); // call this only from main thread
		void addCandidateToGUITable(Object ^ theObject);

		void addCandidate(Database::CandidateRowData ^ data);
		bool addVote(Database::VoteRowData ^ data);
		void addVerifiedTarget(Database::VerifiedTargetRowData ^ data);
		void addVerifiedTargetWithDialogData(Database::DialogEditingData ^ data);
		void addVerifiedTargetToGUITable(Database::VerifiedTargetRowData ^ data);
		
		void modifyCandidate(Database::CandidateRowData ^ data);

		void removeCandidate(Database::CandidateRowData ^ data);
		void removeCandidate(String ^ id);
		void removeVotesForID(String ^ id);
		void removeVerifiedTargetForID(String ^ id);
		void updateAutosearchBox(Bitmap ^ image);

		Database::CandidateRowData ^ candidateWithID(String ^ id);
		Database::TargetRowData ^ targetWithID(String ^ id);
		Database::VerifiedTargetRowData ^ verifiedTargetWithID(String ^ id);
		Database::VotesOnCandidate ^ votesForID(String ^ id);
		Intelligence::Autosearch ^ getAutosearch();
		String ^ imageNameForID(String ^ id);

		void printConsoleMessageInGreen(String ^ message);

		void gotGPS();
		void gotVideo();
		bool guiHasData;
		int frameCount;

		///////////// no longer used ////////////////
		void loadTargetTableFromDisk();
		void addTargetToGUITable(Object ^ theObject);
		bool addTarget(Database::TargetRowData ^ data);
		void modifyTarget(Database::TargetRowData ^ data);
		void removeTarget(Database::TargetRowData ^ data);
		void removeTarget(String ^ id);

	protected:
		Intelligence::Autosearch ^ autosearch;
		Form1 ^ form1View;
		OpenGLForm::COpenGL ^ openGLView;
		Database::DatabaseConnection ^ theDatabase;
		PlaneWatcher ^ theWatcher;
		TargetLock ^ targetLock;
		Communications::GEarthHandler ^ theGEarthHandler;
		
		Database::VoteCounter ^ voteCounter;

		bool hasTelemetry;
		bool hasVideo;

	private:
		array<Intelligence::GPSCoord^>^ getSampleGPSCoords();
	};
}
