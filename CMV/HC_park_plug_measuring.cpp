#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;

namespace UCI {

	// Procedures 
	// Local procedures 
	// Chapter: Resistor-Verification
	// Short Description: Translate the shape model contour to its reference pose 
	void get_shape_model_contour_ref(HObject ho_ModelRegion, HObject *ho_ModelContoursTrans,
		HTuple hv_ModelID)
	{

		// Local iconic variables
		HObject  ho_ModelContours;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Row, hv_Column;
		HTuple  hv_HomMat2DIdentity, hv_HomMat2DTranslate;

		AreaCenter(ho_ModelRegion, &hv_ExpDefaultCtrlDummyVar, &hv_Row, &hv_Column);
		GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dTranslate(hv_HomMat2DIdentity, hv_Row, hv_Column, &hv_HomMat2DTranslate);
		AffineTransContourXld(ho_ModelContours, &(*ho_ModelContoursTrans), hv_HomMat2DTranslate);
		return;
	}




	void action_park_plug_measuring() {

		// Local iconic variables
		HObject  ho_Image, ho_Caltab, ho_ModelImage, ho_ModelRegion;
		HObject  ho_TemplateImage, ho_ModelContours, ho_ModelContoursTrans;
		HObject  ho_CrossRef, ho_MeasureRectangle, ho_Edge1, ho_Edge2;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Width, hv_Height;
		HTuple  hv_WindowHandle, hv_WindowWidth, hv_WindowHeight;
		HTuple  hv_CaltabName, hv_StartCamParam, hv_StartPoses;
		HTuple  hv_Rows, hv_Cols, hv_Index, hv_RCoord, hv_CCoord;
		HTuple  hv_StartPose, hv_X, hv_Y, hv_Z, hv_CamParam, hv_FinalPoses;
		HTuple  hv_Errors, hv_ModelID, hv_Area, hv_RefRow, hv_RefCol;
		HTuple  hv_RectRelRow, hv_RectRelCol, hv_RectRelPhi, hv_RectLen1;
		HTuple  hv_RectLen2, hv_GapSizeMin, hv_GapSizeMax, hv_Row;
		HTuple  hv_Column, hv_Angle, hv_Score, hv_HomMat2D, hv_TransRow;
		HTuple  hv_TransCol, hv_MeasureHandle, hv_Row1, hv_Col1;
		HTuple  hv_Amplitude1, hv_Row2, hv_Col2, hv_Amplitude2;
		HTuple  hv_IntraDistance, hv_InterDistance, hv_StatusText;
		HTuple  hv_XH, hv_YH, hv_ZH, hv_DX, hv_DY, hv_GapSize;


		dev_update_all("off");
		ReadImage(&ho_Image, "spark_plug/spark_plug_01");
		GetImagePointer1(ho_Image, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		GetWindowExtents(hv_WindowHandle, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_WindowWidth, &hv_WindowHeight);
		init_font(hv_WindowHandle, -1, 2);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 3);
		//
		//Calibrate the camera
		hv_CaltabName = "caltab_10mm.descr";
		hv_StartCamParam.Clear();
		hv_StartCamParam[0] = 0;
		hv_StartCamParam[1] = 0;
		hv_StartCamParam[2] = 39e-6;
		hv_StartCamParam[3] = 39e-6;
		hv_StartCamParam[4] = 320;
		hv_StartCamParam[5] = 240;
		hv_StartCamParam[6] = 640;
		hv_StartCamParam[7] = 480;
		hv_StartPoses = HTuple();
		hv_Rows = HTuple();
		hv_Cols = HTuple();
		for (hv_Index = 1; hv_Index <= 14; hv_Index += 1)
		{
			ReadImage(&ho_Image, "spark_plug/spark_plug_calib_" + (hv_Index.TupleString("02")));
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			write_message(hv_WindowHandle, -1, -1, "Collect calibration data", 1);
			FindCaltab(ho_Image, &ho_Caltab, hv_CaltabName, 3, 100, 5);
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "green");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Caltab, HDevWindowStack::GetActive());
			FindMarksAndPose(ho_Image, ho_Caltab, hv_CaltabName, hv_StartCamParam, 128, 10,
				18, 0.6, 15, 100, &hv_RCoord, &hv_CCoord, &hv_StartPose);
			hv_StartPoses = hv_StartPoses.TupleConcat(hv_StartPose);
			hv_Rows = hv_Rows.TupleConcat(hv_RCoord);
			hv_Cols = hv_Cols.TupleConcat(hv_CCoord);
		}
		write_message(hv_WindowHandle, -1, -1, "Calibration of the telecentric camera",
			1);
		CaltabPoints(hv_CaltabName, &hv_X, &hv_Y, &hv_Z);
		CameraCalibration(hv_X, hv_Y, hv_Z, hv_Rows, hv_Cols, hv_StartCamParam, hv_StartPoses,
			"all", &hv_CamParam, &hv_FinalPoses, &hv_Errors);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Create a shape model of the spark plug
		ReadImage(&ho_ModelImage, "spark_plug/spark_plug_01");
		GenRectangle1(&ho_ModelRegion, 120, 230, 220, 445);
		ReduceDomain(ho_ModelImage, ho_ModelRegion, &ho_TemplateImage);
		CreateShapeModel(ho_TemplateImage, "auto", HTuple(-30).TupleRad(), HTuple(60).TupleRad(),
			"auto", "none", "use_polarity", "auto", "auto", &hv_ModelID);
		AreaCenter(ho_ModelRegion, &hv_Area, &hv_RefRow, &hv_RefCol);
		GetShapeModelContours(&ho_ModelContours, hv_ModelID, 1);
		get_shape_model_contour_ref(ho_ModelRegion, &ho_ModelContoursTrans, hv_ModelID);
		GenCrossContourXld(&ho_CrossRef, hv_RefRow, hv_RefCol, 20, 0);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelImage, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "magenta");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelContoursTrans, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_CrossRef, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Contour and reference point of the shape model",
			1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Define the measurement rectangle
		hv_RectRelRow = 80;
		hv_RectRelCol = -26;
		hv_RectRelPhi = HTuple(90).TupleRad();
		hv_RectLen1 = 50;
		hv_RectLen2 = 15;
		GenRectangle2ContourXld(&ho_MeasureRectangle, hv_RefRow + hv_RectRelRow, hv_RefCol + hv_RectRelCol,
			hv_RectRelPhi, hv_RectLen1, hv_RectLen2);
		if (HDevWindowStack::IsOpen())
			DispObj(ho_ModelImage, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_MeasureRectangle, HDevWindowStack::GetActive());
		write_message(hv_WindowHandle, -1, -1, "Measurement rectangle", 1);
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
			1);
		// stop(); only in hdevelop
		//
		//Specify the tolerance of the gap in meters
		hv_GapSizeMin = 0.78e-3;
		hv_GapSizeMax = 0.88e-3;
		//
		//Main loop
		for (hv_Index = 2; hv_Index <= 20; hv_Index += 1)
		{
			ReadImage(&ho_Image, "spark_plug/spark_plug_" + (hv_Index.TupleString("02")));
			//Find the spark plug in the image
			FindShapeModel(ho_Image, hv_ModelID, HTuple(-30).TupleRad(), HTuple(60).TupleRad(),
				0.7, 1, 0.5, "least_squares", 0, 0.9, &hv_Row, &hv_Column, &hv_Angle, &hv_Score);
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			if (0 != ((hv_Score.TupleLength()) > 0))
			{
				VectorAngleToRigid(0, 0, 0, hv_Row, hv_Column, hv_Angle, &hv_HomMat2D);
				//DisDisplay the match
				AffineTransContourXld(ho_ModelContours, &ho_ModelContoursTrans, hv_HomMat2D);
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "magenta");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ModelContoursTrans, HDevWindowStack::GetActive());
				//Transform the model reference point
				AffineTransPixel(hv_HomMat2D, hv_RectRelRow, hv_RectRelCol, &hv_TransRow, &hv_TransCol);
				//DisDisplay measurement rectangle
				GenRectangle2ContourXld(&ho_MeasureRectangle, hv_TransRow, hv_TransCol, hv_Angle + hv_RectRelPhi,
					hv_RectLen1, hv_RectLen2);
				if (HDevWindowStack::IsOpen())
					SetColor(HDevWindowStack::GetActive(), "blue");
				if (HDevWindowStack::IsOpen())
					DispObj(ho_MeasureRectangle, HDevWindowStack::GetActive());
				//Perform the measurement
				GenMeasureRectangle2(hv_TransRow, hv_TransCol, hv_Angle + hv_RectRelPhi, hv_RectLen1,
					hv_RectLen2, hv_Width, hv_Height, "bilinear", &hv_MeasureHandle);
				MeasurePairs(ho_Image, hv_MeasureHandle, 1, 30, "positive_strongest", "all",
					&hv_Row1, &hv_Col1, &hv_Amplitude1, &hv_Row2, &hv_Col2, &hv_Amplitude2,
					&hv_IntraDistance, &hv_InterDistance);
				CloseMeasure(hv_MeasureHandle);
				if (0 != ((hv_IntraDistance.TupleLength()) != 1))
				{
					//Measuring failed
					hv_StatusText = "Measuring failed";
				}
				else
				{
					//Transform measuring result into world units
					GetLineOfSight(hv_Row1.TupleConcat(hv_Row2), hv_Col1.TupleConcat(hv_Col2),
						hv_CamParam, &hv_X, &hv_Y, &hv_Z, &hv_XH, &hv_YH, &hv_ZH);
					hv_DX = HTuple(hv_X[1]) - HTuple(hv_X[0]);
					hv_DY = HTuple(hv_Y[1]) - HTuple(hv_Y[0]);
					hv_GapSize = ((hv_DX*hv_DX) + (hv_DY*hv_DY)).TupleSqrt();
					//Evaluate the result
					if (0 != (hv_GapSize < hv_GapSizeMin))
					{
						//Gap size is too small
						hv_StatusText = "Gap size too small";
						if (HDevWindowStack::IsOpen())
							SetColor(HDevWindowStack::GetActive(), "red");
					}
					else
					{
						if (0 != (hv_GapSize > hv_GapSizeMax))
						{
							//Gap size is too large
							hv_StatusText = "Gap size too large";
							if (HDevWindowStack::IsOpen())
								SetColor(HDevWindowStack::GetActive(), "red");
						}
						else
						{
							//Gap size is within tolerances
							hv_StatusText = "Gap size ok";
							if (HDevWindowStack::IsOpen())
								SetColor(HDevWindowStack::GetActive(), "green");
						}
					}
					//
					//DisDisplay the measuring results
					SetTposition(hv_WindowHandle, (0.5*(hv_Row1 + hv_Row2)) - 7, hv_Col1 + 50);
					WriteString(hv_WindowHandle, ((1000 * hv_GapSize).TupleString(".3f")) + " mm");
					GenContourPolygonXld(&ho_Edge1, (hv_Row1 + (40 * (hv_Angle.TupleSin()))).TupleConcat(hv_Row1 - (40 * (hv_Angle.TupleSin()))),
						(hv_Col1 - (40 * (hv_Angle.TupleCos()))).TupleConcat(hv_Col1 + (40 * (hv_Angle.TupleCos()))));
					GenContourPolygonXld(&ho_Edge2, (hv_Row2 + (40 * (hv_Angle.TupleSin()))).TupleConcat(hv_Row2 - (40 * (hv_Angle.TupleSin()))),
						(hv_Col2 - (40 * (hv_Angle.TupleCos()))).TupleConcat(hv_Col2 + (40 * (hv_Angle.TupleCos()))));
					if (HDevWindowStack::IsOpen())
						DispObj(ho_Edge1, HDevWindowStack::GetActive());
					if (HDevWindowStack::IsOpen())
						DispObj(ho_Edge2, HDevWindowStack::GetActive());
				}
			}
			else
			{
				//No spark plug found
				hv_StatusText = "No spark plug found in the image";
			}
			//
			//Output status message
			write_message(hv_WindowHandle, -1, -1, hv_StatusText, 1);
			write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "Press 'Run' to continue",
				1);
			// stop(); only in hdevelop
		}
		write_message(hv_WindowHandle, hv_WindowHeight - 30, -1, "  Program terminated   ",
			1);
	}

}