#pragma once

using namespace System;
using namespace System::Threading;

namespace Skynet
{
	ref class Form1;
	ref class SkynetController;
}

namespace Database {

	ref class DatabaseConnection;
	ref struct TargetRowData;
	ref struct VotesOnCandidate;

	public ref class VoteCounter {
	public:
		VoteCounter(Skynet::SkynetController ^ del);

		void setController(Skynet::SkynetController ^ del) {controller = del;}
		void setForm1(Skynet::Form1 ^ form) {form1View = form;}
		void setDatabase(Database::DatabaseConnection ^ db) {theDatabase = db;}

		void pause() {shouldRefreshContinuously = false;}
		void resume() {shouldRefreshContinuously = true;}
		void stop();

	protected:

		void refreshLoop();
		void refreshUnverifiedTargetsTable();
		void refreshCandidatesTable();

		TargetRowData ^ countVotesIntoTarget(VotesOnCandidate ^ votes);



		Skynet::SkynetController ^ controller;
		Skynet::Form1 ^ form1View;
		Database::DatabaseConnection ^ theDatabase;

		Thread ^ runLoopThread;

		bool shouldRefreshContinuously;
		bool shouldQuit;
	};
}