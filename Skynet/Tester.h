#pragma once

namespace Testing {

	ref class AutosearchTester;
	ref class DatabaseTester;
	ref class OCRTester;
	ref class PathfindingTester;
	ref class PlaneDataReceiverTester;
	ref class PlaneWatcherTester;
	ref class SaliencyTester;
	ref class SkynetControllerTester;

	public ref class Tester abstract {
	public:
		Tester(System::String^ name);
		ref class TestCase {
		public:
			TestCase(System::String^ testname, System::Func<bool>^ func);
			System::String^ name;
			System::Func<bool>^ func;
			bool run();
		};
		System::String^ name;
		System::Collections::Generic::List<TestCase^>^ tests;
		TestCase^ toTestCase();
		void registerTest(System::String^ name, System::Func<bool>^ func);
		void registerTester(Tester^ tester);
		void registerTesters(...array<Tester^> ^testers);
		bool runTests();
	};

	public ref class SkynetTester : Tester {
	public:
		SkynetTester();
		AutosearchTester^ autosearchTester;
		DatabaseTester^ databaseTester;
		OCRTester^ ocrTester;
		PathfindingTester^ pathfindingTester;
		PlaneDataReceiverTester^ dataReceiverTester;
		PlaneWatcherTester^ planeWatcherTester;
		SaliencyTester^ saliencyTester;
		SkynetControllerTester^ skynetControllerTester;
		virtual bool runTests() override;
	};
}