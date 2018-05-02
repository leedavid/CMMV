#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Procedures 
	// Local procedures 
	// Chapter: Saw-Blade-Inspection
	// Short Description: DisDisplay the result of a single tooth 
	void dev_display_tooth(HObject ho_Image, HObject ho_ToothSides, HTuple hv_Index,
		HTuple hv_Rows1, HTuple hv_Columns1, HTuple hv_Rows2, HTuple hv_Columns2, HTuple hv_MainWindow,
		HTuple hv_ZoomWindow)
	{

		// Local iconic variables
		HObject  ho_SawTooth, ho_Line1, ho_Line2;

		// Local control variables
		HTuple  hv_Row, hv_Column, hv_IsParallel, hv_ZoomRow1;
		HTuple  hv_ZoomRow2, hv_ZoomCenterRow, hv_ZoomCol1, hv_ZoomCol2;
		HTuple  hv_ZoomCenterCol;

		IntersectionLl(HTuple(hv_Rows1[hv_Index - 1]), HTuple(hv_Columns1[hv_Index - 1]), HTuple(hv_Rows2[hv_Index - 1]),
			HTuple(hv_Columns2[hv_Index - 1]), HTuple(hv_Rows1[hv_Index]), HTuple(hv_Columns1[hv_Index]),
			HTuple(hv_Rows2[hv_Index]), HTuple(hv_Columns2[hv_Index]), &hv_Row, &hv_Column,
			&hv_IsParallel);
		GenContourPolygonXld(&ho_SawTooth, (HTuple(hv_Rows1[hv_Index - 1]).TupleConcat(hv_Row)).TupleConcat(HTuple(hv_Rows2[hv_Index])),
			(HTuple(hv_Columns1[hv_Index - 1]).TupleConcat(hv_Column)).TupleConcat(HTuple(hv_Columns2[hv_Index])));
		SelectObj(ho_ToothSides, &ho_Line1, hv_Index);
		SelectObj(ho_ToothSides, &ho_Line2, hv_Index + 1);
		HDevWindowStack::SetActive(hv_MainWindow);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "yellow");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Line1, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Line2, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "magenta");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_SawTooth, HDevWindowStack::GetActive());
		hv_ZoomRow1 = ((HTuple(hv_Rows2[hv_Index - 1]).TupleConcat(HTuple(hv_Rows1[hv_Index]))).TupleMin()) - 10;
		hv_ZoomRow2 = ((HTuple(hv_Rows1[hv_Index - 1]).TupleConcat(HTuple(hv_Rows2[hv_Index]))).TupleMax()) + 10;
		hv_ZoomCenterRow = (hv_ZoomRow1 + hv_ZoomRow2) / 2;
		hv_ZoomCol1 = HTuple(hv_Columns1[hv_Index - 1]) - 30;
		hv_ZoomCol2 = HTuple(hv_Columns2[hv_Index]) + 30;
		hv_ZoomCenterCol = (hv_ZoomCol1 + hv_ZoomCol2) / 2;
		HDevWindowStack::SetActive(hv_ZoomWindow);
		if (HDevWindowStack::IsOpen())
			SetWindowExtents(HDevWindowStack::GetActive(), 400, ((hv_ZoomCol1 / 2).TupleConcat(0)).TupleMax(),
				300, 300);
		if (HDevWindowStack::IsOpen())
			SetPart(HDevWindowStack::GetActive(), hv_ZoomCenterRow - 75, hv_ZoomCenterCol - 75,
				hv_ZoomCenterRow + 75, hv_ZoomCenterCol + 75);
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "yellow");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Line1, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Line2, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "magenta");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_SawTooth, HDevWindowStack::GetActive());
		return;
	}


	void action_inspection_of_saw_blades()
	{

		// Local iconic variables
		HObject  ho_Image, ho_Border, ho_ContoursSplit;
		HObject  ho_SelectedContours, ho_ToothSides, ho_SelectedContour;
		HObject  ho_ToothSidesSorted;

		// Local control variables
		HTuple  hv_PI, hv_PI_2, hv_Pointer, hv_Type, hv_Width;
		HTuple  hv_Height, hv_WindowHandleMain, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_WindowWidth, hv_WindowHeight, hv_WindowHandleZoom;
		HTuple  hv_Index1, hv_Number, hv_Index2, hv_Attrib, hv_Rows1;
		HTuple  hv_Columns1, hv_Rows2, hv_Columns2, hv_Nr, hv_Nc;
		HTuple  hv_Dist, hv_Orientations, hv_Angles, hv_Start, hv_Angle;

		//This program demonstrates how to inspect the teeth of a saw
		//blade by measuring angles in an image.
		//
		//Initialize some constants
		hv_PI = HTuple(180).TupleRad();
		hv_PI_2 = HTuple(90).TupleRad();
		//
		//DisDisplay initialization
		dev_update_all("off");
		ReadImage(&ho_Image, "saw_blade/saw_01");
		GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
		reopen_window_fit(ho_Image, 700, 0, 0, &hv_WindowHandleMain);
		init_font(hv_WindowHandleMain, -1, 2);
		GetWindowExtents(hv_WindowHandleMain, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		SetWindowAttr("background_color", "black");
		OpenWindow(400, hv_WindowWidth / 2, 300, 300, 0, "", "", &hv_WindowHandleZoom);
		HDevWindowStack::Push(hv_WindowHandleZoom);
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		//
		//Main loop: process different images
		for (hv_Index1 = 1; hv_Index1 <= 7; hv_Index1 += 1)
		{
			ReadImage(&ho_Image, "saw_blade/saw_" + (hv_Index1.TupleString("02")));
			//
			//By subpixel precise thresholding the contour of the saw is obtained.
			ThresholdSubPix(ho_Image, &ho_Border, 128);
			//
			//Segment, select and sort linear contour parts
			SegmentContoursXld(ho_Border, &ho_ContoursSplit, "lines_circles", 5, 6, 4);
			SelectShapeXld(ho_ContoursSplit, &ho_SelectedContours, "contlength", "and", 30,
				200);
			CountObj(ho_SelectedContours, &hv_Number);
			GenEmptyObj(&ho_ToothSides);
			{
				HTuple end_val29 = hv_Number;
				HTuple step_val29 = 1;
				for (hv_Index2 = 1; hv_Index2.Continue(end_val29, step_val29); hv_Index2 += step_val29)
				{
					SelectObj(ho_SelectedContours, &ho_SelectedContour, hv_Index2);
					//Get the global contour attribute 'cont_approx'
					GetContourGlobalAttribXld(ho_SelectedContour, "cont_approx", &hv_Attrib);
					//The attribute 'cont_approx' has been set by the operator
					//segment_contours. For lines, the value is -1.
					if (0 != (hv_Attrib == -1))
					{
						ConcatObj(ho_ToothSides, ho_SelectedContour, &ho_ToothSides);
					}
				}
			}
			SortContoursXld(ho_ToothSides, &ho_ToothSidesSorted, "upper_left", "true", "column");
			//
			//Calculate the orientation of the tooth sides
			FitLineContourXld(ho_ToothSidesSorted, "tukey", -1, 0, 5, 2, &hv_Rows1, &hv_Columns1,
				&hv_Rows2, &hv_Columns2, &hv_Nr, &hv_Nc, &hv_Dist);
			LineOrientation(hv_Rows1, hv_Columns1, hv_Rows2, hv_Columns2, &hv_Orientations);
			//
			//Measure the angles between the sides that belong to the same tooth
			hv_Angles = HTuple();
			//Check whether the first tooth is completely visible.
			//If not, start the measurement with the second tooth.
			if (0 != (HTuple(hv_Rows1[0]) > HTuple(hv_Rows2[0])))
			{
				hv_Start = 1;
			}
			else
			{
				hv_Start = 2;
			}
			{
				HTuple end_val54 = (hv_Orientations.TupleLength()) - 1;
				HTuple step_val54 = 2;
				for (hv_Index2 = hv_Start; hv_Index2.Continue(end_val54, step_val54); hv_Index2 += step_val54)
				{
					hv_Angle = (HTuple(hv_Orientations[hv_Index2 - 1]) - HTuple(hv_Orientations[hv_Index2])).TupleAbs();
					//Ensure that the angle is within the interval [0,PI/2] degrees.
					if (0 != (hv_Angle > hv_PI_2))
					{
						hv_Angle = hv_PI - hv_Angle;
					}
					//
					//DisDisplay the result
					dev_display_tooth(ho_Image, ho_ToothSides, hv_Index2, hv_Rows1, hv_Columns1,
						hv_Rows2, hv_Columns2, hv_WindowHandleMain, hv_WindowHandleZoom);
					write_message(hv_WindowHandleMain, 10, -1, "Tooth " + ((hv_Index2 + 1) / 2), 1);
					write_message(hv_WindowHandleMain, 35, -1, ("Included Angle: " + ((hv_Angle.TupleDeg()).TupleString(".2f"))) + " deg",
						1);
					write_message(hv_WindowHandleMain, 10, hv_WindowWidth - 270, "Press 'Run' to continue",
						1);
					hv_Angles = hv_Angles.TupleConcat(hv_Angle);
					// stop(); only in hdevelop
				}
			}
		}
		//
		HDevWindowStack::SetActive(hv_WindowHandleMain);
		write_message(hv_WindowHandleMain, -1, hv_WindowWidth - 270, "  Program terminated   ",
			1);
		HDevWindowStack::SetActive(hv_WindowHandleZoom);
		if (HDevWindowStack::IsOpen())
			CloseWindow(HDevWindowStack::Pop());
	}
}