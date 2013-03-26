#pragma once
#include "PlaneDataReceiver.h"
public ref class SimulatorPlaneDataReceiver : PlaneDataReceiver
{
public:
	SimulatorPlaneDataReceiver(System::String ^ directory,
				  Communications::PlaneWatcher ^ planeWatcher);
protected:
	virtual void run() override;
};