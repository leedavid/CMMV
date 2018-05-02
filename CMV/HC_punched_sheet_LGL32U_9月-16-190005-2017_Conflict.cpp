#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Main procedure 
	void action_puched_sheet_detection()
	{

		// Local iconic variables
		HObject  ho_Image, ho_Region, ho_RegionBorder;
		HObject  ho_RegionBorderDilation, ho_ImageReduced, ho_Boundary;
		HObject  ho_ContoursSplit, ho_CircularArcs, ho_ObjectSelected;
		HObject  ho_UnionContours, ho_Circles, ho_ContCircle, ho_Cross;
		HObject  ho_DistanceContour;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ImageNo, hv_Number;
		HTuple  hv_i, hv_Attrib, hv_Row, hv_Column, hv_Radius, hv_StartPhi;
		HTuple  hv_EndPhi, hv_PointOrder, hv_CircleIds, hv_Index;
		HTuple  hv_RowCenter, hv_ColCenter, hv_Rad, hv_IndicesUppermost;
		HTuple  hv_RowOval, hv_ColOval, hv_Indices, hv_RowOvalSorted;
		HTuple  hv_ColOvalSorted, hv_CircleNo, hv_Distance;

		//
		//This program demonstrates the detection and measurment of circular holes.
		//
		//Initialize the program.
		ReadImage(&ho_Image, "punching_sheet/punching_sheet_01");
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 1);
		dev_update_all("off");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		//
		for (hv_ImageNo = 1; hv_ImageNo <= 3; hv_ImageNo += 1)
		{
			//
			//Read and display the image.
			ReadImage(&ho_Image, "punching_sheet/punching_sheet_" + (hv_ImageNo.TupleString("02")));
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			//
			//Determine a region of interest for the edge extraction.
			Threshold(ho_Image, &ho_Region, 128, 255);
			Boundary(ho_Region, &ho_RegionBorder, "inner");
			DilationCircle(ho_RegionBorder, &ho_RegionBorderDilation, 3.5);
			ReduceDomain(ho_Image, ho_RegionBorderDilation, &ho_ImageReduced);
			//
			//Extract the edges with subpixel-precision.
			EdgesSubPix(ho_ImageReduced, &ho_Boundary, "canny", 2.0, 20, 40);
			//
			//Split the contours into straight line segments and circular arcs.
			SegmentContoursXld(ho_Boundary, &ho_ContoursSplit, "lines_circles", 5, 10, 5);
			//
			//Collect all circular arcs.
			GenEmptyObj(&ho_CircularArcs);
			CountObj(ho_ContoursSplit, &hv_Number);
			{
				HTuple end_val31 = hv_Number;
				HTuple step_val31 = 1;
				for (hv_i = 1; hv_i.Continue(end_val31, step_val31); hv_i += step_val31)
				{
					SelectObj(ho_ContoursSplit, &ho_ObjectSelected, hv_i);
					GetContourGlobalAttribXld(ho_ObjectSelected, "cont_approx", &hv_Attrib);
					//The global attribute 'cont_approx' was set by the operator segment_contours_xld
					//for circular and elliptic arcs to the values are 0 and 1, respectively, for lines to -1
					if (0 != (hv_Attrib == 1))
					{
						ConcatObj(ho_CircularArcs, ho_ObjectSelected, &ho_CircularArcs);
					}
				}
			}
			//
			//Merge cocircular contour pieces and calculate their radii.
			UnionCocircularContoursXld(ho_CircularArcs, &ho_UnionContours, 0.5, 0.5, 0.2,
				30, 10, 20, "true", 1);
			FitCircleContourXld(ho_UnionContours, "geotukey", -1, 0, 0, 3, 1, &hv_Row, &hv_Column,
				&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
			//
			//Select only those circles that have a radius smaller than 500
			hv_CircleIds = ((hv_Radius - 500).TupleSgn()).TupleFind(-1);
			SelectObj(ho_UnionContours, &ho_Circles, hv_CircleIds + 1);
			//
			//DisDisplay the results.
			if (HDevWindowStack::IsOpen())
				ClearWindow(HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "magenta");
			GenEllipseContourXld(&ho_ContCircle, hv_Row.TupleSelect(hv_CircleIds), hv_Column.TupleSelect(hv_CircleIds),
				HTuple(hv_CircleIds.TupleLength(), 0), hv_Radius.TupleSelect(hv_CircleIds),
				hv_Radius.TupleSelect(hv_CircleIds), HTuple(hv_CircleIds.TupleLength(), 0),
				HTuple(hv_CircleIds.TupleLength(), HTuple(360).TupleRad()), hv_PointOrder.TupleSelect(hv_CircleIds),
				1.5);
			if (HDevWindowStack::IsOpen())
				DispObj(ho_ContCircle, HDevWindowStack::GetActive());
			CountObj(ho_Circles, &hv_Number);
			{
				HTuple end_val56 = hv_Number - 1;
				HTuple step_val56 = 1;
				for (hv_Index = 0; hv_Index.Continue(end_val56, step_val56); hv_Index += step_val56)
				{
					hv_RowCenter = ((const HTuple&)hv_Row)[HTuple(hv_CircleIds[hv_Index])];
					hv_ColCenter = ((const HTuple&)hv_Column)[HTuple(hv_CircleIds[hv_Index])];
					hv_Rad = ((const HTuple&)hv_Radius)[HTuple(hv_CircleIds[hv_Index])];
					DispArrow(hv_WindowHandle, hv_RowCenter, hv_ColCenter, hv_RowCenter + ((HTuple(HTuple(-60).TupleRad()).TupleSin())*hv_Rad),
						hv_ColCenter + ((HTuple(HTuple(-60).TupleRad()).TupleCos())*hv_Rad), 2);
					SetTposition(hv_WindowHandle, hv_RowCenter + ((HTuple(HTuple(-60).TupleRad()).TupleSin())*(hv_Rad + 20)),
						hv_ColCenter + ((HTuple(HTuple(-60).TupleRad()).TupleCos())*(hv_Rad + 20)));
					WriteString(hv_WindowHandle, hv_Rad.TupleString(".2f"));
					//stop ()
				}
			}
			//
			//Now,we determine  the size of the two oval holes.
			//For simplicity reasons, we assume that the two uppermost holes are the oval ones.
			//
			//Sort the circle centers.
			//The four uppermost points represent the center of the circles that belong to the oval holes.
			hv_IndicesUppermost = (hv_Row.TupleSortIndex()).TupleSelectRange(0, 3);
			hv_RowOval = hv_Row.TupleSelect(hv_IndicesUppermost);
			hv_ColOval = hv_Column.TupleSelect(hv_IndicesUppermost);
			//
			//Sort these centers from left to right.
			hv_Indices = hv_ColOval.TupleSortIndex();
			hv_RowOvalSorted = hv_RowOval.TupleSelect(hv_Indices);
			hv_ColOvalSorted = hv_ColOval.TupleSelect(hv_Indices);
			//
			//DisDisplay the centers of the circles that belong to the oval holes.
			GenCrossContourXld(&ho_Cross, hv_RowOvalSorted, hv_ColOvalSorted, 12, 0.0);
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "blue");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Cross, HDevWindowStack::GetActive());
			//
			//Calculate and display the size of the oval holes (the distance between the circle centers).
			{
				HTuple end_val86 = (hv_Indices.TupleLength()) - 1;
				HTuple step_val86 = 2;
				for (hv_CircleNo = 0; hv_CircleNo.Continue(end_val86, step_val86); hv_CircleNo += step_val86)
				{
					DistancePp(HTuple(hv_RowOvalSorted[hv_CircleNo]), HTuple(hv_ColOvalSorted[hv_CircleNo]),
						HTuple(hv_RowOvalSorted[hv_CircleNo + 1]), HTuple(hv_ColOvalSorted[hv_CircleNo + 1]),
						&hv_Distance);
					GenContourPolygonXld(&ho_DistanceContour, HTuple(hv_RowOvalSorted[hv_CircleNo]).TupleConcat(HTuple(hv_RowOvalSorted[hv_CircleNo + 1])),
						HTuple(hv_ColOvalSorted[hv_CircleNo]).TupleConcat(HTuple(hv_ColOvalSorted[hv_CircleNo + 1])));
					if (HDevWindowStack::IsOpen())
						DispObj(ho_DistanceContour, HDevWindowStack::GetActive());
					SetTposition(hv_WindowHandle, ((HTuple(hv_RowOvalSorted[hv_CircleNo]) + HTuple(hv_RowOvalSorted[hv_CircleNo + 1]))*0.5) + 5,
						((HTuple(hv_ColOvalSorted[hv_CircleNo]) + HTuple(hv_ColOvalSorted[hv_CircleNo + 1]))*0.5) - 30);
					WriteString(hv_WindowHandle, hv_Distance.TupleString(".2f"));
				}
			}
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		write_message(hv_WindowHandle, 950, 10, "  Program terminated   ", 0);
	}
}
