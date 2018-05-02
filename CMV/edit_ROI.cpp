#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"


using namespace HalconCpp;


// Procedures 
// External procedures 
// Chapter: Develop
// Short Description: Open a new graphics window that preserves the aspect ratio of the given image. 
void dev_open_window_fit_image(HObject ho_Image, HTuple hv_Row, HTuple hv_Column,
	HTuple hv_WidthLimit, HTuple hv_HeightLimit, HTuple *hv_WindowHandle)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_MinWidth, hv_MaxWidth, hv_MinHeight;
	HTuple  hv_MaxHeight, hv_ResizeFactor, hv_ImageWidth, hv_ImageHeight;
	HTuple  hv_TempWidth, hv_TempHeight, hv_WindowWidth, hv_WindowHeight;

	//This procedure opens a new graphics window and adjusts the size
	//such that it fits into the limits specified by WidthLimit
	//and HeightLimit, but also maintains the correct image aspect ratio.
	//
	//If it is impossible to match the minimum and maximum extent requirements
	//at the same time (f.e. if the image is very long but narrow),
	//the maximum value gets a higher priority,
	//
	//Parse input tuple WidthLimit
	if (0 != (HTuple((hv_WidthLimit.TupleLength()) == 0).TupleOr(hv_WidthLimit<0)))
	{
		hv_MinWidth = 500;
		hv_MaxWidth = 800;
	}
	else if (0 != ((hv_WidthLimit.TupleLength()) == 1))
	{
		hv_MinWidth = 0;
		hv_MaxWidth = hv_WidthLimit;
	}
	else
	{
		hv_MinWidth = ((const HTuple&)hv_WidthLimit)[0];
		hv_MaxWidth = ((const HTuple&)hv_WidthLimit)[1];
	}
	//Parse input tuple HeightLimit
	if (0 != (HTuple((hv_HeightLimit.TupleLength()) == 0).TupleOr(hv_HeightLimit<0)))
	{
		hv_MinHeight = 400;
		hv_MaxHeight = 600;
	}
	else if (0 != ((hv_HeightLimit.TupleLength()) == 1))
	{
		hv_MinHeight = 0;
		hv_MaxHeight = hv_HeightLimit;
	}
	else
	{
		hv_MinHeight = ((const HTuple&)hv_HeightLimit)[0];
		hv_MaxHeight = ((const HTuple&)hv_HeightLimit)[1];
	}
	//
	//Test, if window size has to be changed.
	hv_ResizeFactor = 1;
	GetImageSize(ho_Image, &hv_ImageWidth, &hv_ImageHeight);
	//First, expand window to the minimum extents (if necessary).
	if (0 != (HTuple(hv_MinWidth>hv_ImageWidth).TupleOr(hv_MinHeight>hv_ImageHeight)))
	{
		hv_ResizeFactor = (((hv_MinWidth.TupleReal()) / hv_ImageWidth).TupleConcat((hv_MinHeight.TupleReal()) / hv_ImageHeight)).TupleMax();
	}
	hv_TempWidth = hv_ImageWidth*hv_ResizeFactor;
	hv_TempHeight = hv_ImageHeight*hv_ResizeFactor;
	//Then, shrink window to maximum extents (if necessary).
	if (0 != (HTuple(hv_MaxWidth<hv_TempWidth).TupleOr(hv_MaxHeight<hv_TempHeight)))
	{
		hv_ResizeFactor = hv_ResizeFactor*((((hv_MaxWidth.TupleReal()) / hv_TempWidth).TupleConcat((hv_MaxHeight.TupleReal()) / hv_TempHeight)).TupleMin());
	}
	hv_WindowWidth = hv_ImageWidth*hv_ResizeFactor;
	hv_WindowHeight = hv_ImageHeight*hv_ResizeFactor;
	//Resize window
	SetWindowAttr("background_color", "black");
	OpenWindow(hv_Row, hv_Column, hv_WindowWidth, hv_WindowHeight, 0, "", "", &(*hv_WindowHandle));
	HDevWindowStack::Push((*hv_WindowHandle));
	if (HDevWindowStack::IsOpen())
		SetPart(HDevWindowStack::GetActive(), 0, 0, hv_ImageHeight - 1, hv_ImageWidth - 1);
	return;
}


// Local procedures 
void perform_measurement(HObject ho_Image, HTuple hv_DrawID, HTuple hv_Width, HTuple hv_Height,
	HTuple *hv_RowEdge, HTuple *hv_ColumnEdge)
{

	// Local control variables
	HTuple  hv_ParamValues, hv_MeasureHandle, hv_Amplitude;
	HTuple  hv_Distance;

	//Use the parameters of the drawing object to generate a
	//measure object
	GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("phi")).Append("length1")).Append("length2")),
		&hv_ParamValues);
	//GenMeasureRectangle2(HTuple(hv_ParamValues[0]), HTuple(hv_ParamValues[1]), HTuple(hv_ParamValues[2]),
	//	HTuple(hv_ParamValues[3]), HTuple(hv_ParamValues[4]), hv_Width, hv_Height,
	//	"nearest_neighbor", &hv_MeasureHandle);
	////MeasurePos(ho_Image, hv_MeasureHandle, 1, 30, "all", "all", &(*hv_RowEdge), &(*hv_ColumnEdge),
	////	&hv_Amplitude, &hv_Distance);
	//CloseMeasure(hv_MeasureHandle);


	return;
}

void display_results(HTuple hv_RowEdge, HTuple hv_ColumnEdge, HTuple hv_WindowHandle)
{

	// Local iconic variables
	HObject  ho_Cross;

	// Local control variables
	HTuple  hv_OS;

	GetSystem("operating_system", &hv_OS);
	SetSystem("flush_graphic", "false");
	GenCrossContourXld(&ho_Cross, hv_RowEdge, hv_ColumnEdge, 15, 0.785398);
	//if (0 != (hv_OS == HTuple("Linux")))
	//{
	//	ClearWindow(hv_WindowHandle);
	//	SetColor(hv_WindowHandle, "green");
	//	DispObj(ho_Cross, hv_WindowHandle);
	//}
	//else
	//{
	//	if (HDevWindowStack::IsOpen())
	//		ClearWindow(HDevWindowStack::GetActive());
	//	if (HDevWindowStack::IsOpen())
	//		DispObj(ho_Cross, HDevWindowStack::GetActive());
	//}
	DispObj(ho_Cross, hv_WindowHandle);
	SetSystem("flush_graphic", "true");
	//DispCross(hv_WindowHandle, -32, -32, 6, 0);
	return;
}

void ST_MYDISP::EditOneElementLine(UCI::OptionsMap * o)
{
	try {
				
		// 首先是打开原来的保存的图
		this->OnDisButMatchLoadPic();

		this->m_sIPResult.result_cont.GenEmptyObj();

		// 取出 ROI
		double Row1 = (*o)[L"ROI_Row1"];
		double Column1 = (*o)[L"ROI_Column1"];
		double Row2 = (*o)[L"ROI_Row2"];
		double Column2 = (*o)[L"ROI_Column2"];

		if (Row1 == 0.0f) {
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];
			CreateDrawingObjectLine(w / 2, h / 2, w / 2 + 200, h / 2 + 200, &this->hv_DrawID);
		}
		else {
			CreateDrawingObjectLine(Row1, Column1, Row2, Column2, &this->hv_DrawID);
		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_LINE;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneElementLine()：");

	}	
}

void ST_MYDISP::EditOneElementCIRCLE(UCI::OptionsMap * o)
{
	try {

		// 首先是打开原来的保存的图
		this->OnDisButMatchLoadPic();
		this->m_sIPResult.result_cont.GenEmptyObj();

		double hv_RowC = (*o)[L"ROI_RowC"];
		double hv_ColumnC = (*o)[L"ROI_ColumnC"];
		double hv_Radius = (*o)[L"ROI_Radius"];
		double hv_StartPhi = (*o)[L"ROI_StartPhi"];
		double hv_EndPhi = (*o)[L"ROI_EndPhi"];
		//CString hv_PointOrder = (*o)[L"ROI_PointOrder"];
		//CString HTupleDir = (*o)[L"HTupleDir"];
		int HTupleArcOrCircle = (*o)[L"HTupleArcOrCircle"];
	
		// 圆附加对像
		// Create a circle sector which can be modified interactively.
		//LIntExport void CreateDrawingObjectCircleSector(const HTuple& Row, const HTuple& Column, const HTuple& Radius, const HTuple& StartAngle, const HTuple& EndAngle, HTuple* DrawID);

		if (hv_RowC == 0.0f) {  // 没有任何数据，，就画一个圆，方向为圆心指向圆外
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];

			int len = w /8;
			// 圆
			CreateDrawingObjectCircle(w / 2, h / 2, len, &this->hv_DrawID);	
		}
		else {
			// 圆或弧
			CreateDrawingObjectCircleSector(hv_RowC, hv_ColumnC, hv_Radius,  hv_StartPhi, hv_EndPhi, &this->hv_DrawID);		

		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_CIRCLE;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneElementLine()：");

	}
}

void ST_MYDISP::EditOneElementPEAK(UCI::OptionsMap * o)
{
	try {		

		// 首先是打开原来的保存的图
		this->OnDisButMatchLoadPic();
		this->m_sIPResult.result_cont.GenEmptyObj();

		double dRow = (*o)[L"Peak_Rectangle2_Row"];
		double dColumn = (*o)[L"Peak_Rectangle2_Colum"];
		double dPhi = (*o)[L"Peak_Rectangle2_Phi"];
		double dLength1 = (*o)[L"Peak_Rectangle2_Length1"];
		double dLength2 = (*o)[L"Peak_Rectangle2_Length2"];

		if (dRow == 0) {
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];
			int len = w / 8;

			CreateDrawingObjectRectangle2(w/2, h/2, 0.78f, len/2, len, &this->hv_DrawID);
		}
		else {
			CreateDrawingObjectRectangle2(dRow, dColumn, dPhi, dLength1, dLength2, &this->hv_DrawID);
		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_PEAK;

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneElementLine()：");

	}
}

// 1 维测量编辑
void ST_MYDISP::EditOneMEASURE_RECTANGLE2(UCI::OptionsMap * o)
{
	try {		
		this->OnDisButMatchLoadPic();
		this->m_sIPResult.result_cont.GenEmptyObj();

		double hv_Row = (*o)[L"Measure_Rectangle2_Row"];
		double hv_Column = (*o)[L"Measure_Rectangle2_Column"];
		double hv_Phi = (*o)[L"Measure_Rectangle2_Phi"];
		double hv_Length1 = (*o)[L"Measure_Rectangle2_Length1"];
		double hv_Length2 = (*o)[L"Measure_Rectangle2_Length2"];

		if (hv_Row == 0) {
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];
			int len = w / 8;

			CreateDrawingObjectRectangle2(w / 2, h / 2, 0.78f, len / 2, len, &this->hv_DrawID);
		}
		else {
			CreateDrawingObjectRectangle2(hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2, &this->hv_DrawID);
		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_MEASURE_RECTANGLE2;

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneElementLine()：");
	}
}

void ST_MYDISP::EditOneBalloonFlaw(UCI::OptionsMap * o)
{
	try {
		this->OnDisButMatchLoadPic();
		this->m_sIPResult.result_cont.GenEmptyObj();

		double hv_Row = (*o)[L"Flaw_Rectangle2_Row"];
		double hv_Column = (*o)[L"Flaw_Rectangle2_Column"];
		double hv_Phi = (*o)[L"Flaw_Rectangle2_Phi"];
		double hv_Length1 = (*o)[L"Flaw_Rectangle2_Length1"];
		double hv_Length2 = (*o)[L"Flaw_Rectangle2_Length2"];

		if (hv_Row == 0) {
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];
			int len = w / 8;

			CreateDrawingObjectRectangle2(w / 2, h / 2, 0.78f, len / 2, len, &this->hv_DrawID);
		}
		else {
			CreateDrawingObjectRectangle2(hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2, &this->hv_DrawID);
		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_BALLOON_FLAW;

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneBalloonFlaw()：");
	}
}

void ST_MYDISP::EditOneROI(UCI::OptionsMap * o)
{
	try {

		this->m_pMHWinMain->ClearWindow();
		this->m_sIPResult.result_cont.GenEmptyObj();

		HalconCpp::HXLDCont Contours;
		HalconCpp::HImage hImg;

		UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_CAMERA);
		this->MeasureOneCamera(om, hImg, Contours);

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
					return;
				}
			}
			// 校正图像
			hImg = hImg.MapImage(this->ho_RectMap);
		}

		this->m_sIPResult.result_img = hImg;

		//om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_ROI);
		//this->MeasureOneROI(om, hImg, Contours);	



		double hv_Row = (*o)[L"ROI_Rectangle2_Row"];
		double hv_Column = (*o)[L"ROI_Rectangle2_Column"];
		double hv_Phi = (*o)[L"ROI_Rectangle2_Phi"];
		double hv_Length1 = (*o)[L"ROI_Rectangle2_Length1"];
		double hv_Length2 = (*o)[L"ROI_Rectangle2_Length2"];

		if (hv_Row == 0) {
			int w = UCI::OPTIONS[L"图像宽度"];
			int h = UCI::OPTIONS[L"图像高度"];
			int len = w / 8;

			CreateDrawingObjectRectangle2(w / 2, h / 2, 0.78f, len / 2, len, &this->hv_DrawID);
		}
		else {
			CreateDrawingObjectRectangle2(hv_Row, hv_Column, hv_Phi, hv_Length1, hv_Length2, &this->hv_DrawID);
		}

		AttachDrawingObjectToWindow(*m_pMHWinMain, this->hv_DrawID);
		this->m_pMHWinMain->AttachBackgroundToWindow(this->m_sIPResult.result_img);

		this->mAppState = APP_ATTACH_ROI;

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"EditOneROI()：");
	}
}


void ST_MYDISP::OnRButtonDown(MSG * pMsg)
{
	// 判断是不是在图片内
	POINT ptTmp;
	GetCursorPos(&ptTmp);//获取鼠标位置 
	CRect Rect;
	this->PMainStatic->ClientToScreen(Rect);

	ptTmp.x = ptTmp.x - Rect.left;
	ptTmp.y = ptTmp.y - Rect.top;

	CRect rcPIC;
	this->PMainStatic->GetClientRect(&rcPIC);

	if (rcPIC.PtInRect(ptTmp)) {
		switch (this->mAppState) {
		case APP_ATTACH_LINE:                // 当前附加了直线ROI	
			OnRButtonDownAPP_ATTACH_LINE();
		break;
		case APP_ATTACH_CIRCLE:
			OnRButtonDownAPP_ATTACH_CIRCLE();
			break;	
		case APP_ATTACH_PEAK:
			OnRButtonDownAPP_ATTACH_PEAK();
			break;
		case APP_ATTACH_MEASURE_RECTANGLE2:
			OnRButtonDownAPP_ATTACH_MEASURE_RECTANGLE2();
			break;
		case APP_ATTACH_BALLOON_FLAW:
			OnRButtonDownAPP_ATTACH_BALLOON_FLAW();
			break;
		case APP_ATTACH_ROI:
			OnRButtonDownAPP_ATTACH_ROI();
			break;
		default:
			break;
		}
	}
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_LINE()
{
	UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

	HTuple  hv_ParamValues;
	GetDrawingObjectParams(hv_DrawID, (((HTuple("row1").Append("column1")).Append("row2")).Append("column2")),
		&hv_ParamValues);

	double Row1 = hv_ParamValues[0].D();
	double Column1 = hv_ParamValues[1].D();

	double Row2 = hv_ParamValues[2].D();
	double Column2 = hv_ParamValues[3].D();

	// 保存ROI
	(*o)[L"ROI_Row1"] = Row1;
	(*o)[L"ROI_Column1"] = Column1;
	(*o)[L"ROI_Row2"] = Row2;
	(*o)[L"ROI_Column2"] = Column2;

	int hv_Elements = (*o)[L"Rake直线检测点数"];
	double hv_DetectHeight = (*o)[L"Rake直线检测高度"];
	double hv_DetectWidth = (*o)[L"Rake直线检测宽度"];

	draw_rake(&this->m_sIPResult.result_cont,
		hv_Elements,
		hv_DetectHeight,
		hv_DetectWidth,
		&Row1,
		&Column1,
		&Row2,
		&Column2);

	// 测量这个直线
	MeasureOneLine(o, this->m_sIPResult.result_img, this->m_sIPResult.result_cont);

	DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
	this->m_pMHWinMain->DetachBackgroundFromWindow();
	this->OnMainDisp(NULL, NULL);
	this->AppStateChangeTo(APP_READY);
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_PEAK()
{
	UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

	HTuple  hv_ParamValues;
	GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("phi")).Append("length1")).Append("length2")),
		&hv_ParamValues);

	double dRow = hv_ParamValues[0].D();
	double dColumn = hv_ParamValues[1].D();
	double dPhi = hv_ParamValues[2].D();
	double dLength1 = hv_ParamValues[3].D();
	double dLength2 = hv_ParamValues[4].D();


	// 保存
	(*o)[L"Peak_Rectangle2_Row"] = dRow;
	(*o)[L"Peak_Rectangle2_Colum"] = dColumn;
	(*o)[L"Peak_Rectangle2_Phi"] = dPhi;
	(*o)[L"Peak_Rectangle2_Length1"] = dLength1;
	(*o)[L"Peak_Rectangle2_Length2"] = dLength2;

	// 下面显示这个查找到的顶点
	double hv_DetectWidth = (*o)[L"Peak_DetectWidth"];
	double hv_Sigma = (*o)[L"Peak_Sigma"];
	double hv_Threshold = (*o)[L"Peak_Threshold"];
	CString hv_Transition = (*o)[L"Peak_Transition"];
	CString hv_Select = (*o)[L"Peak_Select"];

	HImage Image = this->m_sIPResult.result_img;
	HTuple hv_EdgesY, hv_EdgesX, hv_ResultRow, hv_ResultColumn;

	peak(Image, dRow, dColumn, dPhi, dLength1, dLength2, hv_DetectWidth, hv_Sigma, hv_Threshold,
		CT2A(hv_Transition).m_psz, CT2A(hv_Select).m_psz,
		&hv_EdgesY, &hv_EdgesX, &hv_ResultRow, &hv_ResultColumn);

	HObject  ho_Cross;

	if (hv_EdgesY.Length() > 0) {
		this->m_pMHWinMain->SetColor("yellow");
		GenCrossContourXld(&ho_Cross, hv_EdgesY, hv_EdgesX, 3, 0);

		this->m_pMHWinMain->DispObj(ho_Cross);

		this->m_pMHWinMain->SetColor("green");
		GenCrossContourXld(&ho_Cross, hv_ResultRow, hv_ResultColumn, 10, 0);

		m_sIPResult.result_cont = m_sIPResult.result_cont.ConcatObj(ho_Cross);
	}


	DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
	this->m_pMHWinMain->DetachBackgroundFromWindow();
	this->OnMainDisp(NULL, NULL);
	this->AppStateChangeTo(APP_READY);
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_MEASURE_RECTANGLE2()
{
	UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

	HTuple  hv_ParamValues;
	GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("phi")).Append("length1")).Append("length2")),
		&hv_ParamValues);

	double dRow = hv_ParamValues[0].D();
	double dColumn = hv_ParamValues[1].D();
	double dPhi = hv_ParamValues[2].D();
	double dLength1 = hv_ParamValues[3].D();
	double dLength2 = hv_ParamValues[4].D();


	// 保存
	(*o)[L"Measure_Rectangle2_Row"] = dRow;
	(*o)[L"Measure_Rectangle2_Column"] = dColumn;
	(*o)[L"Measure_Rectangle2_Phi"] = dPhi;
	(*o)[L"Measure_Rectangle2_Length1"] = dLength1;
	(*o)[L"Measure_Rectangle2_Length2"] = dLength2;

	this->MeasureOneELEMENT_MEASURE_RECTANGLE2(o, this->m_sIPResult.result_img, this->m_sIPResult.result_cont);

	DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
	this->m_pMHWinMain->DetachBackgroundFromWindow();
	this->OnMainDisp(NULL, NULL);
	this->AppStateChangeTo(APP_READY);
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_BALLOON_FLAW()
{
	UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

	HTuple  hv_ParamValues;
	GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("phi")).Append("length1")).Append("length2")),
		&hv_ParamValues);

	double dRow = hv_ParamValues[0].D();
	double dColumn = hv_ParamValues[1].D();
	double dPhi = hv_ParamValues[2].D();
	double dLength1 = hv_ParamValues[3].D();
	double dLength2 = hv_ParamValues[4].D();


	// 保存
	(*o)[L"Flaw_Rectangle2_Row"] = dRow;
	(*o)[L"Flaw_Rectangle2_Column"] = dColumn;
	(*o)[L"Flaw_Rectangle2_Phi"] = dPhi;
	(*o)[L"Flaw_Rectangle2_Length1"] = dLength1;
	(*o)[L"Flaw_Rectangle2_Length2"] = dLength2;

	this->MeasureOneELEMENT_MEASURE_BalloonFlaw(o, this->m_sIPResult.result_img, this->m_sIPResult.result_cont);

	DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
	this->m_pMHWinMain->DetachBackgroundFromWindow();
	this->OnMainDisp(NULL, NULL);
	this->AppStateChangeTo(APP_READY);
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_ROI()
{
	UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

	HTuple  hv_ParamValues;
	GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("phi")).Append("length1")).Append("length2")),
		&hv_ParamValues);

	double dRow = hv_ParamValues[0].D();
	double dColumn = hv_ParamValues[1].D();
	double dPhi = hv_ParamValues[2].D();
	double dLength1 = hv_ParamValues[3].D();
	double dLength2 = hv_ParamValues[4].D();


	// 保存
	(*o)[L"ROI_Rectangle2_Row"] = dRow;
	(*o)[L"ROI_Rectangle2_Column"] = dColumn;
	(*o)[L"ROI_Rectangle2_Phi"] = dPhi;
	(*o)[L"ROI_Rectangle2_Length1"] = dLength1;
	(*o)[L"ROI_Rectangle2_Length2"] = dLength2;

	this->MeasureOneROI(o, this->m_sIPResult.result_img, this->m_sIPResult.result_cont);

	DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
	this->m_pMHWinMain->DetachBackgroundFromWindow();
	this->OnMainDisp(NULL, NULL);
	this->AppStateChangeTo(APP_READY);
}

void ST_MYDISP::OnRButtonDownAPP_ATTACH_CIRCLE()
{
	try {

		UCI::OptionsMap* o = UCI::pMain->GetPropertyCurOptionsMap();

		// 得到直线的ROI
		HTuple  hv_ParamValues;


		// 得到圆的ROI
		int HTupleArcOrCircle = (*o)[L"HTupleArcOrCircle"];
		CString hv_PointOrder = (*o)[L"ROI_PointOrder"];

		GetDrawingObjectParams(hv_DrawID, ((((HTuple("row").Append("column")).Append("radius")).Append("start_angle")).Append("end_angle")),
			&hv_ParamValues);

		// 得到修改后的数据
		double hv_RowC = hv_ParamValues[0].D();
		double hv_ColumnC = hv_ParamValues[1].D();
		double hv_Radius = hv_ParamValues[2].D();
		double hv_StartPhi = hv_ParamValues[3].D();
		double hv_EndPhi = hv_ParamValues[4].D();


		// 保存原来的点
		(*o)[L"ROI_RowC"] = hv_RowC;
		(*o)[L"ROI_ColumnC"] = hv_ColumnC;
		(*o)[L"ROI_Radius"] = hv_Radius;
		(*o)[L"ROI_StartPhi"] = hv_StartPhi;
		(*o)[L"ROI_EndPhi"] = hv_EndPhi;


		draw_spoke(o, this->m_sIPResult.result_cont);
		this->MeasureOneCircle(o, this->m_sIPResult.result_img, this->m_sIPResult.result_cont);

		DetachDrawingObjectFromWindow(*m_pMHWinMain, hv_DrawID);
		this->m_pMHWinMain->DetachBackgroundFromWindow();
		this->OnMainDisp(NULL, NULL);
		
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"OnRButtonDownAPP_ATTACH_CIRCLE()：");
	}

	this->AppStateChangeTo(APP_READY);
}

// 双击编辑元素
void ST_MYDISP::EditOneElement(UCI::OptionsMap * o)
{
		
	int etype = (*o)[L"元素类型"];
	switch (etype) 
	{
	case ELEMENT_LINE:
		EditOneElementLine(o);
		break;
	case ELEMENT_CIRCLE:
		EditOneElementCIRCLE(o);
		break;
	case ELEMENT_PEAK:
		EditOneElementPEAK(o);
		break;
	case ELEMENT_MEASURE_RECTANGLE2:
		EditOneMEASURE_RECTANGLE2(o);
		break;
	case ELEMENT_BALLOON_FLAW:
		EditOneBalloonFlaw(o);
		break;
	case ELEMENT_ROI:
		EditOneROI(o);
		break;
	default:
		break;
	}	
	
	return;
	
	// 显示一下这个选择的 ROI
	CString stColor = UCI::OPTIONS[L"ElementDoubleClickColor"];
	int width = UCI::OPTIONS[L"ElementDoubleClickLineWidth"];

	
	if (etype == ELEMENT_DISTANCE) {
		stColor = UCI::OPTIONS[L"ElementDoubleClickDistanceColor"];
		width = UCI::OPTIONS[L"ElementDoubleClickDistancWidth"];
	}
	else if (etype == ELEMENT_RESULT) {
		stColor = UCI::OPTIONS[L"ElementDoubleClickResultColor"];
		width = UCI::OPTIONS[L"ElementDoubleClickResultWidth"];
	}	


	UCI::pDis->m_pMHWinMain->SetColor(CT2A(stColor).m_psz);
	UCI::pDis->m_pMHWinMain->SetLineWidth(width);

	HalconCpp::HXLDCont RoiHXLD;
	RoiHXLD.GenEmptyObj();
	UCI::pDis->DisDrawOneRoi(o, RoiHXLD);     // 显示下当前元素的形状
	UCI::pDis->m_pMHWinMain->DispObj(RoiHXLD);
}

