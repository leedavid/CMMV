#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace std;
using namespace HalconCpp;

void ST_MYDISP::p_disp_dimensions(HalconCpp::HTuple hv_RowEdgeFirst, 
	HalconCpp::HTuple hv_ColumnEdgeFirst,
	HalconCpp::HTuple hv_RowEdgeSecond, 
	HalconCpp::HTuple hv_ColumnEdgeSecond, 
	HalconCpp::HTuple hv_IntraDistance, 
	HalconCpp::HTuple hv_InterDistance, 
	HalconCpp::HTuple hv_Phi, 
	HalconCpp::HTuple hv_Length, double scalemm,
	HalconCpp::HXLDCont& Cont)
{
	// Local iconic variables

	// Local control variables
	HTuple  hv_i, hv_Text;
	int hv_Number;

	//hv_Phi = HTuple(90).TupleRad();

	//Display markers for the edges.
	p_disp_edge_marker(hv_RowEdgeFirst, hv_ColumnEdgeFirst, hv_Phi, hv_Length, "red",
		2, Cont);
	p_disp_edge_marker(hv_RowEdgeSecond, hv_ColumnEdgeSecond, hv_Phi, hv_Length, "red",
		2, Cont);

	//Cont = Cont.ConcatObj(TmpCont);  Õâ¶ù²»ÏÔÊ¾³ß´ç
	return;


	//HXLDCont TmpCont;
	//Loop over all edge pairs.
	hv_Number = (int)hv_RowEdgeFirst.Length();
	{
		HTuple end_val5 = hv_Number - 1;
		HTuple step_val5 = 1;
		for (hv_i = 0; hv_i.Continue(end_val5, step_val5); hv_i += step_val5)
		{
			//
			//Display the IntraDistance between the edges.
			hv_Text = ((const HTuple&)hv_IntraDistance)[hv_i];
			p_disp_text_right_of_center(hv_Text.TupleString(".2f"), HTuple(hv_RowEdgeFirst[hv_i]),
				HTuple(hv_ColumnEdgeFirst[hv_i]), HTuple(hv_RowEdgeSecond[hv_i]), HTuple(hv_ColumnEdgeSecond[hv_i]),
				hv_Phi, 2.0*hv_Length, scalemm,Cont);
		}
	}
	//
	//Loop to display the distance between the edge pairs.
	{
		HTuple end_val13 = hv_Number - 2;
		HTuple step_val13 = 1;
		for (hv_i = 0; hv_i.Continue(end_val13, step_val13); hv_i += step_val13)
		{
			//
			//Display the InterDistance between the edge pairs.
			hv_Text = ((const HTuple&)hv_InterDistance)[hv_i];
			p_disp_text_left_of_center(hv_Text.TupleString(".2f"), HTuple(hv_RowEdgeSecond[hv_i]),
				HTuple(hv_ColumnEdgeSecond[hv_i]), HTuple(hv_RowEdgeFirst[hv_i + 1]), HTuple(hv_ColumnEdgeFirst[hv_i + 1]),
				hv_Phi, 2.0*hv_Length, scalemm, Cont);
		}
	}
	return;
}

void ST_MYDISP::p_disp_edge_marker(HalconCpp::HTuple hv_Rows, 
	HalconCpp::HTuple hv_Cols,
	HalconCpp::HTuple hv_Phi, 
	HalconCpp::HTuple hv_Length,
	HalconCpp::HTuple hv_Color, 
	HalconCpp::HTuple hv_LineWidth, HalconCpp::HXLDCont& Cont)
{
	// Local iconic variables
	HObject  ho_Marker;

	// Local control variables
	HTuple  hv_NumRows, hv_NumCols, hv_Num, hv_i;
	HTuple  hv_Row, hv_Col, hv_RowStart, hv_RowEnd, hv_ColStart;
	HTuple  hv_ColEnd;

	//Determine the number of edges
	hv_NumRows = hv_Rows.TupleLength();
	hv_NumCols = hv_Cols.TupleLength();
	hv_Num = (hv_NumRows.TupleConcat(hv_NumCols)).TupleMin();


	//
	//Loop over the edges
	{
		HTuple end_val6 = hv_Num - 1;
		HTuple step_val6 = 1;
		for (hv_i = 0; hv_i.Continue(end_val6, step_val6); hv_i += step_val6)
		{
			hv_Row = ((const HTuple&)hv_Rows)[hv_i];
			hv_Col = ((const HTuple&)hv_Cols)[hv_i];
			//
			//Determine start and end point of the edge marker.
			hv_RowStart = hv_Row + (hv_Length*(hv_Phi.TupleCos()));
			hv_RowEnd = hv_Row - (hv_Length*(hv_Phi.TupleCos()));
			hv_ColStart = hv_Col + (hv_Length*(hv_Phi.TupleSin()));
			hv_ColEnd = hv_Col - (hv_Length*(hv_Phi.TupleSin()));
			//
			//Generate a contour that connects the start and end point.
			GenContourPolygonXld(&ho_Marker, hv_RowStart.TupleConcat(hv_RowEnd), hv_ColStart.TupleConcat(hv_ColEnd));
			//
			//Display the contour with  the specified style.
			//this->m_pMHWinMain->SetColor(hv_Color);
			//this->m_pMHWinMain->SetLineWidth(hv_LineWidth);
			//this->m_pMHWinMain->DispObj(ho_Marker);
			Cont = Cont.ConcatObj(ho_Marker);
		}
	}
	return;
}

void ST_MYDISP::p_disp_text_right_of_center(HalconCpp::HTuple hv_Text, 
	HalconCpp::HTuple hv_RowFirst, 
	HalconCpp::HTuple hv_ColFirst, 
	HalconCpp::HTuple hv_RowSecond,
	HalconCpp::HTuple hv_ColSecond, 
	HalconCpp::HTuple hv_Phi, 
	HalconCpp::HTuple hv_Distance, double scalemm, HalconCpp::HXLDCont& Cont)
{
	// Local iconic variables

	// Local control variables
	Hlong hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part;
	Hlong hv_RowWin, hv_ColumnWin, hv_WidthWin, hv_HeightWin;
	Hlong  hv_FactorRow, hv_FactorColumn;
	Hlong  hv_Descent, hv_Width, hv_Height;
	HTuple  hv_RowCenter, hv_ColCenter;
	HTuple  hv_RowPos, hv_ColPos, hv_RowText, hv_ColText;

	//Determine factors for the adaptation of the string extents that might be necessary
	//because of image zooming.
	this->m_pMHWinMain->GetPart(&hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
	this->m_pMHWinMain->GetWindowExtents(&hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);

	hv_FactorRow = int(1.*((hv_Row2Part - hv_Row1Part) + 1)) / hv_HeightWin;
	hv_FactorColumn = int(1.*((hv_Column2Part - hv_Column1Part) + 1)) / hv_WidthWin;
	//
	
	//Determine the extent of the string and its position.
	this->m_pMHWinMain->GetStringExtents(hv_Text, &hv_Descent, &hv_Width, &hv_Height);
	hv_Width = hv_Width*hv_FactorColumn;
	hv_Height = hv_Height*hv_FactorRow;

	hv_RowCenter = (hv_RowFirst + hv_RowSecond) / 2.0;
	hv_ColCenter = (hv_ColFirst + hv_ColSecond) / 2.0;
	if (0 != ((hv_Phi.TupleSin())<0))
	{
		hv_Distance = -hv_Distance;
	}
	hv_RowPos = hv_RowCenter + (hv_Distance*(hv_Phi.TupleCos()));
	hv_ColPos = hv_ColCenter + (hv_Distance*(hv_Phi.TupleSin()));
	hv_RowText = hv_RowPos - (hv_Height / 2.0);
	hv_ColText = hv_ColPos;
	//
	//Set the text position and color and display the text.
	this->m_pMHWinMain->SetTposition((int)hv_RowText[0].D(), (int)hv_ColText[0].D());
	this->m_pMHWinMain->SetColor("black");
	this->m_pMHWinMain->WriteString(hv_Text);
	return;
}

void ST_MYDISP::p_disp_text_left_of_center(HalconCpp::HTuple hv_Text, 
	HalconCpp::HTuple hv_RowFirst, 
	HalconCpp::HTuple hv_ColFirst, 
	HalconCpp::HTuple hv_RowSecond,
	HalconCpp::HTuple hv_ColSecond,
	HalconCpp::HTuple hv_Phi, 
	HalconCpp::HTuple hv_Distance, double scalemm, HalconCpp::HXLDCont& Cont)
{
	// Local control variables
	Hlong hv_Row1Part, hv_Column1Part, hv_Row2Part, hv_Column2Part;
	Hlong hv_RowWin, hv_ColumnWin, hv_WidthWin, hv_HeightWin;
	Hlong   hv_FactorRow, hv_FactorColumn;
	Hlong  hv_Descent, hv_Width, hv_Height;
	HTuple hv_RowCenter, hv_ColCenter;
	HTuple  hv_RowPos, hv_ColPos, hv_RowText, hv_ColText;

	//Determine factors for the adaptation of the string extents that might be necessary
	//because of image zooming.
	this->m_pMHWinMain->GetPart(&hv_Row1Part, &hv_Column1Part, &hv_Row2Part, &hv_Column2Part);
	this->m_pMHWinMain->GetWindowExtents(&hv_RowWin, &hv_ColumnWin, &hv_WidthWin, &hv_HeightWin);

	hv_FactorRow = int(1.*((hv_Row2Part - hv_Row1Part) + 1)) / hv_HeightWin;
	hv_FactorColumn = int(1.*((hv_Column2Part - hv_Column1Part) + 1)) / hv_WidthWin;
	//
	//Determine the extent of the string and its position.
	this->m_pMHWinMain->GetStringExtents(hv_Text, &hv_Descent, &hv_Width, &hv_Height);
	hv_Width = hv_Width*hv_FactorColumn;
	hv_Height = hv_Height*hv_FactorRow;
	hv_RowCenter = (hv_RowFirst + hv_RowSecond) / 2.0;
	hv_ColCenter = (hv_ColFirst + hv_ColSecond) / 2.0;
	if (0 != ((hv_Phi.TupleSin())<0))
	{
		hv_Distance = -hv_Distance;
	}
	hv_RowPos = hv_RowCenter - (hv_Distance*(hv_Phi.TupleCos()));
	hv_ColPos = hv_ColCenter - (hv_Distance*(hv_Phi.TupleSin()));
	hv_RowText = hv_RowPos - (hv_Height / 2.0);
	hv_ColText = hv_ColPos - hv_Width;
	//
	//Set the text position and color and display the text.
	this->m_pMHWinMain->SetTposition((int)hv_RowText[0].D(), (int)hv_ColText[0].D());
	this->m_pMHWinMain->SetColor("black");
	this->m_pMHWinMain->WriteString(hv_Text);
	return;
}
