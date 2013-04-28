#include "StdAfx.h"
#include "ShapeSegmentationTester.h"
#include "TargetRecognizer.h"
#include "VisionUtil.h"
#include "Util.h"
#include "ShapeRecognizer.h"
#include "OCRWrapper.h"
#include "VisionRuntimeException.h"

using namespace Vision;

ShapeSegmentationTester::ShapeSegmentationTester()
{
	//do nothing
}

void 
ShapeSegmentationTester::runTest( String^ testDirectory, OCRWrapper ^ theOCRWrapper ) { 
	array<String^>^ fileEntries = System::IO::Directory::GetFiles( testDirectory, "*.png" );

	PRINT("Shape Segmentation testing ...");

	PRINT("LIST OF FILES: ");
	int numFiles = fileEntries->Length;
	for (int i = 0; i < numFiles; ++i) {
			PRINT( fileEntries[i] );
	}

	
	int numFailed = 0;
	for (int i = 0; i < numFiles; ++i) {
			PRINT( fileEntries[i] );
			try {
				TargetResult^ result = theOCRWrapper->targetRecognizer->recognizeTarget( fileEntries[i] );
				if (result != nullptr) 
					PRINT(fileEntries[i] + " -> " + result->shape +", " + result->letter);
			} catch ( std::runtime_error &e ) {
				PRINT( fileEntries[i] + " -> FAILED ");
				numFailed ++;
			} catch (...) {
				PRINT ( "Handling catch here " );
			}
	}

	PRINT( "TEST COMPLETED. " );
	PRINT( " ( " + numFailed + " / " + numFiles + " ) failed segmentation." );
	PRINT( "VISUAL INSPECTION OF SALIENCY REQUIRED IN TEST DIRECTORY" );
	
}