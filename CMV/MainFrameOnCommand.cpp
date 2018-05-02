#include "stdafx.h"
#include "CMV.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ChildView.h"
#include "CExcelOperation.h"
#include "DialogUserLog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;


// 打开一副图片
void CMainFrame::OnFileOpen()
{
	// TODO: 在此添加命令处理程序代码	

	//CChildFrame *pChild = (CChildFrame *)(this->GetActiveFrame());
	//CChildView *pView = &pChild->m_wndView;	

	// 如果子窗口已打开了摄像头
	if (UCI::pDis->IsCanLoadPicture() == false) {
		LogOut(L"当前摄像头正在采图, 不能打开图片!");
		return;
	}
	UCI::pDis->DisOnOpenImage();

	this->m_wndPictureView.AdjustLayout();


}


void CMainFrame::OnFileSave()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnSaveCurrentDISPtoFile();
}


void CMainFrame::OnCamOneShot()
{
	UCI::pDis->DisOnOneShot();
	UCI::pDis->DisOnSaveCurrentDISPtoFile();  // 再保存一下	
}


void CMainFrame::OnCamCapMulti()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnCamCapMulti();
}




void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CMDIFrameWndEx::OnTimer(nIDEvent);
}


void CMainFrame::OnWinZoomOut()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnWinZoomIn()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnWinRestore()
{
	// TODO: 在此添加命令处理程序代码
}



void CMainFrame::OnUserLoging()
{
	if (UCI::User.islogin) {
		MessageBox(L"当前已登录成功了！");
		return;
	}
	
	CDialogUserLog dlg(this);

	if (dlg.DoModal() == IDOK) {

		//CString userName;
		//CString passWord;
		//dlg.ID_UserName.GetWindowText(userName);
		//dlg.ID_Password.GetWindowText(passWord);


	}
}

void CMainFrame::OnExportEXCEL()
{
	if (!UCI::bIsCanOperate(L"报表输出")) return;

	CExcelOperation oApp;
	oApp.exportProductExcel(0);
}


void CMainFrame::OnReportManage()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnSysSetting()
{
	// TODO: 在此添加命令处理程序代码
}


//void CMainFrame::OnFileNew()
//{
//	// TODO: 在此添加命令处理程序代码
//	int a = 0;
//}


void CMainFrame::OnCalibrate()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnButCamCaliInner();
}


void CMainFrame::OnCamPoseSet()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnRectityPicture()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnModelSet()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMatchModel()
{
	// TODO: 在此添加命令处理程序代码
}



void CMainFrame::OnMeasureAuto()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeasureStop()
{
	// TODO: 在此添加命令处理程序代码
}




void CMainFrame::OnComboCameraResolutiion()
{
	// TODO: 在此添加命令处理程序代码
}








void CMainFrame::OnBCrossColor1()
{
	// TODO: 在此添加命令处理程序代码
}

//// 标定相机内参
//void CMainFrame::OnCalibrateInner()
//{
//	// TODO: 在此添加命令处理程序代码
//	this->GetpDis()->DisOnButCamCaliInner();
//}

// 标定相机内参
void CMainFrame::OnButCaliInner()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnButCamCaliInner();
}

void CMainFrame::OnButCaliPose()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnButCamCaliPose();
}

// 标定相机内参是截图保存
void CMainFrame::OnButCaliCapture()
{
	// TODO: 在此添加命令处理程序代码
	int prID = UCI::OPTIONS[L"项目编号"];
	UCI::pDis->DisOnSaveOnCaliPicture(prID);
}

void CMainFrame::OnComboCaliMapType()
{
	// TODO: 在此添加命令处理程序代码
}

// 产生畸变校正映射图像
void CMainFrame::OnButtonGenRectifyMap()
{
	UCI::pDis->DisOnButtonGenRectifyMap();
}


void CMainFrame::OnButtonTakeOne()
{
	UCI::pDis->DisOnOneShot();
}


// 显示项目视图窗口
void CMainFrame::OnViewProjectView()
{
	// TODO: 在此添加命令处理程序代码   m_wndFileView;

	if (this->m_wndProjectView.IsVisible() == FALSE) {
		this->m_wndProjectView.ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->m_wndProjectView.ShowPane(FALSE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateViewProjectView(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(this->m_wndProjectView.IsVisible());
}


void CMainFrame::OnViewOutputwnd()
{
	// TODO: 在此添加命令处理程序代码
	if (this->m_wndOutput.IsVisible() == FALSE) {
		this->m_wndOutput.ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->m_wndOutput.ShowPane(FALSE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateViewOutputwnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->m_wndOutput.IsVisible());
}


void CMainFrame::OnViewPropertieswnd()
{
	if (this->m_wndProperties.IsVisible() == FALSE) {
		this->m_wndProperties.ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->m_wndProperties.ShowPane(FALSE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateViewPropertieswnd(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->m_wndProperties.IsVisible());
}


void CMainFrame::OnViewClassview()
{
	// TODO: 在此添加命令处理程序代码 
	if (this->m_wndClassView.IsVisible() == FALSE) {
		this->m_wndClassView.ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->m_wndClassView.ShowPane(FALSE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateViewClassview(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->m_wndClassView.IsVisible());
}


void CMainFrame::OnViewPictureview()
{
	// TODO: 在此添加命令处理程序代码 
	if (this->m_wndPictureView.IsVisible() == FALSE) {
		this->m_wndPictureView.ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->m_wndPictureView.ShowPane(FALSE, FALSE, TRUE);
	}
}


void CMainFrame::OnUpdateViewPictureview(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->SetCheck(this->m_wndPictureView.IsVisible());
}


void CMainFrame::OnDebugStarp()
{
	// TODO: 在此添加命令处理程序代码
	//CChildFrame *pChild = (CChildFrame *)(this->GetActiveFrame());
	//CChildView *pView = &pChild->m_wndView;
	UCI::pDis->OnButtonFGStart();
}


void CMainFrame::OnDebugStop()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->OnButtonFGStop();
}


void CMainFrame::OnUpdateButCaliCapture(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(UCI::RIBmenu.Rib_CALI_CAPTURE);
}


void CMainFrame::OnComboMatchRoiType()
{
	// TODO: 在此添加命令处理程序代码
	static int imagIndex[] = { 4, 3,2,1 };

	CMFCRibbonComboBox* pComboBox = DYNAMIC_DOWNCAST(
		CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_MATCH_ROI_TYPE));

	//std::string st = CW2A(pCameraComboBox->GetItem(pCameraComboBox->GetCurSel()));

	int sel = pComboBox->GetCurSel();

	UCI::RIBmenu.iCOMBO_MATCH_ROI_TYPE = (eCOMBO_MATCH_ROI_TYPE)sel;

	pComboBox->SetImageIndex(imagIndex[sel], false);

	pComboBox->Redraw();

	//m_wndRibbonBar.RedrawWindow();

}


void CMainFrame::OnUpdateComboMatchRoiType(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}

// 加载当前保存的匹配图片
void CMainFrame::OnButMatchLoadPic()
{
	UCI::pDis->OnDisButMatchLoadPic();
}

// 采集一张匹配用的图，并根据设定做映射，或 算子处理
void CMainFrame::OnMatchTakeOne()
{
	UCI::pDis->DisOnMatchTakeOne();
}


void CMainFrame::OnMatchDelAllRoi()
{
	UCI::pDis->DisOnMatchDelAllRoi();
}

// 斜矩形模板绘制
void CMainFrame::OnRoiDrawRectangle2()
{
	UCI::pDis->DisOnRoiDrawRectangle2();
}

void CMainFrame::OnRoiDrawCircle()
{
	UCI::pDis->DisOnRoiDrawCircle();
}


void CMainFrame::OnRoiDrawEllipse()
{
	UCI::pDis->DisOnRoiDrawEllipse();
}


void CMainFrame::OnRoiDrawRectangle()
{
	UCI::pDis->DisOnRoiDrawRectangle();
}


void CMainFrame::OnRoiDrawAnyLine()
{
	UCI::pDis->DisOnRoiDrawAnyLine();
}

// 显示当前的模板信息\n模板信息
void CMainFrame::OnRoiInfo()
{
	UCI::pDis->DisOnRoiInfo();
}

// 定位功能
void CMainFrame::OnRoiFindModel()
{
	UCI::pDis->DisOnRoiFindModel();
}


void CMainFrame::OnMeaEleArc()
{
	// TODO: 在此添加命令处理程序代码
}

// 椭圆卡尺
void CMainFrame::OnMeaEleEllispe()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnMeaEleEllispe();

}


void CMainFrame::OnMeaEleEllispeArc()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeaEleWidth()
{
	// 一维测量算子
	UCI::pDis->DisOnAddMEASURE_RECTANGLE2();
}


void CMainFrame::OnMeaEleFindEdge()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeaEleFindTop()
{
	UCI::pDis->DisOnAddFindPeekElement();    // 找顶点元素
}


void CMainFrame::OnMeaEleArea()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeaEleCounter()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeaEleMeasure()
{	
	UCI::pDis->DisOnElementMeasureManual();   // 元素手动测量
}





void CMainFrame::OnButMatchSaveModel()
{
	UCI::pDis->DisOnButMatchSaveModel();
	// 保存下全局参数
	UCI::sDB.MySaveSettingToSqlite(UCI::OPTIONS);        // 保存全局参数
}

// 删除所有测量元素
void CMainFrame::OnMeaEleDelAll()
{
	UCI::pDis->DisOnElementDeleteAll();
}


void CMainFrame::OnButCaliCorrect()
{
	if (!UCI::pDis->TestCurrentAppState(APP_LOGGING_OK)) return;
	UCI::pDis->DisOnButCaliCorrect();     // 根据标定板校正图像
}


void CMainFrame::OnMeaEleOutput()
{
	if (!UCI::pDis->TestCurrentAppState(APP_LOGGING_OK)) return;
	UCI::pDis->DisOnEleOutReport();        // 报表元素输出
}

// 像素转换为毫米元素
void CMainFrame::OnMeaEleMillimeter()
{
	UCI::pDis->DisOnMeaEleMillimeter();        // 报表元素输出
}

void CMainFrame::OnMeaEleCamera()
{
	// TODO: 在此添加命令处理程序代码
	UCI::pDis->DisOnEleCameraAdd();
}

// 增加直线ROI
void CMainFrame::OnMeaEleLine()
{
	UCI::pDis->DisOnEleLineAdd();
}

// 增加圆弧ROI
void CMainFrame::OnMeaEleCircle()
{
	UCI::pDis->DisOnEleCircleAdd();
}

// 测量二条直线的角度及交点
void CMainFrame::OnEleAngle()
{
	UCI::pDis->DisOnEleAngle();
}

// 测量二个元素的距离
void CMainFrame::OnEleDistance()
{
	UCI::pDis->DisOnEleDistance();
}

void CMainFrame::OnMeaEleFunction()
{
	UCI::pDis->DisMeaEleFunction();
}


void CMainFrame::OnMeaEleMatching()
{
	UCI::pDis->DisMeaEleMatching();
}

// 通过四个顶点来测量球囊的长度
void CMainFrame::OnMeaEleBalloonLength()
{
	UCI::pDis->DisOnMeaEleBalloonLength();
}


void CMainFrame::OnMeaEleStep()
{
	// TODO: 在此添加命令处理程序代码
}

// 自动检测开始
void CMainFrame::OnAutoStart()
{
	UCI::pDis->DisOnAutoStart();
}

// 自动检测结束
void CMainFrame::OnAutoStop()
{
	UCI::pDis->DisOnAutoStop();
}

// 连续采集图像
void CMainFrame::OnCamContinueCap()
{
	UCI::pDis->DisOnCamContinueCap();
}

void CMainFrame::OnMeaBalloonEccent()
{
	// TODO: 在此添加命令处理程序代码
}


void CMainFrame::OnMeaEleBalloonFlaw()
{
	UCI::pDis->DisOnMeaEleBalloonFlaw();  // 球囊表面缺陷检测
}

// 显示当前的标定信息
void CMainFrame::OnButCaliShowInfo()
{
	UCI::pDis->DisOnButCaliShowInfo(); // 显示当前的标定信息
}


void CMainFrame::OnMeaEleRoi()
{
	UCI::pDis->DisOnMeaEleAddRoi(); // 显示当前的标定信息
}




