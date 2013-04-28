#pragma once

namespace Communications
{
	ref class PlaneWatcher;
}

namespace Simulator
{
	public ref class SimHandler
	{
	public:
		SimHandler(Communications::PlaneWatcher ^ watcher);

		bool load(System::String ^ filename);
		bool nextFrame();
		bool previousFrame();

	private:
		Communications::PlaneWatcher^ const watcher;
	};
}