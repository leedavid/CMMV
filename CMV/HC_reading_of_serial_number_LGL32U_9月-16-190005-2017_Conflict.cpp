#include "stdafx.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Main procedure 
	void action_read_serila_number()
	{

		// Local iconic variables
		HObject  ho_Image, ho_ImageMean, ho_RegionDynThresh;
		HObject  ho_RegionFillUp, ho_ContourBorder, ho_PolarTransImage;
		HObject  ho_RegionThreshold, ho_ConnectedRegions, ho_RegionChar;
		HObject  ho_RegionCharSort, ho_XYTransRegion;

		// Local control variables
		HTuple  hv_WindowHandle, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_Width, hv_Height, hv_Pi, hv_AnnulusInner, hv_AnnulusOuter;
		HTuple  hv_CharWidthFraction, hv_CenterRow, hv_CenterColumn;
		HTuple  hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
		HTuple  hv_WidthPolar, hv_HeightPolar, hv_RadiusStart, hv_RadiusEnd;
		HTuple  hv_AngleStart, hv_AngleEnd, hv_CharWidth, hv_CharHeight;
		HTuple  hv_OCRHandle, hv_Class, hv_Confidence, hv_SNString;

		//This program demonstrates the use of the polar transformation
		//to rectify the serial number on a CD. The rectified number can
		//be read by using OCR.
		//
		//Initialization
		dev_update_all("off");
		ReadImage(&ho_Image, "cd_print");
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		init_font(hv_WindowHandle, -1, 2);
		hv_Pi = HTuple(180).TupleRad();
		//
		//Initialize some variables
		hv_AnnulusInner = 0.90;
		hv_AnnulusOuter = 0.99;
		hv_CharWidthFraction = 0.01;
		//
		//Segment the outer circle
		MeanImage(ho_Image, &ho_ImageMean, 51, 51);
		DynThreshold(ho_Image, ho_ImageMean, &ho_RegionDynThresh, 15, "dark");
		FillUp(ho_RegionDynThresh, &ho_RegionFillUp);
		GenContourRegionXld(ho_RegionFillUp, &ho_ContourBorder, "border");
		FitCircleContourXld(ho_ContourBorder, "ahuber", -1, 0, 0, 3, 2, &hv_CenterRow,
			&hv_CenterColumn, &hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ContourBorder, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Segmented circle", 1);
		// stop(); only in hdevelop
		//
		//Transform the annulus with the characters to polar coordinates
		hv_WidthPolar = ((2 * hv_Pi)*hv_Radius)*hv_AnnulusOuter;
		hv_HeightPolar = hv_Radius*(hv_AnnulusOuter - hv_AnnulusInner);
		hv_RadiusStart = hv_Radius*hv_AnnulusOuter;
		hv_RadiusEnd = hv_Radius*hv_AnnulusInner;
		hv_AngleStart = (2 * hv_Pi) - ((2 * hv_Pi) / hv_WidthPolar);
		hv_AngleEnd = 0;
		PolarTransImageExt(ho_Image, &ho_PolarTransImage, hv_CenterRow, hv_CenterColumn,
			hv_AngleStart, hv_AngleEnd, hv_RadiusStart, hv_RadiusEnd, hv_WidthPolar, hv_HeightPolar,
			"bilinear");
		reopen_window_fit(ho_PolarTransImage, 1000, 0, 0, &hv_WindowHandle);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_PolarTransImage, HDevWindowStack::GetActive());
		// stop(); only in hdevelop
		//
		//Segment the characters
		hv_CharWidth = hv_WidthPolar*hv_CharWidthFraction;
		hv_CharHeight = hv_CharWidth;
		MeanImage(ho_PolarTransImage, &ho_ImageMean, 2 * hv_CharWidth, 2 * hv_CharHeight);
		DynThreshold(ho_PolarTransImage, ho_ImageMean, &ho_RegionThreshold, 10, "dark");
		Connection(ho_RegionThreshold, &ho_ConnectedRegions);
		SelectShape(ho_ConnectedRegions, &ho_RegionChar, ((HTuple("采样高度").Append("采样宽度")).Append("row")),
			"and", ((hv_CharHeight*0.1).TupleConcat(hv_CharWidth*0.3)).TupleConcat(hv_HeightPolar*0.25),
			((hv_CharHeight*1.1).TupleConcat(hv_CharWidth*1.1)).TupleConcat(hv_HeightPolar*0.75));
		SortRegion(ho_RegionChar, &ho_RegionCharSort, "character", "true", "row");
		if (HDevWindowStack::IsOpen())
			SetColored(HDevWindowStack::GetActive(), 12);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_PolarTransImage, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_RegionCharSort, HDevWindowStack::GetActive());
		// stop(); only in hdevelop
		//
		//Read the characters
		ReadOcrClassMlp("Industrial", &hv_OCRHandle);
		DoOcrMultiClassMlp(ho_RegionCharSort, ho_PolarTransImage, hv_OCRHandle, &hv_Class,
			&hv_Confidence);
		//Concatenate the single characters to one string
		hv_SNString = hv_Class.TupleSum();
		//
		//Visualize the results
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 2);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "S/N: " + hv_SNString, 1);
		PolarTransRegionInv(ho_RegionCharSort, &ho_XYTransRegion, hv_CenterRow, hv_CenterColumn,
			hv_AngleStart, hv_AngleEnd, hv_RadiusStart, hv_RadiusEnd, hv_WidthPolar, hv_HeightPolar,
			hv_Width, hv_Height, "nearest_neighbor");
		if (HDevWindowStack::IsOpen())
			SetColored(HDevWindowStack::GetActive(), 12);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_XYTransRegion, HDevWindowStack::GetActive());
		//
		//Clear the classifier
		ClearOcrClassMlp(hv_OCRHandle);
	}
}