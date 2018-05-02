#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


namespace UCI {
	// Procedures 
	// Local procedures 
	// Chapter: BGA-Inspection
	// Short Description: Segments the balls of a BGA. 
	void segment_balls(HObject ho_Image, HObject *ho_Balls, HTuple hv_BrighterThan,
		HTuple hv_MinArea, HTuple hv_MaxArea)
	{

		// Local iconic variables
		HObject  ho_Region, ho_ConnectedRegions, ho_RegionFillUp;

		Threshold(ho_Image, &ho_Region, hv_BrighterThan, 255);
		Connection(ho_Region, &ho_ConnectedRegions);
		FillUp(ho_ConnectedRegions, &ho_RegionFillUp);
		SelectShape(ho_RegionFillUp, &(*ho_Balls), (HTuple("area").Append("circularity")),
			"and", (0.8*hv_MinArea).TupleConcat(0.75), (1.2*hv_MaxArea).TupleConcat(1.0));
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Determine balls that have the wrong size or the wrong shape. 
	void check_balls(HObject ho_Balls, HObject *ho_WrongAreaBalls, HObject *ho_WrongAnisometryBalls,
		HTuple hv_MinArea, HTuple hv_MaxArea, HTuple hv_MaxAnisometry)
	{

		SelectShape(ho_Balls, &(*ho_WrongAreaBalls), (HTuple("area").Append("area")), "or",
			HTuple(0).TupleConcat(hv_MaxArea), hv_MinArea.TupleConcat(10000));
		SelectShape(ho_Balls, &(*ho_WrongAnisometryBalls), "anisometry", "and", hv_MaxAnisometry,
			100);
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Segments the balls of a BGA with sub-pixel accuracy. 
	void segment_balls_sub_pix(HObject ho_Image, HObject *ho_Balls, HTuple hv_BrighterThan,
		HTuple hv_MinArea, HTuple hv_MaxArea)
	{

		// Local iconic variables
		HObject  ho_BallRegions, ho_RegionUnion, ho_RegionDilation;
		HObject  ho_ImageReduced, ho_Boundary;

		segment_balls(ho_Image, &ho_BallRegions, hv_BrighterThan, hv_MinArea, hv_MaxArea);
		Union1(ho_BallRegions, &ho_RegionUnion);
		DilationRectangle1(ho_RegionUnion, &ho_RegionDilation, 5, 5);
		ReduceDomain(ho_Image, ho_RegionDilation, &ho_ImageReduced);
		ThresholdSubPix(ho_ImageReduced, &ho_Boundary, hv_BrighterThan);
		SelectShapeXld(ho_Boundary, &(*ho_Balls), "area", "or", 0.8*hv_MinArea, 1.2*hv_MaxArea);
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Determine balls that have the wrong size or the wrong shape. 
	void check_balls_sub_pix(HObject ho_Balls, HObject *ho_WrongAreaBalls, HObject *ho_WrongAnisometryBalls,
		HTuple hv_MinArea, HTuple hv_MaxArea, HTuple hv_MaxAnisometry)
	{

		SelectShapeXld(ho_Balls, &(*ho_WrongAreaBalls), (HTuple("area").Append("area")),
			"or", HTuple(0).TupleConcat(hv_MaxArea), hv_MinArea.TupleConcat(10000));
		SelectShapeXld(ho_Balls, &(*ho_WrongAnisometryBalls), "anisometry", "or", hv_MaxAnisometry,
			100);
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Displays the checked balls of a BGA in different colors. 
	void display_checked_balls(HObject ho_Image, HObject ho_Balls, HObject ho_WrongAreaBalls,
		HObject ho_WrongAnisometryBalls)
	{

		if (HDevWindowStack::IsOpen())
			DispObj(ho_Image, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetDraw(HDevWindowStack::GetActive(), "margin");
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Balls, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "blue");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WrongAreaBalls, HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "magenta");
		if (HDevWindowStack::IsOpen())
			DispObj(ho_WrongAnisometryBalls, HDevWindowStack::GetActive());
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Creates a BGA model that provides the reference positions of the balls. 
	void gen_bga_model(HTuple hv_BgaLayout, HTuple hv_BallDistRowRef, HTuple hv_BallDistColRef,
		HTuple *hv_BallMatrixRef, HTuple *hv_BallsRowsRef, HTuple *hv_BallsColsRef, HTuple *hv_BallsPerRow,
		HTuple *hv_BallsPerCol)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_CenterRow, hv_CenterCol, hv_I, hv_R;
		HTuple  hv_C, hv_Dist;

		(*hv_BallsPerRow) = 2 * (hv_BgaLayout.TupleLength());
		(*hv_BallsPerCol) = 2 * (hv_BgaLayout.TupleLength());
		(*hv_BallMatrixRef) = HTuple((*hv_BallsPerRow)*(*hv_BallsPerCol), -1);
		(*hv_BallsRowsRef) = HTuple();
		(*hv_BallsColsRef) = HTuple();
		//
		hv_CenterRow = ((*hv_BallsPerRow) - 1)*0.5;
		hv_CenterCol = ((*hv_BallsPerCol) - 1)*0.5;
		hv_I = 0;
		{
			HTuple end_val9 = (*hv_BallsPerRow) - 1;
			HTuple step_val9 = 1;
			for (hv_R = 0; hv_R.Continue(end_val9, step_val9); hv_R += step_val9)
			{
				{
					HTuple end_val10 = (*hv_BallsPerCol) - 1;
					HTuple step_val10 = 1;
					for (hv_C = 0; hv_C.Continue(end_val10, step_val10); hv_C += step_val10)
					{
						hv_Dist = ((((hv_R - hv_CenterRow).TupleConcat(hv_C - hv_CenterCol)).TupleFabs()).TupleInt()).TupleMax();
						if (0 != (HTuple(hv_BgaLayout[hv_Dist])))
						{
							(*hv_BallMatrixRef)[(hv_R*(*hv_BallsPerCol)) + hv_C] = hv_I;
							(*hv_BallsRowsRef) = (*hv_BallsRowsRef).TupleConcat(hv_R*hv_BallDistRowRef);
							(*hv_BallsColsRef) = (*hv_BallsColsRef).TupleConcat(hv_C*hv_BallDistColRef);
							hv_I += 1;
						}
					}
				}
			}
		}
		return;
	}

	// Procedures 
	// Chapter: BGA-Inspection
	// Short Description: Displays the BGA model. 
	void display_bga_matrix(HTuple hv_BallsRows, HTuple hv_BallsCols, HTuple hv_BallDistanceRow,
		HTuple hv_BallDistanceCol, HTuple hv_RectangleSize, HTuple hv_WindowHandle)
	{

		// Local iconic variables
		HObject  ho_Matrix;

		// Local control variables
		HTuple  hv_Scale;

		if (HDevWindowStack::IsOpen())
			ClearWindow(HDevWindowStack::GetActive());
		if (HDevWindowStack::IsOpen())
			SetColor(HDevWindowStack::GetActive(), "green");
		hv_Scale = hv_RectangleSize / (0.8*((hv_BallDistanceRow.TupleConcat(hv_BallDistanceCol)).TupleMin()));
		GenRectangle2ContourXld(&ho_Matrix, (hv_RectangleSize / 2.) + (hv_BallsRows*hv_Scale),
			(hv_RectangleSize / 2.) + (hv_BallsCols*hv_Scale), HTuple(hv_BallsRows.TupleLength(), 0),
			HTuple(hv_BallsRows.TupleLength(), hv_RectangleSize / 2.), HTuple(hv_BallsRows.TupleLength(), hv_RectangleSize / 2.));
		if (HDevWindowStack::IsOpen())
			DispObj(ho_Matrix, HDevWindowStack::GetActive());
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Segment the balls of a BGA and create the BGA matrix. 
	void gen_bga_matrix_from_image(HObject ho_Image, HObject ho_BallsSubPix, HTuple hv_BallsPerRow,
		HTuple hv_BallsPerCol, HTuple hv_WindowHandle, HTuple hv_VisualizeIntermediate,
		HTuple *hv_BallsRows, HTuple *hv_BallsCols, HTuple *hv_BallDistCol, HTuple *hv_BallDistRow,
		HTuple *hv_NumBalls, HTuple *hv_BallMatrix)
	{

		// Local iconic variables
		HObject  ho_RegionBallCenters, ho_BGARect;

		// Local control variables
		HTuple  hv_Area, hv_PointOrder, hv_RowBGARect;
		HTuple  hv_ColumnBGARect, hv_PhiBGARect, hv_Length1BGARect;
		HTuple  hv_Length2BGARect, hv_Temp, hv_HomMat2DIdentity;
		HTuple  hv_HomMat2DRotate, hv_HomMat2DTranslate, hv_HomMat2DScale;
		HTuple  hv_RowNormalized, hv_ColNormalized, hv_BallRowIndex;
		HTuple  hv_BallColIndex, hv_I;

		//
		//Calculate the position of the balls.
		AreaCenterXld(ho_BallsSubPix, &hv_Area, &(*hv_BallsRows), &(*hv_BallsCols), &hv_PointOrder);
		//
		//Sort the ball centers line by line and determine the orientation of the BGA.
		GenRegionPoints(&ho_RegionBallCenters, (*hv_BallsRows), (*hv_BallsCols));
		SmallestRectangle2(ho_RegionBallCenters, &hv_RowBGARect, &hv_ColumnBGARect, &hv_PhiBGARect,
			&hv_Length1BGARect, &hv_Length2BGARect);
		if (0 != (hv_PhiBGARect < (-(HTuple(45).TupleRad()))))
		{
			hv_PhiBGARect += HTuple(90).TupleRad();
			hv_Temp = hv_Length1BGARect;
			hv_Length1BGARect = hv_Length2BGARect;
			hv_Length2BGARect = hv_Temp;
		}
		if (0 != (hv_PhiBGARect > (HTuple(45).TupleRad())))
		{
			hv_PhiBGARect = hv_PhiBGARect - (HTuple(90).TupleRad());
			hv_Temp = hv_Length1BGARect;
			hv_Length1BGARect = hv_Length2BGARect;
			hv_Length2BGARect = hv_Temp;
		}
		if (0 != hv_VisualizeIntermediate)
		{
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "red");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_RegionBallCenters, HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "green");
			if (HDevWindowStack::IsOpen())
				SetDraw(HDevWindowStack::GetActive(), "margin");
			GenRectangle2(&ho_BGARect, hv_RowBGARect, hv_ColumnBGARect, hv_PhiBGARect, hv_Length1BGARect,
				hv_Length2BGARect);
			if (HDevWindowStack::IsOpen())
				DispObj(ho_BGARect, HDevWindowStack::GetActive());
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		//
		//Determine the transformation parameters for the transformation of the ball coordinates
		//into indices of the BGA matrix.
		HomMat2dIdentity(&hv_HomMat2DIdentity);
		HomMat2dRotate(hv_HomMat2DIdentity, -hv_PhiBGARect, hv_RowBGARect, hv_ColumnBGARect,
			&hv_HomMat2DRotate);
		HomMat2dTranslate(hv_HomMat2DRotate, (-hv_RowBGARect) + hv_Length2BGARect, (-hv_ColumnBGARect) + hv_Length1BGARect,
			&hv_HomMat2DTranslate);
		(*hv_BallDistCol) = (2 * hv_Length1BGARect) / (hv_BallsPerCol - 1);
		(*hv_BallDistRow) = (2 * hv_Length2BGARect) / (hv_BallsPerRow - 1);
		HomMat2dScale(hv_HomMat2DTranslate, 1 / (*hv_BallDistRow), 1 / (*hv_BallDistCol), 0,
			0, &hv_HomMat2DScale);
		//
		//Transform the ball coordinates into indices of the BGA matrix.
		AffineTransPoint2d(hv_HomMat2DScale, (*hv_BallsRows), (*hv_BallsCols), &hv_RowNormalized,
			&hv_ColNormalized);
		hv_BallRowIndex = hv_RowNormalized.TupleRound();
		hv_BallColIndex = hv_ColNormalized.TupleRound();
		(*hv_NumBalls) = (*hv_BallsRows).TupleLength();
		if (0 != hv_VisualizeIntermediate)
		{
			display_bga_matrix(hv_BallRowIndex, hv_BallColIndex, 1, 1, 20, hv_WindowHandle);
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		//
		//Create the BGA matrix.
		(*hv_BallMatrix) = HTuple(hv_BallsPerRow*hv_BallsPerCol, -1);
		{
			HTuple end_val53 = (*hv_NumBalls) - 1;
			HTuple step_val53 = 1;
			for (hv_I = 0; hv_I.Continue(end_val53, step_val53); hv_I += step_val53)
			{
				(*hv_BallMatrix)[(HTuple(hv_BallRowIndex[hv_I])*hv_BallsPerCol) + HTuple(hv_BallColIndex[hv_I])] = hv_I;
			}
		}
		return;
	}

	// Chapter: BGA-Inspection
	// Short Description: Determines the transformation parameters from the BGA image to the BGA model and transform the image coordinates of the balls. 
	void transform_ball_coords_image_to_world(HObject ho_BallsSubPix, HTuple hv_BallsPerRow,
		HTuple hv_BallsPerCol, HTuple hv_BallMatrixRef, HTuple hv_BallsRowsRef, HTuple hv_BallsColsRef,
		HTuple hv_BallMatrix, HTuple hv_BallsRows, HTuple hv_BallsCols, HTuple hv_WindowHandle,
		HTuple hv_VisualizeIntermediate, HTuple *hv_BallsMatchedRowsWorld, HTuple *hv_BallsMatchedColsWorld,
		HTuple *hv_BallsMatchedRowsRef, HTuple *hv_BallsMatchedColsRef, HTuple *hv_IndexImage,
		HTuple *hv_HomMat2DInvert)
	{

		// Local iconic variables

		// Local control variables
		HTuple  hv_BallsMatchedRowsImage, hv_BallsMatchedColsImage;
		HTuple  hv_IndexRef, hv_K, hv_I, hv_HomMat2D;

		//Construct the coordinates of the points for the determination of the transformation.
		hv_BallsMatchedRowsImage = HTuple();
		hv_BallsMatchedColsImage = HTuple();
		(*hv_IndexImage) = HTuple();
		(*hv_BallsMatchedRowsRef) = HTuple();
		(*hv_BallsMatchedColsRef) = HTuple();
		hv_IndexRef = HTuple();
		hv_K = 0;
		{
			HTuple end_val8 = (hv_BallsPerRow*hv_BallsPerCol) - 1;
			HTuple step_val8 = 1;
			for (hv_I = 0; hv_I.Continue(end_val8, step_val8); hv_I += step_val8)
			{
				if (0 != (HTuple(HTuple(hv_BallMatrixRef[hv_I]) >= 0).TupleAnd(HTuple(hv_BallMatrix[hv_I]) >= 0)))
				{
					hv_BallsMatchedRowsImage = hv_BallsMatchedRowsImage.TupleConcat(HTuple(hv_BallsRows[HTuple(hv_BallMatrix[hv_I])]));
					hv_BallsMatchedColsImage = hv_BallsMatchedColsImage.TupleConcat(HTuple(hv_BallsCols[HTuple(hv_BallMatrix[hv_I])]));
					(*hv_IndexImage) = (*hv_IndexImage).TupleConcat(HTuple(hv_BallMatrix[hv_I]));
					(*hv_BallsMatchedRowsRef) = (*hv_BallsMatchedRowsRef).TupleConcat(HTuple(hv_BallsRowsRef[HTuple(hv_BallMatrixRef[hv_I])]));
					(*hv_BallsMatchedColsRef) = (*hv_BallsMatchedColsRef).TupleConcat(HTuple(hv_BallsColsRef[HTuple(hv_BallMatrixRef[hv_I])]));
					hv_IndexRef = hv_IndexRef.TupleConcat(HTuple(hv_BallMatrixRef[hv_I]));
					hv_K += 1;
				}
			}
		}
		VectorToSimilarity(hv_BallsMatchedRowsImage, hv_BallsMatchedColsImage, (*hv_BallsMatchedRowsRef),
			(*hv_BallsMatchedColsRef), &hv_HomMat2D);
		HomMat2dInvert(hv_HomMat2D, &(*hv_HomMat2DInvert));
		AffineTransPoint2d(hv_HomMat2D, hv_BallsMatchedRowsImage, hv_BallsMatchedColsImage,
			&(*hv_BallsMatchedRowsWorld), &(*hv_BallsMatchedColsWorld));
		if (0 != hv_VisualizeIntermediate)
		{
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "cyan");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_BallsSubPix, HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "blue");
			if (HDevWindowStack::IsOpen())
				SetDraw(HDevWindowStack::GetActive(), "fill");
			DispArrow(hv_WindowHandle, hv_BallsMatchedRowsImage.TupleReal(), hv_BallsMatchedColsImage.TupleReal(),
				(20 + ((*hv_BallsMatchedRowsRef) * 500)).TupleReal(), (20 + ((*hv_BallsMatchedColsRef) * 500)).TupleReal(),
				1);
			if (HDevWindowStack::IsOpen())
				SetDraw(HDevWindowStack::GetActive(), "margin");
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		return;
	}


	// Main procedure 
	void action_BGA_inspection()
	{

		// Local iconic variables
		HObject  ho_Image, ho_Balls, ho_WrongAreaBalls;
		HObject  ho_WrongAnisometryBalls, ho_BallsSubPix, ho_WrongAreaBallsSubPix;
		HObject  ho_WrongAnisometryBallsSubPix, ho_BallWrongPos;
		HObject  ho_BallMissing, ho_BallExtra;

		// Local control variables
		HTuple  hv_WindowHandle, hv_VisualizeIntermediate;
		HTuple  hv_BrighterThan, hv_MinArea, hv_MaxArea, hv_MaxAnisometry;
		HTuple  hv_MaxDistance, hv_BgaLayout, hv_BallDistRowRef;
		HTuple  hv_BallDistColRef, hv_J, hv_BallMatrixRef, hv_BallsRowsRef;
		HTuple  hv_BallsColsRef, hv_BallsPerRow, hv_BallsPerCol;
		HTuple  hv_BallsRows, hv_BallsCols, hv_BallDistCol, hv_BallDistRow;
		HTuple  hv_NumBalls, hv_BallMatrix, hv_BallsMatchedRowsWorld;
		HTuple  hv_BallsMatchedColsWorld, hv_BallsMatchedRowsRef;
		HTuple  hv_BallsMatchedColsRef, hv_IndexImage, hv_HomMat2DInvert;
		HTuple  hv_Distances, hv_I, hv_BallRad, hv_RowImage, hv_ColImage;

		//
		//This program demonstrates the inspection of ball grid arrays (BGA).
		//First, the size and shape of the balls is checked. Then, the BGA is tested for missing
		//or extraneous balls as well as for wrongly positioned balls.
		//
		//Initialize the program.
		ReadImage(&ho_Image, "bga/bga1_01");
		reopen_window_fit(ho_Image, 640, 0, 0, &hv_WindowHandle);
		init_font(hv_WindowHandle, -1, 2);
		dev_update_all("off");
		hv_VisualizeIntermediate = 0;
		//
		//Set parameters for the segmentation and the check of the balls.
		hv_BrighterThan = 70;
		hv_MinArea = 180;
		hv_MaxArea = 230;
		hv_MaxAnisometry = 1.2;
		hv_MaxDistance = 0.003*25.4;
		//
		//Set parameters for the creation of the BGA model.
		hv_BgaLayout.Clear();
		hv_BgaLayout[0] = 1;
		hv_BgaLayout[1] = 1;
		hv_BgaLayout[2] = 1;
		hv_BgaLayout[3] = 0;
		hv_BgaLayout[4] = 0;
		hv_BgaLayout[5] = 0;
		hv_BgaLayout[6] = 0;
		hv_BgaLayout[7] = 0;
		hv_BgaLayout[8] = 0;
		hv_BgaLayout[9] = 1;
		hv_BgaLayout[10] = 1;
		hv_BgaLayout[11] = 1;
		hv_BgaLayout[12] = 1;
		hv_BallDistRowRef = 0.05*25.4;
		hv_BallDistColRef = 0.05*25.4;
		//
		//Segment the balls and check their area and shape (both pixel and sub-pixel precise)
		for (hv_J = 1; hv_J <= 6; hv_J += 1)
		{
			ReadImage(&ho_Image, "bga/bga1_" + (hv_J.TupleString("02d")));
			segment_balls(ho_Image, &ho_Balls, hv_BrighterThan, hv_MinArea, hv_MaxArea);
			check_balls(ho_Balls, &ho_WrongAreaBalls, &ho_WrongAnisometryBalls, hv_MinArea,
				hv_MaxArea, hv_MaxAnisometry);
			display_checked_balls(ho_Image, ho_Balls, ho_WrongAreaBalls, ho_WrongAnisometryBalls
			);
			//
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
			segment_balls_sub_pix(ho_Image, &ho_BallsSubPix, hv_BrighterThan, hv_MinArea,
				hv_MaxArea);
			check_balls_sub_pix(ho_BallsSubPix, &ho_WrongAreaBallsSubPix, &ho_WrongAnisometryBallsSubPix,
				hv_MinArea, hv_MaxArea, hv_MaxAnisometry);
			display_checked_balls(ho_Image, ho_BallsSubPix, ho_WrongAreaBallsSubPix, ho_WrongAnisometryBallsSubPix
			);
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		//
		//Compare the actual ball positions with the reference positions of the balls.
		//
		//First, create and display a model of the BGA that provides the reference positions for the balls.
		gen_bga_model(hv_BgaLayout, hv_BallDistRowRef, hv_BallDistColRef, &hv_BallMatrixRef,
			&hv_BallsRowsRef, &hv_BallsColsRef, &hv_BallsPerRow, &hv_BallsPerCol);
		display_bga_matrix(hv_BallsRowsRef, hv_BallsColsRef, hv_BallDistRowRef, hv_BallDistColRef,
			20, hv_WindowHandle);
		write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
		// stop(); only in hdevelop
		//
		//Then, extract the positions of the balls and compare them with the reference positions.
		for (hv_J = 1; hv_J <= 6; hv_J += 1)
		{
			ReadImage(&ho_Image, "bga/bga1_" + (hv_J.TupleString("02d")));
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			//
			//Segment the balls and check their area and shape.
			segment_balls_sub_pix(ho_Image, &ho_BallsSubPix, hv_BrighterThan, hv_MinArea,
				hv_MaxArea);
			check_balls_sub_pix(ho_BallsSubPix, &ho_WrongAreaBallsSubPix, &ho_WrongAnisometryBallsSubPix,
				hv_MinArea, hv_MaxArea, hv_MaxAnisometry);
			//
			//Create the BGA matrix for the currently inspected BGA.
			gen_bga_matrix_from_image(ho_Image, ho_BallsSubPix, hv_BallsPerRow, hv_BallsPerCol,
				hv_WindowHandle, hv_VisualizeIntermediate, &hv_BallsRows, &hv_BallsCols,
				&hv_BallDistCol, &hv_BallDistRow, &hv_NumBalls, &hv_BallMatrix);
			//
			//Transform the image coordinates of the balls into the reference coordinate system of the BGA model.
			transform_ball_coords_image_to_world(ho_BallsSubPix, hv_BallsPerRow, hv_BallsPerCol,
				hv_BallMatrixRef, hv_BallsRowsRef, hv_BallsColsRef, hv_BallMatrix, hv_BallsRows,
				hv_BallsCols, hv_WindowHandle, hv_VisualizeIntermediate, &hv_BallsMatchedRowsWorld,
				&hv_BallsMatchedColsWorld, &hv_BallsMatchedRowsRef, &hv_BallsMatchedColsRef,
				&hv_IndexImage, &hv_HomMat2DInvert);
			//
			//Determine the distance between the extracted balls and the reference balls.
			DistancePp(hv_BallsMatchedRowsRef, hv_BallsMatchedColsRef, hv_BallsMatchedRowsWorld,
				hv_BallsMatchedColsWorld, &hv_Distances);
			//
			//Visualize the results of the check for wrongly placed, missing, and extraneous balls as well as for wrongly shaped balls.
			if (HDevWindowStack::IsOpen())
				DispObj(ho_Image, HDevWindowStack::GetActive());
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "green");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_BallsSubPix, HDevWindowStack::GetActive());
			//
			//Wrongly placed ball.
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "cyan");
			{
				HTuple end_val73 = (hv_Distances.TupleLength()) - 1;
				HTuple step_val73 = 1;
				for (hv_I = 0; hv_I.Continue(end_val73, step_val73); hv_I += step_val73)
				{
					if (0 != (HTuple(hv_Distances[hv_I]) > hv_MaxDistance))
					{
						SelectObj(ho_BallsSubPix, &ho_BallWrongPos, HTuple(hv_IndexImage[hv_I]) + 1);
						if (HDevWindowStack::IsOpen())
							DispObj(ho_BallWrongPos, HDevWindowStack::GetActive());
						if (HDevWindowStack::IsOpen())
							DispObj(ho_BallWrongPos, HDevWindowStack::GetActive());
					}
				}
			}
			hv_BallRad = (((hv_MinArea + hv_MaxArea) / 2.0) / 3.1415926).TupleSqrt();
			{
				HTuple end_val81 = (hv_BallsPerRow*hv_BallsPerCol) - 1;
				HTuple step_val81 = 1;
				for (hv_I = 0; hv_I.Continue(end_val81, step_val81); hv_I += step_val81)
				{
					if (0 != (HTuple(HTuple(hv_BallMatrixRef[hv_I]) >= 0).TupleAnd(HTuple(hv_BallMatrix[hv_I]) < 0)))
					{
						//
						//Missing ball.
						if (HDevWindowStack::IsOpen())
							SetColor(HDevWindowStack::GetActive(), "yellow");
						AffineTransPoint2d(hv_HomMat2DInvert, HTuple(hv_BallsRowsRef[HTuple(hv_BallMatrixRef[hv_I])]),
							HTuple(hv_BallsColsRef[HTuple(hv_BallMatrixRef[hv_I])]), &hv_RowImage,
							&hv_ColImage);
						GenEllipseContourXld(&ho_BallMissing, hv_RowImage, hv_ColImage, 0, hv_BallRad,
							hv_BallRad, 0, HTuple(360).TupleRad(), "positive", 1.5);
						if (HDevWindowStack::IsOpen())
							DispObj(ho_BallMissing, HDevWindowStack::GetActive());
					}
					if (0 != (HTuple(HTuple(hv_BallMatrixRef[hv_I]) < 0).TupleAnd(HTuple(hv_BallMatrix[hv_I]) >= 0)))
					{
						//
						//Extraneous ball.
						if (HDevWindowStack::IsOpen())
							SetColor(HDevWindowStack::GetActive(), "red");
						SelectObj(ho_BallsSubPix, &ho_BallExtra, HTuple(hv_BallMatrix[hv_I]) + 1);
						if (HDevWindowStack::IsOpen())
							DispObj(ho_BallExtra, HDevWindowStack::GetActive());
					}
				}
			}
			//
			//Ball with a wrong size.
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "blue");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_WrongAreaBallsSubPix, HDevWindowStack::GetActive());
			//
			//Ball with a wrong shape.
			if (HDevWindowStack::IsOpen())
				SetColor(HDevWindowStack::GetActive(), "magenta");
			if (HDevWindowStack::IsOpen())
				DispObj(ho_WrongAnisometryBallsSubPix, HDevWindowStack::GetActive());
			write_message(hv_WindowHandle, 950, 10, "Press 'Run' to continue", 0);
			// stop(); only in hdevelop
		}
		write_message(hv_WindowHandle, 950, 10, "  Program terminated   ", 0);
	}

}