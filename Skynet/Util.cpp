#include "Util.h"
#include <string>
#include "DatabaseStructures.h"

using namespace System;
using namespace Database;
using namespace System::IO;
using namespace Runtime::InteropServices;

std::string Util::managedToSTL(String ^ s)
{
	if (s == nullptr)
		s = "";

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
	array<String^ >^ filenames = Directory::GetFiles(directory, searchString);
	return filenames->Length;
}


String^ Util::extractFilename(String^ const filepath)
{
	array<String^>^ parts = filepath->ToLower()->Split('\\');
	return parts[parts->Length-1];
}

void Util::copyImageFilesToPath(array<VerifiedRowData^>^ targets, String^ path) {
	for each(VerifiedRowData^ target in targets){
		String^ imagePath = target->target->candidate->imageName;
		String^ filename = extractFilename(imagePath);
		File::Copy(imagePath, path + filename, true);
	}
}