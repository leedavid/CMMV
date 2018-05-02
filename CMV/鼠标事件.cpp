// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "CMV.h"
#include "ChildView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace HalconCpp;

// 鼠标滚轮
//
//BOOL CChildView::OnMouseWheel2(UINT nFlags, short zDelta, CPoint pt)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	
//	
//
//	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
//}


//// 左键单击
//
//void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	//CPoint pt;
//	//GetCursorPos(&pt);
//	//ScreenToClient(&pt);
//
//	//int num;
//	//if (this->MouseInWhichDISP(point, num)) {
//
//	//	this->DISP[num].DisOnMouseLeftDown(nFlags, point);
//	//}
//
//	DISP.DisOnMouseLeftDown(nFlags, point);
//
//	CWnd::OnLButtonDown(nFlags, point);
//}
//
//// 左键双击
//
//void CChildView::OnLButtonDblClk(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	//int num;
//	//if (this->MouseInWhichDISP(point, num)) {
//
//	//	if (this->eScreenMode == ScreenOneLarge) {
//	//		if (this->iScreenWhich == num) {
//	//			this->eScreenMode = ScreenALlSame;
//	//		}
//	//	}
//	//	else {
//	//		this->eScreenMode = ScreenOneLarge;
//	//	}
//
//	//	this->iScreenWhich = num;
//	//	DISP[num].bMouseLBdown = false;
//
//	//	this->FitImageToClient();
//	//}
//
//
//	CWnd::OnLButtonDblClk(nFlags, point);
//}
//
//// 鼠标移动
//
//void CChildView::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	// CClientDC DC(this);
//
//	//int num;
//	//if (this->MouseInWhichDISP(point, num)) {
//	//	this->DISP[num].DisOnMouseMove(nFlags, point);
//	//}
//
//	CWnd::OnMouseMove(nFlags, point);
//}
//
//// 右键上升
//
//void CChildView::OnRButtonUp(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//
//	//int num;
//	//if (this->MouseInWhichDISP(point, num)) {
//	//	this->DISP[num].DisOnRButtonUp(nFlags, point);
//	//}
//
//	CWnd::OnRButtonUp(nFlags, point);
//}