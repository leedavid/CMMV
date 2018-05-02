#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


using namespace HalconCpp;

namespace UCI {

	// Local procedures 
	// Chapter: Graphics / Window
	// Short Description: Reopens the  currently active graphics window with a given maximum extent such that it preserves the aspect ratio of the given image 
	void reopen_window_fit(HObject ho_Image, HTuple hv_MaxExtent, HTuple hv_Row, HTuple hv_Col,
		HTuple *hv_WindowHandle)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Width, hv_Height;
		HTuple  hv_WFac, hv_HFac, hv_WindowWidth, hv_WindowHeight;
		HTuple  hv_Fac;

		if (0 != (hv_MaxExtent <= 0))
		{
			return;
		}
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		hv_WFac = (1.0*hv_Width) / hv_MaxExtent;
		hv_HFac = (1.0*hv_Height) / hv_MaxExtent;
		if (0 != (HTuple(hv_WFac <= 1.0).TupleAnd(hv_HFac <= 1.0)))
		{
			hv_WindowWidth = hv_Width;
			hv_WindowHeight = hv_Height;
		}
		else
		{
			hv_Fac = (hv_WFac.TupleConcat(hv_HFac)).TupleMax();
			hv_WindowWidth = hv_Width / hv_Fac;
			hv_WindowHeight = hv_Height / hv_Fac;
		}
		if (HDevWindowStack::IsOpen())
			CloseWindow(HDevWindowStack::Pop());
		SetWindowAttr("background_color", "black");
		OpenWindow(hv_Row, hv_Col, hv_WindowWidth, hv_WindowHeight, 0, "", "", &(*hv_WindowHandle));
		HDevWindowStack::Push((*hv_WindowHandle));
		if (HDevWindowStack::IsOpen())
			SetPart(HDevWindowStack::GetActive(), 0, 0, hv_Height - 1, hv_Width - 1);
		return;
	}

	// Short Description: set font independent of OS (currently only Courier supported) 
	void init_font(HTuple hv_WindowHandle, HTuple hv_Font, HTuple hv_Size)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_OS, hv_FontNameWin, hv_FontNameUnix;
		HTuple  hv_DefaultFontSize, hv_FontSizes;

		GetSystem("operating_system", &hv_OS);
		if (0 != (hv_Font == HTuple("courier")))
		{
			hv_FontNameWin = "Courier New";
			hv_FontNameUnix = "courier-bold-r-normal";
		}
		else
		{
			hv_FontNameWin = "Courier New";
			hv_FontNameUnix = "courier-bold-r-normal";
		}
		hv_DefaultFontSize = 12;
		hv_FontSizes.Clear();
		hv_FontSizes.Append(hv_DefaultFontSize);
		hv_FontSizes.Append(12);
		hv_FontSizes.Append(14);
		hv_FontSizes.Append(18);
		hv_FontSizes.Append(24);
		//set font on Windows systems
		if (0 != ((hv_OS.TupleSubstr(0, 2)) == HTuple("Win")))
		{
			if (0 != (HTuple(hv_Size > 0).TupleAnd(hv_Size < 5)))
			{
				SetFont(hv_WindowHandle, ((("-" + hv_FontNameWin) + "-") + HTuple(hv_FontSizes[hv_Size])) + "-*-*-*-*-1-");
			}
			else
			{
				SetFont(hv_WindowHandle, ((("-" + hv_FontNameWin) + "-") + HTuple(hv_FontSizes[0])) + "-*-*-*-*-1-");
			}
		}
		else
		{
			//set font for UNIX systems
			if (0 != (HTuple(hv_Size > 0).TupleAnd(hv_Size < 5)))
			{
				SetFont(hv_WindowHandle, ((("-*-" + hv_FontNameUnix) + "-*-") + HTuple(hv_FontSizes[hv_Size])) + "-*-*-*-*-*-iso8859-1");
			}
			else
			{
				SetFont(hv_WindowHandle, ((("-*-" + hv_FontNameUnix) + "-*-") + HTuple(hv_FontSizes[0])) + "-*-*-*-*-*-iso8859-1");
			}
		}
		return;
	}

	// Chapter: Develop
	// Short Description: Set all dev_update_* to 'on' or 'off' 
	void dev_update_all(HTuple hv_DisplayMode)
	{

		// dev_update_pc(...); only in hdevelop
		// dev_update_time(...); only in hdevelop
		// dev_update_var(...); only in hdevelop
		// dev_update_window(...); only in hdevelop
		return;
	}

	// Chapter: Graphics / Text
	// Short Description: Estimate extentions of text , draw rectangle at the specified position, and write text in this rectangle 
	void write_message(HTuple hv_WindowHandle, HTuple hv_Row, HTuple hv_Column, HTuple hv_String,
		HTuple hv_WindowCoordinates)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_Red, hv_Green, hv_Blue, hv_Row1Part;
		HTuple  hv_Column1Part, hv_Row2Part, hv_Column2Part, hv_RowWin;
		HTuple  hv_ColumnWin, hv_WidthWin, hv_HeightWin, hv_Width;
		HTuple  hv_Index, hv_ExpDefaultCtrlDummyVar, hv_W, hv_Function;
		HTuple  hv_Sequence, hv_Chr, hv_Ascent, hv_Descent, hv_Height;
		HTuple  hv_FrameHeight, hv_FrameWidth, hv_FactorRow, hv_FactorColumn;
		HTuple  hv_DrawMode, hv_R1_S, hv_C1_S, hv_R2_S, hv_C2_S;
		HTuple  hv_R1, hv_C1, hv_R2, hv_C2;

		//
		//Estimate extentions of text depending on font size, 
		//draw rectangle at the specified position
		//and write text in this rectangle
		//
		HDevWindowStack::SetActive(hv_WindowHandle);
		GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);
		GetPart(hv_WindowHandle, &hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
		GetWindowExtents(hv_WindowHandle, &hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);
		if (0 != (hv_Row == -1))
		{
			hv_Row = 12;
		}
		if (0 != (hv_Column == -1))
		{
			hv_Column = 12;
		}
		hv_String = (" " + ((("" + hv_String) + "").TupleSplit("\n"))) + " ";
		hv_Width = HTuple();
		{
			HTuple end_val17 = (hv_String.TupleLength()) - 1;
			HTuple step_val17 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val17, step_val17); hv_Index += step_val17)
			{
				GetStringExtents(hv_WindowHandle, HTuple(hv_String[hv_Index]), &hv_ExpDefaultCtrlDummyVar,
					&hv_ExpDefaultCtrlDummyVar, &hv_W, &hv_ExpDefaultCtrlDummyVar);
				hv_Width = hv_Width.TupleConcat(hv_W);
			}
		}
		CreateFunct1dArray(HTuple(256, 0), &hv_Function);
		Funct1dToPairs(hv_Function, &hv_Sequence, &hv_ExpDefaultCtrlDummyVar);
		hv_Chr = hv_Sequence.TupleChr();
		GetStringExtents(hv_WindowHandle, hv_Chr, &hv_Ascent, &hv_Descent, &hv_ExpDefaultCtrlDummyVar,
			&hv_Height);
		hv_Height = (hv_Height.TupleConcat(hv_Ascent + hv_Descent)).TupleMax();
		//
		hv_FrameHeight = hv_Height*(hv_String.TupleLength());
		hv_FrameWidth = hv_Width.TupleMax();
		//
		//Adapt text position and extents because of image zooming
		hv_FactorRow = (1.*((hv_Row2Part - hv_Row1Part) + 1)) / hv_HeightWin;
		hv_FactorColumn = (1.*((hv_Column2Part - hv_Column1Part) + 1)) / hv_WidthWin;
		GetDraw(hv_WindowHandle, &hv_DrawMode);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "fill");
		// Draw rectangle to the specified position
		if (0 != hv_WindowCoordinates)
		{
			SetColor(hv_WindowHandle, "light gray");
			hv_R1_S = (((hv_Row + 3)*hv_FactorRow) + 0.5) + hv_Row1Part;
			hv_C1_S = (((hv_Column + 3)*hv_FactorColumn) + 0.5) + hv_Column1Part;
			hv_R2_S = (((((hv_Row + 3) + hv_FrameHeight) - 1)*hv_FactorRow) + 0.5) + hv_Row1Part;
			hv_C2_S = (((((hv_Column + 3) + hv_FrameWidth) - 1)*hv_FactorColumn) + 0.5) + hv_Column1Part;
			DispRectangle1(hv_WindowHandle, hv_R1_S, hv_C1_S, hv_R2_S, hv_C2_S);
			SetColor(hv_WindowHandle, "white");
			hv_R1 = ((hv_Row*hv_FactorRow) + 0.5) + hv_Row1Part;
			hv_C1 = ((hv_Column*hv_FactorColumn) + 0.5) + hv_Column1Part;
			hv_R2 = ((((hv_Row + hv_FrameHeight) - 1)*hv_FactorRow) + 0.5) + hv_Row1Part;
			hv_C2 = ((((hv_Column + hv_FrameWidth) - 1)*hv_FactorColumn) + 0.5) + hv_Column1Part;
			DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
			SetColor(hv_WindowHandle, "black");
		}
		else
		{
			SetColor(hv_WindowHandle, "light gray");
			hv_R1_S = hv_Row + 3;
			hv_C1_S = hv_Column + 3;
			hv_R2_S = ((hv_Row + 3) + ((hv_FrameHeight - 1)*hv_FactorRow)) + 0.5;
			hv_C2_S = ((hv_Column + 3) + ((hv_FrameWidth - 1)*hv_FactorColumn)) + 0.5;
			DispRectangle1(hv_WindowHandle, hv_R1_S, hv_C1_S, hv_R2_S, hv_C2_S);
			SetColor(hv_WindowHandle, "white");
			hv_R1 = hv_Row;
			hv_C1 = hv_Column;
			hv_R2 = (hv_Row + ((hv_FrameHeight - 1)*hv_FactorRow)) + 0.5;
			hv_C2 = (hv_Column + ((hv_FrameWidth - 1)*hv_FactorColumn)) + 0.5;
			DispRectangle1(hv_WindowHandle, hv_R1, hv_C1, hv_R2, hv_C2);
			SetColor(hv_WindowHandle, "black");
		}
		//Write text into rectangle
		{
			HTuple end_val66 = (hv_String.TupleLength()) - 1;
			HTuple step_val66 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val66, step_val66); hv_Index += step_val66)
			{
				SetTposition(hv_WindowHandle, hv_R1 + ((hv_Height*hv_FactorRow)*hv_Index), hv_C1);
				WriteString(hv_WindowHandle, HTuple(hv_String[hv_Index]));
			}
		}
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), hv_DrawMode);
		SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);
		return;
	}

	// void Common_data::action_park_plug_measuring()
	// Main procedure 
	void action_wafer_dicing()
	{

		// Local iconic variables
		HObject  ho_WaferDies, ho_ImageFFT, ho_CorrelationFFT;
		HObject  ho_Correlation, ho_Rectangle, ho_CorrelationReduced;
		HObject  ho_Crosses, ho_Circle, ho_Die, ho_Template, ho_ROI;
		HObject  ho_TemplateReduced, ho_ModelContours, ho_ContoursAffinTrans;
		HObject  ho_CuttingLine;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_WindowWidth, hv_WindowHeight, hv_Width, hv_Height;
		HTuple  hv_Row, hv_Col, hv_Distance, hv_SortedIndex, hv_Row1;
		HTuple  hv_Col1, hv_DiagonalLength, hv_Phi, hv_DieWidth;
		HTuple  hv_DieHeight, hv_LineWidth, hv_LineWidthFraction;
		HTuple  hv_RefRow, hv_RefCol, hv_ModelID, hv_MinScore, hv_Greediness;
		HTuple  hv_NumMatches, hv_MatchRow, hv_MatchColumn, hv_MatchAngle;
		HTuple  hv_MatchScore, hv_HomMat2D, hv_NumRowMax, hv_NumColMax;
		HTuple  hv_RowIndex, hv_RowCurrent, hv_ColIndex, hv_ColCurrent;

		//This program shows how to find dies on a horizontally aligned wafer
		//
		//DisDisplay initialization
		ReadImage(&ho_WaferDies, "wafer_dies");
		reopen_window_fit(ho_WaferDies, 700, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 2);
		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		dev_update_all("off");
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		GetImagePointer1(ho_WaferDies, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WaferDies, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Image of a wafer", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//1. Determine the size of the dies by using auto correlation
		//
		RftGeneric(ho_WaferDies, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
		CorrelationFft(ho_ImageFFT, ho_ImageFFT, &ho_CorrelationFFT);
		RftGeneric(ho_CorrelationFFT, &ho_Correlation, "from_freq", "n", "real", hv_Width);
		//Exclude local maxima at the image border. It is sufficient
		//to extract the maxima in the upper left quarter of the image because
		//the autocorrelation is an even function.
		GenRectangle1(&ho_Rectangle, 1, 1, hv_Height / 2, hv_Width / 2);
		ReduceDomain(ho_Correlation, ho_Rectangle, &ho_CorrelationReduced);
		//
		//Extract the local maxima of the auto correlation function
		LocalMaxSubPix(ho_CorrelationReduced, "gauss", 2, 5000000, &hv_Row, &hv_Col);
		//
		//Get the size of the dies
		//Find maximum that is closest to the origin. This maximum
		//corresponds to the opposite rectangle corner of the die.
		DistancePp(HTuple(hv_Row.TupleLength(), 0), HTuple(hv_Col.TupleLength(), 0), hv_Row,
			hv_Col, &hv_Distance);
		hv_SortedIndex = hv_Distance.TupleSortIndex();
		hv_Row1 = ((const HTuple&)hv_Row)[HTuple(hv_SortedIndex[0])];
		hv_Col1 = ((const HTuple&)hv_Col)[HTuple(hv_SortedIndex[0])];
		hv_DiagonalLength = ((hv_Row1*hv_Row1) + (hv_Col1*hv_Col1)).TupleSqrt();
		hv_Phi = hv_Row1.TupleAtan2(hv_Col1);
		hv_DieWidth = hv_DiagonalLength*(hv_Phi.TupleCos());
		hv_DieHeight = hv_DiagonalLength*(hv_Phi.TupleSin());
		//
		//The center, the first maximum, and the second maximum
		//together define the orientation and the size of the dies.
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		GenCrossContourXld(&ho_Crosses, hv_Row, hv_Col, 8, 0.785398);
		GenEllipseContourXld(&ho_Circle, hv_Row1, hv_Col1, 0, 8, 8, 0, 6.28318, "positive",
			1.5);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Correlation, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Crosses, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Circle, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Rectangle, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Autocorrelation, region of interest, and local maxima",
			1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Visualize the extracted size of the dies
		GenRectangle2ContourXld(&ho_Die, hv_Height / 2, hv_Width / 2, 0, hv_DieWidth / 2, hv_DieHeight / 2);
		reopen_window_fit(ho_WaferDies, 700, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 2);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WaferDies, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Die, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Extracted die size", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//2. Determine the position of the dies by using shape-based matching
		//
		hv_LineWidth = 7;
		hv_LineWidthFraction = 0.6;
		//
		//Generate an artificial model image of the lines between the dies
		GenImageConst(&ho_Template, "byte", hv_Width, hv_Height);
		ScaleImage(ho_Template, &ho_Template, 1, 128);
		hv_RefRow = (0.5*hv_Height).TupleRound();
		hv_RefCol = (0.5*hv_Width).TupleRound();
		for (hv_Row = -0.5; hv_Row <= 0.5; hv_Row += 1)
		{
			for (hv_Col = -0.5; hv_Col <= 0.5; hv_Col += 1)
			{
				GenRectangle2ContourXld(&ho_Rectangle, hv_RefRow + (hv_Row*hv_DieHeight), hv_RefCol + (hv_Col*hv_DieWidth),
					0, (0.5*hv_DieWidth) - (0.5*hv_LineWidth), (0.5*hv_DieHeight) - (0.5*hv_LineWidth));
				PaintXld(ho_Rectangle, ho_Template, &ho_Template, 0);
			}
		}
		GenRectangle2ContourXld(&ho_Rectangle, hv_RefRow, hv_RefCol, 0, (0.5*hv_LineWidthFraction)*hv_LineWidth,
			hv_DieHeight);
		PaintXld(ho_Rectangle, ho_Template, &ho_Template, 0);
		GenRectangle2ContourXld(&ho_Rectangle, hv_RefRow, hv_RefCol, 0, hv_DieWidth, (0.5*hv_LineWidthFraction)*hv_LineWidth);
		PaintXld(ho_Rectangle, ho_Template, &ho_Template, 0);
		GenRectangle2(&ho_ROI, hv_RefRow, hv_RefCol, 0, hv_DieWidth + 5, hv_DieHeight + 5);
		ReduceDomain(ho_Template, ho_ROI, &ho_TemplateReduced);
		CreateShapeModel(ho_TemplateReduced, "auto", 0, 0, "auto", "auto", "ignore_local_polarity",
			"auto", 5, &hv_ModelID);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Template, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ROI, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Template of 4 adjacent dies", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Find the best instance of the artifical model in the original image
		hv_MinScore = 0.7;
		hv_Greediness = 0.5;
		hv_NumMatches = 1;
		FindShapeModel(ho_WaferDies, hv_ModelID, 0, 0, hv_MinScore, hv_NumMatches, 0.5,
			"least_squares", 0, hv_Greediness, &hv_MatchRow, &hv_MatchColumn, &hv_MatchAngle,
			&hv_MatchScore);
		//
		GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
		VectorAngleToRigid(0, 0, 0, hv_MatchRow, hv_MatchColumn, hv_MatchAngle, &hv_HomMat2D);
		AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2D);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WaferDies, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ContoursAffinTrans, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Best match", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//3. Compute the cutting lines based on the size
		//and the position of the dies
		//
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WaferDies, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		hv_NumRowMax = (hv_Height / hv_DieHeight).TupleCeil();
		hv_NumColMax = (hv_Width / hv_DieWidth).TupleCeil();
		{
			HTuple end_val120 = hv_NumRowMax;
			HTuple step_val120 = 1;
			for (hv_RowIndex = -hv_NumRowMax; hv_RowIndex.Continue(end_val120, step_val120); hv_RowIndex += step_val120)
			{
				hv_RowCurrent = hv_MatchRow + (hv_RowIndex*hv_DieHeight);
				GenContourPolygonXld(&ho_CuttingLine, hv_RowCurrent.TupleConcat(hv_RowCurrent),
					HTuple(0).TupleConcat(hv_Width - 1));
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CuttingLine, HDevWindowStack::GetActive());
			}
		}
		{
			HTuple end_val125 = hv_NumColMax;
			HTuple step_val125 = 1;
			for (hv_ColIndex = -hv_NumColMax; hv_ColIndex.Continue(end_val125, step_val125); hv_ColIndex += step_val125)
			{
				hv_ColCurrent = hv_MatchColumn + (hv_ColIndex*hv_DieWidth);
				GenContourPolygonXld(&ho_CuttingLine, HTuple(0).TupleConcat(hv_Height - 1), hv_ColCurrent.TupleConcat(hv_ColCurrent));
				if (HDevWindowStack::IsOpen())
					DispObj(ho_CuttingLine, HDevWindowStack::GetActive());
			}
		}
		write_message(hv_WindowHandle, -1, -1, "Resulting cutting lines", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "  Program terminated   ",
			1);
	}
}