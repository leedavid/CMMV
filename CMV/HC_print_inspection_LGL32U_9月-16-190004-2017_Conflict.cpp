#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Main procedure 
	void action_pring_inspection()
	{

		// Local iconic variables
		HObject  ho_Image, ho_ROI, ho_ImageReduced, ho_ImaAmp;
		HObject  ho_ImaDir, ho_VariationImage, ho_MatchingROI, ho_Template;
		HObject  ho_Edges, ho_ImageAligned, ho_ImageAlignedReduced;
		HObject  ho_RegionDiff, ho_RegionDilation, ho_ConnectedRegions;
		HObject  ho_RegionIntersection, ho_SelectedRegions, ho_ErrorMarker;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_Width, hv_Height, hv_Pi, hv_Sigma, hv_AbsThreshold;
		HTuple  hv_VarThreshold, hv_VarModelID, hv_ShapeModelID;
		HTuple  hv_ModelArea, hv_ModelRow, hv_ModelColumn, hv_Index;
		HTuple  hv_Row, hv_Column, hv_Angle, hv_Score, hv_HomMat2D;
		HTuple  hv_MinComponentSize, hv_NumberDefects, hv_Ra, hv_Rb;
		HTuple  hv_Phi;

		//
		//This example demonstrates the process of print inspection using a single reference image.
		//
		//Read the reference image and initialize the program.
		ReadImage(&ho_Image, "relay/relay_01");
		reopen_window_fit(ho_Image, 600, 0, 0, &hv_WindowHandle);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		init_font(hv_WindowHandle, -1, 1);
		dev_update_all("off");
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		hv_Pi = HTuple(180).TupleRad();
		//
		//Read the region of interest that has been predefined manually.
		ReadRegion(&ho_ROI, "relay/relay_inspection_roi.reg");
		ReduceDomain(ho_Image, ho_ROI, &ho_ImageReduced);
		//
		//DisDisplay the reference image.
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ImageReduced, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, 10, 10, "Reference image", 0);
		write_message(hv_WindowHandle, 900, 10, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//In this application, the variation model is created from a single reference image by
		//using the edge amplitudes as an approximation for the standard deviation.
		hv_Sigma = 0.5;
		EdgesImage(ho_ImageReduced, &ho_ImaAmp, &ho_ImaDir, "canny", hv_Sigma, "none",
			20, 40);
		GrayDilationRect(ho_ImaAmp, &ho_VariationImage, 3, 3);
		//
		//DisDisplay the variation image.
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_VariationImage, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, 10, 10, "Variation image", 0);
		write_message(hv_WindowHandle, 900, 10, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Create the variation model.
		hv_AbsThreshold = 15;
		hv_VarThreshold = 1;
		CreateVariationModel(hv_Width, hv_Height, "byte", "direct", &hv_VarModelID);
		PrepareDirectVariationModel(ho_ImageReduced, ho_VariationImage, hv_VarModelID,
			hv_AbsThreshold, hv_VarThreshold);
		//
		//Create a shape model for the alignment of the test images.
		//To speed up the matching, ony parts of the print are used as template.
		ReadRegion(&ho_MatchingROI, "relay/relay_matching_roi");
		ReduceDomain(ho_Image, ho_MatchingROI, &ho_Template);
		CreateShapeModel(ho_Template, 5, -(HTuple(5).TupleRad()), HTuple(10).TupleRad(),
			"auto", "auto", "use_polarity", "auto", "auto", &hv_ShapeModelID);
		AreaCenter(ho_MatchingROI, &hv_ModelArea, &hv_ModelRow, &hv_ModelColumn);
		//
		//The subpixel-precise edges are extracted only for visualization purposes.
		EdgesSubPix(ho_ImageReduced, &ho_Edges, "sobel_fast", 0.5, 10, 20);
		//
		//Inspect the imprint on several relays.
		for (hv_Index = 2; hv_Index <= 6; hv_Index += 1)
		{
			//
			//Read test image.
			ReadImage(&ho_Image, "relay/relay_" + (hv_Index.TupleString("02d")));
			//
			//Determine the exact position of the imprint in the test image.
			FindShapeModel(ho_Image, hv_ShapeModelID, -(HTuple(5).TupleRad()), HTuple(10).TupleRad(),
				0.5, 1, 0.5, "least_squares", 0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			//
			//Inspect the imprint if it could be found.
			if (0 != ((hv_Score.TupleLength()) == 1))
			{
				//
				//Align the image to the reference image.
				VectorAngleToRigid(hv_Row, hv_Column, hv_Angle, hv_ModelRow, hv_ModelColumn,
					0, &hv_HomMat2D);
				AffineTransImage(ho_Image, &ho_ImageAligned, hv_HomMat2D, "constant", "false");
				//
				//Compare the current test image with the reference.
				ReduceDomain(ho_ImageAligned, ho_ROI, &ho_ImageAlignedReduced);
				CompareVariationModel(ho_ImageAlignedReduced, &ho_RegionDiff, hv_VarModelID);
				//
				//Select regions that indicate errors in the imprint.
				hv_MinComponentSize = 5;
				DilationCircle(ho_RegionDiff, &ho_RegionDilation, 3.5);
				Connection(ho_RegionDilation, &ho_ConnectedRegions);
				Intersection(ho_ConnectedRegions, ho_RegionDiff, &ho_RegionIntersection);
				SelectShape(ho_RegionIntersection, &ho_SelectedRegions, "area", "and", hv_MinComponentSize,
					hv_ModelArea);
				//
				//DisDisplay the results.
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageAligned, HDevWindowStack::GetActive());
				CountObj(ho_SelectedRegions, &hv_NumberDefects);
				if (0 != (hv_NumberDefects > 0))
				{
					write_message(hv_WindowHandle, 10, 10, ((("Image " + hv_Index) + ": ") + hv_NumberDefects) + " Errors found",
						0);
					if (HDevWindowStack::IsOpen())
						SetColor(HDevWindowStack::GetActive(), "green");
					if (HDevWindowStack::IsOpen())
						SetLineWidth(HDevWindowStack::GetActive(), 1);
					if (HDevWindowStack::IsOpen())
						DispObj(ho_Edges, HDevWindowStack::GetActive());
					if (HDevWindowStack::IsOpen())
						SetColor(HDevWindowStack::GetActive(), "red");
					if (HDevWindowStack::IsOpen())
						SetLineWidth(HDevWindowStack::GetActive(), 3);
					AreaCenter(ho_SelectedRegions, &hv_ExpDefaultCtrlDummyVar, &hv_Row, &hv_Column);
					EllipticAxis(ho_SelectedRegions, &hv_Ra, &hv_Rb, &hv_Phi);
					GenEllipseContourXld(&ho_ErrorMarker, hv_Row, hv_Column, hv_Phi, hv_Ra + 5,
						hv_Rb + 5, HTuple(hv_Row.TupleLength(), 0), HTuple(hv_Row.TupleLength(), 6.28318),
						HTuple(hv_Row.TupleLength(), "positive"), 1.5);
					if (HDevWindowStack::IsOpen())
						DispObj(ho_ErrorMarker, HDevWindowStack::GetActive());
				}
				else
				{
					write_message(hv_WindowHandle, 10, 10, ("Image " + hv_Index) + ": OK", 0);
					if (HDevWindowStack::IsOpen())
						SetColor(HDevWindowStack::GetActive(), "green");
					if (HDevWindowStack::IsOpen())
						SetLineWidth(HDevWindowStack::GetActive(), 1);
					if (HDevWindowStack::IsOpen())
						DispObj(ho_Edges, HDevWindowStack::GetActive());
				}
			}
			else
			{
				if (0 != ((hv_Score.TupleLength()) == 0))
				{
					write_message(hv_WindowHandle, 10, 10, ("Image " + hv_Index) + ": Pattern to be tested could not be found",
						0);
				}
				else
				{
					write_message(hv_WindowHandle, 10, 10, ("Image " + hv_Index) + ": Pattern to be tested was found ambiguously",
						0);
				}
			}
			write_message(hv_WindowHandle, 900, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		//
		//Clean up
		ClearShapeModel(hv_ShapeModelID);
		ClearVariationModel(hv_VarModelID);
		write_message(hv_WindowHandle, 900, 10, "  Program terminated   ", 0);
	}
}