#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;
using namespace std;


void ST_MYDISP::DisProcessROIRegion(HalconCpp::HRegion &HXdis, HalconCpp::HRegion &HXsrc)
{
	
		switch (UCI::RIBmenu.iCOMBO_MATCH_ROI_TYPE)
		{
		case ROI_UNITE:
			HXdis = HXdis.Union2(HXsrc);
			break;
		case ROI_AND:
			HXdis = HXdis.Intersection(HXsrc);
			break;
		case ROI_SUB:
			HXdis = HXdis.Difference(HXsrc);
			break;
		case ROI_XOR:
			HXdis = HXdis.SymmDifference(HXsrc);
			break;
		default:
			break;
		}

}

void ST_MYDISP::DisProcessROImodel(HalconCpp::HImage &Img, HRegion &HXdis)
{
	
	try {
		HalconCpp::HImage ImageReduced;
		ImageReduced = Img.ReduceDomain(HXdis);

		UCI::OptionsMap* o = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);

		CString CreatNumLevels = (*o)[L"CreateShapeModleNumLevels"];
		double CreatAngleStart = (*o)[L"CreateShapeModleAngleStart"];
		double CreatAngleExtend = (*o)[L"CreateShapeModleAngleExtend"];
		CString CreateAngleStep = (*o)[L"CreateShapeModleAngleStep"];
		CString CreateOptimization = (*o)[L"CreateShapeModleOptimization"];
		CString CreateMetric = (*o)[L"CreateShapeModleMetric"];
		CString CreateContrast = (*o)[L"CreateShapeModleContrast"];
		CString CreateMinContrast = (*o)[L"CreateShapeModleMinContrast"];

		this->hv_Model.CreateShapeModel(
			ImageReduced,
			CT2A(CreatNumLevels).m_psz,
			CreatAngleStart,
			CreatAngleExtend,
			CT2A(CreateAngleStep).m_psz,
			CT2A(CreateOptimization).m_psz,
			CT2A(CreateMetric).m_psz,
			CT2A(CreateContrast).m_psz,
			CT2A(CreateMinContrast).m_psz);

		double FindAngleStart = (*o)[L"FindShapeModleAngleStart"];
		double FindAngleExtend = (*o)[L"FindShapeModleAngleExtend"];
		double FindMinScore = (*o)[L"FindShapeModleMinScore"];
		int FindNumMatches = (*o)[L"FindShapeModleNumMatches"];
		double FindOverlap = (*o)[L"FindShapeModleMaxOverlap"];
		CString FindSubPixel = (*o)[L"FindShapeModleSubPixel"];
		int FindNumLevels = (*o)[L"FindShapeModleNumLevels"];
		double FindGreediness = (*o)[L"FindShapeModleGreediness"];

		HTuple Row3, Column3, Angle1, Score1;
		this->hv_Model.FindShapeModel(
			Img,
			FindAngleStart,
			FindAngleExtend,
			FindMinScore,
			FindNumMatches,
			FindOverlap,
			CT2A(FindSubPixel).m_psz,
			FindNumLevels,
			FindGreediness,
			&Row3,
			&Column3,
			&Angle1,
			&Score1
		);


		EnterCriticalSection(&m_csIPDisp);
		m_sIPResult.result_cont = this->ho_ModelRegion;   // region 可转换成 xld
		LeaveCriticalSection(&m_csIPDisp);

		this->OnMainDisp(NULL, NULL);              // 主窗口显示一下

		CString DispShapeC = (*o)[L"DispShapeColor"];

		dev_display_shape_matching_results(
			this->hv_Model,
			CT2A(DispShapeC).m_psz,
			Row3,
			Column3,
			Angle1,
			1,
			1,
			0);


	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisProcessROImodel：");

	}
}

// ROI Rectangle2
void ST_MYDISP::DisOnRoiDrawRectangle2()
{
	
	if (UCI::RIBmenu.bIsInROIdrawing) {
		MyLog(L"当前已在绘ROI中。。。");
		return;
	}
	
	try {
		double Row, Column, Phi, Length1, Length2;
		
		HalconCpp::HRegion Hreg;                     // 区域
		HalconCpp::HImage hImg;

		EnterCriticalSection(&m_csIPDisp);  
		hImg = m_sIPResult.result_img;
		LeaveCriticalSection(&m_csIPDisp);    

		// 提示信息
		this->write_message(20, 20, "画模板区域,点击鼠标右键确认");

		UCI::RIBmenu.bIsInROIdrawing = true;
		// 画区域
		this->m_pMHWinMain->DrawRectangle2(&Row, &Column, &Phi, &Length1, &Length2);

		UCI::RIBmenu.bIsInROIdrawing = false;

		Hreg.GenRectangle2(Row, Column, Phi, Length1, Length2);

		// 产生区域
		DisProcessROIRegion(this->ho_ModelRegion, Hreg);	


		DisProcessROImodel(hImg, this->ho_ModelRegion);   // 查找模型




	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiDrawRectangle2()：");

	}
}

void ST_MYDISP::DisOnRoiDrawCircle()
{
	if (UCI::RIBmenu.bIsInROIdrawing) {
		MyLog(L"当前已在绘ROI中。。。");
		return;
	}
	
	try {
		double Row, Column, Radius;

		HalconCpp::HRegion Hreg;                     // 区域
		HalconCpp::HImage hImg;

		EnterCriticalSection(&m_csIPDisp);	
		hImg = m_sIPResult.result_img;
		LeaveCriticalSection(&m_csIPDisp);

		// 提示信息
		this->write_message(20, 20, "画模板区域,点击鼠标右键确认");

		UCI::RIBmenu.bIsInROIdrawing = true;
		// 画区域
		this->m_pMHWinMain->DrawCircle(&Row, &Column, &Radius);	

		UCI::RIBmenu.bIsInROIdrawing = false;


		// 产生区域
		Hreg.GenCircle(Row, Column, Radius);

		DisProcessROIRegion(this->ho_ModelRegion, Hreg);

		DisProcessROImodel(hImg, this->ho_ModelRegion);   // 查找模型
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiDrawCircle()：");

	}
}

void ST_MYDISP::DisOnRoiDrawEllipse()
{
	if (UCI::RIBmenu.bIsInROIdrawing) {
		MyLog(L"当前已在绘ROI中。。。");
		return;
	}
	
	try {
		double Row, Column, Phi,Radius1, Radius2;

		HalconCpp::HRegion Hreg;                     // 区域
		HalconCpp::HImage hImg;

		EnterCriticalSection(&m_csIPDisp);
		hImg = m_sIPResult.result_img;
		LeaveCriticalSection(&m_csIPDisp);

		// 提示信息
		this->write_message(20, 20, "画模板区域,点击鼠标右键确认");

		UCI::RIBmenu.bIsInROIdrawing = true;
		// 画区域
		this->m_pMHWinMain->DrawEllipse(&Row, &Column, &Phi, &Radius1, &Radius2);

		UCI::RIBmenu.bIsInROIdrawing = false;

		// 产生区域
		Hreg.GenEllipse(Row, Column, Phi, Radius1, Radius2);

		// 产生区域
		DisProcessROIRegion(this->ho_ModelRegion, Hreg);


		DisProcessROImodel(hImg, this->ho_ModelRegion);   // 查找模型
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiDrawEllipse()");

	}
}

void ST_MYDISP::DisOnRoiDrawRectangle()
{
	if (UCI::RIBmenu.bIsInROIdrawing) {
		MyLog(L"当前已在绘ROI中。。。");
		return;
	}
	
	try {
		double Row1, Column1, Row2, Column2;

		HalconCpp::HRegion Hreg;                     // 区域
		HalconCpp::HImage hImg;

		EnterCriticalSection(&m_csIPDisp);
		hImg = m_sIPResult.result_img;
		LeaveCriticalSection(&m_csIPDisp);

		// 提示信息
		this->write_message(20, 20, "画模板区域,点击鼠标右键确认");

		UCI::RIBmenu.bIsInROIdrawing = true;
		// 画区域
		this->m_pMHWinMain->DrawRectangle1(&Row1, &Column1, &Row2, &Column2);
		UCI::RIBmenu.bIsInROIdrawing = false;

		// 产生区域
		Hreg.GenRectangle1(Row1, Column1, Row2, Column2);

		// 与原来的区域进行运算
		DisProcessROIRegion(this->ho_ModelRegion, Hreg);

		//this->m_pMHWinMain->ClearWindow();	   
		//this->m_pMHWinMain->DispObj(hImg);
		//this->m_pMHWinMain->DispObj(this->ho_ModelRegion);

		DisProcessROImodel(hImg, this->ho_ModelRegion);   // 查找模型

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiDrawRectangle()");

	}
}

void ST_MYDISP::DisOnRoiDrawAnyLine()
{
	if (UCI::RIBmenu.bIsInROIdrawing) {
		MyLog(L"当前已在绘ROI中。。。");
		return;
	}
	
	try {
		
		HalconCpp::HImage hImg;                      // 显示的图像
		HalconCpp::HRegion Hreg;                     // 区域

		EnterCriticalSection(&m_csIPDisp);
		hImg = m_sIPResult.result_img;
		LeaveCriticalSection(&m_csIPDisp);

		// 提示信息
		this->write_message(20, 20, "画模板区域,点击鼠标右键确认");

		UCI::RIBmenu.bIsInROIdrawing = true;
		Hreg = this->m_pMHWinMain->DrawRegion();
		UCI::RIBmenu.bIsInROIdrawing = false;

		DisProcessROIRegion(this->ho_ModelRegion,Hreg);	

		DisProcessROImodel(hImg, this->ho_ModelRegion);   // 查找模型

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiDrawAnyLine：");

	}
}


// 显示当前的模板信息\n模板信息
void ST_MYDISP::DisOnRoiInfo()
{
	//try {

	//	// 打开当前保存的图片

	//	int prID = UCI::OPTIONS[L"项目编号"];

	//	this->OnDisButMatchLoadPic();

	//	CString stRegion = this->GetShapeModelRegionPathFile(prID);
	//	CString stModel = this->GetShapeModelPathFile(prID);

	//	if (GetFileAttributes(stRegion) == -1) { // 文件不存在
	//		MyLog(stRegion);
	//		MyLog("DisOnRoiInfo() ROI　区域文件不存在！ ");
	//		return;
	//	}

	//	//
	//	HXLDCont HXdis;
	//	HalconCpp::HImage hImg;
	//	HXdis.ReadObject(CT2A(stRegion).m_psz);

	//	if (GetFileAttributes(stModel) == -1) { // 文件不存在
	//		MyLog(stModel);
	//		MyLog("DisOnRoiInfo()ROI　模型文件不存在！ ");
	//		return;
	//	}

	//	this->hv_Model.ReadShapeModel(CT2A(stModel).m_psz);

	//	EnterCriticalSection(&m_csIPDisp);
	//	m_sIPResult.result_cont = HXdis;
	//	hImg = m_sIPResult.result_img;
	//	LeaveCriticalSection(&m_csIPDisp);

	//	this->OnMainDisp(NULL, NULL);

	//	double AngleStart = UCI::OPTIONS[L"FindShapeModleAngleStart"];
	//	double AngleExtend = UCI::OPTIONS[L"FindShapeModleAngleExtend"];

	//	HTuple Row3, Column3, Angle1, Score1;
	//	this->hv_Model.FindShapeModel(
	//		hImg,
	//		AngleStart,
	//		AngleExtend,
	//		0.5,
	//		1,
	//		0.5,
	//		"least_squares",
	//		0,
	//		0.9,
	//		&Row3,
	//		&Column3,
	//		&Angle1,
	//		&Score1
	//	);


	//	dev_display_shape_matching_results(
	//		this->hv_Model,
	//		"red",
	//		Row3,
	//		Column3,
	//		Angle1,
	//		1,
	//		1,
	//		0);

	//}
	//catch (HalconCpp::HException& except) {
	//	UCI::sDB.Log(except, L"DisOnRoiInfo()：");

	//}
}

void ST_MYDISP::DisOnRoiFindModel()
{
	try {		

		this->m_pMHWinMain->ClearWindow();

		HalconCpp::HImage hImg;
		HalconCpp::HXLDCont Cont;


		UCI::OPTIONS[L"匹配模板 OK"] = FALSE;

		UCI::OptionsMap *om;
		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_CAMERA);
		this->MeasureOneCamera(om, hImg, Cont);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_ROI);
		this->MeasureOneROI(om, hImg, Cont);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_FUNCTION);
		this->MeasureOneFUNCTION(om, hImg, Cont);
		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);
		if (this->MeasureOneMATCH(om, hImg, Cont) == FALSE) {
			write_message(10, 10, "没有找到相应的模型");
			return;
		}

		//Cont = Cont.ConcatObj(this->ho_ModelRegion);

		EnterCriticalSection(&m_csIPDisp);
		this->m_sIPResult.result_img = hImg;
		this->m_sIPResult.result_cont = Cont;
		LeaveCriticalSection(&m_csIPDisp);
		this->OnMainDisp(NULL, NULL);              // 主窗口显示一下


												   // 显示一下模板结果
		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);
		HTuple Row = (*om)[L"RES_FindShapeRow"];
		HTuple Column = (*om)[L"RES_FindShapeColumn"];
		HTuple Angle = (*om)[L"RES_FindShapeAngle"];
		HTuple Score = (*om)[L"RES_FindShapeScore"];

		if (Row.Length() > 0) {

			write_message(100, 20, "找到了对象，得分：" + Score, true);

			CString mcolor = (*om)[L"DispShapeColor"];

			dev_display_shape_matching_results(
				this->hv_Model,
				CT2A(mcolor).m_psz,
				Row,
				Column,
				Angle,
				1,
				1,
				0);
		}




		//int prID = UCI::OPTIONS[L"项目编号"];
		//// 采集一个图片
		//CString stModel = this->GetShapeModelPathFile(prID);		//

		//HalconCpp::HImage hImg;
		//if (GetFileAttributes(stModel) == -1) { // 文件不存在
		//	MyLog(stModel);
		//	MyLog("DisOnRoiInfo() 文件不存在！ ");
		//	return;
		//}
		//this->hv_Model.ReadShapeModel(CT2A(stModel).m_psz);

		//EnterCriticalSection(&m_csIPDisp);
		////m_sIPResult.result_cont = HXdis;
		//hImg = m_sIPResult.result_img;
		//LeaveCriticalSection(&m_csIPDisp);

		//this->OnMainDisp(NULL, NULL);

		//double AngleStart = UCI::OPTIONS[L"FindShapeModleAngleStart"];
		//double AngleExtend = UCI::OPTIONS[L"FindShapeModleAngleExtend"];

		//HTuple Row3, Column3, Angle1, Score1;
		//this->hv_Model.FindShapeModel(
		//	hImg,
		//	AngleStart,
		//	AngleExtend,
		//	0.5,
		//	1,
		//	0.5,
		//	"least_squares",
		//	0,
		//	0.9,
		//	&Row3,
		//	&Column3,
		//	&Angle1,
		//	&Score1
		//);

		//if (Row3.Length() < 1) {
		//	MyLog("DisOnRoiFindModel() 没有找到对应的产品形状");
		//	return;
		//}
		//

		//double dMin = UCI::OPTIONS[L"FindShapeModleMinScore"];

		//this->write_message(20, 20, "当前查找得分：" + Score1 + " 设置值：" + dMin);
		//if (Score1 >= dMin) {
		//	dev_display_shape_matching_results(
		//		this->hv_Model,
		//		"red",
		//		Row3,
		//		Column3,
		//		Angle1,
		//		1,
		//		1,
		//		0);
		//}
		//else {
		//	MyLog("DisOnRoiFindModel() 没有找到对应的产品形状");
		//}

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnRoiFindModel()：");

	}
}

// 保存ROI 
void ST_MYDISP::DisOnButMatchSaveModel()
{
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;

	try {
		int prID = UCI::OPTIONS[L"项目编号"];
		// 先判断一下文件是否存在
		CString str = this->GetMatchPathFile(prID);
		if (GetFileAttributes(str) != -1) { // 文件存在
			int res = ::MessageBox(NULL,
				L"点击 确定  重新实现采样一副！\n"
				"点击 取消  返回。",
				L"匹配文件已存在！",
				MB_OKCANCEL | MB_ICONSTOP);
			if (res == IDCANCEL) return;
		}

		// 1. 保存当前的图像
		this->SaveImgToROI();

		// 保存一下这个查找的区域 ROI
		this->ho_ModelRegion.WriteRegion(CT2A(this->GetShapeModelRegionPathFile(prID)).m_psz);

		// 保存一下这个查找的模型 
		this->hv_Model.WriteShapeModel(CT2A(this->GetShapeModelPathFile(prID)).m_psz);

		this->write_message(12, 12, "保存匹配信息成功了！");
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnButMatchSaveModel：保存匹配");

		this->write_message(12, 12, "保存匹配信息失败！ 详情请查看日志");
	}
}

// 加载上次保存的匹配的图
void ST_MYDISP::OnDisButMatchLoadPic()
{	
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;

	try {

		this->m_pMHWinMain->ClearWindow();

		int prID = UCI::OPTIONS[L"项目编号"];

		// 先看有没有加载标定数据
		BOOL isLoad = UCI::OPTIONS[L"标定信息 OK"];
		if (isLoad == FALSE) {
			if (this->LoadCaliPoseAndParamter() == FALSE) {
				write_message(200, 20, "加载标定数据没有成功，请先标定相机");
				return;
			}
		}

		HalconCpp::HImage hImg;
		HalconCpp::HXLDCont Contours;

		this->ho_ModelRegion.GenEmptyObj();            // 清空一下模板区域
		UCI::OPTIONS[L"匹配模板 OK"] = FALSE;
		Contours.GenEmptyObj();

		HTuple hv_ImageWidth, hv_ImageHeight;

		CString str = this->GetMatchPathFile(prID);    // 得到匹配文件名		
		hImg.ReadImage(CT2A(str).m_psz);
		hImg.GetImageSize(&hv_ImageWidth, &hv_ImageHeight);		

		int iHeight = hv_ImageHeight.I();
		int iWidth = hv_ImageWidth.I();

		int oldH = UCI::OPTIONS[L"图像高度"];
		int oldW = UCI::OPTIONS[L"图像宽度"];
		if (oldH != iHeight || oldW != iWidth) {

			UCI::OPTIONS[L"图像宽度"] = iWidth;
			UCI::OPTIONS[L"图像高度"] = iHeight;
			UCI::OPTIONS[L"ORGwhByCamera"] = 0;
		}

		this->m_pMHWinMain->SetPart(0, 0, iHeight - 1, iWidth - 1);


		// 同时也保存到抓取图像变量中去
		this->GrabImageDisp = hImg;
		this->m_pHWinFG->SetPart(0, 0, iHeight - 1, iWidth - 1);
		this->m_pHWinFG->DispObj(hImg);     // 缩略窗口显示


		// 下面走后面二步
		UCI::OptionsMap * om;
		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_ROI);
		this->MeasureOneROI(om, hImg, Contours);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_FUNCTION);
		this->MeasureOneFUNCTION(om, hImg, Contours);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);
		this->MeasureOneMATCH(om, hImg, Contours);


		Contours = Contours.ConcatObj(this->ho_ModelRegion);

		EnterCriticalSection(&m_csIPDisp);
		this->m_sIPResult.result_img = hImg;
		this->m_sIPResult.result_cont = Contours;
		LeaveCriticalSection(&m_csIPDisp);
		this->OnMainDisp(NULL, NULL);              // 主窗口显示一下


		// 显示一下模板结果
		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_MATCH);
		HTuple Row = (*om)[L"RES_FindShapeRow"];
		HTuple Column = (*om)[L"RES_FindShapeColumn"];
		HTuple Angle = (*om)[L"RES_FindShapeAngle"];
		HTuple Score = (*om)[L"RES_FindShapeScore"];

		if (Row.Length() > 0) {

			write_message(100, 20, "找到了对象，得分：" + Score, true);

			CString mcolor = (*om)[L"DispShapeColor"];

			dev_display_shape_matching_results(
				this->hv_Model,
				CT2A(mcolor).m_psz,
				Row,
				Column,
				Angle,
				1,
				1,
				0);
		}

		



		//HXLDCont ho_RoiHxld;
		//ho_RoiHxld.GenEmptyObj();
		//this->DisDrawAllRoi(ho_RoiHxld);            // 显示所有的元素的 ROI
		//this->m_pMHWinMain->DispObj(ho_RoiHxld);		

		return;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"OnDisButMatchLoadPic：");

		return;
	}
}

// 只是加载原图像
void ST_MYDISP::OnDisButLoadPic()
{
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;

	try {

		this->m_pMHWinMain->ClearWindow();

		// 做手动测量程序的前三步
		HalconCpp::HXLDCont Contours;
		HalconCpp::HImage hImg;

		UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_CAMERA);
		this->MeasureOneCamera(om, hImg, Contours);

		this->m_pMHWinMain->DispObj(hImg);

		om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_ROI);
		this->MeasureOneROI(om, hImg, Contours);
		
		return;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"OnDisButLoadPic()");

		return;
	}
}

void ST_MYDISP::DisOnElementDeleteAll()
{
	if (!this->TestCurrentAppState(APP_LOGGING_OK)) return;
	
	int res = ::MessageBox(NULL,
		L"点击 确定  删除所有测量元素！\n"
		"点击 取消  返回。",
		L"警告！亲，您真的要删除所有测量元素吗？",
		MB_OKCANCEL | MB_ICONSTOP);
	if (res == IDCANCEL) return;

	int prID = UCI::OPTIONS[L"项目编号"];

	// 从第三个开始一个一个删除啊！
	int size = (int)UCI::ELE.EleList.GetCount();

	int i = size;
	POSITION pos = UCI::ELE.EleList.GetTailPosition();
	while (pos != NULL) {
		UCI::OptionsMap *ot = &UCI::ELE.EleList.GetPrev(pos);
		if (i > 4) {
			UCI::ELE.EleList.RemoveTail();
		}
		i--;
	}

	UCI::pMain->MProjectTreeFillByPrID();

}






