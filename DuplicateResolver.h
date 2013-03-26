#pragma once
#include "Delegates.h"

namespace Database
{
	ref struct CandidateRowData;
	ref struct UnverifiedRowData;
}

namespace Skynet
{
	ref class SkynetController;
}

namespace Vision
{

    public ref class DuplicateResolver
    {
    public:
			
    	DuplicateResolver(Skynet::SkynetController^ skynetController);
		bool betterThanDuplicate(Database::CandidateRowData^ candidate, Database::UnverifiedRowData^ duplicate);
		Database::UnverifiedRowData^ DuplicateResolver::maybeFindDuplicate(Database::CandidateRowData ^ candidate);
	private:
		Skynet::SkynetController^ skynetController;
    };
}