#pragma once

#include "TelemetryStructures.h"
#include "DatabaseStructures.h"

using namespace System;
using namespace System::Data::Odbc;



namespace Database
{


	enum ColumnName
	{
		columnID
	};
	
	enum TableName
	{
		tableCandidateRegions,
		tableUnverifiedTargets,
		tableVerifiedTargets
	};

	public ref class DatabaseConnection
	{
	public:
		//ctor/dtor
		DatabaseConnection();
		~DatabaseConnection();

		static void TestDatabase();
		static void FillDatabaseForTesting();

		// SQL operations
		bool disconnect(void); 
		bool reset(void);
		OdbcDataReader ^ runQuery(String ^ query);

		array<CandidateRowData ^>^ getAllCandidates();
		array<UnverifiedRowData ^>^ getAllUnverified();
		array<VerifiedRowData ^>^ getAllVerified();
		
		// transfered to here

		String ^ getTabDelimitedVerifiedTargetDataForSubmission();

		String ^ imageNameForID(String ^ id);
		
		void clearCandidatesTable();
		void clearUnverifiedTable();
		void clearVerifiedTable();

		int addCandidate( CandidateRowData ^ data);
		int addUnverified( UnverifiedRowData ^ data);
		int addVerified( VerifiedRowData ^ data);
		int addGPSPosition ( GPSPositionRowData ^ data);
		VerifiedRowData ^ addVerifiedWithDialogData(DialogEditingData ^ data);

		void modifyCandidate( CandidateRowData ^ data);
		void modifyUnverified(UnverifiedRowData ^ data);
		void modifyVerified( VerifiedRowData ^ data);
		void modifyLocation ( LocationRowData ^ data);
		void modifyDescription ( DescriptionRowData ^ data);
		void modifyGPSPosition ( GPSPositionRowData ^ data);
		void modifyTelemetry ( TelemetryRowData ^ data);

		void removeCandidate( String ^ id);
		void removeUnverified( String ^ id);
		void removeVerified( String ^id );
		
		CandidateRowData ^ candidateWithID(String ^ id);
		UnverifiedRowData ^ unverifiedWithID(String ^ id);
		VerifiedRowData ^ verifiedWithID(String ^ id);

		bool connect(void);
		
		///////////////////// no longer used /////////////////////
		// void saveNewVerifiedTarget(VerifiedTargetRowData ^ data);
		// void modifyVerifiedTarget(VerifiedTargetRowData ^ data);
		// void removeVerifiedTarget( String ^id );
		// void verifiedTargetForID(String ^ id);
		// VerifiedTargetRowData ^ verifiedTargetFromReader(OdbcDataReader ^ theReader);
		// void removeVotesForId( String ^ id);
		// void addVote(VoteRowData ^ data);
		// void clearVotesTable();

		// void fillDatabase();
		
		// String ^ getField( TableName table, int field, String ^ rowID );
		//bool insertData( TableName table, RowData ^ data );
		//bool updateValue( TableName table, String ^ field, String ^ value, String ^ rowID );
		// String ^ formatHomography( array<float> ^ input );
		//bool move( TableName source, TableName dest, String ^ verifiedStatus );		
		// bool moveProcessed( TableName source, TableName dest, String ^ processedStatus );

		// array<float> ^ getHomography( TableName table, String ^ rowID );

	protected:
		array<Object ^>^ getAllRows(String ^query, int tableType);
		String ^ getTableName( TableName table );
		
		CandidateRowData ^ candidateFromReader(OdbcDataReader ^ theReader);
		VerifiedRowData ^ verifiedFromReader(OdbcDataReader ^ theReader);
		UnverifiedRowData ^ unverifiedFromReader(OdbcDataReader ^ theReader);


	private:
		OdbcConnection ^ _database;
		OdbcCommand ^ _command;
		OdbcDataReader ^ _reader;
		bool databaseOpen;


	};
}


