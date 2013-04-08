#pragma once

#include <cv.h>
#include <highgui.h>

namespace Vision {

	#define SOFTWARE_DIRECTORY "C:\\Users\\ucsd_auvsi\\Dropbox\\AUVSI\\Technical\\Software\\"
	#define TRAINING_DIRECTORY (SOFTWARE_DIRECTORY + "OCR training data\\")
	#define TESTING_DIRECTORY (SOFTWARE_DIRECTORY + "OCR testing data\\")

	#define LETTER_DIRECTORY (TRAINING_DIRECTORY + "Letters")
	#define SHAPE_DIRECTORY (TRAINING_DIRECTORY + "Shapes")
	#define COMBINED_DIRECTORY (TRAINING_DIRECTORY + "Combined")

	ref class Dataset;
	ref class OCRWrapper;


	/** OCRTrainer is responsible for managing the letter and shape Datasets
	 *
	 * It will load the datasets from disk or compute new ones.
	 */
	ref class OCRTrainer
	{
	public:
		OCRTrainer(OCRWrapper ^ theOCR) 
		{
			ocrWrapper = theOCR;
		}

		void BuildDatasets();
		Dataset ^getShapeDataset()	{ return shapeDataset; }

	private:
		void loadTrainingData();
		void computeNewTrainingData();
		void trainOnArtificialShapes();
		void trainOnRealImages();

		cv::FileStorage getTrainingFileStorage(int flags);
		void saveDatasets();

		OCRWrapper ^ ocrWrapper;
		Dataset ^ shapeDataset;
	};
}