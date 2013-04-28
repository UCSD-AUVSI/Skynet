#include "Util.h"
#include <string>

using namespace System;

std::string Util::managedToSTL(String ^ s)
{
	if (s == nullptr)
		s = "";

	using namespace Runtime::InteropServices;
	const char* chars = 
		(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	std::string retVal = chars;
	Marshal::FreeHGlobal(IntPtr((void*)chars));
	return retVal;
}

const char * Util::managedToCharPtr(String ^ s)
{
	return managedToSTL(s).c_str();
}

int Util::countFilesInDirectory(System::String ^ directory, System::String ^ searchString)
{
	using namespace System::IO;
	array<String^ >^ filenames = Directory::GetFiles(directory, searchString);
	return filenames->Length;
}

String^ Util::extractFilename(String^ const filepath)
{
	array<String^>^ parts = filepath->ToLower()->Split('\\');
	return parts[parts->Length-1];
}