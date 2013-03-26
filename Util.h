#pragma once
#include <string>

#include "MasterHeader.h"

#define THROW_NOT_IMPL throw gcnew Exception ("Not implemented")

namespace Util {
	std::string managedToSTL(System::String ^ s);
	const char *managedToCharPtr(System::String ^ s);
	int countFilesInDirectory(System::String ^ directory, System::String ^ searchString);
	System::String^ extractFilename(System::String^ const filepath);
}