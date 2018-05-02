#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;



void ST_MYDISP::draw_rake(
	HalconCpp::HXLDCont * ho_Regions,
	HalconCpp::HTuple hv_Elements,
	HalconCpp::HTuple hv_DetectHeight,
	HalconCpp::HTuple hv_DetectWidth,
	double * hv_Row1,
	double * hv_Column1,
	double * hv_Row2,
	double * hv_Column2)

{

	// Local iconic variables
	HXLDCont  ho_RegionLines, ho_Rectangle, ho_Arrow1;

	// Local control variables
	HTuple  hv_ATan, hv_i, hv_RowC, hv_ColC, hv_Distance;
	HTuple  hv_RowL2, hv_RowL1, hv_ColL2, hv_ColL1;

	//this->write_message(12, 12, "鼠标左键画一条直线，右键确认", true);

	
	//产生一个空显示对象，用于显示
	//ho_Regions->GenEmptyObj();
	//GenEmptyObj(&(*ho_Regions));

	//this->m_pMHWinMain->DrawLine(&(*hv_Row1), &(*hv_Column1), &(*hv_Row2), &(*hv_Column2));
	//产生直线 XLD
	ho_RegionLines.GenContourPolygonXld(HTuple(*hv_Row1).TupleConcat((*hv_Row2)), HTuple(*hv_Column1).TupleConcat((*hv_Column2)));
	//GenContourPolygonXld(&ho_RegionLines, HTuple(*hv_Row1).TupleConcat((*hv_Row2)), HTuple(*hv_Column1).TupleConcat((*hv_Column2)));
	//存储到显示对象
	*ho_Regions = ho_Regions->ConcatObj(ho_RegionLines);
	//ConcatObj((*ho_Regions), ho_RegionLines, &(*ho_Regions));
	//计算直线与X轴的夹角，逆时针方向为正向
	AngleLx((*hv_Row1), (*hv_Column1), (*hv_Row2), (*hv_Column2), &hv_ATan);
	//边缘检测方向垂直检测直线，直线方向旋转90度为边缘检测方向
	hv_ATan += HTuple(90).TupleRad();
	//根据检测直线按顺序产生测量区域，并存储到显示对象
	{
		HTuple end_val13 = hv_Elements;
		HTuple step_val13 = 1;
		for (hv_i = 1; hv_i.Continue(end_val13, step_val13); hv_i += step_val13)
		{
			//如果只有一个测量矩形，作为卡尺工具，宽度为检测直线的长度
			if (0 != (hv_Elements == 1))
			{
				hv_RowC = ((*hv_Row1) + (*hv_Row2))*0.5;
				hv_ColC = ((*hv_Column1) + (*hv_Column2))*0.5;
				DistancePp((*hv_Row1), (*hv_Column1), (*hv_Row2), (*hv_Column2), &hv_Distance);
				ho_Rectangle.GenRectangle2ContourXld(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_Distance / 2);
				//GenRectangle2ContourXld(&ho_Rectangle, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
				//	hv_Distance / 2);
			}
			else
			{
				//如果有多个测量矩形，产生该测量矩形xld
				hv_RowC = (*hv_Row1) + ((((*hv_Row2) - (*hv_Row1))*(hv_i - 1)) / (hv_Elements - 1));
				hv_ColC = (*hv_Column1) + ((((*hv_Column2) - (*hv_Column1))*(hv_i - 1)) / (hv_Elements - 1));
				ho_Rectangle.GenRectangle2ContourXld(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_DetectWidth / 2);
				//GenRectangle2ContourXld(&ho_Rectangle1, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
				//	hv_DetectWidth / 2);

			}
			//把测量矩形XLD存储到显示对象
			*ho_Regions = ho_Regions->ConcatObj(ho_Rectangle);
			//ConcatObj((*ho_Regions), ho_Rectangle, &(*ho_Regions));
			if (0 != (hv_i == 1))
			{
				//在第一个测量矩形绘制一个箭头xld,用于只是边缘检测方向
				hv_RowL2 = hv_RowC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				
				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2,
					25, 25);

				//把箭头XLD存储到显示对象
				*ho_Regions = ho_Regions->ConcatObj(ho_Arrow1);	
			}
		}
	}
	return;
}



void ST_MYDISP::rake(
	HalconCpp::HImage ho_Image,
	HalconCpp::HXLDCont * ho_Regions,
	HalconCpp::HTuple hv_Elements,
	HalconCpp::HTuple hv_DetectHeight,
	HalconCpp::HTuple hv_DetectWidth,
	HalconCpp::HTuple hv_Sigma,
	HalconCpp::HTuple hv_Threshold,
	HalconCpp::HTuple hv_Transition,
	HalconCpp::HTuple hv_Select,
	HalconCpp::HTuple hv_Row1,
	HalconCpp::HTuple hv_Column1,
	HalconCpp::HTuple hv_Row2,
	HalconCpp::HTuple hv_Column2,
	HalconCpp::HTuple * hv_ResultRow,
	HalconCpp::HTuple * hv_ResultColumn)

{

	// Local iconic variables
	HXLDCont  ho_RegionLines, ho_Rectangle, ho_Arrow1;

	// Local control variables
	Hlong  hv_Width, hv_Height;
	HTuple  hv_ATan; // 
	HTuple  hv_i, hv_RowC, hv_ColC, hv_Distance, hv_RowL2, hv_RowL1;
	HTuple  hv_ColL2, hv_ColL1,  hv_RowEdge;
	HTuple  hv_ColEdge, hv_Amplitude, hv_tRow, hv_tCol, hv_t;
	HTuple  hv_Number, hv_j;
	//HTuple hv_MsrHandle_Measure;
	HMeasure hv_Measure;

	//读取图像尺寸
	ho_Image.GetImageSize(&hv_Width, &hv_Height);
	//GetImageSize(ho_Image, &hv_Width, &hv_Height);
	//产生一个空显示对象，用于显示
	ho_Regions->GenEmptyObj();
	//GenEmptyObj(&(*ho_Regions));
	//初始化边缘坐标数组
	(*hv_ResultRow) = HTuple();
	(*hv_ResultColumn) = HTuple();
	//产生直线xld
	ho_RegionLines.GenContourPolygonXld(hv_Row1.TupleConcat(hv_Row2), hv_Column1.TupleConcat(hv_Column2));
	//GenContourPolygonXld(&ho_RegionLines, hv_Row1.TupleConcat(hv_Row2), hv_Column1.TupleConcat(hv_Column2));
	//存储到显示对象
	*ho_Regions = ho_Regions->ConcatObj(ho_RegionLines);
	//ConcatObj((*ho_Regions), ho_RegionLines, &(*ho_Regions));   //DispObj(*ho_Regions, this->hv_HWD); return;
	//计算直线与x轴的夹角，逆时针方向为正向
	AngleLx(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_ATan);  //write_message(200, 20, hv_ATan);
	//边缘检测方向垂直于检测直线， 直线方向正向旋转90度为边缘检测方向
	hv_ATan += HTuple(90).TupleRad();                            //write_message(200, 220, "90:" + hv_ATan);
	//根据检测直线按顺序产生测量区域矩形，并存储到显示对象
	{
		HTuple end_val16 = hv_Elements;
		HTuple step_val16 = 1;
		for (hv_i = 1; hv_i.Continue(end_val16, step_val16); hv_i += step_val16)
		{
			if (0 != (hv_Elements == 1))
			{
				hv_RowC = (hv_Row1 + hv_Row2)*0.5;
				hv_ColC = (hv_Column1 + hv_Column2)*0.5;
				//判断是否超出图像，超出不检测边缘
				if (0 != (HTuple(HTuple(HTuple(hv_RowC>(hv_Height - 1)).TupleOr(hv_RowC<0)).TupleOr(hv_ColC>(hv_Width - 1))).TupleOr(hv_ColC<0)))
				{
					continue;
				}
				DistancePp(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_Distance);
				hv_DetectWidth = hv_Distance;
				ho_Rectangle.GenRectangle2ContourXld(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_Distance / 2);
				//GenRectangle2ContourXld(&ho_Rectangle, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
				//	hv_Distance / 2);
			}
			else
			{
				//如果有多个测量矩形，产生该测量矩形xld
				hv_RowC = hv_Row1 + (((hv_Row2 - hv_Row1)*(hv_i - 1)) / (hv_Elements - 1));
				hv_ColC = hv_Column1 + (((hv_Column2 - hv_Column1)*(hv_i - 1)) / (hv_Elements - 1));
				if (0 != (HTuple(HTuple(HTuple(hv_RowC>(hv_Height - 1)).TupleOr(hv_RowC<0)).TupleOr(hv_ColC>(hv_Width - 1))).TupleOr(hv_ColC<0)))
				{
					continue;
				}

				//write_message(100, 12, "hv_RowC:" + hv_RowC);
				//write_message(150, 12, "hv_Col:" + hv_ColC);
				//write_message(200, 12, "hv_ATan:" + hv_ATan);

				//write_message(250, 12, "hv_DetectHeight:" + hv_DetectHeight);
				//write_message(300, 12, "hv_Distance:" + hv_Distance);

				ho_Rectangle.GenRectangle2ContourXld(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
					hv_DetectWidth / 2);   //DispObj(ho_Rectangle, this->hv_HWD); return;
				//GenRectangle2ContourXld(&ho_Rectangle, hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2,
				//	hv_DetectWidth / 2);   //DispObj(ho_Rectangle, this->hv_HWD); return;
			}
			//把测量矩形xld存储到显示对象
			*ho_Regions = ho_Regions->ConcatObj(ho_Rectangle);
			//ConcatObj((*ho_Regions), ho_Rectangle, &(*ho_Regions));
			if (0 != (hv_i == 1))
			{
				//在第一个测量矩形绘制一个箭头xld,用于只是边缘检测方向
				hv_RowL2 = hv_RowC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColC + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColC - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
					25);
				//把xld存储到显示对象
				*ho_Regions = ho_Regions->ConcatObj(ho_Arrow1);
				//ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
			}

			// DispObj(*ho_Regions, this->hv_HWD); return;

			//产生测量对象句柄
			hv_Measure.GenMeasureRectangle2(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
				hv_Width, hv_Height, (HString)"nearest_neighbor");
			//GenMeasureRectangle2(hv_RowC, hv_ColC, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
			//	hv_Width, hv_Height, "nearest_neighbor", &hv_MsrHandle_Measure);
			//设置极性
			if (0 != (hv_Transition == HTuple("negative")))
			{
				hv_Transition = "negative";
			}
			else
			{
				if (0 != (hv_Transition == HTuple("positive")))
				{
					hv_Transition = "positive";
				}
				else
				{
					hv_Transition = "all";
				}
			}
			//设置边缘位置，最强点是从所有边缘中选择幅度绝对值最大点，需要设置为'all'
			if (0 != (hv_Select == HTuple("first")))
			{
				hv_Select = "first";
			}
			else
			{
				if (0 != (hv_Select == HTuple("last")))
				{
					hv_Select = "last";
				}
				else
				{
					hv_Select = "all";
				}
			}

			hv_Measure.MeasurePos(ho_Image, hv_Sigma, hv_Threshold, hv_Transition,
				hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);

			//检测边缘
			//MeasurePos(ho_Image, hv_MsrHandle_Measure, hv_Sigma, hv_Threshold, hv_Transition,
			//	hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);
			
				
			//清除测量对象句柄
			//CloseMeasure(hv_MsrHandle_Measure);


			//临时变量初始化
			//tRow,tCol保存找到指定边缘的坐标
			hv_tRow = 0;
			hv_tCol = 0;
			//t保存边缘的幅度绝对值
			hv_t = 0;
			//找到的边缘必须至少为1个
			TupleLength(hv_RowEdge, &hv_Number);
			if (0 != (hv_Number<1))
			{
				continue;
			}
			//有多个边缘时，选择幅度绝对值最大的边缘
			{
				HTuple end_val88 = hv_Number - 1;
				HTuple step_val88 = 1;
				for (hv_j = 0; hv_j.Continue(end_val88, step_val88); hv_j += step_val88)
				{
					if (0 != ((HTuple(hv_Amplitude[hv_j]).TupleAbs())>hv_t))
					{
						hv_tRow = ((const HTuple&)hv_RowEdge)[hv_j];
						hv_tCol = ((const HTuple&)hv_ColEdge)[hv_j];
						hv_t = HTuple(hv_Amplitude[hv_j]).TupleAbs();
					}
				}
			}
			//把找到的边缘保存在输出数组
			if (0 != (hv_t>0))
			{
				(*hv_ResultRow) = (*hv_ResultRow).TupleConcat(hv_tRow);
				(*hv_ResultColumn) = (*hv_ResultColumn).TupleConcat(hv_tCol);
			}
		}
	}
	return;
}

void ST_MYDISP::pts_to_best_line
(
	HalconCpp::HXLDCont * ho_Line,
	HalconCpp::HTuple hv_Rows,
	HalconCpp::HTuple hv_Cols,
	HalconCpp::HTuple hv_ActiveNum,
	HalconCpp::HTuple * hv_Row1,
	HalconCpp::HTuple * hv_Column1,
	HalconCpp::HTuple * hv_Row2,
	HalconCpp::HTuple * hv_Column2)

{

	// Local iconic variables
	HObject  ho_Contour;

	// Local control variables
	HTuple  hv_Length, hv_Nr, hv_Nc, hv_Dist, hv_Length1;

	//初始化
	(*hv_Row1) = 0;
	(*hv_Column1) = 0;
	(*hv_Row2) = 0;
	(*hv_Column2) = 0;
	//产生一个空的直线对象
	GenEmptyObj(&(*ho_Line));
	//计算边缘数量
	TupleLength(hv_Cols, &hv_Length);
	//当边缘数量不小于有效点数时进行拟合
	if (0 != (HTuple(hv_Length >= hv_ActiveNum).TupleAnd(hv_ActiveNum>1)))
	{
		//halcon的拟合是基于xld的，需要把边缘连接成xld
		GenContourPolygonXld(&ho_Contour, hv_Rows, hv_Cols);
		//拟合直线，使用的算法是'tukey',其它算法请参考fit_line_contour_xld的描述部分
		FitLineContourXld(ho_Contour, "tukey", -1, 0, 5, 2, &(*hv_Row1), &(*hv_Column1),
			&(*hv_Row2), &(*hv_Column2), &hv_Nr, &hv_Nc, &hv_Dist);
		//判断拟合直线是否效，如果拟合成功，数组中元素的数量大于0
		TupleLength(hv_Dist, &hv_Length1);
		if (0 != (hv_Length<1))
		{
			return;
		}
		//根据拟合结果，产生直线xld
		GenContourPolygonXld(&(*ho_Line), (*hv_Row1).TupleConcat((*hv_Row2)), (*hv_Column1).TupleConcat((*hv_Column2)));
	}
	return;
}

void ST_MYDISP::draw_spoke(UCI::OptionsMap * o, HalconCpp::HXLDCont &Cont)
{
	
	try {

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

		HTuple hv_RowXLD, hv_ColXLD;
		ho_ContCircle.GetContourXld(&hv_RowXLD, &hv_ColXLD);

		//如果xld是圆弧，有Length2个点，从起点开始，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点
		//如果xld是圆，有Length2个点，从0度为起点，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点

		int elements = (*o)[L"Spoke_Circle_Elements"];
		double detect_height = (*o)[L"Spoke_Circle_Detect_Height"];
		double detect_width = (*o)[L"Spoke_Circle_Detect_Width"];
		//得到圆或圆弧上点的数量
		HTuple hv_Length2 = hv_ColXLD.Length();

		HTuple end_val42 = elements - 1;
		HTuple step_val42 = 1;
		HTuple hv_i, hv_j;
		HTuple hv_RowE, hv_ColE;
		HTuple hv_ATan;

		for (hv_i = 0; hv_i.Continue(end_val42, step_val42); hv_i += step_val42)
		{

			if (0 != (HTuple(hv_RowXLD[0]) == HTuple(hv_RowXLD[hv_Length2 - 1])))
			{
				//xld的起点和终点坐标相对，为圆
				TupleInt(((1.0*hv_Length2) / elements)*hv_i, &hv_j);

				(*o)[L"HTupleArcOrCircle"] = 0;
			}
			else
			{
				//否则为圆弧
				TupleInt(((1.0*hv_Length2) / (elements - 1))*hv_i, &hv_j);

				(*o)[L"HTupleArcOrCircle"] = 1;
			}
			// 索引越界,强制赋值为最后一个索引
			if (0 != (hv_j >= hv_Length2))
			{
				hv_j = hv_Length2 - 1;
				//	continue;
			}
			//获取卡尺中心
			hv_RowE = ((const HTuple&)hv_RowXLD)[hv_j];
			hv_ColE = ((const HTuple&)hv_ColXLD)[hv_j];


			//产生卡尺xld,并保持到显示对象
			HXLDCont ho_Rectangle1;
			ho_Rectangle1.GenRectangle2ContourXld(hv_RowE, hv_ColE, hv_ATan, HTuple(detect_height / 2),
				HTuple(detect_width / 2));

			Cont = Cont.ConcatObj(ho_Rectangle1);

			if (0 != (hv_i == 0))
			{
				HTuple hv_RowL2 = hv_RowE + ((detect_height / 2)*((-hv_ATan).TupleSin()));
				HTuple hv_RowL1 = hv_RowE - ((detect_height / 2)*((-hv_ATan).TupleSin()));
				HTuple hv_ColL2 = hv_ColE + ((detect_height / 2)*((-hv_ATan).TupleCos()));
				HTuple hv_ColL1 = hv_ColE - ((detect_height / 2)*((-hv_ATan).TupleCos()));

				HXLDCont ho_Arrow1;
				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
					25);
				Cont = Cont.ConcatObj(ho_Arrow1);
			}
		}
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"draw_spoke()：");

	}
	

}



// 交互画圆周
void ST_MYDISP::draw_spoke(
	HalconCpp::HXLDCont * ho_Regions,
	HalconCpp::HTuple hv_Elements,
	HalconCpp::HTuple hv_DetectHeight,
	HalconCpp::HTuple hv_DetectWidth,
	HalconCpp::HTuple * hv_ROIRows,
	HalconCpp::HTuple * hv_ROICols,
	HalconCpp::HTuple * hv_Direct)

{

	// Local iconic variables
	HXLDCont  ho_ContOut1, ho_Contour, ho_ContCircle;
	HXLDCont  ho_Cross, ho_Rectangle1, ho_Arrow1;

	// Local control variables
	HTuple  hv_Rows, hv_Cols, hv_Weights, hv_Length1;
	HTuple  hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi, hv_EndPhi;
	HTuple  hv_PointOrder, hv_RowXLD, hv_ColXLD;
	double	hv_Row1, hv_Column1;
	double  hv_Row2, hv_Column2;
	HTuple  hv_DistanceStart, hv_DistanceEnd;
	HTuple  hv_Length2, hv_i, hv_j, hv_RowE, hv_ColE, hv_ATan;
	HTuple  hv_RowL2, hv_RowL1, hv_ColL2, hv_ColL1;

	write_message(12, 12, "1.画4个以上的点确定一个圆弧，右键确认", true);
	ho_Regions->GenEmptyObj();
	//GenEmptyObj(&(*ho_Regions));
	//DrawNurbs(&ho_ContOut1, hv_WindowHandle, "true", "true", "true", "true", 3, &hv_Rows,
	//	&hv_Cols, &hv_Weights);

	ho_ContOut1 = this->m_pMHWinMain->DrawNurbs("true", "true", "true", "true", 3, &hv_Rows,
		&hv_Cols, &hv_Weights);

	TupleLength(hv_Weights, &hv_Length1);
	if (0 != (hv_Length1<4))
	{
		write_message(62, 12, "提示，点数太少，请重画", true);
		(*hv_ROIRows) = HTuple();
		(*hv_ROICols) = HTuple();
		return;
	}
	(*hv_ROIRows) = hv_Rows;
	(*hv_ROICols) = hv_Cols;
	ho_Contour.GenContourPolygonXld((*hv_ROIRows), (*hv_ROICols));
	//GenContourPolygonXld(&ho_Contour, (*hv_ROIRows), (*hv_ROICols));
	ho_Contour.FitCircleContourXld("algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	//FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
	//	&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	ho_ContCircle.GenCircleContourXld(hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
		hv_EndPhi, hv_PointOrder, 3);
	//GenCircleContourXld(&ho_ContCircle, hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
	//	hv_EndPhi, hv_PointOrder, 3);
	*ho_Regions = ho_Regions->ConcatObj(ho_ContCircle);
	//ConcatObj((*ho_Regions), ho_ContCircle, &(*ho_Regions));
	ho_ContCircle.GetContourXld(&hv_RowXLD, &hv_ColXLD);
	// GetContourXld(ho_ContCircle, &hv_RowXLD, &hv_ColXLD);

	//this->m_pMHWinMain->DispObj(ho_Image);
	//if (HDevWindowStack::IsOpen())
	//	DispObj(ho_Image, HDevWindowStack::GetActive());
	this->m_pMHWinMain->DispObj(ho_ContCircle);
	//if (HDevWindowStack::IsOpen())
	//	DispObj(ho_ContCircle, HDevWindowStack::GetActive());
	ho_Cross.GenCrossContourXld(hv_RowC, hv_ColumnC, HTuple(60), 0.785398);
	//GenCrossContourXld(&ho_Cross, hv_RowC, hv_ColumnC, 60, 0.785398);
	this->m_pMHWinMain->DispObj(ho_Cross);
	//if (HDevWindowStack::IsOpen())
	//	DispObj(ho_Cross, HDevWindowStack::GetActive());

	write_message(12, 12, "2) 远离圆心，画箭头确定边缘检测方向，右键确认",true);
	//画线确认检测方向
	this->m_pMHWinMain->DrawLine(&hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);

	DistancePp(hv_RowC, hv_ColumnC, hv_Row1, hv_Column1, &hv_DistanceStart);
	//求圆心到检测方向直线终点的距离
	DistancePp(hv_RowC, hv_ColumnC, hv_Row2, hv_Column2, &hv_DistanceEnd);
	//得到圆或圆弧上点的数量
	TupleLength(hv_ColXLD, &hv_Length2);

	if (0 != (hv_Elements<3))
	{
		(*hv_ROIRows) = HTuple();
		(*hv_ROICols) = HTuple();
		write_message(62, 12, "检测的边缘数量太少，请重新设置", true);
		return;
	}
	//如果xld是圆弧，有Length2个点，从起点开始，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点
	//如果xld是圆，有Length2个点，从0度为起点，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点
	{
		HTuple end_val42 = hv_Elements - 1;
		HTuple step_val42 = 1;
		for (hv_i = 0; hv_i.Continue(end_val42, step_val42); hv_i += step_val42)
		{

			if (0 != (HTuple(hv_RowXLD[0]) == HTuple(hv_RowXLD[hv_Length2 - 1])))
			{
				//xld的起点和终点坐标相对，为圆
				TupleInt(((1.0*hv_Length2) / hv_Elements)*hv_i, &hv_j);
			}
			else
			{
				//否则为圆弧
				TupleInt(((1.0*hv_Length2) / (hv_Elements - 1))*hv_i, &hv_j);
			}
			// 索引越界,强制赋值为最后一个索引
			if (0 != (hv_j >= hv_Length2))
			{
				hv_j = hv_Length2 - 1;
		  	   //	continue;
			}
			//获取卡尺中心
			hv_RowE = ((const HTuple&)hv_RowXLD)[hv_j];
			hv_ColE = ((const HTuple&)hv_ColXLD)[hv_j];

			//如果圆心到检测方向直线的起点距离大于圆心到检测方向直线的终点的距离，搜索方向由圆心指向圆心
			if (0 != (hv_DistanceStart>hv_DistanceEnd))
			{
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
				hv_ATan = (HTuple(180).TupleRad()) + hv_ATan;
				(*hv_Direct) = "inner";
			}
			else
			{
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
				(*hv_Direct) = "outer";
			}
			//产生卡尺xld,并保持到显示对象
			ho_Rectangle1.GenRectangle2ContourXld(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
				hv_DetectWidth / 2);
			//GenRectangle2ContourXld(&ho_Rectangle1, hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
			//	hv_DetectWidth / 2);
			*ho_Regions = ho_Regions->ConcatObj(ho_Rectangle1);
			//ConcatObj((*ho_Regions), ho_Rectangle1, &(*ho_Regions));

			if (0 != (hv_i == 0))
			{
				hv_RowL2 = hv_RowE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
					25);
				*ho_Regions = ho_Regions->ConcatObj(ho_Arrow1);
				//ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
			}
		}
	}
	return;
}

//void ST_MYDISP::draw_spoke(
//	HalconCpp::HXLDCont * ho_Regions, 
//	HalconCpp::HTuple hv_Elements, 
//	HalconCpp::HTuple hv_DetectHeight, 
//	HalconCpp::HTuple hv_DetectWidth, 
//	HalconCpp::HTuple * hv_ROIRows, 
//	HalconCpp::HTuple * hv_ROICols, 
//	HalconCpp::HTuple * hv_Direct, 
//	HalconCpp::HTuple * hvv_Radius, 
//	HalconCpp::HTuple * hvv_StartPhi, 
//	HalconCpp::HTuple * hvv_EndPhi, 
//	HalconCpp::HTuple * hvv_PointOrder)
//{
//	// Local iconic variables
//	HXLDCont  ho_ContOut1, ho_Contour, ho_ContCircle;
//	HXLDCont  ho_Cross, ho_Rectangle1, ho_Arrow1;
//
//	// Local control variables
//	HTuple  hv_Rows, hv_Cols, hv_Weights, hv_Length1;
//	HTuple  hv_RowC, hv_ColumnC;
//
//	//HTuple  hv_Radius, hv_StartPhi, hv_EndPhi;
//	//HTuple  hv_PointOrder;
//
//	HTuple  hv_RowXLD, hv_ColXLD;
//	double	hv_Row1, hv_Column1;
//	double  hv_Row2, hv_Column2;
//	HTuple  hv_DistanceStart, hv_DistanceEnd;
//	HTuple  hv_Length2, hv_i, hv_j, hv_RowE, hv_ColE, hv_ATan;
//	HTuple  hv_RowL2, hv_RowL1, hv_ColL2, hv_ColL1;
//
//	write_message(12, 12, "1.画4个以上的点确定一个圆弧，右键确认", true);
//	ho_Regions->GenEmptyObj();
//	//GenEmptyObj(&(*ho_Regions));
//	//DrawNurbs(&ho_ContOut1, hv_WindowHandle, "true", "true", "true", "true", 3, &hv_Rows,
//	//	&hv_Cols, &hv_Weights);
//
//	ho_ContOut1 = this->m_pMHWinMain->DrawNurbs("true", "true", "true", "true", 3, &hv_Rows,
//		&hv_Cols, &hv_Weights);
//
//	TupleLength(hv_Weights, &hv_Length1);
//	if (0 != (hv_Length1<4))
//	{
//		write_message(62, 12, "提示，点数太少，请重画", true);
//		(*hv_ROIRows) = HTuple();
//		(*hv_ROICols) = HTuple();
//		return;
//	}
//	(*hv_ROIRows) = hv_Rows;
//	(*hv_ROICols) = hv_Cols;
//	ho_Contour.GenContourPolygonXld((*hv_ROIRows), (*hv_ROICols));
//	//GenContourPolygonXld(&ho_Contour, (*hv_ROIRows), (*hv_ROICols));
//	ho_Contour.FitCircleContourXld("algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
//		hvv_Radius, hvv_StartPhi, hvv_EndPhi, hvv_PointOrder);
//	//FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
//	//	&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
//	ho_ContCircle.GenCircleContourXld(hv_RowC, hv_ColumnC, *hvv_Radius, *hvv_StartPhi,
//		*hvv_EndPhi, *hvv_PointOrder, 3);
//
//	// 再保存一下
//	(*hv_ROIRows) = hv_RowC;
//	(*hv_ROICols) = hv_ColumnC;
//
//	//GenCircleContourXld(&ho_ContCircle, hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
//	//	hv_EndPhi, hv_PointOrder, 3);
//	*ho_Regions = ho_Regions->ConcatObj(ho_ContCircle);
//	//ConcatObj((*ho_Regions), ho_ContCircle, &(*ho_Regions));
//	ho_ContCircle.GetContourXld(&hv_RowXLD, &hv_ColXLD);
//	// GetContourXld(ho_ContCircle, &hv_RowXLD, &hv_ColXLD);
//
//	//this->m_pMHWinMain->DispObj(ho_Image);
//	//if (HDevWindowStack::IsOpen())
//	//	DispObj(ho_Image, HDevWindowStack::GetActive());
//	this->m_pMHWinMain->DispObj(ho_ContCircle);
//	//if (HDevWindowStack::IsOpen())
//	//	DispObj(ho_ContCircle, HDevWindowStack::GetActive());
//	ho_Cross.GenCrossContourXld(hv_RowC, hv_ColumnC, HTuple(60), 0.785398);
//	//GenCrossContourXld(&ho_Cross, hv_RowC, hv_ColumnC, 60, 0.785398);
//	this->m_pMHWinMain->DispObj(ho_Cross);
//	//if (HDevWindowStack::IsOpen())
//	//	DispObj(ho_Cross, HDevWindowStack::GetActive());
//
//	write_message(12, 12, "2) 远离圆心，画箭头确定边缘检测方向，右键确认", true);
//	//画线确认检测方向
//	this->m_pMHWinMain->DrawLine(&hv_Row1, &hv_Column1, &hv_Row2, &hv_Column2);
//
//	DistancePp(hv_RowC, hv_ColumnC, hv_Row1, hv_Column1, &hv_DistanceStart);
//	//求圆心到检测方向直线终点的距离
//	DistancePp(hv_RowC, hv_ColumnC, hv_Row2, hv_Column2, &hv_DistanceEnd);
//	//得到圆或圆弧上点的数量
//	TupleLength(hv_ColXLD, &hv_Length2);
//
//	if (0 != (hv_Elements<3))
//	{
//		(*hv_ROIRows) = HTuple();
//		(*hv_ROICols) = HTuple();
//		write_message(62, 12, "检测的边缘数量太少，请重新设置", true);
//		return;
//	}
//	//如果xld是圆弧，有Length2个点，从起点开始，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点
//	//如果xld是圆，有Length2个点，从0度为起点，等间距（间距为Length2/(Element2-1),取ELements个点，作为卡尺工具的中点
//	{
//		HTuple end_val42 = hv_Elements - 1;
//		HTuple step_val42 = 1;
//		for (hv_i = 0; hv_i.Continue(end_val42, step_val42); hv_i += step_val42)
//		{
//
//			if (0 != (HTuple(hv_RowXLD[0]) == HTuple(hv_RowXLD[hv_Length2 - 1])))
//			{
//				//xld的起点和终点坐标相对，为圆
//				TupleInt(((1.0*hv_Length2) / hv_Elements)*hv_i, &hv_j);
//			}
//			else
//			{
//				//否则为圆弧
//				TupleInt(((1.0*hv_Length2) / (hv_Elements - 1))*hv_i, &hv_j);
//			}
//			// 索引越界,强制赋值为最后一个索引
//			if (0 != (hv_j >= hv_Length2))
//			{
//				hv_j = hv_Length2 - 1;
//				//	continue;
//			}
//			//获取卡尺中心
//			hv_RowE = ((const HTuple&)hv_RowXLD)[hv_j];
//			hv_ColE = ((const HTuple&)hv_ColXLD)[hv_j];
//
//			//如果圆心到检测方向直线的起点距离大于圆心到检测方向直线的终点的距离，搜索方向由圆心指向圆心
//			if (0 != (hv_DistanceStart>hv_DistanceEnd))
//			{
//				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
//				hv_ATan = (HTuple(180).TupleRad()) + hv_ATan;
//				(*hv_Direct) = "inner";
//			}
//			else
//			{
//				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
//				(*hv_Direct) = "outer";
//			}
//			//产生卡尺xld,并保持到显示对象
//			ho_Rectangle1.GenRectangle2ContourXld(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
//				hv_DetectWidth / 2);
//			//GenRectangle2ContourXld(&ho_Rectangle1, hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
//			//	hv_DetectWidth / 2);
//			*ho_Regions = ho_Regions->ConcatObj(ho_Rectangle1);
//			//ConcatObj((*ho_Regions), ho_Rectangle1, &(*ho_Regions));
//
//			if (0 != (hv_i == 0))
//			{
//				hv_RowL2 = hv_RowE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
//				hv_RowL1 = hv_RowE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
//				hv_ColL2 = hv_ColE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
//				hv_ColL1 = hv_ColE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
//				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
//					25);
//				*ho_Regions = ho_Regions->ConcatObj(ho_Arrow1);
//				//ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
//			}
//		}
//	}
//	return;
//}

void ST_MYDISP::spoke(
	HalconCpp::HImage ho_Image,
	HalconCpp::HXLDCont * ho_Regions,
	//HalconCpp::HTuple hv_WindowHandle,
	HalconCpp::HTuple hv_Elements,
	HalconCpp::HTuple hv_DetectHeight,
	HalconCpp::HTuple hv_DetectWidth,
	HalconCpp::HTuple hv_Sigma,
	HalconCpp::HTuple hv_Threshold,
	HalconCpp::HTuple hv_Transition,
	HalconCpp::HTuple hv_Select,
	HalconCpp::HTuple hv_ROIRows,
	HalconCpp::HTuple hv_ROICols,
	HalconCpp::HTuple hv_Direct,
	HalconCpp::HTuple * hv_ResultRow,
	HalconCpp::HTuple * hv_ResultColumn,
	HalconCpp::HTuple * hv_ArcType)

{

	// Local iconic variables
	HXLDCont  ho_Contour, ho_ContCircle, ho_Rectangle1;
	HXLDCont  ho_Arrow1;

	// Local control variables
	HTuple  hv_Width, hv_Height, hv_RowC, hv_ColumnC;
	HTuple  hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;
	HTuple  hv_RowXLD, hv_ColXLD, hv_Length2, hv_i, hv_j, hv_RowE;
	HTuple  hv_ColE, hv_ATan, hv_RowL2, hv_RowL1, hv_ColL2;
	HTuple  hv_ColL1, hv_RowEdge, hv_ColEdge;
	HTuple  hv_Amplitude, hv_Distance, hv_tRow, hv_tCol, hv_t;
	HTuple  hv_Number, hv_k;

	HMeasure hv_Measure;

	ho_Image.GetImageSize(&hv_Width, &hv_Height);
	//GetImageSize(ho_Image, &hv_Width, &hv_Height);
	ho_Regions->GenEmptyObj();
	//GenEmptyObj(ho_Regions);

	(*hv_ResultRow) = HTuple();
	(*hv_ResultColumn) = HTuple();

	ho_Contour.GenContourPolygonXld(hv_ROIRows, hv_ROICols);
	//GenContourPolygonXld(&ho_Contour, hv_ROIRows, hv_ROICols);
	ho_Contour.FitCircleContourXld("algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
		&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	//FitCircleContourXld(ho_Contour, "algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
	//	&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);
	ho_ContCircle.GenCircleContourXld(hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
		hv_EndPhi, hv_PointOrder, 3);
	//GenCircleContourXld(&ho_ContCircle, hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
	//	hv_EndPhi, hv_PointOrder, 3);
	*ho_Regions = ho_Regions->ConcatObj(ho_ContCircle);
	//ConcatObj((*ho_Regions), ho_ContCircle, &(*ho_Regions));
	// 获取圆或圆弧 xld 上的点坐标
	ho_ContCircle.GetContourXld(&hv_RowXLD, &hv_ColXLD);
	//GetContourXld(ho_ContCircle, &hv_RowXLD, &hv_ColXLD);
	// 求圆或圆弧上点的数量
	TupleLength(hv_ColXLD, &hv_Length2);

	if (0 != (hv_Elements<3))
	{
		this->write_message(62, 12, "检测边缘数量太少,请重新设置", true);
		return;
	}

	{
		HTuple end_val20 = hv_Elements - 1;
		HTuple step_val20 = 1;
		for (hv_i = 0; hv_i.Continue(end_val20, step_val20); hv_i += step_val20)
		{
			if (0 != (HTuple(hv_RowXLD[0]) == HTuple(hv_RowXLD[hv_Length2 - 1])))
			{
				// xld 的起点和终点坐标相同为圆
				TupleInt(((1.0*hv_Length2) / hv_Elements)*hv_i, &hv_j);
				(*hv_ArcType) = "circle";
			}
			else
			{
				// 否则为圆弧
				TupleInt(((1.0*hv_Length2) / (hv_Elements - 1))*hv_i, &hv_j);
				(*hv_ArcType) = "arc";
			}
			// 索引越界, 强制赋值为最后一个索引
			if (0 != (hv_j >= hv_Length2))
			{
				hv_j = hv_Length2 - 1;
				//continue;
			}
			// 获取卡尺工具中心
			hv_RowE = ((const HTuple&)hv_RowXLD)[hv_j];
			hv_ColE = ((const HTuple&)hv_ColXLD)[hv_j];

			// 超出图像区域,不检测,否则容易报异常
			if (0 != (HTuple(HTuple(HTuple(hv_RowE>(hv_Height - 1)).TupleOr(hv_RowE<0)).TupleOr(hv_ColE>(hv_Width - 1))).TupleOr(hv_ColE<0)))
			{
				continue;
			}
			// 边缘搜索方向类型: inner 搜索方向由圆外指向圆心, outer 搜索方向由圆心指向圆外
			if (0 != (hv_Direct == HTuple("inner")))
			{
				// 求卡尺工具的边缘搜索方向
				// 求圆心指向边缘的矢量的角度
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
				hv_ATan = (HTuple(180).TupleRad()) + hv_ATan;
			}
			else
			{
				TupleAtan2((-hv_RowE) + hv_RowC, hv_ColE - hv_ColumnC, &hv_ATan);
			}

			//this->write_message(20, 20, hv_RowE);
			//this->write_message(80, 20, hv_ColE);
			//this->write_message(140, 20, hv_ATan);
			// 产生卡尺 xld, 并保持到显示对象
			ho_Rectangle1.GenRectangle2ContourXld(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
				hv_DetectWidth / 2);
			//GenRectangle2ContourXld(&ho_Rectangle1, hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2,
			//	hv_DetectWidth / 2);
			*ho_Regions = ho_Regions->ConcatObj(ho_Rectangle1);
			//ConcatObj((*ho_Regions), ho_Rectangle1, &(*ho_Regions));

			// 用箭头 xld 指示边缘搜索方向, 并保持到显示对象
			if (0 != (hv_i == 0))
			{
				hv_RowL2 = hv_RowE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_RowL1 = hv_RowE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleSin()));
				hv_ColL2 = hv_ColE + ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));
				hv_ColL1 = hv_ColE - ((hv_DetectHeight / 2)*((-hv_ATan).TupleCos()));

				gen_arrow_contour_xld(&ho_Arrow1, hv_RowL1, hv_ColL1, hv_RowL2, hv_ColL2, 25,
					25);
				*ho_Regions = ho_Regions->ConcatObj(ho_Arrow1);
				//ConcatObj((*ho_Regions), ho_Arrow1, &(*ho_Regions));
			}

			//产生测量对象句柄
			hv_Measure.GenMeasureRectangle2(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
				hv_Width, hv_Height, (HString)"nearest_neighbor");
			//GenMeasureRectangle2(hv_RowE, hv_ColE, hv_ATan, hv_DetectHeight / 2, hv_DetectWidth / 2,
			//	hv_Width, hv_Height, "nearest_neighbor", &hv_MrsHandle_Measure);

			if (0 != (hv_Transition == HTuple("negative")))
			{
				hv_Transition = "negative";
			}
			else
			{
				if (0 != (hv_Transition == HTuple("positive")))
				{
					hv_Transition = "positive";
				}
				else
				{
					hv_Transition = "all";
				}
			}

			if (0 != (hv_Select == HTuple("first")))
			{
				hv_Select = "first";
			}
			else
			{
				if (0 != (hv_Select == HTuple("last")))
				{
					hv_Select = "last";
				}
				else
				{
					hv_Select = "all";
				}
			}
			//检测边缘
			hv_Measure.MeasurePos(ho_Image, hv_Sigma, hv_Threshold, hv_Transition,
				hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);
			//MeasurePos(ho_Image, hv_MrsHandle_Measure, hv_Sigma, hv_Threshold, hv_Transition,
			//	hv_Select, &hv_RowEdge, &hv_ColEdge, &hv_Amplitude, &hv_Distance);
			//CloseMeasure(hv_MrsHandle_Measure);
			hv_tRow = 0;
			hv_tCol = 0;
			hv_t = 0;
			TupleLength(hv_RowEdge, &hv_Number);
			if (0 != (hv_Number<1))
			{
				continue;
			}
			//有多个边缘时，选择幅度绝对值最大的边缘
			{
				HTuple end_val93 = hv_Number - 1;
				HTuple step_val93 = 1;
				for (hv_k = 0; hv_k.Continue(end_val93, step_val93); hv_k += step_val93)
				{
					if (0 != ((HTuple(hv_Amplitude[hv_k]).TupleAbs())>hv_t))
					{
						hv_tRow = ((const HTuple&)hv_RowEdge)[hv_k];
						hv_tCol = ((const HTuple&)hv_ColEdge)[hv_k];
						hv_t = HTuple(hv_Amplitude[hv_k]).TupleAbs();
					}
				}
			}
			//把找到的边缘保存在输出数组
			if (0 != (hv_t>0))
			{
				(*hv_ResultRow) = (*hv_ResultRow).TupleConcat(hv_tRow);
				(*hv_ResultColumn) = (*hv_ResultColumn).TupleConcat(hv_tCol);
			}
		}
	}
	return;
}

void ST_MYDISP::pts_to_best_circle(
	HalconCpp::HXLDCont * ho_Circle,
	HalconCpp::HTuple hv_Rows,
	HalconCpp::HTuple hv_Cols,
	HalconCpp::HTuple hv_ActiveNum,
	HalconCpp::HTuple hv_ArcType,
	HalconCpp::HTuple * hv_RowCenter,
	HalconCpp::HTuple * hv_ColCenter,
	HalconCpp::HTuple * hv_Radius,
	HalconCpp::HTuple * hv_StartPhi,
	HalconCpp::HTuple * hv_EndPhi,
	HalconCpp::HTuple * hv_PointOrder)

{

	// Local iconic variables
	HXLDCont  ho_Contour;

	// Local control variables
	HTuple  hv_Length, hv_Length1;

	//初始化
	(*hv_RowCenter) = 0;
	(*hv_ColCenter) = 0;
	(*hv_Radius) = 0;
	ho_Circle->GenEmptyObj();
	//GenEmptyObj(&(*ho_Circle));
	TupleLength(hv_Cols, &hv_Length);

	if (0 != (HTuple(hv_Length >= hv_ActiveNum).TupleAnd(hv_ActiveNum>2)))
	{
		if (0 != (hv_ArcType == HTuple("circle")))
		{
			ho_Contour.GenContourPolygonXld(hv_Rows.TupleConcat(HTuple(hv_Rows[0])),
				hv_Cols.TupleConcat(HTuple(hv_Cols[0])));
			//GenContourPolygonXld(&ho_Contour, hv_Rows.TupleConcat(HTuple(hv_Rows[0])),
			//	hv_Cols.TupleConcat(HTuple(hv_Cols[0])));
		}
		else
		{
			ho_Contour.GenContourPolygonXld(hv_Rows, hv_Cols);
			//GenContourPolygonXld(&ho_Contour, hv_Rows, hv_Cols);
		}
		ho_Contour.FitCircleContourXld("geotukey", -1, 0, 0, 3, 2, &(*hv_RowCenter),
			&(*hv_ColCenter), &(*hv_Radius), &(*hv_StartPhi), &(*hv_EndPhi), &(*hv_PointOrder));
		//FitCircleContourXld(ho_Contour, "geotukey", -1, 0, 0, 3, 2, &(*hv_RowCenter),
		//	&(*hv_ColCenter), &(*hv_Radius), &(*hv_StartPhi), &(*hv_EndPhi), &(*hv_PointOrder));
		TupleLength((*hv_StartPhi), &hv_Length1);
		if (0 != (hv_Length1<1))
		{
			return;
		}
		//根据拟合结果，产生圆xld
		if (0 != (hv_ArcType == HTuple("arc")))
		{
			ho_Circle->GenCircleContourXld((*hv_RowCenter), (*hv_ColCenter), (*hv_Radius),
				(*hv_StartPhi), (*hv_EndPhi), (*hv_PointOrder), 1);
			//GenCircleContourXld(&(*ho_Circle), (*hv_RowCenter), (*hv_ColCenter), (*hv_Radius),
			//	(*hv_StartPhi), (*hv_EndPhi), (*hv_PointOrder), 1);
		}
		else
		{
			(*hv_StartPhi) = 0;
			(*hv_EndPhi) = HTuple(360).TupleRad();
			ho_Circle->GenCircleContourXld((*hv_RowCenter), (*hv_ColCenter), (*hv_Radius),
				(*hv_StartPhi), (*hv_EndPhi), (*hv_PointOrder), 1);
			//GenCircleContourXld(&(*ho_Circle), (*hv_RowCenter), (*hv_ColCenter), (*hv_Radius),
			//	(*hv_StartPhi), (*hv_EndPhi), (*hv_PointOrder), 1);
		}
	}
	return;
}


// 画剪头

void ST_MYDISP::gen_arrow_contour_xld(
	HalconCpp::HXLDCont * ho_Arrow,
	HalconCpp::HTuple hv_Row1,
	HalconCpp::HTuple hv_Column1,
	HalconCpp::HTuple hv_Row2,
	HalconCpp::HTuple hv_Column2,
	HalconCpp::HTuple hv_HeadLength,
	HalconCpp::HTuple hv_HeadWidth)

{

	// Local iconic variables
	HXLDCont  ho_TempArrow;

	// Local control variables
	HTuple  hv_Length, hv_ZeroLengthIndices, hv_DR;
	HTuple  hv_DC, hv_HalfHeadWidth, hv_RowP1, hv_ColP1, hv_RowP2;
	HTuple  hv_ColP2, hv_Index;

	//This procedure generates arrow shaped XLD contours,
	//pointing from (Row1, Column1) to (Row2, Column2).
	//If starting and end point are identical, a contour consisting
	//of a single point is returned.
	//
	//input parameteres:
	//Row1, Column1: Coordinates of the arrows' starting points
	//Row2, Column2: Coordinates of the arrows' end points
	//HeadLength, HeadWidth: Size of the arrow heads in pixels
	//
	//output parameter:
	//Arrow: The resulting XLD contour
	//
	//The input tuples Row1, Column1, Row2, and Column2 have to be of
	//the same length.
	//HeadLength and HeadWidth either have to be of the same length as
	//Row1, Column1, Row2, and Column2 or have to be a single element.
	//If one of the above restrictions is violated, an error will occur.
	//
	//
	//Init
	ho_Arrow->GenEmptyObj();
	//GenEmptyObj(&(*ho_Arrow));
	//
	//Calculate the arrow length
	DistancePp(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_Length);
	//
	//Mark arrows with identical start and end point
	//(set Length to -1 to avoid division-by-zero exception)
	hv_ZeroLengthIndices = hv_Length.TupleFind(0);
	if (0 != (hv_ZeroLengthIndices != -1))
	{
		hv_Length[hv_ZeroLengthIndices] = -1;
	}
	//
	//Calculate auxiliary variables.
	hv_DR = (1.0*(hv_Row2 - hv_Row1)) / hv_Length;
	hv_DC = (1.0*(hv_Column2 - hv_Column1)) / hv_Length;
	hv_HalfHeadWidth = hv_HeadWidth / 2.0;
	//
	//Calculate end points of the arrow head.
	hv_RowP1 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) + (hv_HalfHeadWidth*hv_DC);
	hv_ColP1 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) - (hv_HalfHeadWidth*hv_DR);
	hv_RowP2 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) - (hv_HalfHeadWidth*hv_DC);
	hv_ColP2 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) + (hv_HalfHeadWidth*hv_DR);
	//
	//Finally create output XLD contour for each input point pair
	{
		HTuple end_val45 = (hv_Length.TupleLength()) - 1;
		HTuple step_val45 = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val45, step_val45); hv_Index += step_val45)
		{
			if (0 != (HTuple(hv_Length[hv_Index]) == -1))
			{
				//Create_ single points for arrows with identical start and end point
				ho_TempArrow.GenContourPolygonXld(HTuple(hv_Row1[hv_Index]), HTuple(hv_Column1[hv_Index]));
				//GenContourPolygonXld(&ho_TempArrow, HTuple(hv_Row1[hv_Index]), HTuple(hv_Column1[hv_Index]));
			}
			else
			{
				//Create arrow contour
				ho_TempArrow.GenContourPolygonXld(((((HTuple(hv_Row1[hv_Index]).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP1[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP2[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index])),
					((((HTuple(hv_Column1[hv_Index]).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP1[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP2[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index])));
				//GenContourPolygonXld(&ho_TempArrow, ((((HTuple(hv_Row1[hv_Index]).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP1[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP2[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index])),
				//	((((HTuple(hv_Column1[hv_Index]).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP1[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP2[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index])));
			}
			*ho_Arrow = ho_Arrow->ConcatObj(ho_TempArrow);
			//ConcatObj((*ho_Arrow), ho_TempArrow, &(*ho_Arrow));
		}
	}
	return;
}

void ST_MYDISP::gen_double_arrow_contour_xld(
	HalconCpp::HXLDCont * ho_Arrow,
	HalconCpp::HTuple hv_Row1,
	HalconCpp::HTuple hv_Column1, 
	HalconCpp::HTuple hv_Row2, 
	HalconCpp::HTuple hv_Column2,
	HalconCpp::HTuple hv_HeadLength, 
	HalconCpp::HTuple hv_HeadWidth)
{
	// Local iconic variables
	HXLDCont  ho_TempArrow, ho2_TempArrow;

	// Local control variables
	HTuple  hv_Length, hv_ZeroLengthIndices, hv_DR;
	HTuple  hv_DC, hv_HalfHeadWidth, hv_RowP1, hv_ColP1, hv_RowP2;
	HTuple  hv_ColP2, hv_Index;
	
	HTuple hv2_RowP1, hv2_ColP1, hv2_RowP2, hv2_ColP2;    // 另一个点的箭头
	HTuple hv2_DR, hv2_DC;
	//
	//Init
	ho_Arrow->GenEmptyObj();
	//GenEmptyObj(&(*ho_Arrow));
	//
	//Calculate the arrow length
	DistancePp(hv_Row1, hv_Column1, hv_Row2, hv_Column2, &hv_Length);
	//
	//Mark arrows with identical start and end point
	//(set Length to -1 to avoid division-by-zero exception)
	hv_ZeroLengthIndices = hv_Length.TupleFind(0);
	if (0 != (hv_ZeroLengthIndices != -1))
	{
		hv_Length[hv_ZeroLengthIndices] = -1;
	}
	//
	//Calculate auxiliary variables.
	hv_DR = (1.0*(hv_Row2 - hv_Row1)) / hv_Length;
	hv_DC = (1.0*(hv_Column2 - hv_Column1)) / hv_Length;

	hv2_DR = (1.0*(hv_Row1 - hv_Row2)) / hv_Length;
	hv2_DC = (1.0*(hv_Column1 - hv_Column2)) / hv_Length;

	hv_HalfHeadWidth = hv_HeadWidth / 2.0;
	//
	//Calculate end points of the arrow head.
	hv_RowP1 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) + (hv_HalfHeadWidth*hv_DC);
	hv_ColP1 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) - (hv_HalfHeadWidth*hv_DR);
	hv_RowP2 = (hv_Row1 + ((hv_Length - hv_HeadLength)*hv_DR)) - (hv_HalfHeadWidth*hv_DC);
	hv_ColP2 = (hv_Column1 + ((hv_Length - hv_HeadLength)*hv_DC)) + (hv_HalfHeadWidth*hv_DR);

	hv2_RowP1 = (hv_Row2 + ((hv_Length - hv_HeadLength)*hv2_DR)) + (hv_HalfHeadWidth*hv2_DC);
	hv2_ColP1 = (hv_Column2 + ((hv_Length - hv_HeadLength)*hv2_DC)) - (hv_HalfHeadWidth*hv2_DR);
	hv2_RowP2 = (hv_Row2 + ((hv_Length - hv_HeadLength)*hv2_DR)) - (hv_HalfHeadWidth*hv2_DC);
	hv2_ColP2 = (hv_Column2 + ((hv_Length - hv_HeadLength)*hv2_DC)) + (hv_HalfHeadWidth*hv2_DR);

	//
	//Finally create output XLD contour for each input point pair
	{
		HTuple end_val45 = (hv_Length.TupleLength()) - 1;
		HTuple step_val45 = 1;
		for (hv_Index = 0; hv_Index.Continue(end_val45, step_val45); hv_Index += step_val45)
		{
			if (0 != (HTuple(hv_Length[hv_Index]) == -1))
			{
				//Create_ single points for arrows with identical start and end point
				ho_TempArrow.GenContourPolygonXld(HTuple(hv_Row1[hv_Index]), HTuple(hv_Column1[hv_Index]));
			}
			else
			{
				//Create arrow contour
				ho_TempArrow.GenContourPolygonXld(((((HTuple(hv_Row1[hv_Index]).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP1[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index]))).TupleConcat(HTuple(hv_RowP2[hv_Index]))).TupleConcat(HTuple(hv_Row2[hv_Index])),
					((((HTuple(hv_Column1[hv_Index]).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP1[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index]))).TupleConcat(HTuple(hv_ColP2[hv_Index]))).TupleConcat(HTuple(hv_Column2[hv_Index])));

				ho2_TempArrow.GenContourPolygonXld(((((HTuple(hv_Row2[hv_Index]).TupleConcat(HTuple(hv_Row1[hv_Index]))).TupleConcat(HTuple(hv2_RowP1[hv_Index]))).TupleConcat(HTuple(hv_Row1[hv_Index]))).TupleConcat(HTuple(hv2_RowP2[hv_Index]))).TupleConcat(HTuple(hv_Row1[hv_Index])),
					((((HTuple(hv_Column2[hv_Index]).TupleConcat(HTuple(hv_Column1[hv_Index]))).TupleConcat(HTuple(hv2_ColP1[hv_Index]))).TupleConcat(HTuple(hv_Column1[hv_Index]))).TupleConcat(HTuple(hv2_ColP2[hv_Index]))).TupleConcat(HTuple(hv_Column1[hv_Index])));

			}
			*ho_Arrow = ho_Arrow->ConcatObj(ho_TempArrow);
			*ho_Arrow = ho_Arrow->ConcatObj(ho2_TempArrow);
			//ConcatObj((*ho_Arrow), ho_TempArrow, &(*ho_Arrow));
		}
	}
	return;
}
