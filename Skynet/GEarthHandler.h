#pragma once

using namespace std;
using namespace System;
using namespace System::Runtime::InteropServices;


namespace OpenGLForm
{
	ref class COpenGL;
}

namespace Database
{
	ref class DatabaseConnection;
}

namespace Skynet
{
	ref class SkynetController;
}

namespace Communications
{
	
	ref struct PlaneState;
	ref class PlaneWatcher;

	public ref class GEarthHandler
	{
	public:
		GEarthHandler(PlaneWatcher ^ tpw, OpenGLForm::COpenGL ^ togl);
		void setWatcher(PlaneWatcher ^ tpw);
		void setOpenGL(OpenGLForm::COpenGL ^ togl);
		void setDatabase(Database::DatabaseConnection ^ tdc);
		void setController(Skynet::SkynetController ^ theCntl);
		void start();
		void pause();
		void unpause();
		void end();
	
	private:
		void writeMasterFile();
		void writeImageryRoot();
		void startNewImageryFile(); // TODO: automatically set currentImageryKMLName, add to imageryRootData
		void updatePlaneFile(PlaneState ^ state);
		void updateImageryFile();
		void updateImageryData(PlaneState ^ state);
		void updateTargetsFile();
		String ^ pointToString(double lat, double lon, double alt);
		
		String ^ currentImageryData;
		String ^ currentImageryKMLName;
		String ^ imageryRoot;
		String ^ imageryRootData;
		int lastFrame;

		Database::DatabaseConnection ^ theDatabase;
		OpenGLForm::COpenGL ^ theOpenGL;
		Threading::Thread ^ runLoopThread;
		PlaneWatcher ^ thePlaneWatcher;
		Skynet::SkynetController ^ theController;
		bool paused;
		bool shouldEnd;
		void runLoop();
	};
}