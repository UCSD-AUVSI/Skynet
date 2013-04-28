#pragma once

namespace Database
{

	ref struct CandidateRowData;
	ref struct DescriptionRowData;
	ref struct GPSPositionRowData;
	ref struct LocationRowData;
	ref struct TelemetryRowData;
	ref struct UnverifiedRowData;
	ref struct VerifiedRowData;
	ref struct DialogEditingData;

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

	// TODO: Implement
	/**
	public ref class DBObject {
		virtual String^ updateString() = 0;
		virtual String^ insertString() = 0;
		virtual String^ deleteString() = 0;
	};
	*/


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
		System::Data::Odbc::OdbcDataReader ^ runQuery(System::String ^ query);

		array<CandidateRowData ^>^ getAllCandidates();
		array<UnverifiedRowData ^>^ getAllUnverified();
		array<VerifiedRowData ^>^ getAllVerified();
		
		// transfered to here

		System::String ^ getTabDelimitedVerifiedTargetDataForSubmission();

		System::String ^ imageNameForID(System::String ^ id);
		
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

		void removeCandidate( System::String ^ id);
		void removeUnverified( System::String ^ id);
		void removeVerified( System::String ^id );
		
		CandidateRowData ^ candidateWithID(System::String ^ id);
		UnverifiedRowData ^ unverifiedWithID(System::String ^ id);
		VerifiedRowData ^ verifiedWithID(System::String ^ id);

		bool connect(void);
		
	protected:
		array<Object ^>^ getAllRows(System::String ^query, int tableType);
		System::String ^ getTableName( TableName table );
		
		CandidateRowData ^ candidateFromReader(System::Data::Odbc::OdbcDataReader ^ theReader);
		VerifiedRowData ^ verifiedFromReader(System::Data::Odbc::OdbcDataReader ^ theReader);
		UnverifiedRowData ^ unverifiedFromReader(System::Data::Odbc::OdbcDataReader ^ theReader);


	private:
		System::Data::Odbc::OdbcConnection ^ _database;
		System::Data::Odbc::OdbcCommand ^ _command;
		System::Data::Odbc::OdbcDataReader ^ _reader;
		bool databaseOpen;
	};
}


