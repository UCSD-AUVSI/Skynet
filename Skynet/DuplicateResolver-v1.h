#pragma once

#include "Delegates.h"

using namespace System;
using namespace Database;
namespace Database
{
	ref class DatabaseConnection;
	ref struct CandidateRowData;
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
	ref class Recognizer;

    public ref class DuplicateResolver
    {
    public:
    	DuplicateResolver(Database::DatabaseConnection ^ db, Vision::OCR ^ theOCR, Skynet::Form1 ^ gui);

    	void receiveCandidate(CandidateRowData ^ candidate);
		Delegates::candidateRowDataToVoid ^ saliencyAddTarget;
    // TOOD: finish this, make it actually work (see ctor, receiveCandidate())

	protected:
    	Database::DatabaseConnection ^ database;
    	Vision::OCR ^ ocr;
		Recognizer ^ recognizer;
    	
    	Skynet::Form1 ^ form1;

        //List<UnverifiedRowData ^>^ unverifiedRows;

    };
}

