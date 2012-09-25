#pragma once

#include "Delegates.h"

using namespace System;
using namespace Database;

namespace Database
{
	ref class DatabaseConnection;
	ref struct CandidateRowData;
	ref struct UnverifiedRowData;
}

namespace Skynet
{
	ref class Form1;
}

namespace Delegates
{
	ref class rowDataToVoid;
}

namespace Vision
{
	//ref class Recognizer;

    public ref class DuplicateResolver
    {
    public:
    	DuplicateResolver(Database::DatabaseConnection ^ db, Skynet::Form1 ^ gui);

    	void receiveCandidate(CandidateRowData ^ candidate);
		Delegates::candidateRowDataToVoid ^ saliencyAddTarget;

	protected:
    	Database::DatabaseConnection ^ database;
		//Recognizer ^ recognizer;
    	
    	Skynet::Form1 ^ form1;

        array<UnverifiedRowData ^>^ unverifiedRows;

    };
}

