#include "stdafx.h"

#include "MyData.h"
#include "MyDispLay.h"
using namespace HalconCpp;

namespace UCI {

	// Main procedure 
	void action_sureface_inspection()
	{

		// Local iconic variables
		HObject  ho_Image, ho_ImageMean, ho_RegionBrightBorder;
		HObject  ho_RegionDoorknob, ho_RegionErosion, ho_StructElement1;
		HObject  ho_StructElement2, ho_RegionClosing, ho_Rectangle1;
		HObject  ho_Rectangle2, ho_StructElementRef, ho_RegionOpening1;
		HObject  ho_RegionOpening2, ho_RegionOpening, ho_RegionInner;
		HObject  ho_RegionSquares, ho_RegionSquaresDilation, ho_RegionSurface;
		HObject  ho_ImageReduced, ho_ImageMedian, ho_RegionDeviation;
		HObject  ho_ConnectedRegions, ho_RegionDeviationNoNoise;
		HObject  ho_RegionDilation, ho_RegionIntersection, ho_RegionErrors;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_WindowWidth, hv_WindowHeight, hv_ClipRegion;
		HTuple  hv_HomMat2DIdentity, hv_KnobSizeMin, hv_BorderWidth;
		HTuple  hv_BorderTolerance, hv_ScratchWidthMax, hv_ScratchGrayDiffMin;
		HTuple  hv_ScratchAreaMin, hv_InnerSquareSizeFraction, hv_Index;
		HTuple  hv_Row, hv_Column, hv_Phi, hv_KnobLen1, hv_KnobLen2;
		HTuple  hv_KnobSize, hv_InnerSquareSize, hv_HomMat2DRotate;
		HTuple  hv_NumErrors;

		//This program demonstrates how to find scratches on the surface
		//of a doorknob by using region morphology and region features.
		//
		//Initialization
		dev_update_all("off");
		ReadImage(&ho_Image, "doorknob/doorknob_01");
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		init_font(hv_WindowHandle, -1, 2);
		GetSystem("clip_region", &hv_ClipRegion);
		SetSystem("clip_region", "false");
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		//
		//Specify the appearance of the doorknobs in the image
		hv_KnobSizeMin = 100;
		hv_BorderWidth = 7;
		hv_BorderTolerance = 3;
		hv_ScratchWidthMax = 11;
		hv_ScratchGrayDiffMin = 15;
		hv_ScratchAreaMin = 20;
		hv_InnerSquareSizeFraction = 0.205;
		//
		//Main loop: read and process a sequence of images
		for (hv_Index = 1; hv_Index <= 11; hv_Index += 1)
		{
			ReadImage(&ho_Image, "doorknob/doorknob_" + (hv_Index.TupleString("02")));
			//
			//To distinguish unwanted scratches from the regular structure
			//of the object a suitable region of interest must be defined.
			//This is done in four steps:
			//1. Segment the object in the image
			//2. Determine the orientation of the object
			//3. Segment the 4 small inner squares
			//4. Generate a suitable ROI for defect detection
			//
			//1. Segment the object in the image
			MeanImage(ho_Image, &ho_ImageMean, hv_KnobSizeMin, hv_KnobSizeMin);
			DynThreshold(ho_Image, ho_ImageMean, &ho_RegionBrightBorder, 50, "light");
			FillUp(ho_RegionBrightBorder, &ho_RegionDoorknob);
			//
			//2. Robustly determine the orientation and size of the doorknob
			ErosionCircle(ho_RegionDoorknob, &ho_RegionErosion, hv_KnobSizeMin / 4);
			SmallestRectangle2(ho_RegionErosion, &hv_Row, &hv_Column, &hv_Phi, &hv_KnobLen1,
				&hv_KnobLen2);
			hv_KnobSize = (hv_KnobLen1 + hv_KnobLen2) + (hv_KnobSizeMin / 2);
			//
			//3. Segment the 4 small inner squares
			//Based on the known orientation, appropriate structure elements
			//can be generated that can be used for region morphology.
			//- Closing helps to repair defects in the border of the inner squares
			GenRectangle2(&ho_StructElement1, 0, 0, hv_Phi, hv_ScratchWidthMax / 2, 1);
			GenRectangle2(&ho_StructElement2, 0, 0, hv_Phi + (HTuple(90).TupleRad()), hv_ScratchWidthMax / 2,
				1);
			Closing(ho_RegionBrightBorder, ho_StructElement1, &ho_RegionClosing);
			Closing(ho_RegionClosing, ho_StructElement2, &ho_RegionClosing);
			//- Opening frees the squares from overlapping scratches
			hv_InnerSquareSize = hv_KnobSize*hv_InnerSquareSizeFraction;
			GenRectangle2(&ho_Rectangle1, (-hv_InnerSquareSize) / 2.0, 0, 0, hv_InnerSquareSize / 4.0,
				0);
			GenRectangle2(&ho_Rectangle2, hv_InnerSquareSize / 2.0, 0, 0, hv_InnerSquareSize / 4.0,
				0);
			Union2(ho_Rectangle1, ho_Rectangle2, &ho_StructElementRef);
			HomMat2dRotate(hv_HomMat2DIdentity, hv_Phi, 0, 0, &hv_HomMat2DRotate);
			AffineTransRegion(ho_StructElementRef, &ho_StructElement1, hv_HomMat2DRotate,
				"false");
			HomMat2dRotate(hv_HomMat2DIdentity, hv_Phi + (HTuple(90).TupleRad()), 0, 0, &hv_HomMat2DRotate);
			AffineTransRegion(ho_StructElementRef, &ho_StructElement2, hv_HomMat2DRotate,
				"false");
			Opening(ho_RegionClosing, ho_StructElement1, &ho_RegionOpening1);
			Opening(ho_RegionClosing, ho_StructElement2, &ho_RegionOpening2);
			Union2(ho_RegionOpening1, ho_RegionOpening2, &ho_RegionOpening);
			ErosionCircle(ho_RegionDoorknob, &ho_RegionInner, hv_InnerSquareSize / 2);
			Intersection(ho_RegionInner, ho_RegionOpening, &ho_RegionSquares);
			//
			//4. Generate the ROI
			ErosionCircle(ho_RegionDoorknob, &ho_RegionInner, hv_BorderWidth + hv_BorderTolerance);
			DilationCircle(ho_RegionSquares, &ho_RegionSquaresDilation, hv_BorderTolerance);
			Difference(ho_RegionInner, ho_RegionSquaresDilation, &ho_RegionSurface);
			//
			//Search for defects within the ROI
			ReduceDomain(ho_Image, ho_RegionSurface, &ho_ImageReduced);
			MedianImage(ho_ImageReduced, &ho_ImageMedian, "circle", hv_ScratchWidthMax, "mirrored");
			DynThreshold(ho_ImageReduced, ho_ImageMedian, &ho_RegionDeviation, hv_ScratchGrayDiffMin,
				"light");
			//Separate defects from noise
			Connection(ho_RegionDeviation, &ho_ConnectedRegions);
			SelectShape(ho_ConnectedRegions, &ho_RegionDeviationNoNoise, "area", "and", 4,
				10000000);
			DilationCircle(ho_RegionDeviationNoNoise, &ho_RegionDilation, 2);
			Connection(ho_RegionDilation, &ho_ConnectedRegions);
			Intersection(ho_ConnectedRegions, ho_RegionDeviationNoNoise, &ho_RegionIntersection
			);
			SelectShape(ho_RegionIntersection, &ho_RegionErrors, "area", "and", hv_ScratchAreaMin,
				10000000);
			CountObj(ho_RegionErrors, &hv_NumErrors);
			//
			//DisDisplay the results
			if (0 != (hv_NumErrors > 0))
			{
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "red");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_RegionErrors, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle, -1, -1, "Surface shows defects", 1);
			}
			else
			{
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "green");
				write_message(hv_WindowHandle, -1, -1, "Surface is o.k.", 1);
			}
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
		}
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "  Program terminated   ",
			1);
		SetSystem("clip_region", hv_ClipRegion);
	}
}