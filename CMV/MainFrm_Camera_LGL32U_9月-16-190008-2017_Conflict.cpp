#include "stdafx.h"
#include "CMV.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "ChildView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 选择那一个相机
void CMainFrame::OnComboCameraName()
{
	// TODO: 在此添加命令处理程序代码
	//// 2. 得到 device 	
	//CMFCRibbonComboBox* pCameraComboBox = DYNAMIC_DOWNCAST(
	//	CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CAMERA_NAME));

	//std::string str = CW2A(pCameraComboBox->GetItem(pCameraComboBox->GetCurSel()));
	//OPTIONS[L"相机名称"] = str;
}

// 相机连接类型， DirectShow, File ....
void CMainFrame::OnComboCameraConnectType()
{
	// TODO: 在此添加命令处理程序代码

	//CMFCRibbonComboBox* pCameraComboBox = DYNAMIC_DOWNCAST(
	//	CMFCRibbonComboBox, m_wndRibbonBar.FindByID(ID_COMBO_CAMERA_CONNECT_TYPE));
	//
	//std::string st = CW2A(pCameraComboBox->GetItem(pCameraComboBox->GetCurSel()));
	//OPTIONS[L"相机协议"] = st;

}

// 软触发定时
void CMainFrame::OnEditCameraTrigTime()
{
	// TODO: 在此添加命令处理程序代码

	// 1. 得到连接类型 Directshow File...
	//CMFCRibbonEdit* pE = DYNAMIC_DOWNCAST(
	//	CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_CAMERA_TRIG_TIME));

	//CString text = pE->GetEditText();
	//int v = _wtoi(text);
	//OPTIONS[L"软触发定时"] = v;	

	//int k = UCI::OPTIONS[L"软触发定时"];
	//int a = 0;
}

// 相机的显示模式 模拟 RGB
void CMainFrame::OnCheckCameraIsRgb()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL bRGB = UCI::OPTIONS[L"bRGBmode"];
	//OPTIONS[L"bRGBmode"] = !bRGB;
	
}

void CMainFrame::OnUpdateCheckCameraIsRgb(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	//BOOL bRGB = UCI::OPTIONS[L"bRGBmode"];
	//pCmdUI->SetCheck(bRGB);
}

void CMainFrame::OnUpdateCheckCameraSoftTrig(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	//BOOL isH = UCI::OPTIONS[L"bIsHardTrigger"];
	//pCmdUI->SetCheck(!isH);
}


// 相机的触发方式调整
void CMainFrame::OnCheckCameraSoftTrig()
{
	// TODO: 在此添加命令处理程序代码
	//BOOL isH = UCI::OPTIONS[L"bIsHardTrigger"];
	//OPTIONS[L"bIsHardTrigger"] = !isH;
}

// 相机的曝光方式调整
void CMainFrame::OnCheckCameraExposureMode()
{
	// TODO: 在此添加命令处理程序代码
	//CCamera* pCam = this->GetpDis()->GetCam();
	//UCI::pDis->pCAM->SetExposureMode(!UCI::pDis->pCAM->GetExposureMode());
}

void CMainFrame::OnUpdateCheckCameraExposureMode(CCmdUI *pCmdUI)
{
	// pCmdUI->Enable(this->GetpDis()->pACQU->IsOpened());
	
	// TODO: 在此添加命令更新用户界面处理程序代码
	//CCamera* pCam = this->GetpDis()->GetCam();
	//Sleep(100);
	//pCmdUI->SetCheck(UCI::pDis->pCAM->GetExposureMode());

	
}




void CMainFrame::OnEditCameraGain()
{
	// TODO: 在此添加命令处理程序代码
	//CMFCRibbonEdit* pE = DYNAMIC_DOWNCAST(
	//	CMFCRibbonEdit, m_wndRibbonBar.FindByID(ID_EDIT_CAMERA_GAIN));

	//CString text = pE->GetEditText();
	//double v = _wtof(text);

	//UCI::pDis->pCAM->SetGain(v);

	
}

