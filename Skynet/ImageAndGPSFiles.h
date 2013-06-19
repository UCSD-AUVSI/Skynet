#pragma once
#using <System.dll>

ref struct ImageWithPlaneData;
namespace Intelligence {
	public ref struct ImageAndGPSFiles: public System::IComparable<ImageAndGPSFiles^> {
		System::String^ imageFilename;
		System::String^ dataFilename;
		ImageWithPlaneData^ toData();
		System::UInt64 getTimestamp();
		virtual System::Int32 CompareTo(ImageAndGPSFiles^ other);
		static ImageAndGPSFiles^ fromImageFilename(System::String^ imageFilename);
		static ImageAndGPSFiles^ fromDataFilename(System::String^ imageFilename);
		static System::String ^ imageFilenameToDataFilename(System::String ^ imageFilename);
		static System::String ^ dataFilenameToImageFilename(System::String ^ dataFilename);
		static System::UInt64 filenameToTimestamp(System::String ^ filename);
		static System::String ^ extractFilename(System::String ^ path);
		ImageAndGPSFiles(System::String^ dataFilename, System::String^ imageFilename);
	};
}