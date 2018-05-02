#include "stdafx.h"
#include "MainFrm.h"
#include "ChildView.h"

#include "MyData.h"
#include "MyDispLay.h"


using namespace HalconCpp;
using namespace std;



// 标定按钮命令, 只是标定内参
void ST_MYDISP::DisOnButCamCaliInner()
{
	this->PushCaptureDown = 0;

	if (this->mAppState == App_CamInner) {
		this->StopContiouscap();
		this->AppStateChangeTo(APP_READY);
		this->MyLog(L"已停止标定程序！");
		return;
	}

	if (!TestCurrentAppState(APP_LOGGING_OK)) return;


	int prID = UCI::OPTIONS[L"项目编号"];
	CString CaliPathFile = this->pCAM->GetCaliCalPathFile(prID);

	if (PathFileExists(CaliPathFile))
	{
		int res = ::MessageBox(NULL,
			L"点击 确定  重新标定！\n"
			"点击 取消  返回。",
			L"相机内参标定文件已存在！",
			MB_OKCANCEL | MB_ICONSTOP);
		if (res == IDCANCEL) return;
		if (res == IDOK) {
			CFile TempFile;
			TempFile.Remove(CaliPathFile);
		}
	}

	int total = UCI::OPTIONS[L"标定图片数量"];
	// 当前图片数量已满足了
	if (this->GetTotalCariInnerPicNum(prID) >= total) {
		int res = ::MessageBox(NULL,
			L"点击 确定  重新采集图片！\n"
			"点击 取消  返回。",
			L"相机标定内参图片数量已达到要求了。！",
			MB_OKCANCEL | MB_ICONSTOP);
		if (res == IDCANCEL) return;
		if (res == IDOK) {
			this->DelTotalCariInnerPic(prID); // 则删除所有的采集图像文件
		}
	}		

	this->DisOpenCamera();                                 // 打开相机

	UCI::OptionsMap * camo = &UCI::ELE.EleList.GetHead();
	UCI::sDB.SetCamPara(*camo);
	

	// 设定一下当前的相机标定参数	
	this->AppStateChangeTo(App_CamInner);
	this->StartContiouscap();
}

// 保存按钮
void ST_MYDISP::DisOnSaveOnCaliPicture(int prID)
{
	// 如果当前是标定模式，则要保存图片


	this->PushCaptureDown = 1; 
}


// 标定按钮命令, 只是标定姿态
void ST_MYDISP::DisOnButCamCaliPose()
{
	this->PushCaptureDown = 0;

	if (this->mAppState == App_CamPose) {
		this->StopContiouscap();
		this->AppStateChangeTo(APP_READY);
		this->MyLog(L"已停止标定姿态！");
	}
	int prID = UCI::OPTIONS[L"项目编号"];
	// 查找是不是已存在了标定内参文件及相机姿态文件  PathFileExists
	if (PathFileExists(this->pCAM->GetCaliPoseDataPathFile(prID)))
	{
		int res = ::MessageBox(NULL,
			L"点击 确定  重新标定！\n"
			"点击 取消  返回。",
			L"相机姿态标定文件已存在！",
			MB_OKCANCEL | MB_ICONSTOP);
		if (res == IDCANCEL) return;
		if (res == IDOK) {
			CFile TempFile;
			TempFile.Remove(this->pCAM->GetCaliPoseDataPathFile(prID));
		}
	}

	//int prID = UCI::OPTIONS[L"项目编号"];

	// 当前图片数量已满足了
	if (this->GetTotalCariPosePicNum(prID) >= 1) {
		int res = ::MessageBox(NULL,
			L"点击 确定  重新采集图片！\n"
			"点击 取消  返回。",
			L"相机标定姿态图片数量已达到要求了。！",
			MB_OKCANCEL | MB_ICONSTOP);
		if (res == IDCANCEL) return;
		if (res == IDOK) {
			this->DelTotalCariPosePic(prID); // 则删除所有的采集图像文件 
		}
	}

	this->DisOpenCamera();   // 打开相机

	UCI::OptionsMap * camo = &UCI::ELE.EleList.GetHead();
	UCI::sDB.SetCamPara(*camo);

	this->AppStateChangeTo(App_CamPose);
	this->StartContiouscap();

}

// 根据标定板校正图像
void ST_MYDISP::DisOnButCaliCorrect()
{
	
	BOOL isLoad = UCI::OPTIONS[L"标定信息 OK"];
	if (isLoad == FALSE) {
		if (this->LoadCaliPoseAndParamter() == FALSE) {
			write_message(200, 20, "加载标定数据没有成功，请先标定相机");
			return;
		}
	}

	try{
		HImage ImageMapped;

		int width = UCI::OPTIONS[L"图像宽度"];
		int height = UCI::OPTIONS[L"图像高度"];

		this->m_pMHWinMain->SetPart(0, 0, height - 1, width - 1);

		MapImage(m_sIPResult.result_img, this->ho_RectMap, &ImageMapped);

		// this->m_sIPResult.result_img = ImageMapped;

		this->m_pMHWinMain->DispObj(ImageMapped);

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"ST_MYDISP::DisOnButCaliCorrect()：");
	}

	// http://blog.csdn.net/u011058765/article/details/48469633

	int a = 0;
}

// 匹配图像，采集一张匹配用的图，并根据设定做映射，或 算子处理
void ST_MYDISP::DisOnMatchTakeOne()
{
	
	
	// 1 检测当前的状态
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;

	try {

		this->m_pMHWinMain->ClearWindow();

		// 做手动测量程序的前三步
		HalconCpp::HXLDCont Contours;
		HalconCpp::HImage hImg;

		UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_CAMERA);
		this->MeasureOneCamera(om, hImg, Contours);

	//	this->m_pMHWinMain->DispObj(hImg);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_ROI);
		this->MeasureOneROI(om, hImg, Contours);

	//	this->m_pMHWinMain->ClearWindow();
	//	this->m_pMHWinMain->DispObj(hImg);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_FUNCTION);
		this->MeasureOneFUNCTION(om, hImg, Contours);
		//om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);
		//this->MeasureOneMATCH(om, hImg, Contours);
	//	this->m_pMHWinMain->ClearWindow();
	//	this->m_pMHWinMain->DispObj(hImg);

		EnterCriticalSection(&m_csIPDisp);
		this->m_sIPResult.result_img = hImg;
		this->m_sIPResult.result_cont = Contours;
		LeaveCriticalSection(&m_csIPDisp);
		this->OnMainDisp(NULL, NULL);              // 主窗口显示一下

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnMatchTakeOne()：");
	}
}

// 画出所有ROI HXLD
void ST_MYDISP::DisDrawAllRoi(HalconCpp::HXLDCont& hv_Cont)
{
	POSITION pos = UCI::ELE.EleList.GetHeadPosition();
	while (pos != NULL) {
		UCI::OptionsMap *o = &UCI::ELE.EleList.GetNext(pos);
		this->DisDrawOneRoi(o, hv_Cont);
	}
}

void ST_MYDISP::DisDrawOneRoi(UCI::OptionsMap * o, HalconCpp::HXLDCont& hv_Cont)
{
	try {

		int type = (*o)[L"元素类型"];

		HalconCpp::HXLDCont hv_Out;

		switch (type) {
		case ELEMENT_LINE:
		{
			HTuple ht_Rows = (*o)[L"HTupleRows"];
			HTuple ht_Columns = (*o)[L"HTupleCols"];
			hv_Out.GenContourPolygonXld(ht_Rows, ht_Columns);

			//HTuple hv_Length = hv_Out.LengthXld();
			//this->write_message(300, 30, "hv_Length" + hv_Length);

			hv_Cont = hv_Cont.ConcatObj(hv_Out);
		}
		break;
		case ELEMENT_CIRCLE:
		{
			HTuple ht_Rows = (*o)[L"HTupleRows"];
			HTuple ht_Columns = (*o)[L"HTupleCols"];

			HTuple hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi, hv_EndPhi, hv_PointOrder;


			hv_Out.GenContourPolygonXld(ht_Rows, ht_Columns);
			//GenContourPolygonXld(&ho_Contour, (*hv_ROIRows), (*hv_ROICols));
			hv_Out.FitCircleContourXld("algebraic", -1, 0, 0, 3, 2, &hv_RowC, &hv_ColumnC,
				&hv_Radius, &hv_StartPhi, &hv_EndPhi, &hv_PointOrder);

			hv_Out.GenCircleContourXld(hv_RowC, hv_ColumnC, hv_Radius, hv_StartPhi,
				hv_EndPhi, hv_PointOrder, 3);
			//HTuple hv_Length = hv_Out.LengthXld();
			//this->write_message(300, 30, "hv_Length" + hv_Length);

			hv_Cont = hv_Cont.ConcatObj(hv_Out);

		}
		break;
		// 显示一个距离元素
		case ELEMENT_DISTANCE:
		{
			// 得到二条测量几何
			CString idDis1 = (*o)[L"引用元素名称1"];
			CString idDis2 = (*o)[L"引用元素名称1"];

			if (idDis1 == L"" || idDis2 == L"") return;

			if (idDis1 == idDis2) return;			

			UCI::OptionsMap * oDIS1 = UCI::ELE.GetOptionsMapByEleName(idDis1);
			UCI::OptionsMap * oDIS2 = UCI::ELE.GetOptionsMapByEleName(idDis2);

			CString eleParameter1 = (*o)[L"引用元素1参数名"];
			CString eleParameter2 = (*o)[L"引用元素2参数名"];

			int etype1 = (*oDIS1)[L"元素类型"];
			int etype2 = (*oDIS2)[L"元素类型"];
	

			double scalemm = UCI::OPTIONS[L"单个像素长度mm"];    // 测量象素到mm的比例

			HalconCpp::HXLDCont hx_tmp;

			// 1. 二个几何都没有选参数名称
			if (eleParameter1 == "" && eleParameter2 == "")
			{
				if (etype1 == ELEMENT_CIRCLE && etype2 == ELEMENT_CIRCLE)
				{
					//// 二个圆, 是求二个圆的圆心距
					//double e1row = (*oDIS1)["RES_Circle_Center_Row"];
					//double e1col = (*oDIS1)["RES_Circle_Center_Column"];

					//double e2row = (*oDIS2)["RES_Circle_Center_Row"];
					//double e2col = (*oDIS2)["RES_Circle_Center_Column"];

					//this->gen_double_arrow_contour_xld(     // 画出双头箭头
					//	&hx_tmp,
					//	e1row/scalemm,
					//	e1col / scalemm,
					//	e2row / scalemm,
					//	e2col / scalemm,
					//	alength,
					//	awidth);

					//hv_Cont = hv_Cont.ConcatObj(hx_tmp);
				}
				//else if ((etype1 == ELEMENT_LINE && etype2 == ELEMENT_CIRCLE)
				//	|| (etype2 == ELEMENT_LINE && etype1 == ELEMENT_CIRCLE))
				//{
				//	// 圆心到直线的垂直距离
				//	double Rrow, Rcol;                     // 圆心
				//	double Lrow1, Lcol1, Lrow2, Lcol2;     // 直线二个端点

				//	UCI::OptionsMap * oCir = oDIS1;
				//	UCI::OptionsMap * oLine = oDIS2;

				//	if (etype1 == ELEMENT_LINE) {
				//		oCir = oDIS2;
				//		oLine = oDIS1;
				//	}


				//}
			}
		}
			break;
		case ELEMENT_ANGLE:
			break;
		default:
			break;
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisDrawOneRoi 画 ROI 出错：");
	}
}


// 删除所有的匹配对象
void ST_MYDISP::DisOnMatchDelAllRoi()
{

	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;


	this->m_sIPResult.result_cont.GenEmptyObj();
	this->ho_ModelRegion.GenEmptyRegion();
	this->hv_Model.Clear();

	// 还要删除保存的区域文件，ROI文件
	int prID = UCI::OPTIONS[L"项目编号"];

	CString stRegion = this->GetShapeModelRegionPathFile(prID);
	CString stModel = this->GetShapeModelPathFile(prID);

	CFile TempFile;
	if (GetFileAttributes(stRegion) != -1) { // 文件不存在
		TempFile.Remove(stRegion);
	}
	if (GetFileAttributes(stModel) != -1) { // 文件不存在
		TempFile.Remove(stModel);
	}

	this->OnMainDisp(NULL, NULL);
}
