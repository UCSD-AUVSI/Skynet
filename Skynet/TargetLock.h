#pragma once

using namespace System;
using namespace System::Threading;

namespace Communications
{
	ref class Comms;
	ref class PlaneWatcher;

	public ref class TargetLock
	{
	public:
		TargetLock(Comms ^ theComms, PlaneWatcher ^ theWatcher);
		~TargetLock(void);

		void lockOntoGPS( float lat, float lon );
		void endLock();

	protected:
		Comms ^ comms;
		PlaneWatcher ^ planeWatcher;

		float lat;
		float lon;
		bool locked;

		Thread ^ targetLockThread;

		void targetLockThreadFunction();
	};
}
