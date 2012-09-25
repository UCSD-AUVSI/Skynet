#pragma once
#include "Comport.h"
#include "Database.h"

namespace Delegates
{
	public delegate void comportDownstreamToVoid( Communications::ComportDownstream * data );
	public delegate void twointThreedoubleToVoid( int a, int b, double c, double d, double e );
	public delegate void rowDataToVoid( Database::RowData ^ data );
	public delegate void dataGridViewRowToVoid( DataGridViewRow ^ row );
	public delegate void voidToVoid( void );
	public delegate void candidateRowDataToVoid( Database::CandidateRowData ^ data );
	public delegate void targetRowDataToVoid( Database::TargetRowData ^ data );
	public delegate void verifiedTargetRowDataToVoid( Database::VerifiedTargetRowData ^ data );
	public delegate void stringToVoid( String ^ id );
	public delegate void arrayTargetRowDataToVoid( array<Database::TargetRowData ^> ^ data);
	public delegate void arrayIntToVoid( array<int> ^ data);
}