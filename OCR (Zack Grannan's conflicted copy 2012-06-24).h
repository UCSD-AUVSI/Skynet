//#pragma once
//#include <cv.h>
//#include <highgui.h>
//
//namespace Vision
//{
//	using namespace System;
//	using namespace System::Threading;
//
//	ref struct ocrItem
//	{
//		array<float> ^ _homography;
//		double _heading;
//		String ^ _path;
//		DataGridViewRow ^ _row;
//	};
//
//	ref class OCR
//	{
//	public:
//		OCR( Object ^ parent, int nThreads );
//
//		void trainData( void );
//
//		void ocrThread( void );
//
//		void startProcessing(void);
//		void stopProcessing(void);
//		void buildTrainingSet();
//	private:
//		static Mutex ^ mutex_Data = gcnew Mutex;
//		int _nThreads;
//		array<Thread ^> ^ _threads;
//		Object ^ _parent;
//
//
//	};
//}
