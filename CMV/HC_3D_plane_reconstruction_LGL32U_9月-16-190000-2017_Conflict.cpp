#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;



using namespace HalconCpp;

namespace UCI {

	// Procedures 
	// Local procedures 
	// Chapter: 3D-Plane-Reconstruction-With-Stereo
	// Short Description: Calibration of a stereo camera setup. 
	void calibrate_stereo_setup(HTuple hv_ImagePath, HTuple hv_NumCalibImages, HTuple hv_CaltabName,
		HTuple hv_StartCamParam1, HTuple hv_StartCamParam2, HTuple hv_WindowHandle1,
		HTuple hv_WindowHandle2, HTuple *hv_CamParam1, HTuple *hv_CamParam2, HTuple *hv_NFinalPose1,
		HTuple *hv_NFinalPose2, HTuple *hv_RelPose, HTuple *hv_Errors)
	{

		// Local iconic variables
		HObject  ho_Image1, ho_Image2, ho_Caltab;

		// Local control variables
		HTuple  hv_Rows1, hv_Cols1, hv_StartPoses1, hv_Rows2;
		HTuple  hv_Cols2, hv_StartPoses2, hv_Index, hv_RCoord, hv_CCoord;
		HTuple  hv_StartPose, hv_X, hv_Y, hv_Z;

		hv_Rows1 = HTuple();
		hv_Cols1 = HTuple();
		hv_StartPoses1 = HTuple();
		hv_Rows2 = HTuple();
		hv_Cols2 = HTuple();
		hv_StartPoses2 = HTuple();
		{
			HTuple end_val6 = hv_NumCalibImages;
			HTuple step_val6 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val6, step_val6); hv_Index += step_val6)
			{
				//
				//Read the calibration images.
				HDevWindowStack::SetActive(hv_WindowHandle1);
				ReadImage(&ho_Image1, (hv_ImagePath + "calib_cam1_") + (hv_Index.TupleString("02d")));
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image1, HDevWindowStack::GetActive());
				HDevWindowStack::SetActive(hv_WindowHandle2);
				ReadImage(&ho_Image2, (hv_ImagePath + "calib_cam2_") + (hv_Index.TupleString("02d")));
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Image2, HDevWindowStack::GetActive());
				//
				//Find the calibration plate, extract the calibration marks and
				//determine the initial pose of the calibration plate.
				HDevWindowStack::SetActive(hv_WindowHandle1);
				FindCaltab(ho_Image1, &ho_Caltab, hv_CaltabName, 5, 85, 10);
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Caltab, HDevWindowStack::GetActive());
				FindMarksAndPose(ho_Image1, ho_Caltab, hv_CaltabName, hv_StartCamParam1, 128,
					10, 18, 0.5, 15, 100, &hv_RCoord, &hv_CCoord, &hv_StartPose);
				hv_Rows1 = hv_Rows1.TupleConcat(hv_RCoord);
				hv_Cols1 = hv_Cols1.TupleConcat(hv_CCoord);
				hv_StartPoses1 = hv_StartPoses1.TupleConcat(hv_StartPose);
				HDevWindowStack::SetActive(hv_WindowHandle2);
				FindCaltab(ho_Image2, &ho_Caltab, hv_CaltabName, 5, 85, 10);
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Caltab, HDevWindowStack::GetActive());
				FindMarksAndPose(ho_Image2, ho_Caltab, hv_CaltabName, hv_StartCamParam2, 128,
					10, 18, 0.5, 15, 100, &hv_RCoord, &hv_CCoord, &hv_StartPose);
				hv_Rows2 = hv_Rows2.TupleConcat(hv_RCoord);
				hv_Cols2 = hv_Cols2.TupleConcat(hv_CCoord);
				hv_StartPoses2 = hv_StartPoses2.TupleConcat(hv_StartPose);
			}
		}
		//
		//Perform the calibration.
		CaltabPoints(hv_CaltabName, &hv_X, &hv_Y, &hv_Z);
		BinocularCalibration(hv_X, hv_Y, hv_Z, hv_Rows1, hv_Cols1, hv_Rows2, hv_Cols2,
			hv_StartCamParam1, hv_StartCamParam2, hv_StartPoses1, hv_StartPoses2, "all",
			&(*hv_CamParam1), &(*hv_CamParam2), &(*hv_NFinalPose1), &(*hv_NFinalPose2),
			&(*hv_RelPose), &(*hv_Errors));
		return;
	}

	// Chapter: 3D-Plane-Reconstruction-With-Stereo
	// Short Description: Finds faces with a gradient direction of about 0 and 180 degree 
	void find_two_planes(HObject ho_Distance, HObject *ho_Faces, HTuple hv_CamParamRect1,
		HTuple *hv_NormalVector1, HTuple *hv_NormalVector2)
	{

		// Local iconic variables
		HObject  ho_Domain, ho_DistanceScaled, ho_DistanceConverted;
		HObject  ho_ImaAmp, ho_ImaDir, ho_Region, ho_FirstRegion;
		HObject  ho_LastRegion, ho_RegionUnion, ho_ObjectsSelected;
		HObject  ho_Regions, ho_FaceCandidates, ho_ObjectSelected;
		HObject  ho_ConnectedRegions, ho_LargestRegion, ho_RegionOpening;
		HObject  ho_ImageSurface, ho_ImageReduced, ho_ImageSub, ho_RegionTrans;
		HObject  ho_RegionErosion;

		// Local control variables
		HTuple  hv_ExpDefaultCtrlDummyVar, hv_Width, hv_Height;
		HTuple  hv_Min, hv_Max, hv_Range, hv_AbsoluteHisto, hv_RelativeHisto;
		HTuple  hv_HistoFunction, hv_SmoothedHistoFunction, hv_Min1;
		HTuple  hv_Max1, hv_MinThresh, hv_Number, hv_Deviations;
		HTuple  hv_FaceCandidateNx, hv_FaceCandidateNy, hv_FaceCandidateNz;
		HTuple  hv_Index, hv_Area, hv_Row, hv_Col, hv_Alpha, hv_Beta;
		HTuple  hv_Gamma, hv_Mean, hv_Deviation, hv_Nx, hv_Ny, hv_Nz;
		HTuple  hv_Length, hv_FaceIndices;

		GetDomain(ho_Distance, &ho_Domain);
		GetImagePointer1(ho_Distance, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width, &hv_Height);
		MinMaxGray(ho_Domain, ho_Distance, 0, &hv_Min, &hv_Max, &hv_Range);
		ScaleImage(ho_Distance, &ho_DistanceScaled, (HTuple(2).TuplePow(16)) / hv_Range,
			((-hv_Min)*(HTuple(2).TuplePow(16))) / hv_Range);
		ConvertImageType(ho_DistanceScaled, &ho_DistanceConverted, "uint2");
		EdgesImage(ho_DistanceConverted, &ho_ImaAmp, &ho_ImaDir, "canny", 1.5, "none",
			20, 40);
		GrayHisto(ho_Domain, ho_ImaDir, &hv_AbsoluteHisto, &hv_RelativeHisto);
		CreateFunct1dArray((hv_AbsoluteHisto.TupleSelectRange(0, 179)).TupleConcat(hv_AbsoluteHisto.TupleSelectRange(0, 179)),
			&hv_HistoFunction);
		SmoothFunct1dGauss(hv_HistoFunction, 4, &hv_SmoothedHistoFunction);
		LocalMinMaxFunct1d(hv_SmoothedHistoFunction, "strict_min_max", "true", &hv_Min1,
			&hv_Max1);
		hv_MinThresh = hv_Min1.TupleSelectRange(0, HTuple((((hv_Min1 - 179).TupleSgn()).TupleFind(1))[0]) - 1);
		Threshold(ho_ImaDir, &ho_Region, HTuple(0).TupleConcat((hv_MinThresh.TupleInt()) + 1),
			(hv_MinThresh.TupleInt()).TupleConcat(179));
		CountObj(ho_Region, &hv_Number);
		SelectObj(ho_Region, &ho_FirstRegion, 1);
		SelectObj(ho_Region, &ho_LastRegion, hv_Number);
		Union2(ho_FirstRegion, ho_LastRegion, &ho_RegionUnion);
		CopyObj(ho_Region, &ho_ObjectsSelected, 2, hv_Number - 2);
		ConcatObj(ho_RegionUnion, ho_ObjectsSelected, &ho_Regions);
		CountObj(ho_Regions, &hv_Number);
		hv_Deviations = HTuple();
		GenEmptyObj(&ho_FaceCandidates);
		hv_FaceCandidateNx = HTuple();
		hv_FaceCandidateNy = HTuple();
		hv_FaceCandidateNz = HTuple();
		{
			HTuple end_val24 = hv_Number;
			HTuple step_val24 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val24, step_val24); hv_Index += step_val24)
			{
				SelectObj(ho_Regions, &ho_ObjectSelected, hv_Index);
				Connection(ho_ObjectSelected, &ho_ConnectedRegions);
				AreaCenter(ho_ConnectedRegions, &hv_Area, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar);
				SelectObj(ho_ConnectedRegions, &ho_LargestRegion, HTuple(((-hv_Area).TupleSortIndex())[0]) + 1);
				OpeningCircle(ho_LargestRegion, &ho_RegionOpening, 3.5);
				AreaCenter(ho_RegionOpening, &hv_ExpDefaultCtrlDummyVar, &hv_Row, &hv_Col);
				FitSurfaceFirstOrder(ho_RegionOpening, ho_Distance, "regression", 5, 2, &hv_Alpha,
					&hv_Beta, &hv_Gamma);
				GenImageSurfaceFirstOrder(&ho_ImageSurface, "real", hv_Alpha, hv_Beta, hv_Gamma,
					hv_Row, hv_Col, hv_Width, hv_Height);
				ReduceDomain(ho_ImageSurface, ho_RegionOpening, &ho_ImageReduced);
				SubImage(ho_Distance, ho_ImageReduced, &ho_ImageSub, 1, 0);
				Intensity(ho_RegionOpening, ho_ImageSub, &hv_Mean, &hv_Deviation);
				hv_Deviations = hv_Deviations.TupleConcat(hv_Deviation);
				ConcatObj(ho_FaceCandidates, ho_RegionOpening, &ho_FaceCandidates);
				hv_Nx = (-HTuple(hv_CamParamRect1[3]))*hv_Alpha;
				hv_Ny = (-HTuple(hv_CamParamRect1[2]))*hv_Beta;
				hv_Nz = HTuple(hv_CamParamRect1[3])*HTuple(hv_CamParamRect1[2]);
				hv_Length = (((hv_Nx*hv_Nx) + (hv_Ny*hv_Ny)) + (hv_Nz*hv_Nz)).TupleSqrt();
				hv_FaceCandidateNx = hv_FaceCandidateNx.TupleConcat(hv_Nx / hv_Length);
				hv_FaceCandidateNy = hv_FaceCandidateNy.TupleConcat(hv_Ny / hv_Length);
				hv_FaceCandidateNz = hv_FaceCandidateNz.TupleConcat(hv_Nz / hv_Length);
			}
		}
		ShapeTrans(ho_FaceCandidates, &ho_RegionTrans, "convex");
		ErosionCircle(ho_RegionTrans, &ho_RegionErosion, 3.5);
		hv_FaceIndices = (hv_Deviations.TupleSortIndex()).TupleSelectRange(0, 1);
		SelectObj(ho_RegionErosion, &(*ho_Faces), hv_FaceIndices + 1);
		(*hv_NormalVector1).Clear();
		(*hv_NormalVector1).Append(HTuple(hv_FaceCandidateNx[HTuple(hv_FaceIndices[0])]));
		(*hv_NormalVector1).Append(HTuple(hv_FaceCandidateNy[HTuple(hv_FaceIndices[0])]));
		(*hv_NormalVector1).Append(HTuple(hv_FaceCandidateNz[HTuple(hv_FaceIndices[0])]));
		(*hv_NormalVector2).Clear();
		(*hv_NormalVector2).Append(HTuple(hv_FaceCandidateNx[HTuple(hv_FaceIndices[1])]));
		(*hv_NormalVector2).Append(HTuple(hv_FaceCandidateNy[HTuple(hv_FaceIndices[1])]));
		(*hv_NormalVector2).Append(HTuple(hv_FaceCandidateNz[HTuple(hv_FaceIndices[1])]));
		//stop ()
		//
		return;
	}

	// Chapter: 3D-Plane-Reconstruction-With-Stereo
	// Short Description: Visualize a distance image. 
	void visualize_3d_plot(HObject ho_Distance, HTuple hv_WindowHandle, HTuple hv_CamParamRect1)
	{

		// Local iconic variables
		HObject  ho_Domain, ho_DistanceScaled, ho_DistanceRotated;
		HObject  ho_DomainRotated, ho_DomainFull, ho_DomainComplement;
		HObject  ho_Tube3D;

		// Local control variables
		HTuple  hv_Min, hv_Max, hv_Range, hv_HomMat2DIdentity;
		HTuple  hv_RotMat2D;

		HDevWindowStack::SetActive(hv_WindowHandle);
		GetDomain(ho_Distance, &ho_Domain);
		MinMaxGray(ho_Domain, ho_Distance, 0, &hv_Min, &hv_Max, &hv_Range);
		ScaleImage(ho_Distance, &ho_DistanceScaled, -1.0, hv_Max);
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dRotate(hv_HomMat2DIdentity, HTuple(90.).TupleRad(), HTuple(hv_CamParamRect1[7]) / 2,
			HTuple(hv_CamParamRect1[6]) / 2, &hv_RotMat2D);
		AffineTransImage(ho_DistanceScaled, &ho_DistanceRotated, hv_RotMat2D, "constant",
			"false");
		AffineTransRegion(ho_Domain, &ho_DomainRotated, hv_RotMat2D, "false");
		FullDomain(ho_DistanceRotated, &ho_DistanceRotated);
		GetDomain(ho_DistanceRotated, &ho_DomainFull);
		Difference(ho_DomainFull, ho_DomainRotated, &ho_DomainComplement);
		PaintRegion(ho_DomainComplement, ho_DistanceRotated, &ho_Tube3D, 0.0, "fill");
		if (HDevWindowStack::IsOpen())
			SetPaint(HDevWindowStack::GetActive(), (((((((HTuple("3D-plot_hidden").Append(7)).Append(1)).Append(110)).Append(120)).Append(200)).Append(-60)).Append(-25)));
		if (HDevWindowStack::IsOpen())
			SetLut(HDevWindowStack::GetActive(), "hsi");
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Tube3D, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetPaint(HDevWindowStack::GetActive(), "default");
		if (HDevWindowStack::IsOpen())
			SetLut(HDevWindowStack::GetActive(), "default");
		write_message(hv_WindowHandle, -1, -1, "3D plot of the reconstructed surface",
			0);
		return;
	}

	// Chapter: Graphics / Window
	// Short Description: Opens a graphics window with a given maximum extent such that it preserves the aspect ratio of the given image 
	void open_window_fit(HObject ho_Image, HTuple hv_MaxExtent, HTuple hv_Row, HTuple hv_Col,
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
		SetWindowAttr("background_color", "black");
		OpenWindow(hv_Row, hv_Col, hv_WindowWidth, hv_WindowHeight, 0, "", "", &(*hv_WindowHandle));
		HDevWindowStack::Push((*hv_WindowHandle));
		if (HDevWindowStack::IsOpen())
			SetPart(HDevWindowStack::GetActive(), 0, 0, hv_Height - 1, hv_Width - 1);
		return;
	}


	// Main procedure 
	void action_3D_reconstrucion()
	{

		// Local iconic variables
		HObject  ho_Image1, ho_Image2, ho_Map1, ho_Map2;
		HObject  ho_ImageRect1, ho_ImageRect2, ho_Distance, ho_Score;
		HObject  ho_FaceRegions;

		// Local control variables
		HTuple  hv_ImagePath, hv_ExpDefaultCtrlDummyVar;
		HTuple  hv_Width1, hv_Height1, hv_WindowHandle1, hv_Width2;
		HTuple  hv_Height2, hv_WindowHandle2, hv_CaltabName, hv_StartCamParam1;
		HTuple  hv_StartCamParam2, hv_NumCalibImages, hv_NumObjectImages;
		HTuple  hv_CamParam1, hv_CamParam2, hv_NFinalPose1, hv_NFinalPose2;
		HTuple  hv_RelPose, hv_Errors, hv_CamParamRect1, hv_CamParamRect2;
		HTuple  hv_CamPoseRect1, hv_CamPoseRect2, hv_RelPoseRect;
		HTuple  hv_Index, hv_MaxDisparity, hv_MinDisparity, hv_NormalVector1;
		HTuple  hv_NormalVector2, hv_FaceAngle;

		//
		//In this application, the angle between two 3d planes is determined.
		//First, the object surface is reconstructed with a stereo approach.
		//Then, two planes are fitted into the surface and the angle between them
		//is calculated.
		//
		//Read one image and initialize the program.
		hv_ImagePath = "stereo/planes/";
		ReadImage(&ho_Image1, hv_ImagePath + "calib_cam1_01");
		GetImagePointer1(ho_Image1, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width1, &hv_Height1);
		reopen_window_fit(ho_Image1, 320, 0, 0, &hv_WindowHandle1);
		init_font(hv_WindowHandle1, -1, 1);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), (HTuple("blue").Append("yellow")));
		ReadImage(&ho_Image2, hv_ImagePath + "calib_cam2_01");
		GetImagePointer1(ho_Image2, &hv_ExpDefaultCtrlDummyVar, &hv_ExpDefaultCtrlDummyVar,
			&hv_Width2, &hv_Height2);
		open_window_fit(ho_Image2, 320, 0, 330, &hv_WindowHandle2);
		init_font(hv_WindowHandle2, -1, 1);
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetLineWidth(HDevWindowStack::GetActive(), 2);
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), (HTuple("blue").Append("yellow")));
		dev_update_all("off");
		hv_CaltabName = "caltab_30mm.descr";
		hv_StartCamParam1.Clear();
		hv_StartCamParam1[0] = 0.025;
		hv_StartCamParam1[1] = 0;
		hv_StartCamParam1[2] = 7.5e-6;
		hv_StartCamParam1[3] = 7.5e-6;
		hv_StartCamParam1.Append(hv_Width1 / 2.0);
		hv_StartCamParam1.Append(hv_Height1 / 2.0);
		hv_StartCamParam1.Append(hv_Width1);
		hv_StartCamParam1.Append(hv_Height1);
		hv_StartCamParam2.Clear();
		hv_StartCamParam2[0] = 0.025;
		hv_StartCamParam2[1] = 0;
		hv_StartCamParam2[2] = 7.5e-6;
		hv_StartCamParam2[3] = 7.5e-6;
		hv_StartCamParam2.Append(hv_Width2 / 2.0);
		hv_StartCamParam2.Append(hv_Height2 / 2.0);
		hv_StartCamParam2.Append(hv_Width2);
		hv_StartCamParam2.Append(hv_Height2);
		hv_NumCalibImages = 11;
		hv_NumObjectImages = 25;
		//
		//Calibration of the stereo setup.
		calibrate_stereo_setup(hv_ImagePath, hv_NumCalibImages, hv_CaltabName, hv_StartCamParam1,
			hv_StartCamParam2, hv_WindowHandle1, hv_WindowHandle2, &hv_CamParam1, &hv_CamParam2,
			&hv_NFinalPose1, &hv_NFinalPose2, &hv_RelPose, &hv_Errors);
		//
		//Create the rectification maps for the rectification of the stereo images.
		GenBinocularRectificationMap(&ho_Map1, &ho_Map2, hv_CamParam1, hv_CamParam2, hv_RelPose,
			1, "geometric", "bilinear", &hv_CamParamRect1, &hv_CamParamRect2, &hv_CamPoseRect1,
			&hv_CamPoseRect2, &hv_RelPoseRect);
		//
		{
			HTuple end_val35 = hv_NumObjectImages;
			HTuple step_val35 = 1;
			for (hv_Index = 1; hv_Index.Continue(end_val35, step_val35); hv_Index += step_val35)
			{
				//
				//Read and rectify the object images.
				HDevWindowStack::SetActive(hv_WindowHandle1);
				ReadImage(&ho_Image1, (hv_ImagePath + "planes_cam1_") + (hv_Index.TupleString("02d")));
				MapImage(ho_Image1, ho_Map1, &ho_ImageRect1);
				if (HDevWindowStack::IsOpen())
					ClearWindow(HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageRect1, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle1, -1, -1, "Rectified image 1", 0);
				HDevWindowStack::SetActive(hv_WindowHandle2);
				ReadImage(&ho_Image2, (hv_ImagePath + "planes_cam2_") + (hv_Index.TupleString("02d")));
				MapImage(ho_Image2, ho_Map2, &ho_ImageRect2);
				if (HDevWindowStack::IsOpen())
					ClearWindow(HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageRect2, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle2, -1, -1, "Rectified image 2", 0);
				write_message(hv_WindowHandle2, 440, -1, "Press 'Run' to continue", 0);
				// stop(); only in hdevelop
				//
				//Determine the distance map.
				hv_MaxDisparity = -20;
				hv_MinDisparity = -70;
				BinocularDistance(ho_ImageRect1, ho_ImageRect2, &ho_Distance, &ho_Score, hv_CamParamRect1,
					hv_CamParamRect2, hv_RelPoseRect, "ncc", 21, 21, 5, hv_MinDisparity, hv_MaxDisparity,
					4, 0.1, "left_right_check", "interpolation");
				HDevWindowStack::SetActive(hv_WindowHandle1);
				if (HDevWindowStack::IsOpen())
					ClearWindow(HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Distance, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle1, -1, -1, "Distance image", 0);
				HDevWindowStack::SetActive(hv_WindowHandle2);
				if (HDevWindowStack::IsOpen())
					ClearWindow(HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_Score, HDevWindowStack::GetActive());
				write_message(hv_WindowHandle2, -1, -1, "Score image", 0);
				write_message(hv_WindowHandle2, 440, -1, "Press 'Run' to continue", 0);
				// stop(); only in hdevelop
				//
				//Determine the two faces.
				find_two_planes(ho_Distance, &ho_FaceRegions, hv_CamParamRect1, &hv_NormalVector1,
					&hv_NormalVector2);
				HDevWindowStack::SetActive(hv_WindowHandle1);
				if (HDevWindowStack::IsOpen())
					DispObj(ho_ImageRect1, HDevWindowStack::GetActive());
				if (HDevWindowStack::IsOpen())
					DispObj(ho_FaceRegions, HDevWindowStack::GetActive());
				//
				//Calculate the angle between the two faces.
				hv_FaceAngle = (((hv_NormalVector1*hv_NormalVector2).TupleSum()).TupleAcos()).TupleDeg();
				write_message(hv_WindowHandle1, -1, -1, ("Angle between the two planes: " + (hv_FaceAngle.TupleString(".1f"))) + "°",
					0);
				//
				//DisDisplay a 3D plot of the object surface.
				visualize_3d_plot(ho_Distance, hv_WindowHandle2, hv_CamParamRect1);
				write_message(hv_WindowHandle2, 440, -1, "Press 'Run' to continue", 0);
				// stop(); only in hdevelop
			}
		}
		HDevWindowStack::SetActive(hv_WindowHandle2);
		if (HDevWindowStack::IsOpen())
			CloseWindow(HDevWindowStack::Pop());
		HDevWindowStack::SetActive(hv_WindowHandle1);
		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		write_message(hv_WindowHandle1, 440, -1, " Program terminated  ", 0);
	}

}