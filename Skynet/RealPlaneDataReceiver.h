#pragma once
#include "PlaneDataReceiver.h"
#using <System.dll>
public ref class RealPlaneDataReceiver : PlaneDataReceiver
{
public:
	RealPlaneDataReceiver(System::String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher);
protected:
	virtual void run() override;
private: 
	void fileAdded(Object^ sender, System::IO::FileSystemEventArgs^ e);
};