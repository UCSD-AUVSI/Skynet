#include "StdAfx.h"

#include "VoteCounter.h"

#include "SkynetController.h"
#include "Database.h"
#include "Form1.h"
#include "Delegates.h"

#include "MasterHeader.h"

using namespace Database;
using namespace Skynet;
using namespace System::Threading;


VoteCounter::VoteCounter(Skynet::SkynetController ^ del) 
{
	controller = del;
	form1View = nullptr;
	theDatabase = nullptr;

	shouldRefreshContinuously = true;
	shouldQuit = false;

	runLoopThread = gcnew Thread(gcnew ThreadStart(this, &VoteCounter::refreshLoop));
	runLoopThread->Name = "Vote Counter Thread";
	runLoopThread->Start();
}


void VoteCounter::refreshLoop()
{
	try {
		int counter = 0;

		while (!shouldQuit)
		{
			Thread::Sleep(1000);
			if (!shouldRefreshContinuously)
				continue;

			
			// every iteration, refresh the "processed" checkmark in candidates
			refreshCandidatesTable();

			// every ten iterations, refresh the unverified targets table
			if (counter%10 == 0)
				refreshUnverifiedTargetsTable();
		}
	}
	catch (ThreadAbortException ^ )
	{}
	catch (Exception ^ e)
	{
		PRINT("VoteCounter::refreshLoop() ERROR: " + e);
	}
}

void VoteCounter::refreshCandidatesTable()
{
	if (theDatabase == nullptr)
		return;
	
	array<VotesOnCandidate ^>^ votes = theDatabase->getAllVotes();
	if (votes == nullptr)
		return;

	array<int> ^ idsToSet = gcnew array<int>(votes->Length);

	for (int i = 0; i < votes->Length; i++)
		idsToSet[i] = votes[i]->candidateid;

	form1View->setProcessedForCandidates( idsToSet );
	
	Delegates::arrayIntToVoid ^ blahdelegate = gcnew Delegates::arrayIntToVoid(form1View, &Form1::setProcessedForCandidates );

	try {
		form1View->Invoke( blahdelegate, gcnew array<Object ^>{idsToSet} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in VoteCounter::refreshCandidatesTable(): Failed to set unverified table contents: " + e);
	}

}

TargetRowData ^ VoteCounter::countVotesIntoTarget(VotesOnCandidate ^ votes)
{
	if (votes == nullptr || votes->votes == nullptr || votes->candidateid < 0)
		return nullptr;

	CandidateRowData ^ matchingCandidate = theDatabase->candidateWithID("" + votes->candidateid);
	TargetRowData ^ aggregateTarget = gcnew TargetRowData(matchingCandidate);
	
	aggregateTarget->updateFrom(votes->votes[0]);

	return aggregateTarget;
}

void VoteCounter::refreshUnverifiedTargetsTable()
{
	if (theDatabase == nullptr)
		return;
	
	
	array<VotesOnCandidate ^>^ votes = theDatabase->getAllVotes();
	if (votes == nullptr)
		return;


	// analyze votes, convert each into a TargetRowData
	array<TargetRowData ^> ^ unverifiedTargets = gcnew array<TargetRowData ^>(votes->Length);

	for (int i = 0; i < votes->Length; i++)
	{
		if (votes[i] != nullptr)
			if (nullptr != theDatabase->verifiedTargetForID("" + votes[i]->candidateid))
				votes[i] = nullptr;
	}

	for (int i = 0; i < votes->Length; i++)
		unverifiedTargets[i] = countVotesIntoTarget(votes[i]);

	
	//form1View->setUnverifiedTableContents(unverifiedTargets);
	Delegates::arrayTargetRowDataToVoid ^ blahdelegate = gcnew Delegates::arrayTargetRowDataToVoid(form1View, &Form1::setUnverifiedTableContents );

	try {
		form1View->Invoke( blahdelegate, gcnew array<Object ^>{unverifiedTargets} );
	}
	catch(Exception ^ e) {
		System::Diagnostics::Trace::WriteLine("ERROR in VoteCounter::refreshUnverifiedTargetsTable(): Failed to set unverified table contents: " + e);
	}
}


void VoteCounter::stop()
{
	shouldQuit = true;

	Thread::Sleep(250);

	runLoopThread->Abort();
	runLoopThread = nullptr;
}
