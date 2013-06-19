#pragma once

namespace Communications {
	ref class PlaneWatcher;
}

namespace Intelligence {
	ref struct ImageAndGPSFiles;
}


public ref class PlaneDataReceiver abstract{
public:
	PlaneDataReceiver(System::String ^ directory,
					  Communications::PlaneWatcher ^ planeWatcher);

	virtual bool play();
	virtual bool pause();
	virtual bool next();
	virtual bool previous();
	virtual bool stop();
	virtual bool gotoFrame(int frame);
	bool isPlaying;
	int index;
	System::Collections::Generic::List<Intelligence::ImageAndGPSFiles^>^ frames;

protected:
	System::String ^ directory;
	Communications::PlaneWatcher ^ planeWatcher;
	void sendToPlaneWatcher(Intelligence::ImageAndGPSFiles^ data);
	virtual void run() = 0;
};
