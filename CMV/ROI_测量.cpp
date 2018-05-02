#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;
using namespace std;
using namespace Gdiplus;

// 测量元素
BOOL ST_MYDISP::MeasureOneElement(UCI::OptionsMap * o, HImage &himg, HXLDCont& Cont)
{
	int itype = (*o)[L"元素类型"];
	int etype = eELEMENT_TYPE(itype);

	switch (etype) {
	case ELEMENT_CAMERA:
		return this->MeasureOneCamera(o, himg, Cont);
		break;
	case ELEMENT_LINE:
		return this->MeasureOneLine(o, himg, Cont);
		break;
	case ELEMENT_CIRCLE:
		return this->MeasureOneCircle(o, himg, Cont);
		break;
	case ELEMENT_RECT:
		break;
	case ELEMENT_ELLIPSE:
		break;
	case ELEMENT_DISTANCE:
		return this->MeasureOneDistance(o, himg, Cont);
		break;
	case ELEMENT_ANGLE:
		return this->MeasureOneAngle(o, himg, Cont);
		break;
	case ELEMENT_RESULT:      // 采集结果，并输出
		return MeasureOneResult(o, Cont);
		break;
	case ELEMENT_MM:          // 
		return MeasureOneMM(o, Cont);
		break;
	case ELEMENT_PEAK:
		return MeasureOnePeak(o, himg, Cont);
		break;
	case ELEMENT_BALLOONLENGTH:
		return MeasureOneBalloonLenght(o, Cont);
		break;
	case ELEMENT_FUNCTION:
		return MeasureOneFUNCTION(o, himg, Cont);
		break;
	case ELEMENT_MATCH:
		return MeasureOneMATCH(o, himg, Cont);
		break;
	case ELEMENT_MEASURE_RECTANGLE2:
		return MeasureOneELEMENT_MEASURE_RECTANGLE2(o, himg, Cont);
		break;
	case ELEMENT_BALLOON_FLAW:
		return MeasureOneELEMENT_MEASURE_BalloonFlaw(o, himg, Cont);
		break;
	case ELEMENT_ROI:
		return MeasureOneROI(o, himg, Cont);
		break;
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

// 测量一条直线
BOOL ST_MYDISP::MeasureOneLine(UCI::OptionsMap* o, HImage &himg, HXLDCont& Cont)
{
	
	try {
		BOOL isHomMat2DOK = UCI::OPTIONS[L"转换矩阵成功"];
		if (isHomMat2DOK == FALSE) return FALSE;

		// 取出 ROI
		double Row1 = (*o)[L"ROI_Row1"];
		double Column1 = (*o)[L"ROI_Column1"];
		double Row2 = (*o)[L"ROI_Row2"];
		double Column2 = (*o)[L"ROI_Column2"];

		HTuple Rows, Cols;
		Rows.Append(Row1);
		Rows.Append(Row2);
		Cols.Append(Column1);
		Cols.Append(Column2);

		// 对测量直线ROI进行变换
		HTuple NewLine1Row, NewLine1Col;
		AffineTransPoint2d(this->HomMat2D,
			Rows, Cols,
			&NewLine1Row, &NewLine1Col);

		// 拟合直线 1
		// 找边缘
		int hv_Elements = (*o)[L"Rake直线检测点数"];
		double hv_DetectHeight = (*o)[L"Rake直线检测高度"];
		double hv_DetectWidth = (*o)[L"Rake直线检测宽度"];
		double hv_Sigma = (*o)[L"Rake直线检测Sigma"];
		double hv_Threshold = (*o)[L"Rake直线检测阈值"];
		CString hv_Transition = (*o)[L"Rake_Line_Detect_Transition"];
		CString hv_Select = (*o)[L"Rake_Line_Detect_Select"];

		HXLDCont RegionsLine;
		HTuple ResultRow1, ResultColumn1;
		rake(
			himg,
			&RegionsLine,                                   // 输出 rake 查找区域
			hv_Elements,                                    // elements
			hv_DetectHeight,
			hv_DetectWidth,
			hv_Sigma,
			hv_Threshold,
			CT2A(hv_Transition).m_psz,
			CT2A(hv_Select).m_psz,
			NewLine1Row[0],
			NewLine1Col[0],
			NewLine1Row[1],
			NewLine1Col[1],
			&ResultRow1,
			&ResultColumn1);

		//this->m_pMHWinMain->DispObj(RegionsLine);

		int hv_ActiveNum = (*o)[L"直线拟合最小点数"];

		// 拟合
		HXLDCont Line1;
		HTuple 	Line1Row11, Line1Column11, Line1Row21, Line1Column21;
		pts_to_best_line(
			&Line1,
			ResultRow1,
			ResultColumn1,
			hv_ActiveNum,
			&Line1Row11,
			&Line1Column11,
			&Line1Row21,
			&Line1Column21);

		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];    // 测量象素到mm的比例

		double L1row = Line1Row11[0].D() * scalemm;
		double L2row = Line1Row21[0].D() * scalemm;

		double L1col = Line1Column11[0].D() * scalemm;
		double L2col = Line1Column21[0].D() * scalemm;

															 // 保存这个测量结果到元素中去
		(*o)[L"_RES_Line_Row1"] = L1row;
		(*o)[L"_RES_Line_Row2"] = L2row;

		(*o)[L"_RES_Line_Col1"] = L1col;
		(*o)[L"_RES_Line_Col2"] = L2col;

		Cont = Cont.ConcatObj(Line1);
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneLine(): ");
		return FALSE;
	}

	(*o)[L"元素已匹配"] = 1;
	return TRUE;
}

// 测量一个圆
BOOL ST_MYDISP::MeasureOneCircle(UCI::OptionsMap* o, HImage &himg, HXLDCont& Cont)
{
	try {

		BOOL isHomMat2DOK = UCI::OPTIONS[L"转换矩阵成功"];
		if (isHomMat2DOK == FALSE) return FALSE;

		// 取出保存的ROI
		double ROI_RowC = (*o)[L"ROI_RowC"];
		double ROI_ColumnC = (*o)[L"ROI_ColumnC"];
		double ROI_Radius = (*o)[L"ROI_Radius"];
		double ROI_StartPhi = (*o)[L"ROI_StartPhi"];
		double ROI_EndPhi = (*o)[L"ROI_EndPhi"];
		CString ROI_PointOrder = (*o)[L"ROI_PointOrder"];

		HXLDCont ho_ContCircle;
		ho_ContCircle.GenCircleContourXld(ROI_RowC, 
			ROI_ColumnC, 
			ROI_Radius, 
			ROI_StartPhi,
			ROI_EndPhi, CT2A(ROI_PointOrder).m_psz, 3.0f);

		HTuple Rows, Cols;
		ho_ContCircle.GetContourXld(&Rows, &Cols);

		if (Rows.Length() < 4) {
			MyLog("圆ROI不正确!, Rows 中点数过少");
			return FALSE;
		}

		// 对测量直线ROI进行变换
		HTuple NewCircleRow, NewCircleCol;

		AffineTransPoint2d(this->HomMat2D,
			Rows, Cols,
			&NewCircleRow, &NewCircleCol);
		
		int hv_Elements = (*o)[L"Spoke_Circle_Elements"];
		double hv_DetectHeight = (*o)[L"Spoke_Circle_Detect_Height"];
		double hv_DetectWidth = (*o)[L"Spoke_Circle_Detect_Width"];
		double hv_Sigma = (*o)[L"Spoke_Circle_Detect_Sigma"];
		double hv_Threshold = (*o)[L"Spoke_Circle_Detect_Threshold"];
		CString hv_Transition = (*o)[L"Spoke_Circle_Detect_Transition"];
		CString hv_Select = (*o)[L"Spoke_Circle_Detect_Select"];
		CString hv_Direction = (*o)[L"Spoke_Circle_Direction"];

		HTuple hv_ResultRow, hv_ResultColumn, hv_ArcType;

		HXLDCont RegionsSpoke;
		spoke(himg,
			&RegionsSpoke,
			hv_Elements,
			hv_DetectHeight,
			hv_DetectWidth,
			hv_Sigma,
			hv_Threshold,
			CT2A(hv_Transition).m_psz,
			CT2A(hv_Select).m_psz,
			NewCircleRow,
			NewCircleCol,
			CT2A(hv_Direction).m_psz,
			&hv_ResultRow,
			&hv_ResultColumn,
			&hv_ArcType);

		// 
		if (hv_ResultRow.Length() == 0) {
			MyLog(L"没有找到圆 spoke");
			return FALSE;
		}

		int CrossSize = (*o)[L"Spoke_十字尺寸"];

		HXLDCont Cross; // 圆上组成的点
		Cross.GenCrossContourXld(hv_ResultRow, hv_ResultColumn, HTuple(CrossSize), 0.785398);
		Cont = Cont.ConcatObj(Cross);

		int Min_Points_Num = (*o)[L"Circle_Fit_Min_Points_Num"];
		// 判断边缘点数是否大于等于最小有效点数
		if (hv_ResultRow.Length() < Min_Points_Num) {
			MyLog(L"没有找到圆 spoke");
			return FALSE;
		}

		HTuple hv_Result_Center_Row, hv_Result_Center_Column, hv_Result_Radius;
		HTuple hv_Result_StartPhi, hv_Result_EndPhi, hv_PointOrders;

		HXLDCont Result_xld;

		// 拟合圆 
		pts_to_best_circle(
			&Result_xld,
			hv_ResultRow,
			hv_ResultColumn,
			Min_Points_Num,
			hv_ArcType,
			&hv_Result_Center_Row,
			&hv_Result_Center_Column,
			&hv_Result_Radius,
			&hv_Result_StartPhi,
			&hv_Result_EndPhi,
			&hv_PointOrders);

		Cont = Cont.ConcatObj(Result_xld);

		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];    // 测量象素到mm的比例

															 // 保存测量数据
		(*o)[L"RES_Circle_Center_Row"] = hv_Result_Center_Row * scalemm;
		(*o)[L"RES_Circle_Center_Column"] = hv_Result_Center_Column * scalemm;

		//double cRow = hv_Result_Center_Row * scalemm;
		//double cCol = hv_Result_Center_Column * scalemm;

		(*o)[L"RES_圆半径"] = hv_Result_Radius* scalemm;
		(*o)[L"RES_圆直径"] = hv_Result_Radius* scalemm*2.0f;
		(*o)[L"RES_Circle_StartPhi"] = hv_Result_StartPhi;                  // 开始角
		(*o)[L"RES_Circle_EndPhi"] = hv_Result_EndPhi;                      // 结束角

		(*o)[L"RES_Circle_PointOrders"] = (CString)hv_PointOrders[0].S();
		(*o)[L"RES_Circle_Type"] = (CString)hv_ArcType[0].S();
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneCircle(): ");
		return FALSE;
	}

	(*o)[L"元素已匹配"] = 1;
	return TRUE;
}

// 测量一个顶点
BOOL ST_MYDISP::MeasureOnePeak(UCI::OptionsMap * o, HImage &himg, HXLDCont &Cont)
{
	
	try {
		BOOL isHomMat2DOK = UCI::OPTIONS[L"转换矩阵成功"];
		if (isHomMat2DOK == FALSE) return FALSE;

		double hv_Row = (*o)[L"Peak_Rectangle2_Row"];
		double hv_Column = (*o)[L"Peak_Rectangle2_Colum"];
		double hv_Phi = (*o)[L"Peak_Rectangle2_Phi"];

		double hv_Length1 = (*o)[L"Peak_Rectangle2_Length1"];
		double hv_Length2 = (*o)[L"Peak_Rectangle2_Length2"];

		double hv_DetectWidth = (*o)[L"Peak_DetectWidth"];
		double hv_Sigma = (*o)[L"Peak_Sigma"];
		double hv_Threshold = (*o)[L"Peak_Threshold"];

		CString hv_Transition = (*o)[L"Peak_Transition"];
		CString hv_Select = (*o)[L"Peak_Select"];

		HTuple new_hv_Row, new_hv_Column, new_hv_Phi;
		HTuple hv_EdgesY, hv_EdgesX, hv_ResultRow, hv_ResultColumn;

		// 要变换一下啊
		AffineTransPoint2d(this->HomMat2D,
			hv_Row, hv_Column,
			&new_hv_Row, &new_hv_Column);

		new_hv_Phi = hv_Phi + this->HomMatAngle;


		this->peak(himg, new_hv_Row, new_hv_Column, new_hv_Phi,
			hv_Length1, hv_Length2, hv_DetectWidth, hv_Sigma, hv_Threshold,
			CT2A(hv_Transition).m_psz, CT2A(hv_Select).m_psz, &hv_EdgesY, &hv_EdgesX, &hv_ResultRow, &hv_ResultColumn);


		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];                // 测量象素到mm的比例
		 
		(*o)[L"RES_hv_ResultRow"] = hv_ResultRow * scalemm;              // 顶点
		(*o)[L"RES_hv_ResultColumn"] = hv_ResultColumn * scalemm;

		double cross_size = (*o)[L"Peak_cross_size"];
		double cross_angle = (*o)[L"Peak_cross_angle"];

		HObject  ho_Cross;
		GenCrossContourXld(&ho_Cross, hv_ResultRow, hv_ResultColumn, cross_size, cross_angle);

		Cont = Cont.ConcatObj(ho_Cross);

	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOnePeak(): ");
		return FALSE;
	}

	(*o)[L"元素已匹配"] = 1;
	return TRUE;	

}

// 球囊长度
BOOL ST_MYDISP::MeasureOneBalloonLenght(UCI::OptionsMap * o, HXLDCont &Cont)
{
	try {
		// 取出四个顶点数据
		CString iPeek1 = (*o)[L"引用顶点1"];
		CString iPeek2 = (*o)[L"引用顶点2"];

		CString iPeek3 = (*o)[L"引用顶点3"];
		CString iPeek4 = (*o)[L"引用顶点4"];

		if (iPeek1 == iPeek2) {
			MyLog("MeasureOneBalloonLenght, 1，2 二个顶点是同一个！");
			return FALSE;
		}
		if (iPeek3 == iPeek4) {
			MyLog("MeasureOneBalloonLenght, 3，4 二个顶点是同一个！");
			return FALSE;
		}

		UCI::OptionsMap * oPeak1 = UCI::ELE.GetOptionsMapByEleName(iPeek1);
		UCI::OptionsMap * oPeak2 = UCI::ELE.GetOptionsMapByEleName(iPeek2);

		UCI::OptionsMap * oPeak3 = UCI::ELE.GetOptionsMapByEleName(iPeek3);
		UCI::OptionsMap * oPeak4 = UCI::ELE.GetOptionsMapByEleName(iPeek4);

		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];      // 测量象素到mm的比例

		double arrSize = UCI::OPTIONS[L"ElementArrowSize"];   // 显示箭头的大小、

		CString stColor = UCI::OPTIONS[L"ElementDimColor"];
		int width = UCI::OPTIONS[L"ElementDimWidth"];

		
		HTuple hv_p1_row = (*oPeak1)[L"RES_hv_ResultRow"];
		HTuple hv_p1_col = (*oPeak1)[L"RES_hv_ResultColumn"];

		double p1row = hv_p1_row[0].D();
		double p1col = hv_p1_col[0].D();

		HTuple hv_p2_row = (*oPeak2)[L"RES_hv_ResultRow"];
		HTuple hv_p2_col = (*oPeak2)[L"RES_hv_ResultColumn"];

		double p2row = hv_p2_row[0].D();
		double p2col = hv_p2_col[0].D();

		HTuple hv_p3_row = (*oPeak3)[L"RES_hv_ResultRow"];
		HTuple hv_p3_col = (*oPeak3)[L"RES_hv_ResultColumn"];

		double p3row = hv_p3_row[0].D();
		double p3col = hv_p3_col[0].D();

		HTuple hv_p4_row = (*oPeak4)[L"RES_hv_ResultRow"];
		HTuple hv_p4_col = (*oPeak4)[L"RES_hv_ResultColumn"];

		double p4row = hv_p4_row[0].D();
		double p4col = hv_p4_col[0].D();


		// 1. 求出左边二个顶点的中点

		double mid12row = (p1row + p2row) / 2;
		double mid12col = (p1col + p2col) / 2;

		double mid34row = (p3row + p4row) / 2;
		double mid34col = (p3col + p4col) / 2;

		HTuple Distance;

		// 2. 求出二个中点的距离
		DistancePp(mid12row, mid12col, mid34row, mid34col, &Distance);

		double dDis = Distance[0].D();

		(*o)[L"RES_球囊长度"] = dDis;

		(*o)[L"Balloon距离Row1"] = mid12row / scalemm;
		(*o)[L"Balloon距离Col1"] = mid12col / scalemm;
		(*o)[L"Balloon距离Row2"] = mid34row / scalemm;
		(*o)[L"Balloon距离Col2"] = mid34col / scalemm;

		// 产生一个箭头，表示一下球囊的长度
		HXLDCont Arrow;
		double Arrow_HeadLength = (*o)[L"尺寸箭头长度"];
		double Arrow_HeadWidth = (*o)[L"尺寸箭头宽度"];
		gen_double_arrow_contour_xld(
			&Arrow,
			mid12row / scalemm,
			mid12col / scalemm,
			mid34row / scalemm,
			mid34col / scalemm,
			Arrow_HeadLength,
			Arrow_HeadWidth);

		Cont = Cont.ConcatObj(Arrow);    // 保存箭头

	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneBalloonLenght：");
		return FALSE;
	}

	(*o)[L"元素已匹配"] = 1;
	return TRUE;
}

// 测量一个角度
BOOL ST_MYDISP::MeasureOneAngle(UCI::OptionsMap *o, HImage & hImg, HXLDCont& Cont)
{
	try {
		// 得到二条直线
		CString iLine1 = (*o)[L"USE_LINE1_Parament_Name"];
		CString iLine2 = (*o)[L"USE_LINE2_Parament_Name"];

		if (iLine1 == iLine2)
		{
			MyLog(L"MeasureOneAngle, 角度测量，二条线是同一条线！");
			return FALSE;
		}

		UCI::OptionsMap * oLine1 = UCI::ELE.GetOptionsMapByEleName(iLine1);
		UCI::OptionsMap * oLine2 = UCI::ELE.GetOptionsMapByEleName(iLine2);

		double L1Row1 = (*oLine1)[L"_RES_Line_Row1"];
		double L1Column1 = (*oLine1)[L"_RES_Line_Col1"];
		double L1Row2 = (*oLine1)[L"_RES_Line_Row2"];
		double L1Column2 = (*oLine1)[L"_RES_Line_Col2"];

		double L2Row1 = (*oLine2)[L"_RES_Line_Row1"];
		double L2Column1 = (*oLine2)[L"_RES_Line_Col1"];
		double L2Row2 = (*oLine2)[L"_RES_Line_Row2"];
		double L2Column2 = (*oLine2)[L"_RES_Line_Col2"];

		HTuple HV_Angle;

		// 求二条线的夹角
		AngleLl(
			L1Row1,
			L1Column1,
			L1Row2,
			L1Column2,
			L2Row1,
			L2Column1,
			L2Row2,
			L2Column2,
			&HV_Angle);

		double angleRad = HV_Angle[0].D();
		(*o)[L"RES_2LINE_Angle_Rad"] = angleRad;

		HTuple angleDeg;
		TupleDeg(angleRad, &angleDeg);
		(*o)[L"RES_夹角"] = angleDeg;

	
		HTuple InterRow, InterCol, IsOver;

		// 求二条线的交点
		IntersectionLines(
			L1Row1,
			L1Column1,
			L1Row2,
			L1Column2,
			L2Row1,
			L2Column1,
			L2Row2,
			L2Column2,
			&InterRow,
			&InterCol,
			&IsOver);  // 是不是同一条线

		double dInterRow = InterRow[0].D();
		double dInterCol = InterCol[0].D();

		HTuple hv_points;
		hv_points.Clear();
		hv_points[0] = dInterRow;
		hv_points[1] = dInterCol;
		
		(*o)[L"RES_Intersection_Row"] = dInterRow;
		(*o)[L"RES_Intersection_Column"] = dInterCol;
		(*o)[L"RES_Intersection_Points"] = hv_points;		
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneAngle：");
		return FALSE;
	}
	(*o)[L"元素已匹配"] = 1;
	return TRUE;
	
}

// 测量一个距离
BOOL ST_MYDISP::MeasureOneDistance(UCI::OptionsMap * o, HImage & hImg, HXLDCont& Cont)
{
	try
	{
		// 得到二条测量几何
		CString idDis1 = (*o)[L"引用元素名称1"];
		CString idDis2 = (*o)[L"引用元素名称2"];

		if (idDis1 == idDis2)
		{
			MyLog("MeasureOneAngle, 距离测量，二个几何是同一个！");
			return FALSE;
		}

		UCI::OptionsMap * oDIS1 = UCI::ELE.GetOptionsMapByEleName(idDis1);
		UCI::OptionsMap * oDIS2 = UCI::ELE.GetOptionsMapByEleName(idDis2);

		CString eleParameter1 = (*o)[L"引用元素1参数名"];
		CString eleParameter2 = (*o)[L"引用元素2参数名"];

		int etype1 = (*oDIS1)[L"元素类型"];
		int etype2 = (*oDIS2)[L"元素类型"];

		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];      // 测量象素到mm的比例

		double arrSize = UCI::OPTIONS[L"ElementArrowSize"];   // 显示箭头的大小、

		CString stColor = UCI::OPTIONS[L"ElementDimColor"];
		int width = UCI::OPTIONS[L"ElementDimWidth"];

		//  有可能是冲突
		//EnterCriticalSection(&m_csIPDisp);
		//UCI::pDis->m_pMHWinMain->SetColor(stColor.c_str());
		//UCI::pDis->m_pMHWinMain->SetLineWidth(width);
		//LeaveCriticalSection(&this->m_csFGDisp);

		//BOOL isDisDim = (*o)[L"IsDisplayDimension"];


		// 1. 二个几何都没有选参数名称
		if (eleParameter1 == "" && eleParameter2 == "")
		{
			if (etype1 == ELEMENT_CIRCLE && etype2 == ELEMENT_CIRCLE)
			{
				// 二个圆, 是求二个圆的圆心距
				double e1row = (*oDIS1)[L"RES_Circle_Center_Row"];
				double e1col = (*oDIS1)[L"RES_Circle_Center_Column"];

				double e2row = (*oDIS2)[L"RES_Circle_Center_Row"];
				double e2col = (*oDIS2)[L"RES_Circle_Center_Column"];

				HTuple hv_Dis;
				DistancePp(e1row, e1col, e2row, e2col, &hv_Dis);

				(*o)[L"RES_距离"] = hv_Dis;    // 这儿已乘了比例了	

				(*o)[L"距离Row1"] = e1row / scalemm;
				(*o)[L"距离Col1"] = e1col / scalemm;
				(*o)[L"距离Row2"] = e2row / scalemm;
				(*o)[L"距离Col2"] = e2col / scalemm;	

			}
			else if ((etype1 == ELEMENT_LINE && etype2 == ELEMENT_CIRCLE)
				|| (etype2 == ELEMENT_LINE && etype1 == ELEMENT_CIRCLE))
			{
				// 圆心到直线的垂直距离
				double Rrow, Rcol;                     // 圆心
				double Lrow1, Lcol1, Lrow2, Lcol2;     // 直线二个端点

				UCI::OptionsMap * oCir = oDIS1;
				UCI::OptionsMap * oLine = oDIS2;

				if (etype1 == ELEMENT_LINE) {
					oCir = oDIS2;
					oLine = oDIS1;
				}

				Rrow = (*oCir)[L"RES_Circle_Center_Row"];
				Rcol = (*oCir)[L"RES_Circle_Center_Column"];

				Lrow1 = (*oLine)[L"_RES_Line_Row1"];
				Lcol1 = (*oLine)[L"_RES_Line_Col1"];

				Lrow2 = (*oLine)[L"_RES_Line_Row2"];
				Lcol2 = (*oLine)[L"_RES_Line_Col2"];
				HTuple hv_Dis;

				DistancePl(Rrow, Rcol, Lrow1, Lcol1, Lrow2, Lcol2, &hv_Dis);
				(*o)[L"RES_距离"] = hv_Dis;

				// 求得圆心到直线垂足
				HTuple hv_foot_row, hv_foot_col;
				ProjectionPl(
					Rrow, Rcol, Lrow1, Lcol1, Lrow2, Lcol2,
					&hv_foot_row, &hv_foot_col);


				(*o)[L"距离Row1"] = Rrow / scalemm;
				(*o)[L"距离Col1"] = Rcol / scalemm;
				(*o)[L"距离Row2"] = hv_foot_row[0].D() / scalemm;
				(*o)[L"距离Col2"] = hv_foot_col[0].D() / scalemm;				

			}
			else if (etype1 == ELEMENT_LINE && etype2 == ELEMENT_LINE) {
				// 直线到直线的距离， 我们要从一条直线的中点，求到另一条直线的距离
				double L1row1, L1col1, L1row2, L1col2;     // 直线1 二个端点
				double L2row1, L2col1, L2row2, L2col2;     // 直线2 二个端点

				L1row1 = (*oDIS1)[L"_RES_Line_Row1"];
				L1col1 = (*oDIS1)[L"_RES_Line_Col1"];

				L1row2 = (*oDIS1)[L"_RES_Line_Row2"];
				L1col2 = (*oDIS1)[L"_RES_Line_Col2"];

				L2row1 = (*oDIS2)[L"_RES_Line_Row1"];
				L2col1 = (*oDIS2)[L"_RES_Line_Col1"];

				L2row2 = (*oDIS2)[L"_RES_Line_Row2"];
				L2col2 = (*oDIS2)[L"_RES_Line_Col2"];

				// 1. 求第一直线的中点
				double L1MidRow = (L1row1 + L1row2) / 2;
				double L1MidCol = (L1col1 + L1col2) / 2;

				// 2. 求中点到直线的距离
				HTuple hv_Dis;

				DistancePl(L1MidRow, L1MidCol, L2row1, L2col1, L2row2, L2col2, &hv_Dis);
				(*o)[L"RES_距离"] = hv_Dis;

				// 求得圆心到直线垂足
				HTuple hv_foot_row, hv_foot_col;
				ProjectionPl(
					L1MidRow, L1MidCol, L2row1, L2col1, L2row2, L2col2,
					&hv_foot_row, &hv_foot_col);

				(*o)[L"距离Row1"] = L1MidRow / scalemm;
				(*o)[L"距离Col1"] = L1MidCol / scalemm;
				(*o)[L"距离Row2"] = hv_foot_row[0].D() / scalemm;
				(*o)[L"距离Col2"] = hv_foot_col[0].D() / scalemm;		

			}
			else if (etype1 == ELEMENT_ANGLE && etype2 == ELEMENT_ANGLE) {
				// 求二个交点的长度

				// 二个圆, 是求二个圆的圆心距
				double e1row = (*oDIS1)[L"RES_Intersection_Row"];
				double e1col = (*oDIS1)[L"RES_Intersection_Column"];

				double e2row = (*oDIS2)[L"RES_Intersection_Row"];
				double e2col = (*oDIS2)[L"RES_Intersection_Column"];

				HTuple hv_Dis;
				DistancePp(e1row, e1col, e2row, e2col, &hv_Dis);

				(*o)[L"RES_距离"] = hv_Dis;    // 这儿已乘了比例了	

				(*o)[L"距离Row1"] = e1row / scalemm;
				(*o)[L"距离Col1"] = e1col / scalemm;
				(*o)[L"距离Row2"] = e2row / scalemm;
				(*o)[L"距离Col2"] = e2col / scalemm;

			}
		}
		

		// 产生一个箭头，表示一下距离

		double row1 = (*o)[L"距离Row1"];
		double col1 = (*o)[L"距离Col1"];
		double row2 = (*o)[L"距离Row2"];
		double col2 = (*o)[L"距离Col2"];

		HXLDCont Arrow;
		double Arrow_HeadLength = (*o)[L"尺寸箭头长度"];
		double Arrow_HeadWidth = (*o)[L"尺寸箭头宽度"];
		gen_double_arrow_contour_xld(
			&Arrow,
			row1,
			col1,
			row2,
			col2,
			Arrow_HeadLength,
			Arrow_HeadWidth);
		Cont = Cont.ConcatObj(Arrow);    // 保存箭头

	}
	catch (HException& except){		
		UCI::sDB.Log(except, L"MeasureOneDistance：");
		return FALSE;
	}


	(*o)[L"元素已匹配"] = 1;
	return TRUE;

}

// 测量一个相机元素
BOOL ST_MYDISP::MeasureOneCamera(UCI::OptionsMap * o, HImage & hImg, HXLDCont& Cont)
{
	
	try {

		if (this->mAppState == APP_MANUAL_MEASURING) {   // 当前是手工测量 我们需要打开测量查找模型	
			UCI::OPTIONS[L"转换矩阵成功"] = FALSE;
			UCI::OPTIONS[L"匹配模板 OK"] = FALSE;
		}

		// 2. 采集一个图像,
		this->OnDisMeasureOnePic(o, &hImg);                              // 实时采集一个图，并做图像校正	
		Cont.GenEmptyObj();
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneCamera：");
		return FALSE;
	}

	return TRUE;
}

BOOL ST_MYDISP::MeasureOneROI(UCI::OptionsMap * o, HalconCpp::HImage & hImg, HalconCpp::HXLDCont &)
{
	try {
		//HImage ImageTmp;

		//EnterCriticalSection(&m_csIPDisp);
		//ImageTmp = this->m_sIPResult.result_img;    // 取得校正后的图像
		//LeaveCriticalSection(&m_csIPDisp);

		//this->m_pMHWinMain->ClearWindow();


		double Row = (*o)[L"ROI_Rectangle2_Row"];
		double Column = (*o)[L"ROI_Rectangle2_Column"];
		double Phi = (*o)[L"ROI_Rectangle2_Phi"];
		double Length1 = (*o)[L"ROI_Rectangle2_Length1"];
		double Length2 = (*o)[L"ROI_Rectangle2_Length2"];

		//=============================================================================
		// 先校正图像
		BOOL bIsMap = (*o)[L"USE_map_image"];
		if (bIsMap) {
			BOOL bLoadCalibrateOK = UCI::OPTIONS[L"标定信息 OK"];
			if (bLoadCalibrateOK == FALSE) {

				this->LoadCaliPoseAndParamter();
				bLoadCalibrateOK = UCI::OPTIONS[L"标定信息 OK"];
				if (bLoadCalibrateOK == FALSE)
				{
					write_message(200, 20, "加载标定数据没有成功，请先标定相机");
					UCI::MyLog(L"MeasureOneFUNCTION()： 加载映射图像失败！");
					return FALSE;
				}
			}
			// 校正图像
			hImg = hImg.MapImage(this->ho_RectMap);				
		}

		EnterCriticalSection(&m_csIPDisp);
		this->m_sIPResult.result_img = hImg;
		LeaveCriticalSection(&m_csIPDisp);

		HalconCpp::HRegion Rectangle;
		Rectangle.GenRectangle2(Row, Column, Phi, Length1, Length2);

		hImg = hImg.ReduceDomain(Rectangle);

	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneROI()：");
		return FALSE;
	}
	return TRUE;
}

// 算子
BOOL ST_MYDISP::MeasureOneFUNCTION(UCI::OptionsMap * o, HImage & hImg, HXLDCont &Cont)
{
	try {

		HImage ImageTmp = hImg;
		HRegion RegionTmp;

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(ImageTmp);



		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(ImageTmp);

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(hImg);

		//=============================================================================
		// 直方图均衡
		BOOL bIsequ_histo_image = (*o)[L"USE_equ_histo_image"];
		if (bIsequ_histo_image) {
			ImageTmp = ImageTmp.EquHistoImage();
		}
		
		//=============================================================================
		// 强调图像

		BOOL bIsUSE_emphasize = (*o)[L"USE_emphasize"];
		if (bIsUSE_emphasize) {

			int emphasize_MaskWidth = (*o)[L"emphasize_MaskWidth"];
			int emphasize_MaskHeight = (*o)[L"emphasize_MaskHeight"];
			double emphasize_Factor = (*o)[L"emphasize_Factor"];

			ImageTmp = ImageTmp.Emphasize(emphasize_MaskWidth, emphasize_MaskHeight, emphasize_Factor);
		}

		//=============================================================================
		// mean_image
		BOOL bIsUSE_mean_image = (*o)[L"USE_mean_image"];
		if (bIsUSE_mean_image) {

			int mean_image_MaskWidth = (*o)[L"mean_image_MaskWidth"];
			int mean_image_MaskHeight = (*o)[L"mean_image_MaskHeight"];			

			ImageTmp = ImageTmp.MeanImage(mean_image_MaskWidth, mean_image_MaskHeight);
		}

	//	this->m_pMHWinMain->ClearWindow();
	//	this->m_pMHWinMain->DispObj(ImageTmp);

		BOOL bIsUSE_threshold = (*o)[L"USE_threshold"];
		if (bIsUSE_threshold) {

			double threshold_MinGray = (*o)[L"threshold_MinGray"];
			double threshold_MaxGray = (*o)[L"threshold_MaxGray"];

			RegionTmp = ImageTmp.Threshold(threshold_MinGray, threshold_MaxGray);
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// 膨胀
		BOOL isUSE_dilation_circle = (*o)[L"USE_dilation_circle"];
		if (isUSE_dilation_circle) {
			double dilation_circle_radius = (*o)[L"dilation_circle_radius"];

			RegionTmp = RegionTmp.DilationCircle(dilation_circle_radius);
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// fill_up
		BOOL USE_fill_up = (*o)[L"USE_fill_up"];
		if (USE_fill_up) {
			RegionTmp = RegionTmp.FillUp();
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);
		
		// 收缩
		BOOL USE_erosion_circle = (*o)[L"USE_erosion_circle"];
		if (USE_erosion_circle) {
			double erosion_circle_radius = (*o)[L"erosion_circle_radius"];

			RegionTmp = RegionTmp.ErosionCircle(erosion_circle_radius);			
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// 把不是想要的部分部分搞成白色的。	
		
		//= hImg.Difference(hImg);

	//	this->m_pMHWinMain->ClearWindow();
	//	this->m_pMHWinMain->DispObj(RegionBack);
	//	HImage reTmp;

		BOOL bIsUSE_reduce_domain = (*o)[L"USE_reduce_domain"];
		if (bIsUSE_reduce_domain) {

			HRegion RegionBack;
			Difference(hImg, RegionTmp, &RegionBack);
			RegionBack.OverpaintRegion(ImageTmp, 0, "fill");

			BOOL USE_reduce_domain_Gray = (*o)[L"USE_reduce_domain_Gray"];
			if (USE_reduce_domain_Gray) {
				RegionTmp.OverpaintRegion(ImageTmp, 255, "fill");
			}
		}

		hImg = ImageTmp;
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneFUNCTION()：");
		return FALSE;
	}
	return TRUE;
}

// 匹配
BOOL ST_MYDISP::MeasureOneMATCH(UCI::OptionsMap * o, HImage & hImg, HXLDCont &Cont)
{
	try {

		BOOL bLoadROI = UCI::OPTIONS[L"匹配模板 OK"];

		if (bLoadROI == FALSE) {

			int prID = UCI::OPTIONS[L"项目编号"];
			
			CString stRegion = this->GetShapeModelRegionPathFile(prID); // 1. 打开原来保存的查找模型
			CString stModel = this->GetShapeModelPathFile(prID);

			if (GetFileAttributes(stRegion) == -1) { // 文件不存在
				MyLog(stRegion);
				MyLog("MeasureOneMATCH() ROI　区域文件不存在！ ");
				return FALSE;
			}

			if (GetFileAttributes(stModel) == -1) { // 文件不存在
				MyLog(stModel);
				MyLog("MeasureOneMATCH() ROI　模板文件不存在！ ");
				return FALSE;
			}

			this->hv_Model.ReadShapeModel(CT2A(stModel).m_psz);       // 读出模板文件
			this->ho_ModelRegion.ReadRegion(CT2A(stRegion));          // 读出匹配区域文件

			UCI::OPTIONS[L"匹配模板 OK"] = TRUE;
		}

		// 1. 查找图像中的产品
		double AngleStart = (*o)[L"FindShapeModleAngleStart"];
		double AngleExtend = (*o)[L"FindShapeModleAngleExtend"];
		double minScroe = (*o)[L"FindShapeModleMinScore"];
		int NumMatches = (*o)[L"FindShapeModleNumMatches"];
		double MaxOverlap = (*o)[L"FindShapeModleMaxOverlap"];
		CString SubPixel = (*o)[L"FindShapeModleSubPixel"];
		int NumLevels = (*o)[L"FindShapeModleNumLevels"];
		double Greediness = (*o)[L"FindShapeModleGreediness"];

		HTuple Row, Column, Angle, Score;
		this->hv_Model.FindShapeModel(
			hImg,
			AngleStart,
			AngleExtend,
			minScroe,
			NumMatches,
			MaxOverlap,
			CT2A(SubPixel).m_psz,
			NumLevels,
			Greediness,
			&Row,
			&Column,
			&Angle,
			&Score
		);

		(*o)[L"RES_FindShapeRow"] = Row;
		(*o)[L"RES_FindShapeColumn"] = Column;
		(*o)[L"RES_FindShapeAngle"] = Angle;
		(*o)[L"RES_FindShapeScore"] = Score;



		if (Row.Length() < 1 ) {

			//write_message(420, 20, "没有发现对象, 请重新采集对象", true);

			this->IsSpaceKeyDown = FALSE;   // 当前的按钮无效
			return FALSE;
		}



		//write_message(100, 20, "找到了对象，得分：" + Score, true);


		//dev_display_shape_matching_results(
		//	this->hv_Model,
		//	"red",
		//	Row,
		//	Column,
		//	Angle,
		//	1,
		//	1,
		//	0);

		//

		HXLDCont Contour, Cross;
		HTuple Row0, Col0;

		//HHomMat2D HomMat2D_T;
		// 4. 定位转换 找到物体, 求仿射矩阵, 对 ROI 特征点进行仿射变换	
		// 产生模板轮廓仿射变换矩阵
		//HomMat2D_T.VectorAngleToRigid(HTuple(0), HTuple(0), HTuple(0), Row, Column, Angle);
		// 获取模板轮廓		
		//Contour = this->hv_Model.GetShapeModelContours(1);
		// 对模板轮廓进行仿射变换
		//Contour = HomMat2D_T.AffineTransContourXld(Contour);
		// 把模板轮廓添加到显示图形
		//objDisp = objDisp.ConcatObj(Contour);
		//this->m_pMHWinMain->DispObj(objDisp);

		// 在模板中心产生一个x,表示模板中心
		// Cross.GenCrossContourXld(Row, Column, HTuple(20), HTuple(45.).TupleRad());		


		// 把模板中心x添加到显示图形
		//Cont = Cont.ConcatObj(Cross);

		// 计算模板区域中心

		//this->m_pMHWinMain->DispObj(this->ho_ModelRegion);

		this->ho_ModelRegion.AreaCenter(&Row0, &Col0);

		//this->write_message(300, 50, "Row0 " +  Row0 + " Col0 " + Col0);
		//this->write_message(600, 50, "Row  " + Row + " Column " + Column);

		// 产生ROI仿射变换矩阵  其它测量元素需要这个变换		
		this->HomMat2D.VectorAngleToRigid(Row0, Col0, HTuple(0), Row, Column, Angle);
	
		this->HomMatAngle = Angle;  // 保存一下变换角度

		UCI::OPTIONS[L"转换矩阵成功"] = TRUE;
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneFUNCTION()：");
		return FALSE;
	}
	return TRUE;
}

// 1维尺寸测量
BOOL ST_MYDISP::MeasureOneELEMENT_MEASURE_RECTANGLE2(UCI::OptionsMap * o, HImage & hImg, HXLDCont &Cont)
{
	try {
		// 准备数据
		double hv_Row = (*o)[L"Measure_Rectangle2_Row"];
		double hv_Column = (*o)[L"Measure_Rectangle2_Column"];
		double hv_Phi = (*o)[L"Measure_Rectangle2_Phi"];
		double hv_Length1 = (*o)[L"Measure_Rectangle2_Length1"];
		double hv_Length2 = (*o)[L"Measure_Rectangle2_Length2"];

		CString Interpolation = (*o)[L"Measure_Rectangle2_Interpolation"];

		int width = UCI::OPTIONS[L"图像宽度"];
		int height = UCI::OPTIONS[L"图像高度"];

		HTuple new_hv_Row, new_hv_Column, new_hv_Phi;
		// 要变换一下啊
		AffineTransPoint2d(this->HomMat2D,
			hv_Row, hv_Column,
			&new_hv_Row, &new_hv_Column);

		new_hv_Phi = hv_Phi + this->HomMatAngle;

		// 显示一下测量矩形
		HXLDCont Rect;
		Rect.GenRectangle2ContourXld(new_hv_Row[0].D(),
			new_hv_Column[0].D(),
			new_hv_Phi[0].D(),
			hv_Length1,
			hv_Length2);

		Cont = Cont.ConcatObj(Rect);

		HMeasure ho_Measure;
		ho_Measure.GenMeasureRectangle2(
			new_hv_Row[0].D(),
			new_hv_Column[0].D(),
			new_hv_Phi[0].D(),
			hv_Length1,
			hv_Length2,
			width,
			height,
			CT2A(Interpolation).m_psz);

		// 设置测量算子参数
		double Sigma = (*o)[L"measure_pairs_Sigma"];
		double Threshold = (*o)[L"measure_pairs_Threshold"];
		CString Transition = (*o)[L"measure_pairs_Transition"];
		CString Select = (*o)[L"measure_pairs_Select"];

		HTuple RowEdgeFirst, ColumnEdgeFirst, AmplitudeFirst, RowEdgeSecond, ColumnEdgeSecond, AmplitudeSecond, IntraDistance, InterDistance;
	
		ho_Measure.MeasurePairs(
			hImg,
			Sigma,
			Threshold,
			CT2A(Transition).m_psz,
			CT2A(Select).m_psz,
			&RowEdgeFirst,
			&ColumnEdgeFirst,
			&AmplitudeFirst,
			&RowEdgeSecond,
			&ColumnEdgeSecond,
			&AmplitudeSecond,
			&IntraDistance,
			&InterDistance);

		int len = (int)RowEdgeFirst.Length();

		// 保存测量结果
		(*o)[L"measure_pairs_RowEdgeFirst"] = RowEdgeFirst;
		(*o)[L"measure_pairs_ColumnEdgeFirst"] = ColumnEdgeFirst;
		(*o)[L"measure_pairs_AmplitudeFirst"] = AmplitudeFirst;
		(*o)[L"measure_pairs_RowEdgeSecond"] = RowEdgeSecond;
		(*o)[L"measure_pairs_ColumnEdgeSecond"] = ColumnEdgeSecond;
		(*o)[L"measure_pairs_AmplitudeSecond"] = AmplitudeSecond;
		(*o)[L"RES_measure_pairs_IntraDistance"] = IntraDistance;
		(*o)[L"RES_measure_pairs_InterDistance"] = InterDistance;

		double scalemm = UCI::OPTIONS[L"单个像素长度mm"];                // 测量象素到mm的比例

		double dis = 0.0f;

		for (int i = 0; i < IntraDistance.Length(); i++) {
			dis += IntraDistance[i].D();
		}

		for (int i = 0; i < InterDistance.Length(); i++) {
			dis += InterDistance[i].D();
		}

		(*o)[L"RES_一维总长"] = dis * scalemm;              // 把所有距离加起来


		// 显示这个尺寸
		//hv_Length2 

		p_disp_dimensions(RowEdgeFirst, ColumnEdgeFirst, RowEdgeSecond, ColumnEdgeSecond,
			IntraDistance, InterDistance, new_hv_Phi, hv_Length2, scalemm,Cont);

	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneELEMENT_MEASURE_RECTANGLE2()：");
		return FALSE;
	}
	return TRUE;

	//return 0;
}

// 检测球囊的缺陷
BOOL ST_MYDISP::MeasureOneELEMENT_MEASURE_BalloonFlaw(UCI::OptionsMap * o, HalconCpp::HImage & hImg, HalconCpp::HXLDCont &Cont)
{
	
	try {
		HImage ImageTmp;

		EnterCriticalSection(&m_csIPDisp);
		ImageTmp = this->m_sIPResult.result_img;    // 取得校正后的图像
		LeaveCriticalSection(&m_csIPDisp);

		//this->m_pMHWinMain->ClearWindow();

		// reduce_domain
		double Row = (*o)[L"Flaw_Rectangle2_Row"];
		double Column = (*o)[L"Flaw_Rectangle2_Column"];
		double Phi = (*o)[L"Flaw_Rectangle2_Phi"];
		double Length1 = (*o)[L"Flaw_Rectangle2_Length1"];
		double Length2 = (*o)[L"Flaw_Rectangle2_Length2"];

		// 要转换一下啊

		HTuple new_Row, new_Column, new_Phi;
		// 要变换一下啊
		AffineTransPoint2d(this->HomMat2D,
			Row, Column,
			&new_Row, &new_Column);

		new_Phi = Phi + this->HomMatAngle;


		HalconCpp::HRegion Rectangle;
		Rectangle.GenRectangle2(new_Row, new_Column, new_Phi, (HTuple)Length1, (HTuple)Length2);

		ImageTmp = ImageTmp.ReduceDomain(Rectangle);

		//this->m_pMHWinMain->DispObj(ImageTmp);

		int mean_image_MaskWidth = (*o)[L"Flaw_mean_image_MaskWidth"];
		int mean_image_MaskHeight = (*o)[L"Flaw_mean_image_MaskHeight"];

		HImage ImageMean;
		ImageMean = ImageTmp.MeanImage(mean_image_MaskWidth, mean_image_MaskHeight);

		HalconCpp::HRegion RegionDynThresh;

		double dyn_threshold_Offset = (*o)[L"Flaw_dyn_threshold_Offset"];
		CString dyn_threshold_LightDark = (*o)[L"Flaw_dyn_threshold_LightDark"];

		RegionDynThresh = ImageTmp.DynThreshold(ImageMean, dyn_threshold_Offset, CT2A(dyn_threshold_LightDark).m_psz);

		//this->m_pMHWinMain->DispObj(RegionDynThresh);

		HalconCpp::HRegion ConnectedRegions;
		ConnectedRegions = RegionDynThresh.Connection();

		CString select_shape_Features = (*o)[L"Flaw_select_shape_Features"];
		CString select_shape_Operation = (*o)[L"Flaw_select_shape_Operation"];
		double select_shape_Min = (*o)[L"Flaw_select_shape_Min"];
		double select_shape_Max = (*o)[L"Flaw_select_shape_Max"];		

		HalconCpp::HRegion SelectedRegions;

		SelectedRegions = ConnectedRegions.SelectShape(CT2A(select_shape_Features).m_psz,
			CT2A(select_shape_Operation).m_psz,
			select_shape_Min,
			select_shape_Max);

		double closing_circle_Radius = (*o)[L"Flaw_closing_circle_Radius"];

		HalconCpp::HRegion RegionClosing;

		RegionClosing = SelectedRegions.ClosingCircle(closing_circle_Radius);

		HTuple Areas, Rows, Columns;
		Areas = RegionClosing.AreaCenter(&Rows, &Columns);

		int len = (int)Areas.Length();
		
		(*o)[L"RES_缺陷数量"] = (double)len;
		double min_area = select_shape_Max;
		double max_area = 0.0;

		for (int i = 0; i < len; i++) {
			// 画图标出缺陷所在
			HalconCpp::HXLDCont Circle;

			double area = Areas[i].D();

			if (area > max_area) {
				max_area = area;
			}
			if (area < min_area) {
				min_area = area;
			}

			double radius = sqrt(area / 3.14f) * 3.0f;
			Circle.GenCircleContourXld(Rows[i].D(),
				Columns[i].D(), radius,0.0, 6.28318f,"positive",1.0f);
			Cont = Cont.ConcatObj(Circle);
		}

		(*o)[L"RES_缺陷最大面积"] = max_area;
		(*o)[L"RES_缺陷最小面积"] = min_area;

		//int num = RegionClosing.nu
		//this->m_pMHWinMain->DispObj(RegionClosing);
		//int a = 0;

	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MEASURE_BalloonFlaw()：");
		return FALSE;
	}
	return TRUE;
}

// 测量一个结果 输出到报表
BOOL ST_MYDISP::MeasureOneResult(UCI::OptionsMap * o, HXLDCont& Cont)
{
	CString ResNormal = (*o)[L"报表列名称"];
	CString eleName = (*o)[L"输出元素名称"];
	CString eleParameter = (*o)[L"输出元素的参数"];

	// 查找到这个元素
	UCI::OptionsMap * ores = UCI::ELE.GetOptionsMapByEleName(eleName);

	if (ores == nullptr) {
		MyLog(L"要测量的元素不存在！");
		MyLog(eleName + eleParameter);
		return FALSE;
	}

	if (eleParameter == "") {
		return FALSE;
	}

	double dRes = (*ores)[eleParameter];

	(*o)[L"输出的测量值"] = dRes;

	double lower_limit = (*o)[L"输出的下公差"];
	double upper_limit = (*o)[L"输出的上公差"];
	double standard = (*o)[L"输出的标准值"];

	if (dRes < (standard + lower_limit)) {
		(*o)[L"输出测量结论"] = CString(L"TS");
		UCI::OPTIONS[L"GLO_Measure_OK"] = FALSE;
	}
	else if (dRes >(standard + upper_limit)) {
		(*o)[L"输出测量结论"] = CString(L"TL");
		UCI::OPTIONS[L"GLO_Measure_OK"] = FALSE;
	}
	else {
		(*o)[L"输出测量结论"] = CString(L"OK");
	}


	(*o)[L"元素已匹配"] = 1;

	return TRUE;
}

// 自动校正长度至 mm 
BOOL ST_MYDISP::MeasureOneMM(UCI::OptionsMap * o, HXLDCont& Cont)
{
	//string ResNormal = (*o)[L"报表列名称"];

	CString eleNameX = (*o)[L"校正元素 X"];	

	CString eleNameY = (*o)[L"校正元素 Y"];

	// 查找到这个元素
	UCI::OptionsMap * oresX = UCI::ELE.GetOptionsMapByEleName(eleNameX);
	UCI::OptionsMap * oresY = UCI::ELE.GetOptionsMapByEleName(eleNameY);

	if (oresX == nullptr) {
		MyLog(L"要测量的元素 X 不存在！");
		MyLog(eleNameX);
		return FALSE;
	}
	if (oresY == nullptr) {
		MyLog(L"要测量的元素 Y 不存在！");
		MyLog(eleNameY);
		return FALSE;
	}


	double dResX = (*oresX)[L"输出的测量值"];
	double dResY = (*oresY)[L"输出的测量值"];


	double standardX = (*oresX)[L"输出的标准值"];
	double standardY = (*oresY)[L"输出的标准值"];

	if (standardX == 0) standardX = 1.0f;
	if (standardY == 0) standardY = 1.0f;

	double scalemm = UCI::OPTIONS[L"单个像素长度mm"];      // 测量象素到mm的比例

	dResX = dResX / scalemm;    // 求出像素
	dResY = dResY / scalemm;

	double scalemmX = standardX / dResX;
	double scalemmY = standardY / dResY;

	double scalem = (scalemmX + scalemmY) / 2.0f;
	
	UCI::OPTIONS[L"单个像素长度mm"] = scalem;

	MyLog(L"校正 mm 成功！");
	CString st = to_wstring(scalem).c_str();
	MyLog(L"OnePixelScalemm = " + st);

	// 保存一下参数
	UCI::sDB.MySaveSettingToSqlite(UCI::OPTIONS);     // 保存全局参数
	
	return TRUE;
}

// 手动测量一次
void ST_MYDISP::DisOnElementMeasureManual()
{
	
	// 1 检测当前的状态
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;
	this->AppStateChangeTo(APP_MANUAL_MEASURING);
	
	this->bManualMeasureReady = FALSE;	

	try {

		this->m_pMHWinMain->ClearWindow(); 

		HXLDCont Contours;
		HImage hImg;

		BOOL isLoad = UCI::OPTIONS[L"标定信息 OK"];
		if (isLoad == FALSE) {
			if (this->LoadCaliPoseAndParamter() == FALSE) {

				UCI::OptionsMap * camo = &UCI::ELE.EleList.GetHead();
				//UCI::pDis->OnDisMeasureOnePic(camo);

				if (this->LoadCaliPoseAndParamter() == FALSE) {
					write_message(200, 20, "加载标定数据没有成功，请先标定相机");

					this->AppStateChangeTo(APP_READY);
					return;
				}
			}
		}

		HTuple T1, T2;
		CountSeconds(&T1);
		
		// 5. 依次测量并保存测量结果到元素中
		// 插入所有该项目的元素
		UCI::OPTIONS[L"GLO_Measure_OK"] = TRUE;                // 是否合格
		
		//this->elementResultIndex = 1;
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *o = &UCI::ELE.EleList.GetNext(pos);
			//////////////////////////////////////////////////
			// 对每一个元素进行测量		
			if (!this->MeasureOneElement(o, hImg, Contours)) {				

				CString eleName = (*o)[L"元素名称"];
				CString str = L"手动测量出错：元素名称：" + eleName;			
				MyLog(str);

				this->AppStateChangeTo(APP_READY);
				return;   // 有一个元素没有测量成功
			}
		}


		// 显示最终的测量结果 
		CString stColor = UCI::OPTIONS[L"ElementHDLXColor"];
		int width = UCI::OPTIONS[L"ElementHDLXWidth"];

		UCI::pDis->m_pMHWinMain->SetColor(CT2A(stColor).m_psz);
		UCI::pDis->m_pMHWinMain->SetLineWidth(width);

		// 显示一下计算轮廓
		EnterCriticalSection(&m_csIPDisp);
		this->m_sIPResult.result_cont = Contours;

		// 判断是不是显示原图
		BOOL isDispOrgPic = UCI::OPTIONS[L"显示原图"];
		if (!isDispOrgPic) {
			this->m_sIPResult.result_img = hImg;	        // 这儿显示原来的图像好一些
		}
		LeaveCriticalSection(&m_csIPDisp);		

		
		this->OnMainDisp(NULL, NULL);	

		// 输出测量结果到LIST中
		UCI::pMain->ResDeleAllItems();
		pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *o = &UCI::ELE.EleList.GetNext(pos);

			int itype = (*o)[L"元素类型"];
			if (itype == ELEMENT_RESULT) {
				UCI::pMain->insertOneResult(o);	

				// 显示一下尺寸
				CString outID = (*o)[L"输出元素名称"];
				UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEleName(outID);				
				this->DisElementDimension(om);				
			}
		}
		CountSeconds(&T2);

		HTuple time = 1000 * (T2[0].D() - T1[0].D());
		CString str;
		str.Format(_T("手动测量时间: %4.2f ms"), time[0].D());
		UCI::MyLog(str);
		
		BOOL MOK = UCI::OPTIONS[L"GLO_Measure_OK"];
		if (MOK) {
			UCI::Speech(L"合格", true);

			SolidBrush blackBrush(Color(255, 0, 0, 255));  //半透明+文字RGB颜色
			this->DisFontLarge(L"PASS", 0, 0, 60.0f, blackBrush);
		}
		else {
			UCI::Speech(L"不合格", true);

			SolidBrush blackBrush(Color(255, 255, 0, 0));  //半透明+文字RGB颜色
			this->DisFontLarge(L"NG", 0, 0, 60.0f, blackBrush);
		}
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"DisOnElementMeasureManual：");

	}

	this->bManualMeasureReady = TRUE;
	this->AppStateChangeTo(APP_READY);
}

// 显示元素的尺寸 
void ST_MYDISP::DisElementDimension(UCI::OptionsMap * o)
{
	if (o == nullptr) return;
	
	int etype = (*o)[L"元素类型"];
	switch (etype) 
	{
	case ELEMENT_MEASURE_RECTANGLE2:  // 一维测量
	{
		double midrow = (*o)[L"Measure_Rectangle2_Row"];     // 测量矩形的中心
		double midcol = (*o)[L"Measure_Rectangle2_Column"];

		double hv_Dis = (*o)[L"RES_一维总长"];

		DisElemnetDimension(o, hv_Dis, midrow, midcol);

	}
		break;
	case ELEMENT_ANGLE:      // 二条直线组成的长度
		break;
	case ELEMENT_DISTANCE:   // 二条线的距离
	{
		double row1 = (*o)[L"距离Row1"];
		double col1 = (*o)[L"距离Col1"];
		double row2 = (*o)[L"距离Row2"];
		double col2 = (*o)[L"距离Col2"];
		double hv_Dis = (*o)[L"RES_距离"];

		double midrow = (row1 + row2) / 2;
		double midcol = (col1 + col2) / 2;	

		DisElemnetDimension(o, hv_Dis, midrow, midcol);

	}
		break;
	case ELEMENT_BALLOONLENGTH: // 四个顶点组成的球囊长度
	{
		double row1 = (*o)[L"Balloon距离Row1"];
		double col1 = (*o)[L"Balloon距离Col1"];
		double row2 = (*o)[L"Balloon距离Row2"];
		double col2 = (*o)[L"Balloon距离Col2"];
		double hv_Dis = (*o)[L"RES_球囊长度"];

		double midrow = (row1 + row2) / 2;
		double midcol = (col1 + col2) / 2;

		DisElemnetDimension(o, hv_Dis, midrow, midcol);
	}
		break;
	default:
		break;
	}
}

void ST_MYDISP::DisElemnetDimension(UCI::OptionsMap * o, double hv_Dis, double midrow, double midcol)
{
	double dimOffsetCol = (*o)[L"尺寸显示偏移X"];
	double dimOffsetRow = (*o)[L"尺寸显示偏移Y"];

	// 显示引线
	// this->m_pMHWinMain->DispLine(midrow, midcol, midrow + dimOffsetRow, midcol + dimOffsetCol);
	this->m_pMHWinMain->SetTposition(int(midrow + dimOffsetRow), int(midcol + dimOffsetCol));

	// 显示
	HTuple hv_string;
	TupleString(hv_Dis, ".3f", &hv_string);
	this->m_pMHWinMain->WriteString(hv_string);
}



