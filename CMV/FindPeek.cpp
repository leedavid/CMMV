#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;
using namespace std;


void ST_MYDISP::peak(
	HalconCpp::HImage ho_Image, 
	HalconCpp::HTuple hv_Row, 
	HalconCpp::HTuple hv_Coloumn, 
	HalconCpp::HTuple hv_Phi, 
	HalconCpp::HTuple hv_Length1, 
	HalconCpp::HTuple hv_Length2, 
	HalconCpp::HTuple hv_DetectWidth, 
	HalconCpp::HTuple hv_Sigma, 
	HalconCpp::HTuple hv_Threshold,
	HalconCpp::HTuple hv_Transition, 
	HalconCpp::HTuple hv_Select, 
	HalconCpp::HTuple * hv_EdgeRows, 
	HalconCpp::HTuple * hv_EdgeColumns, 
	HalconCpp::HTuple * hv_ResultRow,
	HalconCpp::HTuple * hv_ResultColumn)
{
	// Local iconic variables
	HXLDCont  ho_Regions1;

	// Local control variables
	HTuple  hv_ResultCol, hv_ROILineRow1, hv_ROILineCol1;
	HTuple  hv_ROILineRow2, hv_ROILineCol2, hv_StdLineRow1;
	HTuple  hv_StdLineCol1, hv_StdLineRow2, hv_StdLineCol2;
	HTuple  hv_Cos, hv_Sin, hv_Col1, hv_Row1, hv_Col2, hv_Row2;
	HTuple  hv_Col3, hv_Row3, hv_Col4, hv_Row4, hv_ResultRows;
	HTuple  hv_ResultColumns, hv_Max, hv_i, hv_Distance1;

	//初始化
	(*hv_ResultRow) = -9999;
	hv_ResultCol = -9999;
	(*hv_EdgeColumns) = HTuple();
	(*hv_EdgeRows) = HTuple();

	//仿射矩形Length2所在直线作为rake工具的ROI
	hv_ROILineRow1 = 0;
	hv_ROILineCol1 = 0;
	hv_ROILineRow2 = 0;
	hv_ROILineCol2 = 0;

	//仿射矩形方向所直线的边做基准线
	hv_StdLineRow1 = 0;
	hv_StdLineCol1 = 0;
	hv_StdLineRow2 = 0;
	hv_StdLineCol2 = 0;
	//判断仿射矩形是否有效
	if (0 != (HTuple(hv_Length1 <= 0).TupleOr(hv_Length2 <= 0)))
	{
		return;
	}

	//计算仿射矩形角度的正弦值、余弦值
	TupleCos(hv_Phi, &hv_Cos);
	TupleSin(hv_Phi, &hv_Sin);

	//矩形第一个端点坐标
	hv_Col1 = 1.0*((hv_Coloumn - (hv_Length1*hv_Cos)) - (hv_Length2*hv_Sin));
	hv_Row1 = 1.0*(hv_Row - (((-hv_Length1)*hv_Sin) + (hv_Length2*hv_Cos)));

	//矩形第二个端点坐标
	hv_Col2 = 1.0*((hv_Coloumn + (hv_Length1*hv_Cos)) - (hv_Length2*hv_Sin));
	hv_Row2 = 1.0*(hv_Row - ((hv_Length1*hv_Sin) + (hv_Length2*hv_Cos)));

	//矩形第三个端点坐标
	hv_Col3 = 1.0*((hv_Coloumn + (hv_Length1*hv_Cos)) + (hv_Length2*hv_Sin));
	hv_Row3 = 1.0*(hv_Row - ((hv_Length1*hv_Sin) - (hv_Length2*hv_Cos)));

	//矩形第四个端点坐标
	hv_Col4 = 1.0*((hv_Coloumn - (hv_Length1*hv_Cos)) + (hv_Length2*hv_Sin));
	hv_Row4 = 1.0*(hv_Row - (((-hv_Length1)*hv_Sin) - (hv_Length2*hv_Cos)));

	//仿射矩形方向所直线的边做基准线
	hv_StdLineRow1 = hv_Row2;
	hv_StdLineCol1 = hv_Col2;
	hv_StdLineRow2 = hv_Row3;
	hv_StdLineCol2 = hv_Col3;

	//仿射矩形Length2所在直线作为rake工具的ROI
	hv_ROILineRow1 = (hv_Row1 + hv_Row2)*0.5;
	hv_ROILineCol1 = (hv_Col1 + hv_Col2)*0.5;
	hv_ROILineRow2 = (hv_Row3 + hv_Row4)*0.5;
	hv_ROILineCol2 = (hv_Col3 + hv_Col4)*0.5;
	//gen_rectangle2_contour_xld (Rectangle, Row, Coloumn, Phi, Length1, Length2)
	rake(ho_Image, &ho_Regions1, (1.0*hv_Length2) * 2, hv_Length1 * 2, hv_DetectWidth,
		hv_Sigma, hv_Threshold, hv_Transition, hv_Select, hv_ROILineRow1, hv_ROILineCol1,
		hv_ROILineRow2, hv_ROILineCol2, &hv_ResultRows, &hv_ResultColumns);

	//求所有边缘点到基准线的距离，保存最大距离及其对应的边缘点坐标，作为顶点
	hv_Max = 0;
	if (0 != ((hv_ResultColumns.TupleLength())>0))
	{
		(*hv_EdgeRows) = hv_ResultRows;
		(*hv_EdgeColumns) = hv_ResultColumns;
		{
			HTuple end_val61 = (hv_ResultColumns.TupleLength()) - 1;
			HTuple step_val61 = 1;
			for (hv_i = 0; hv_i.Continue(end_val61, step_val61); hv_i += step_val61)
			{
				DistancePl(HTuple(hv_ResultRows[hv_i]), HTuple(hv_ResultColumns[hv_i]), hv_StdLineRow1,
					hv_StdLineCol1, hv_StdLineRow2, hv_StdLineCol2, &hv_Distance1);
				if (0 != (hv_Max<hv_Distance1))
				{
					hv_Max = hv_Distance1;
					(*hv_ResultRow) = ((const HTuple&)hv_ResultRows)[hv_i];
					(*hv_ResultColumn) = ((const HTuple&)hv_ResultColumns)[hv_i];
				}

			}
		}
	}


	return;

}


