#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Procedures 
	// Local procedures 
	// Chapter: Molding-Flash-Detection
	// Short Description: Molding flash detection using region morphology 
	void detect_molding_flash_region(HObject ho_Image, HTuple hv_WindowHandle, HTuple hv_MinFlashSize)
	{

		// Local iconic variables
		HObject  ho_Object, ho_OpenedObject, ho_RegionDifference;
		HObject  ho_FlashRegion, ho_DistanceImage, ho_RegionBoundary;
		HObject  ho_FlashRegions, ho_RegionCircleSeg, ho_EndPoints;
		HObject  ho_JuncPoints, ho_EndPointsSelected, ho_ContourSector;
		HObject  ho_CircleSegSelected, ho_CircleCross1, ho_CircleCross2;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_WindowWidth;
		HTuple  hv_WindowHeight, hv_Pointer, hv_Type, hv_Width;
		HTuple  hv_Height, hv_NumFlashRegion, hv_AreaFlash, hv_Row;
		HTuple  hv_Column, hv_DistanceMin, hv_DistanceMax, hv_DistanceRange;
		HTuple  hv_Area, hv_CenterRow, hv_CenterCol, hv_NumFlash;
		HTuple  hv_Index, hv_SegRow, hv_SegCol, hv_Angle1, hv_Angle2;
		HTuple  hv_AngleAux, hv_AngleRange, hv_TextOutput;

		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
		//
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Extract the molding flash using region morphology",
			1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Segment the dark object from the bright background
		Threshold(ho_Image, &ho_Object, 0, 180);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 3);
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Object, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Segmented object", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Eliminate the flash by opening the object region
		OpeningCircle(ho_Object, &ho_OpenedObject, 400.5);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_OpenedObject, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Opened object region", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Segment the flash by computing the difference
		Difference(ho_Object, ho_OpenedObject, &ho_RegionDifference);
		//Eliminate small components
		OpeningRectangle1(ho_RegionDifference, &ho_FlashRegion, hv_MinFlashSize, hv_MinFlashSize);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "red");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_FlashRegion, HDevWindowStack::GetActive());
		//Return if no flashes are found
		CountObj(ho_FlashRegion, &hv_NumFlashRegion);
		AreaCenter(ho_FlashRegion, &hv_AreaFlash, &hv_Row, &hv_Column);
		if (0 != (HTuple(hv_NumFlashRegion.TupleNot()).TupleOr(hv_AreaFlash.TupleNot())))
		{
			write_message(hv_WindowHandle, 12, -1, "No flashes detected", 1);
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
			return;
		}
		write_message(hv_WindowHandle, -1, -1, "Segmented flash region(s)", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Compute the maximum distance from the ideal border
		//using the distance transform of the closed background
		DistanceTransform(ho_OpenedObject, &ho_DistanceImage, "euclidean", "false", hv_Width,
			hv_Height);
		MinMaxGray(ho_FlashRegion, ho_DistanceImage, 0, &hv_DistanceMin, &hv_DistanceMax,
			&hv_DistanceRange);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_DistanceImage, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_OpenedObject, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "red");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_FlashRegion, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Distance transform of the opened object region",
			1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Determine the start and end point of the flash on
		//the boundary of the molded plastic
		Boundary(ho_OpenedObject, &ho_RegionBoundary, "outer");
		Connection(ho_FlashRegion, &ho_FlashRegions);
		Intersection(ho_FlashRegions, ho_RegionBoundary, &ho_RegionCircleSeg);
		JunctionsSkeleton(ho_RegionCircleSeg, &ho_EndPoints, &ho_JuncPoints);
		//
		//Get the center of the circular object
		AreaCenter(ho_OpenedObject, &hv_Area, &hv_CenterRow, &hv_CenterCol);
		//
		//DisDisplay the results
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		CountObj(ho_EndPoints, &hv_NumFlash);
		{
			HTuple end_val73 = hv_NumFlash;
			HTuple step_val73 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val73, step_val73); hv_Index += step_val73)
			{
				SelectObj(ho_EndPoints, &ho_EndPointsSelected, hv_Index);
				GetRegionPoints(ho_EndPointsSelected, &hv_SegRow, &hv_SegCol);
				//Compute the start and end angle of the flash in positive order
				hv_Angle1 = (hv_CenterRow - HTuple(hv_SegRow[0])).TupleAtan2(HTuple(hv_SegCol[0]) - hv_CenterCol);
				hv_Angle2 = (hv_CenterRow - HTuple(hv_SegRow[1])).TupleAtan2(HTuple(hv_SegCol[1]) - hv_CenterCol);
				if (0 != (hv_Angle1 > hv_Angle2))
				{
					hv_AngleAux = hv_Angle1;
					hv_Angle1 = hv_Angle2;
					hv_Angle2 = hv_AngleAux;
				}
				hv_AngleRange = hv_Angle2 - hv_Angle1;
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image, HDevWindowStack::GetActive());
				//Visualize the corresponding circle sector
				GenContourPolygonXld(&ho_ContourSector, (HTuple(hv_SegRow[0]).TupleConcat(hv_CenterRow)).TupleConcat(HTuple(hv_SegRow[1])),
					(HTuple(hv_SegCol[0]).TupleConcat(hv_CenterCol)).TupleConcat(HTuple(hv_SegCol[1])));
				SetLineStyle(hv_WindowHandle, (HTuple(5).Append(5)));
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "green");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourSector, HDevWindowStack::GetActive());
				SetLineStyle(hv_WindowHandle, HTuple());
				//Visualize the corresponding circle segment
				SelectObj(ho_RegionCircleSeg, &ho_CircleSegSelected, hv_Index);
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "red");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleSegSelected, HDevWindowStack::GetActive());
				GenCrossContourXld(&ho_CircleCross1, HTuple(hv_SegRow[0]), HTuple(hv_SegCol[0]),
					16, 0);
				GenCrossContourXld(&ho_CircleCross2, HTuple(hv_SegRow[1]), HTuple(hv_SegCol[1]),
					16, 0);
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "blue");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleCross1, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleCross2, HDevWindowStack::GetActive());
				//Output the result as text
				hv_TextOutput = hv_NumFlash + " flash(es) detected";
				hv_TextOutput = hv_TextOutput.TupleConcat(("Maximum distance: " + hv_DistanceMax) + " pixels");
				hv_TextOutput = hv_TextOutput.TupleConcat(("Flash " + hv_Index) + ":");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angle 1: " + ((hv_Angle1.TupleDeg()).TupleString(" 4.2f"))) + "°");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angle 2: " + ((hv_Angle2.TupleDeg()).TupleString(" 4.2f"))) + "°");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angular Size: " + ((hv_AngleRange.TupleDeg()).TupleString(" 4.2f"))) + "°");
				write_message(hv_WindowHandle, 12, -1, hv_TextOutput, 1);
				write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
					1);
				// stop(); only in hdevelop
			}
		}
		//
		return;
	}


	// Chapter: Molding-Flash-Detection
	// Short Description: Molding flash detection based on xld contours 
	void detect_molding_flash_contour(HObject ho_Image, HTuple hv_WindowHandle, HTuple hv_MinFlashSize)
	{

		// Local iconic variables
		HObject  ho_Object, ho_RegionBorder, ho_RegionDilation;
		HObject  ho_ImageReduced, ho_Edges, ho_BorderContour, ho_ContCircle;
		HObject  ho_ContourSector, ho_CircleSeg, ho_CircleCross1;
		HObject  ho_CircleCross2;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_WindowWidth;
		HTuple  hv_WindowHeight, hv_Pointer, hv_Type, hv_Width;
		HTuple  hv_Height, hv_CenterRow, hv_CenterCol, hv_Radius;
		HTuple  hv_StartPhi, hv_EndPhi, hv_PointOrder, hv_Distances;
		HTuple  hv_DistanceMax, hv_DistancesOffset, hv_Function;
		HTuple  hv_ZeroCrossings, hv_Num, hv_ContRow, hv_ContCol;
		HTuple  hv_Index, hv_Start, hv_End, hv_Angle1, hv_Angle2;
		HTuple  hv_AngleAux, hv_AngleRange, hv_TextOutput;

		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		GetImagePointer1(ho_Image, &hv_Pointer, &hv_Type, &hv_Width, &hv_Height);
		//
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Extract the molding flash based on xld contours",
			1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Segment the subpixel-precise border
		Threshold(ho_Image, &ho_Object, 0, 180);
		Boundary(ho_Object, &ho_RegionBorder, "outer");
		DilationCircle(ho_RegionBorder, &ho_RegionDilation, 5.5);
		ReduceDomain(ho_Image, ho_RegionDilation, &ho_ImageReduced);
		EdgesSubPix(ho_ImageReduced, &ho_Edges, "canny", 2.0, 20, 40);
		UnionAdjacentContoursXld(ho_Edges, &ho_BorderContour, 20, 1, "attr_keep");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 3);
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_BorderContour, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Segmented subpixel-precise border", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Robustly fit a circle to the border
		FitCircleContourXld(ho_BorderContour, "geotukey", -1, 0, 0, 3, 2, &hv_CenterRow,
			&hv_CenterCol, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
		GenEllipseContourXld(&ho_ContCircle, hv_CenterRow, hv_CenterCol, 0, hv_Radius,
			hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder, 1.5);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ContCircle, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Robustly fitted circle", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Compute the distances to the circle
		DistEllipseContourPointsXld(ho_BorderContour, "unsigned", 0, hv_CenterRow, hv_CenterCol,
			0, hv_Radius, hv_Radius, &hv_Distances);
		//
		//Compute the maximum distance
		hv_DistanceMax = hv_Distances.TupleMax();
		//
		//Find intervals on the border that have a large
		//distance from the fitted circle
		hv_DistancesOffset = hv_Distances - hv_MinFlashSize;
		CreateFunct1dArray(hv_DistancesOffset, &hv_Function);
		ZeroCrossingsFunct1d(hv_Function, &hv_ZeroCrossings);
		//Take the 360 degree wrap around into account
		if (0 != (HTuple(hv_DistancesOffset[0]) > 0))
		{
			hv_Num = hv_ZeroCrossings.TupleLength();
			hv_ZeroCrossings = HTuple(hv_ZeroCrossings[hv_Num - 1]).TupleConcat(hv_ZeroCrossings.TupleSelectRange(0, hv_Num - 2));
		}
		//Plot the corrected distance function
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		plot_tuple(hv_WindowHandle, hv_DistancesOffset * 100, "Contour Point Index", "Corrected Distance");
		write_message(hv_WindowHandle, -1, -1, "Corrected distance function", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		if (0 != (HTuple(hv_ZeroCrossings.TupleLength()).TupleNot()))
		{
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			write_message(hv_WindowHandle, 12, -1, "No flashes detected", 1);
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
			return;
		}
		//DisDisplay the results
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		GetContourXld(ho_BorderContour, &hv_ContRow, &hv_ContCol);
		{
			HTuple end_val65 = (hv_ZeroCrossings.TupleLength()) - 1;
			HTuple step_val65 = 2;
			for (hv_Index = 0; hv_Index.Continue(end_val65, step_val65); hv_Index += step_val65)
			{
				hv_Start = HTuple(hv_ZeroCrossings[hv_Index]).TupleRound();
				hv_End = HTuple(hv_ZeroCrossings[hv_Index + 1]).TupleRound();
				//Compute the start and end angle of the flash in positive order
				hv_Angle1 = (hv_CenterRow - HTuple(hv_ContRow[hv_Start])).TupleAtan2(HTuple(hv_ContCol[hv_Start]) - hv_CenterCol);
				hv_Angle2 = (hv_CenterRow - HTuple(hv_ContRow[hv_End])).TupleAtan2(HTuple(hv_ContCol[hv_End]) - hv_CenterCol);
				if (0 != (hv_Angle1 > hv_Angle2))
				{
					hv_AngleAux = hv_Angle1;
					hv_Angle1 = hv_Angle2;
					hv_Angle2 = hv_AngleAux;
				}
				hv_AngleRange = hv_Angle2 - hv_Angle1;
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image, HDevWindowStack::GetActive());
				//Visualize the corresponding circle sector
				GenContourPolygonXld(&ho_ContourSector, (HTuple(hv_ContRow[hv_Start]).TupleConcat(hv_CenterRow)).TupleConcat(HTuple(hv_ContRow[hv_End])),
					(HTuple(hv_ContCol[hv_Start]).TupleConcat(hv_CenterCol)).TupleConcat(HTuple(hv_ContCol[hv_End])));
				SetLineStyle(hv_WindowHandle, (HTuple(5).Append(5)));
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "green");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ContourSector, HDevWindowStack::GetActive());
				SetLineStyle(hv_WindowHandle, HTuple());
				//Visualize the corresponding circle segment
				GenEllipseContourXld(&ho_CircleSeg, hv_CenterRow, hv_CenterCol, 0, hv_Radius,
					hv_Radius, hv_Angle1, hv_Angle2, "positive", 1.5);
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "red");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleSeg, HDevWindowStack::GetActive());
				GenCrossContourXld(&ho_CircleCross1, HTuple(hv_ContRow[hv_Start]), HTuple(hv_ContCol[hv_Start]),
					12, HTuple(45).TupleRad());
				GenCrossContourXld(&ho_CircleCross2, HTuple(hv_ContRow[hv_End]), HTuple(hv_ContCol[hv_End]),
					12, HTuple(45).TupleRad());
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "blue");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleCross1, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CircleCross2, HDevWindowStack::GetActive());
				//Output the result as text
				hv_TextOutput = HTuple((hv_ZeroCrossings.TupleLength()) / 2) + " flash(es) detected";
				hv_TextOutput = hv_TextOutput.TupleConcat(("Maximum distance: " + (hv_DistanceMax.TupleString("4.1f"))) + " pixels");
				hv_TextOutput = hv_TextOutput.TupleConcat(("Flash " + ((hv_Index / 2) + 1)) + ":");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angle 1: " + ((hv_Angle1.TupleDeg()).TupleString(" 4.2f"))) + "°");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angle 2: " + ((hv_Angle2.TupleDeg()).TupleString(" 4.2f"))) + "°");
				hv_TextOutput = hv_TextOutput.TupleConcat(("  Angular Size: " + ((hv_AngleRange.TupleDeg()).TupleString(" 4.2f"))) + "°");
				write_message(hv_WindowHandle, 12, -1, hv_TextOutput, 1);
				write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
					1);
				// stop(); only in hdevelop
			}
		}
		return;
	}

	// Chapter: Inspection / Tuple
	// Short Description: Function plot of the values of a tuple in the specified window. 
	void plot_tuple(HTuple hv_WindowHandle, HTuple hv_Tuple, HTuple hv_TextXAxis, HTuple hv_TextYAxis)
	{

		// Local iconic variables
		HObject  ho_ArrowX, ho_ArrowY, ho_ArrowXDisp;
		HObject  ho_ArrowYDisp, ho_FunctionPlot, ho_ContoursAffinTrans;

		// Local control variables
		HTuple  hv_LineWidth, hv_Red, hv_Green, hv_Blue;
		HTuple  hv_Border, hv_PartRow1, hv_PartCol1, hv_PartRow2;
		HTuple  hv_PartCol2, hv_DPartRow, hv_DPartCol, hv_DisplayAreaRow1;
		HTuple  hv_DisplayAreaCol1, hv_DisplayAreaRow2, hv_DisplayAreaCol2;
		HTuple  hv_DDR, hv_DDC, hv_MinX, hv_MaxX, hv_MinY, hv_MaxY;
		HTuple  hv_DX, hv_DY, hv_Scale, hv_HomMat2DScale, hv_HomMat2DTranslate;
		HTuple  hv_AxisLength, hv_ArrowSize, hv_RowTrans, hv_ColTrans;
		HTuple  hv_Function, hv_Col, hv_ExpDefaultCtrlDummyVar;

		//
		//Remember the current color settings.
		GetLineWidth(hv_WindowHandle, &hv_LineWidth);
		GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
		//
		//Determine the area to be used for the plot.
		hv_Border = 0.2;
		GetPart(hv_WindowHandle, &hv_PartRow1, &hv_PartCol1, &hv_PartRow2, &hv_PartCol2);
		hv_DPartRow = hv_PartRow2 - hv_PartRow1;
		hv_DPartCol = hv_PartCol2 - hv_PartCol1;
		hv_DisplayAreaRow1 = hv_PartRow1 + (hv_Border*hv_DPartRow);
		hv_DisplayAreaCol1 = hv_PartCol1 + (hv_Border*hv_DPartCol);
		hv_DisplayAreaRow2 = hv_PartRow2 - (hv_Border*hv_DPartRow);
		hv_DisplayAreaCol2 = hv_PartCol2 - (hv_Border*hv_DPartCol);
		hv_DDR = hv_DisplayAreaRow2 - hv_DisplayAreaRow1;
		hv_DDC = hv_DisplayAreaCol2 - hv_DisplayAreaCol1;
		//
		//Determine the extent of the function to be plotted.
		hv_MinX = 0;
		hv_MaxX = (hv_Tuple.TupleLength()) - 1;
		hv_MinY = (hv_Tuple.TupleConcat(0)).TupleMin();
		hv_MaxY = (hv_Tuple.TupleConcat(0)).TupleMax();
		hv_DX = hv_MaxX - hv_MinX;
		hv_DY = hv_MaxY - hv_MinY;
		//
		//Determine the scaling factor
		if (0 != ((hv_Tuple.TupleLength()) == 0))
		{
			hv_DX = 1;
			hv_DY = 1;
		}
		hv_Scale = ((hv_DDR.TupleConcat(hv_DDC)).TupleMin()) / ((hv_DX.TupleConcat(hv_DY)).TupleMax());
		//
		//Determine the transformation matrix.
		HomMat2dScale((((((HTuple(1).Append(0)).Append(-0.5)).Append(0)).Append(1)).Append(-0.5)),
			-hv_Scale, hv_Scale, 0, 0, &hv_HomMat2DScale);
		HomMat2dTranslate(hv_HomMat2DScale, (hv_DDR + hv_DisplayAreaRow1) + 0.5, hv_DisplayAreaCol1 + 0.5,
			&hv_HomMat2DTranslate);
		//
		//DisDisplay the tuple coordinate system.
		hv_AxisLength = (hv_DX.TupleConcat(hv_DY)).TupleMax();
		hv_ArrowSize = hv_AxisLength*0.05;
		gen_arrow_contour_xld(&ho_ArrowX, 0, 0, 0, hv_AxisLength + (1.5*hv_ArrowSize), hv_ArrowSize);
		gen_arrow_contour_xld(&ho_ArrowY, 0, 0, hv_AxisLength + (1.5*hv_ArrowSize), 0, hv_ArrowSize);
		AffineTransContourXld(ho_ArrowX, &ho_ArrowXDisp, hv_HomMat2DTranslate);
		AffineTransContourXld(ho_ArrowY, &ho_ArrowYDisp, hv_HomMat2DTranslate);
		SetLineWidth(hv_WindowHandle, 1);
		SetColor(hv_WindowHandle, "white");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ArrowXDisp, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ArrowYDisp, HDevWindowStack::GetActive());
		//
		//DisDisplay axis lables.
		AffineTransPixel(hv_HomMat2DTranslate, -0.5*hv_ArrowSize, hv_AxisLength + (0.5*hv_ArrowSize),
			&hv_RowTrans, &hv_ColTrans);
		write_message(hv_WindowHandle, hv_RowTrans, hv_ColTrans, hv_TextXAxis, 0);
		AffineTransPixel(hv_HomMat2DTranslate, hv_AxisLength + (0.5*hv_ArrowSize), 0.5*hv_ArrowSize,
			&hv_RowTrans, &hv_ColTrans);
		write_message(hv_WindowHandle, hv_RowTrans, hv_ColTrans, hv_TextYAxis, 0);
		//
		//Return for empty tuples.
		if (0 != ((hv_Tuple.TupleLength()) == 0))
		{
			return;
		}
		//
		//DisDisplay the tuple values.
		CreateFunct1dArray(hv_Tuple, &hv_Function);
		Funct1dToPairs(hv_Function, &hv_Col, &hv_ExpDefaultCtrlDummyVar);
		if (0 != ((hv_Tuple.TupleLength()) == 1))
		{
			hv_Col = (hv_Col - (0.05*hv_AxisLength)).TupleConcat(hv_Col + (0.05*hv_AxisLength));
			hv_Tuple = hv_Tuple.TupleConcat(hv_Tuple);
		}
		GenContourPolygonXld(&ho_FunctionPlot, hv_Tuple, hv_Col);
		AffineTransContourXld(ho_FunctionPlot, &ho_ContoursAffinTrans, hv_HomMat2DTranslate);
		SetLineWidth(hv_WindowHandle, hv_LineWidth);
		SetColor(hv_WindowHandle, "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ContoursAffinTrans, HDevWindowStack::GetActive());
		return;
	}



	// Chapter: XLD / Creation
	// Short Description: Creates an xld contour in form of an arrow 
	void gen_arrow_contour_xld(HObject *ho_Arrow, HTuple hv_Row1, HTuple hv_Col1, HTuple hv_Row2,
		HTuple hv_Col2, HTuple hv_Size)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Length, hv_DR, hv_DC, hv_ArrowLength;
		HTuple  hv_HalfArrowWidth, hv_RowP1, hv_ColP1, hv_RowP2;
		HTuple  hv_ColP2, hv_RowP3, hv_ColP3;

		DistancePp(hv_Row1, hv_Col1, hv_Row2, hv_Col2, &hv_Length);
		//
		hv_DR = (1.0*(hv_Row2 - hv_Row1)) / hv_Length;
		hv_DC = (1.0*(hv_Col2 - hv_Col1)) / hv_Length;
		hv_ArrowLength = hv_Size;
		hv_HalfArrowWidth = hv_ArrowLength / 4.0;
		//
		hv_RowP1 = (hv_Row1 + ((hv_Length - hv_ArrowLength)*hv_DR)) + (hv_HalfArrowWidth*hv_DC);
		hv_ColP1 = (hv_Col1 + ((hv_Length - hv_ArrowLength)*hv_DC)) - (hv_HalfArrowWidth*hv_DR);
		hv_RowP2 = hv_Row1 + (((hv_Length - hv_ArrowLength) + hv_HalfArrowWidth)*hv_DR);
		hv_ColP2 = hv_Col1 + (((hv_Length - hv_ArrowLength) + hv_HalfArrowWidth)*hv_DC);
		hv_RowP3 = (hv_Row1 + ((hv_Length - hv_ArrowLength)*hv_DR)) - (hv_HalfArrowWidth*hv_DC);
		hv_ColP3 = (hv_Col1 + ((hv_Length - hv_ArrowLength)*hv_DC)) + (hv_HalfArrowWidth*hv_DR);
		//
		GenContourPolygonXld(&(*ho_Arrow), ((((hv_Row1.TupleConcat(hv_RowP2)).TupleConcat(hv_RowP3)).TupleConcat(hv_Row2)).TupleConcat(hv_RowP1)).TupleConcat(hv_RowP2),
			((((hv_Col1.TupleConcat(hv_ColP2)).TupleConcat(hv_ColP3)).TupleConcat(hv_Col2)).TupleConcat(hv_ColP1)).TupleConcat(hv_ColP2));
		return;
	}

	// Main procedure 
	void action_mold_flash_detection()
	{

		// Local iconic variables
		HObject  ho_Image;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Width, hv_Height;
		HTuple  hv_WindowHandle, hv_WindowWidth, hv_WindowHeight;
		HTuple  hv_MinFlashSize, hv_Index;

		//This program demonstrates how to detect a molding flash on a
		//plastic part that was manufactured by injection molding.
		//Two different solutions that lead to the same result are
		//presented: One using region morphology and one that is based
		//on xld contours.
		//
		//Initialization
		dev_update_all("off");
		ReadImage(&ho_Image, "molding_flashes/molding_flashes_01");
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		init_font(hv_WindowHandle, -1, 2);
		//
		//Initialize the minimum flash distance
		hv_MinFlashSize = 5;
		//
		for (hv_Index = 1; hv_Index <= 9; hv_Index += 1)
		{
			ReadImage(&ho_Image, "molding_flashes/molding_flashes_" + (hv_Index.TupleString("02")));
			//
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			write_message(hv_WindowHandle, -1, -1, "Image " + hv_Index, hv_WindowHandle);
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
			//
			//Molding flash detection using region morphology
			detect_molding_flash_region(ho_Image, hv_WindowHandle, hv_MinFlashSize);
			//
			//Molding flash detection based on xld contours
			detect_molding_flash_contour(ho_Image, hv_WindowHandle, hv_MinFlashSize);
			//
		}
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "  Program terminated   ",
			1);
	}
}