#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Procedures 
	// Local procedures 
	// Chapter: Resistor-Verification
	// Short Description: Transform a matching pose into a homogeneous matrix 
	void matching_pose_to_hom_mat2d(HTuple hv_Row, HTuple hv_Column, HTuple hv_Angle,
		HTuple hv_ScaleR, HTuple hv_ScaleC, HTuple *hv_HomMat2D)
	{

		HomMat2dIdentity(&(*hv_HomMat2D));
		HomMat2dScale((*hv_HomMat2D), hv_ScaleR, hv_ScaleC, 0, 0, &(*hv_HomMat2D));
		HomMat2dRotate((*hv_HomMat2D), hv_Angle, 0, 0, &(*hv_HomMat2D));
		HomMat2dTranslate((*hv_HomMat2D), hv_Row, hv_Column, &(*hv_HomMat2D));
		return;
	}

	// Main procedure 
	void action_pose_verfication_of_res()
	{

		// Local iconic variables
		HObject  ho_Image, ho_Rectangle, ho_ModelImageGeneric;
		HObject  ho_ModelContours, ho_ModelImage, ho_ModelContours330;
		HObject  ho_ModelContoursTrans, ho_ModelContours1R1, ho_ImageOnline;
		HObject  ho_ImageReduced, ho_ResistorBorder, ho_Cross, ho_ModelContour;
		HObject  ho_ModelContourAffineTrans;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Width, hv_Height;
		HTuple  hv_WindowHandle, hv_WindowWidth, hv_WindowHeight;
		HTuple  hv_AngleTol, hv_ScaleTol, hv_ResistorTypes, hv_ReferenceTypes;
		HTuple  hv_MeanModelHeight, hv_MeanModelWidth, hv_PrintFraction;
		HTuple  hv_PrintRefFraction, hv_ModelIDGeneric, hv_Row;
		HTuple  hv_Column, hv_Angle, hv_ScaleR, hv_ScaleC, hv_Score;
		HTuple  hv_ModelID330, hv_ModelID1R1, hv_NumContour330;
		HTuple  hv_NumContour1R1, hv_ContourIndexStart, hv_NumContour;
		HTuple  hv_Index, hv_PrintRow, hv_PrintColumn, hv_PrintAngle;
		HTuple  hv_PrintScore, hv_PrintModel, hv_Model, hv_ResistorType;
		HTuple  hv_HomMat2D, hv_ReferenceType;

		//This example shows how to verify the pose and the type
		//of resistors on a board with the shape based matching.
		//
		//Initialize the visualization
		dev_update_all("off");
		ReadImage(&ho_Image, "resistor/resistor_01");
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		if (HDevWindowStack::IsOpen())
			CloseWindow(HDevWindowStack::Pop());
		SetWindowAttr("background_color", "black");
		OpenWindow(0, 0, hv_Width*1.5, hv_Height*1.5, 0, "", "", &hv_WindowHandle);
		HDevWindowStack::Push(hv_WindowHandle);
		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		init_font(hv_WindowHandle, -1, 2);
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		//
		//Initialize some variables
		hv_AngleTol = HTuple(5).TupleRad();
		hv_ScaleTol = 0.1;
		hv_ResistorTypes.Clear();
		hv_ResistorTypes[0] = "330";
		hv_ResistorTypes[1] = "1R1";
		hv_ReferenceTypes = hv_ResistorTypes.TupleSelect((((((((HTuple(0).Append(1)).Append(0)).Append(0)).Append(1)).Append(0)).Append(1)).Append(0)));
		hv_MeanModelHeight = 185;
		hv_MeanModelWidth = 100;
		hv_PrintFraction = 0.5;
		hv_PrintRefFraction = 0.25;
		//
		//Create a shape model of the resistor from a synthetic model image
		GenImageConst(&ho_Image, "byte", hv_Width, hv_Height);
		GenRectangle2ContourXld(&ho_Rectangle, hv_Height / 2, hv_Width / 2, 0, hv_MeanModelWidth / 2.0,
			hv_MeanModelHeight / 2.0);
		PaintXld(ho_Rectangle, ho_Image, &ho_ModelImageGeneric, 128);
		CreateAnisoShapeModel(ho_ModelImageGeneric, 3, -hv_AngleTol, 2.0*hv_AngleTol, "auto",
			1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, "auto", 1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol,
			"auto", "auto", "use_polarity", "auto", 10, &hv_ModelIDGeneric);
		//Visualize the generic model
		get_shape_model_contour_ref(ho_ModelImageGeneric, &ho_ModelContours, hv_ModelIDGeneric);
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelImageGeneric, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelContours, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Generic resistor model", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Create a shape model of the '330' print
		ReadImage(&ho_Image, "resistor/resistor_01");
		FindAnisoShapeModel(ho_Image, hv_ModelIDGeneric, -hv_AngleTol, 2.0*hv_AngleTol,
			1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 0.7, 1,
			0.5, "least_squares", 0, 0.7, &hv_Row, &hv_Column, &hv_Angle, &hv_ScaleR, &hv_ScaleC,
			&hv_Score);
		GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Angle, ((hv_PrintFraction*0.5)*hv_ScaleC)*hv_MeanModelWidth,
			((hv_PrintFraction*0.5)*hv_ScaleR)*hv_MeanModelHeight);
		ReduceDomain(ho_Image, ho_Rectangle, &ho_ModelImage);
		CreateShapeModel(ho_ModelImage, "auto", HTuple(-90).TupleRad(), HTuple(360).TupleRad(),
			"auto", "auto", "use_polarity", (HTuple("auto_contrast_hyst").Append(20)),
			"auto", &hv_ModelID330);
		GetShapeModelContours(&ho_ModelContours330, hv_ModelID330, 1);
		//Visualize the print model
		get_shape_model_contour_ref(ho_Rectangle, &ho_ModelContoursTrans, hv_ModelID330);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelContoursTrans, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Model for resistor type 330", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Create a shape model of the '1R1' print
		ReadImage(&ho_Image, "resistor/resistor_02");
		FindAnisoShapeModel(ho_Image, hv_ModelIDGeneric, -hv_AngleTol, 2.0*hv_AngleTol,
			1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 0.7, 1,
			0.5, "least_squares", 0, 0.7, &hv_Row, &hv_Column, &hv_Angle, &hv_ScaleR, &hv_ScaleC,
			&hv_Score);
		GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Angle, ((hv_PrintFraction*0.5)*hv_ScaleC)*hv_MeanModelWidth,
			((hv_PrintFraction*0.5)*hv_ScaleR)*hv_MeanModelHeight);
		ReduceDomain(ho_Image, ho_Rectangle, &ho_ModelImage);
		CreateShapeModel(ho_ModelImage, "auto", HTuple(-90).TupleRad(), HTuple(360).TupleRad(),
			"auto", "auto", "use_polarity", (HTuple("auto_contrast_hyst").Append(20)),
			"auto", &hv_ModelID1R1);
		GetShapeModelContours(&ho_ModelContours1R1, hv_ModelID1R1, 1);
		//Visualize the print model
		get_shape_model_contour_ref(ho_Rectangle, &ho_ModelContoursTrans, hv_ModelID1R1);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelContoursTrans, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Model for resistor type 1R1", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//For visualization purposes concat the contours of the
		//models and remember the associated indices.
		CountObj(ho_ModelContours330, &hv_NumContour330);
		CountObj(ho_ModelContours1R1, &hv_NumContour1R1);
		ConcatObj(ho_ModelContours330, ho_ModelContours1R1, &ho_ModelContours);
		hv_ContourIndexStart.Clear();
		hv_ContourIndexStart[0] = 1;
		hv_ContourIndexStart.Append(hv_NumContour330 + 1);
		hv_NumContour.Clear();
		hv_NumContour.Append(hv_NumContour330);
		hv_NumContour.Append(hv_NumContour1R1);
		//
		//Main loop
		for (hv_Index = 3; hv_Index <= 8; hv_Index += 1)
		{
			ReadImage(&ho_ImageOnline, "resistor/resistor_" + (hv_Index.TupleString("02")));
			//
			//Find the resistor
			FindAnisoShapeModel(ho_ImageOnline, hv_ModelIDGeneric, -hv_AngleTol, 2.0*hv_AngleTol,
				1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 1.0 - hv_ScaleTol, 1.0 + hv_ScaleTol, 0.7,
				1, 0.5, "least_squares", 0, 0.7, &hv_Row, &hv_Column, &hv_Angle, &hv_ScaleR,
				&hv_ScaleC, &hv_Score);
			//
			//Identify the found resistor
			if (0 != (hv_Score.TupleLength()))
			{
				GenRectangle2(&ho_Rectangle, hv_Row, hv_Column, hv_Angle, ((hv_PrintRefFraction*hv_ScaleC)*0.5)*hv_MeanModelWidth,
					((hv_PrintRefFraction*hv_ScaleR)*0.5)*hv_MeanModelHeight);
				ReduceDomain(ho_ImageOnline, ho_Rectangle, &ho_ImageReduced);
				FindShapeModels(ho_ImageReduced, ((hv_ModelID330.TupleConcat(hv_ModelID330)).TupleConcat(hv_ModelID1R1)).TupleConcat(hv_ModelID1R1),
					(((HTuple(0).TupleConcat(HTuple(180).TupleRad())).TupleConcat(0)).TupleConcat(HTuple(180).TupleRad())) - hv_AngleTol,
					2 * hv_AngleTol, 0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_PrintRow, &hv_PrintColumn,
					&hv_PrintAngle, &hv_PrintScore, &hv_PrintModel);
				//
				//DisDisplay the result
				GenRectangle2ContourXld(&ho_ResistorBorder, hv_Row, hv_Column, hv_Angle, (hv_ScaleC*hv_MeanModelWidth) / 2.0,
					(hv_ScaleR*hv_MeanModelHeight) / 2.0);
				GenCrossContourXld(&ho_Cross, hv_Row, hv_Column, 6, 0.785398);
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageOnline, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "green");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ResistorBorder, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Cross, HDevWindowStack::GetActive());
				if (0 != (hv_PrintModel.TupleLength()))
				{
					hv_Model = hv_PrintModel / 2;
					hv_ResistorType = ((const HTuple&)hv_ResistorTypes)[hv_Model];
					matching_pose_to_hom_mat2d(hv_PrintRow, hv_PrintColumn, hv_PrintAngle, 1.0,
						1.0, &hv_HomMat2D);
					CopyObj(ho_ModelContours, &ho_ModelContour, HTuple(hv_ContourIndexStart[hv_Model]),
						HTuple(hv_NumContour[hv_Model]));
					AffineTransContourXld(ho_ModelContour, &ho_ModelContourAffineTrans, hv_HomMat2D);
					if (HDevWindowStack::IsOpen())
						SetColor(HDevWindowStack::GetActive(), "blue");
					if (HDevWindowStack::IsOpen())
						DispObj(ho_ModelContourAffineTrans, HDevWindowStack::GetActive());
				}
				else
				{
					//No print on the resistor found -> unknown resistor type
					hv_ResistorType = "unknown";
				}
				write_message(hv_WindowHandle, -1, -1, ((HTuple("Resistor pose:").TupleConcat(((("  Pos.: (" + (hv_Row.TupleString(".2f"))) + ",") + (hv_Column.TupleString(".2f"))) + ")")).TupleConcat(("  Angle: " + ((hv_Angle.TupleDeg()).TupleString(".2f"))) + "°")).TupleConcat(((("  Scale: (" + (hv_ScaleR.TupleString(".2f"))) + ",") + (hv_ScaleC.TupleString(".2f"))) + ")"),
					1);
				hv_ReferenceType = ((const HTuple&)hv_ReferenceTypes)[hv_Index - 1];
				if (0 != (hv_ResistorType == hv_ReferenceType))
				{
					//Resistor has the correct type
					write_message(hv_WindowHandle, 100, -1, ("Resistor type: " + hv_ResistorType) + " (correct)",
						1);
				}
				else
				{
					//Resistor has the wrong type
					write_message(hv_WindowHandle, 100, -1, (("Resistor type: " + hv_ResistorType) + " (not correct)").TupleConcat("Expected type: " + hv_ReferenceType),
						1);
				}
			}
			else
			{
				//No resistor found
				write_message(hv_WindowHandle, -1, -1, HTuple("No resistor found").TupleConcat("Expected type: " + hv_ReferenceType),
					1);
			}
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
		}
		//
		//Clear the shape models
		ClearShapeModel(hv_ModelID330);
		ClearShapeModel(hv_ModelID1R1);
		ClearShapeModel(hv_ModelIDGeneric);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "  Program terminated   ",
			1);
	}
}