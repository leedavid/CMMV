#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Chapter: Texture-Classification
	// Short Description: Generates edge based features for the given image. 
	void gen_sobel_features(HObject ho_ImageRR, HTuple *hv_SobelFeatures)
	{

		// Local iconic variables
		HObject  ho_EdgeAmplitude;

		// Local control variables
		HTuple  hv_AbsoluteHisto;

		SobelAmp(ho_ImageRR, &ho_EdgeAmplitude, "sum_abs", 3);
		GrayHistoAbs(ho_EdgeAmplitude, ho_EdgeAmplitude, 8, &hv_AbsoluteHisto);
		(*hv_SobelFeatures) = (hv_AbsoluteHisto.TupleReal()) / (hv_AbsoluteHisto.TupleSum());
		return;
	}

	// Procedures 
	// Local procedures 
	// Chapter: Texture-Classification
	// Short Description: Generates a set of texture features for the given image. 
	void gen_features(HObject ho_Image, HTuple *hv_Features)
	{

		// Local iconic variables
		HObject  ho_ImagePyramid, ho_ImageRR;

		// Local control variables
		HTuple  hv_Index, hv_SobelFeatures, hv_Entropy;
		HTuple  hv_Anisotropy;

		(*hv_Features) = HTuple();
		GenGaussPyramid(ho_Image, &ho_ImagePyramid, "constant", 0.5);
		for (hv_Index = 1; hv_Index <= 3; hv_Index += 1)
		{
			SelectObj(ho_ImagePyramid, &ho_ImageRR, hv_Index);
			gen_sobel_features(ho_ImageRR, &hv_SobelFeatures);
			(*hv_Features) = (*hv_Features).TupleConcat(hv_SobelFeatures);
			EntropyGray(ho_ImageRR, ho_ImageRR, &hv_Entropy, &hv_Anisotropy);
			(*hv_Features) = ((*hv_Features).TupleConcat(hv_Entropy)).TupleConcat(hv_Anisotropy);
		}
		//
		(*hv_Features) = (*hv_Features).TupleReal();
		return;
	}

	// Main procedure 
	void action_classification_fabrics()
	{

		// Local iconic variables
		HObject  ho_Image;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_Width, hv_Height, hv_NumClasses, hv_NumTrainingImages;
		HTuple  hv_NumTestImages, hv_NumImagesToBeClassified, hv_Features;
		HTuple  hv_NumFeatures, hv_NumHidden, hv_MLPHandle, hv_AllFeatures;
		HTuple  hv_AllClasses, hv_Class, hv_ImageNum, hv_Error;
		HTuple  hv_ErrorLog, hv_NumCorrect, hv_NumError, hv_ErrorTrueClass;
		HTuple  hv_ErrorImageNumber, hv_ErrorClassifiedAs, hv_ClassifiedClass;
		HTuple  hv_Confidence, hv_Message, hv_MLPHandleImproved;
		HTuple  hv_Index;

		//
		//This application demonstrates texture classification with neural nets.
		//22 different types of nonwoven samples are distinguished automatically.
		//(Please note that for robust applications typically many more training
		// samples have to be used, up to several thousands)
		//
		//Read the reference image and initialize the program.
		ReadImage(&ho_Image, "nonwoven/nonwoven_01_01");
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 1);
		dev_update_all("off");
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		//
		//Number of classes, training images, test images, and images to be classified
		hv_NumClasses = 22;
		hv_NumTrainingImages = 6;
		hv_NumTestImages = 4;
		hv_NumImagesToBeClassified = 2;
		//
		//Determine the number of features that are generated for each image
		GenImageConst(&ho_Image, "byte", hv_Width, hv_Height);
		gen_features(ho_Image, &hv_Features);
		hv_NumFeatures = hv_Features.TupleLength();
		//
		//Create the neural network classifier.
		hv_NumHidden = 10;
		CreateClassMlp(hv_NumFeatures, hv_NumHidden, hv_NumClasses, "softmax", "normalization",
			-1, 42, &hv_MLPHandle);
		//
		//Determine features from the training images and
		//add these training samples to the classifier.
		hv_AllFeatures = HTuple();
		hv_AllClasses = HTuple();
		{
			HTuple end_val32 = hv_NumClasses - 1;
			HTuple step_val32 = 1;
			for (hv_Class = 0; hv_Class.Continue(end_val32, step_val32); hv_Class += step_val32)
			{
				{
					HTuple end_val33 = hv_NumTrainingImages;
					HTuple step_val33 = 1;
					for (hv_ImageNum = 1; hv_ImageNum.Continue(end_val33, step_val33); hv_ImageNum += step_val33)
					{
						ReadImage(&ho_Image, (("nonwoven/nonwoven_" + ((hv_Class + 1).TupleString("02"))) + "_") + (hv_ImageNum.TupleString("02")));
						if (HDevWindowStack::IsOpen())
							DispObj(ho_Image, HDevWindowStack::GetActive());
						write_message(hv_WindowHandle, -1, -1, (HTuple("Training phase").Append(" ")).TupleConcat((("Class: " + hv_Class) + ", Image: ") + hv_ImageNum),
							0);
						write_message(hv_WindowHandle, 950, -1, (((100.0*((hv_Class*hv_NumTrainingImages) + hv_ImageNum)) / (hv_NumClasses*hv_NumTrainingImages)).TupleString(".0f")) + "% completed",
							0);
						//
						//Determine the features for the current training sample
						gen_features(ho_Image, &hv_Features);
						//
						//Add the training samples to the classifier.
						AddSampleClassMlp(hv_MLPHandle, hv_Features, hv_Class);
						//
						//Store the features and classes.
						hv_AllFeatures = hv_AllFeatures.TupleConcat(hv_Features);
						hv_AllClasses = hv_AllClasses.TupleConcat(hv_Class);
					}
				}
			}
		}
		//
		//Train the classifier.
		TrainClassMlp(hv_MLPHandle, 200, 0.1, 0.001, &hv_Error, &hv_ErrorLog);
		//
		//Delete the training samples stored in the classifier.
		ClearSamplesClassMlp(hv_MLPHandle);
		//
		//DisDisplay error progression.
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Training phase completed", 0);
		write_message(hv_WindowHandle, 100, 350, "The progression of the training error",
			0);
		plot_tuple(hv_WindowHandle, hv_ErrorLog, "Iterations", "Error");
		write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Test the new classifier by reclassification of the training images.
		hv_NumCorrect = 0;
		hv_NumError = 0;
		hv_ErrorTrueClass = HTuple();
		hv_ErrorImageNumber = HTuple();
		hv_ErrorClassifiedAs = HTuple();
		{
			HTuple end_val71 = hv_NumClasses - 1;
			HTuple step_val71 = 1;
			for (hv_Class = 0; hv_Class.Continue(end_val71, step_val71); hv_Class += step_val71)
			{
				{
					HTuple end_val72 = hv_NumTrainingImages;
					HTuple step_val72 = 1;
					for (hv_ImageNum = 1; hv_ImageNum.Continue(end_val72, step_val72); hv_ImageNum += step_val72)
					{
						ReadImage(&ho_Image, (("nonwoven/nonwoven_" + ((hv_Class + 1).TupleString("02"))) + "_") + (hv_ImageNum.TupleString("02")));
						if (HDevWindowStack::IsOpen())
							DispObj(ho_Image, HDevWindowStack::GetActive());
						write_message(hv_WindowHandle, -1, -1, (HTuple("Reclassification").Append(" ")).TupleConcat((("Class: " + hv_Class) + ", Image: ") + hv_ImageNum),
							0);
						write_message(hv_WindowHandle, 950, -1, (((100.0*((hv_Class*hv_NumTrainingImages) + hv_ImageNum)) / (hv_NumClasses*hv_NumTrainingImages)).TupleString(".0f")) + "% completed",
							0);
						//
						//Determine the features for the current training sample
						gen_features(ho_Image, &hv_Features);
						//
						//Reclassify the training image.
						ClassifyClassMlp(hv_MLPHandle, hv_Features, 1, &hv_ClassifiedClass, &hv_Confidence);
						//
						//Check the reclassification result.
						if (0 != (hv_ClassifiedClass == hv_Class))
						{
							hv_NumCorrect += 1;
							write_message(hv_WindowHandle, 150, -1, "Classified correctly", 0);
						}
						else
						{
							hv_NumError += 1;
							hv_ErrorTrueClass = hv_ErrorTrueClass.TupleConcat(hv_Class);
							hv_ErrorImageNumber = hv_ErrorImageNumber.TupleConcat(hv_ImageNum);
							hv_ErrorClassifiedAs = hv_ErrorClassifiedAs.TupleConcat(hv_ClassifiedClass);
							write_message(hv_WindowHandle, 150, -1, "Classified incorrectly", 0);
							// stop(); only in hdevelop
						}
					}
				}
			}
		}
		hv_Message.Clear();
		hv_Message.Append(("Reclassification of " + (hv_NumCorrect + hv_NumError)) + " images completed");
		hv_Message.Append(" ");
		hv_Message = (hv_Message.TupleConcat("Correctly classified: " + hv_NumCorrect)).TupleConcat("Wrongly classified:   " + hv_NumError);
		hv_Message = (hv_Message.TupleConcat(" ")).TupleConcat(("Ratio: " + (((100.0*hv_NumCorrect) / (hv_NumCorrect + hv_NumError)).TupleString(".1f"))) + "% correctly classified");
		write_message(hv_WindowHandle, -1, -1, hv_Message, 0);
		if (0 != (hv_NumError > 0))
		{
			write_message(hv_WindowHandle, 450, 350, (HTuple("There were false classifications!").Append("Check the Error-variables for details.")),
				0);
		}
		write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Test the new classifier with images not used for the training.
		hv_NumCorrect = 0;
		hv_NumError = 0;
		hv_ErrorTrueClass = HTuple();
		hv_ErrorImageNumber = HTuple();
		hv_ErrorClassifiedAs = HTuple();
		{
			HTuple end_val114 = hv_NumClasses - 1;
			HTuple step_val114 = 1;
			for (hv_Class = 0; hv_Class.Continue(end_val114, step_val114); hv_Class += step_val114)
			{
				{
					HTuple end_val115 = hv_NumTrainingImages + hv_NumTestImages;
					HTuple step_val115 = 1;
					for (hv_ImageNum = hv_NumTrainingImages + 1; hv_ImageNum.Continue(end_val115, step_val115); hv_ImageNum += step_val115)
					{
						ReadImage(&ho_Image, (("nonwoven/nonwoven_" + ((hv_Class + 1).TupleString("02"))) + "_") + (hv_ImageNum.TupleString("02")));
						if (HDevWindowStack::IsOpen())
							DispObj(ho_Image, HDevWindowStack::GetActive());
						write_message(hv_WindowHandle, -1, -1, (HTuple("Test classification").Append(" ")).TupleConcat((("Class: " + hv_Class) + ", Image: ") + hv_ImageNum),
							0);
						write_message(hv_WindowHandle, 950, -1, (((100.0*(((hv_Class*hv_NumTestImages) + hv_ImageNum) - hv_NumTrainingImages)) / (hv_NumClasses*hv_NumTestImages)).TupleString(".0f")) + "% completed",
							0);
						//
						//Determine the features for the current image.
						gen_features(ho_Image, &hv_Features);
						//
						//Classify the image.
						ClassifyClassMlp(hv_MLPHandle, hv_Features, 1, &hv_ClassifiedClass, &hv_Confidence);
						//
						//Check the classification result.
						if (0 != (hv_ClassifiedClass == hv_Class))
						{
							hv_NumCorrect += 1;
							write_message(hv_WindowHandle, 150, -1, "Classified correctly", 0);
						}
						else
						{
							hv_NumError += 1;
							hv_ErrorTrueClass = hv_ErrorTrueClass.TupleConcat(hv_Class);
							hv_ErrorImageNumber = hv_ErrorImageNumber.TupleConcat(hv_ImageNum);
							hv_ErrorClassifiedAs = hv_ErrorClassifiedAs.TupleConcat(hv_ClassifiedClass);
							write_message(hv_WindowHandle, 150, -1, "Classified incorrectly", 0);
							write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
							// stop(); only in hdevelop
						}
					}
				}
			}
		}
		hv_Message.Clear();
		hv_Message.Append(("Test classification of " + (hv_NumCorrect + hv_NumError)) + " images completed");
		hv_Message.Append(" ");
		hv_Message = (hv_Message.TupleConcat("Correctly classified: " + hv_NumCorrect)).TupleConcat("Wrongly classified:   " + hv_NumError);
		hv_Message = (hv_Message.TupleConcat(" ")).TupleConcat(("Ratio: " + (((100.0*hv_NumCorrect) / (hv_NumCorrect + hv_NumError)).TupleString(".1f"))) + "% correctly classified");
		write_message(hv_WindowHandle, -1, -1, hv_Message, 0);
		if (0 != (hv_NumError > 0))
		{
			write_message(hv_WindowHandle, 450, 350, (HTuple("There were false classifications!").Append("Check the Error variables for details.")),
				0);
		}
		write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Use the wrongly classified samples as training samples, too.
		CreateClassMlp(hv_NumFeatures, hv_NumHidden, hv_NumClasses, "softmax", "normalization",
			-1, 42, &hv_MLPHandleImproved);
		//
		//First add the training samples used above.
		{
			HTuple end_val156 = (hv_AllClasses.TupleLength()) - 1;
			HTuple step_val156 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val156, step_val156); hv_Index += step_val156)
			{
				AddSampleClassMlp(hv_MLPHandleImproved, hv_AllFeatures.TupleSelectRange(hv_Index*hv_NumFeatures, ((hv_Index + 1)*hv_NumFeatures) - 1),
					HTuple(hv_AllClasses[hv_Index]));
			}
		}
		//
		//Add the new training samples.
		{
			HTuple end_val161 = (hv_ErrorTrueClass.TupleLength()) - 1;
			HTuple step_val161 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val161, step_val161); hv_Index += step_val161)
			{
				hv_Class = ((const HTuple&)hv_ErrorTrueClass)[hv_Index];
				hv_ImageNum = ((const HTuple&)hv_ErrorImageNumber)[hv_Index];
				ReadImage(&ho_Image, (("nonwoven/nonwoven_" + ((hv_Class + 1).TupleString("02"))) + "_") + (hv_ImageNum.TupleString("02")));
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle, -1, -1, (HTuple("Add samples").Append(" ")).TupleConcat((("Class: " + hv_Class) + ", Image: ") + hv_ImageNum),
					0);
				write_message(hv_WindowHandle, 950, -1, (((100.0*(hv_Index + 1)) / (hv_ErrorTrueClass.TupleLength())).TupleString(".0f")) + "% completed",
					0);
				//
				//Determine the features for the current training sample
				gen_features(ho_Image, &hv_Features);
				//
				//Add the training samples to the classifier.
				AddSampleClassMlp(hv_MLPHandleImproved, hv_Features, hv_Class);
			}
		}
		//
		//Train the classifier.
		TrainClassMlp(hv_MLPHandleImproved, 200, 0.1, 0.001, &hv_Error, &hv_ErrorLog);
		//
		//DisDisplay error progression.
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Training phase completed", 0);
		write_message(hv_WindowHandle, 100, 350, "The progression of the training error",
			0);
		plot_tuple(hv_WindowHandle, hv_ErrorLog, "Iterations", "Error");
		write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Delete the training samples stored in the classifier.
		ClearSamplesClassMlp(hv_MLPHandleImproved);
		//
		//Apply the new classifier.
		{
			HTuple end_val191 = hv_NumClasses - 1;
			HTuple step_val191 = 1;
			for (hv_Class = 0; hv_Class.Continue(end_val191, step_val191); hv_Class += step_val191)
			{
				{
					HTuple end_val192 = (hv_NumTrainingImages + hv_NumTestImages) + hv_NumImagesToBeClassified;
					HTuple step_val192 = 1;
					for (hv_ImageNum = (hv_NumTrainingImages + hv_NumTestImages) + 1; hv_ImageNum.Continue(end_val192, step_val192); hv_ImageNum += step_val192)
					{
						ReadImage(&ho_Image, (("nonwoven/nonwoven_" + ((hv_Class + 1).TupleString("02"))) + "_") + (hv_ImageNum.TupleString("02")));
						if (HDevWindowStack::IsOpen())
							DispObj(ho_Image, HDevWindowStack::GetActive());
						write_message(hv_WindowHandle, -1, -1, ((HTuple("Recognition phase").Append(" ")).Append(" ")),
							0);
						//
						//Determine the features for the current image.
						gen_features(ho_Image, &hv_Features);
						//
						//Classify the image.
						ClassifyClassMlp(hv_MLPHandleImproved, hv_Features, 1, &hv_ClassifiedClass,
							&hv_Confidence);
						write_message(hv_WindowHandle, 150, -1, "Classification result: Class " + hv_ClassifiedClass,
							0);
						write_message(hv_WindowHandle, 950, -1, "Press 'Run' to continue", 0);
						// stop(); only in hdevelop
					}
				}
			}
		}
		//
		//Free the allocated memory.
		ClearClassMlp(hv_MLPHandleImproved);
		ClearClassMlp(hv_MLPHandle);
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, 950, -1, " Program terminated    ", 0);
	}
}

