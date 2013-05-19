#pragma once

namespace Communications
{
	ref struct Waypoint;
}

namespace Database
{
	ref struct CandidateRowData;
	ref struct UnverifiedRowData;
	ref struct VerifiedRowData;
}

namespace Intelligence 
{
	ref struct GPSCoord;
}

namespace Delegates
{
	public delegate void gpsCoordArrayToVoid(array<Intelligence::GPSCoord^>^);
	public delegate void twointThreedoubleToVoid( int a, int b, double c, double d, double e );
	public delegate void dataGridViewRowToVoid( System::Windows::Forms::DataGridViewRow ^ row );
	public delegate void voidToVoid( void );
	public delegate void candidateRowDataToVoid( Database::CandidateRowData ^ data );
	public delegate bool candidateRowDataToBool( Database::CandidateRowData ^ data );
	public delegate void unverifiedRowDataToVoid( Database::UnverifiedRowData ^ data );
	public delegate void verifiedRowDataToVoid( Database::VerifiedRowData ^ data );
	public delegate void waypointArraytoVoid( array<Communications::Waypoint ^>^ data);
	public delegate void verifiedRowDataArrayToVoid (array<Database::VerifiedRowData ^> ^ data);
	public delegate void unverifiedRowDataArrayToVoid (array<Database::UnverifiedRowData ^> ^ data);
	public delegate void candidateRowDataArrayToVoid (array<Database::CandidateRowData ^> ^ data);
	public delegate void stringToVoid( System::String ^ id );
	public delegate void arrayUnverifiedRowDataToVoid( array<Database::UnverifiedRowData ^> ^ data);
	public delegate void arrayIntToVoid( array<int> ^ data);
}