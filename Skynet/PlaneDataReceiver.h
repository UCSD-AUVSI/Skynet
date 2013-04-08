#pragma once

namespace Communications {
	ref class PlaneWatcher;
}

public ref class PlaneDataReceiver abstract{
public:
	PlaneDataReceiver(System::String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher);

	static System::String ^ imageFilenameToDataFilename(System::String ^ imageFilename);
protected:
	System::String ^ directory;
	Communications::PlaneWatcher ^ planeWatcher;
	void processImage(System::String ^ imageFilename, System::String ^ dataFilename);
	static System::Int64 filenameToTimestamp(System::String ^ filename);
	static System::String ^ extractFilename(System::String ^ path);
	virtual void run() = 0;
};
