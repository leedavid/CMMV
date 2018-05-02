// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

// ChildView.cpp : CChildView 类的实现
//

#include "stdafx.h"
#include "CMV.h"
#include "MainFrm.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace HalconCpp;
// CChildView

CChildView::CChildView()
{
	EnableActiveAccessibility();
}

CChildView::~CChildView()
{
	delete UCI::pDis;                     // 清除变量空间
}



BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_REGISTERED_MESSAGE(WM_FGDISP, OnFGDisp)
	ON_REGISTERED_MESSAGE(WM_IPDISP, OnMainDisp)
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()



// CChildView 消息处理程序

void CChildView::FitImageToClient()
{
	//CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();

	if (GetSafeHwnd() == NULL) // || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	if (UCI::pDis->GetpFGWinStatic() == NULL) {
		return;
	}

	CRect rect;
	GetClientRect(&rect);

	int gap = 1; //  pMain->MyData.iGap;
	double dRatio = UCI::OPTIONS[L"图像宽长比"];


	int w = rect.Width() - gap;
	int h = int(w*dRatio);
	int x = gap;
	int y = gap;

	CRect srect = { x,y,w,h };
	UCI::pDis->GetpMainStatic()->MoveWindow(srect, TRUE);

	UCI::pDis->m_pMHWinMain->SetWindowExtents(0, 0, srect.Width(), srect.Height()); // 只改变图像的显示大小，不改变图像的显示区域

	int width = UCI::OPTIONS[L"图像宽度"];
	int height = UCI::OPTIONS[L"图像高度"];

	UCI::pDis->m_pMHWinMain->SetPart(0, 0,  height - 1, width - 1);
}

//void CChildView::OnButtonFGStart()
//{
//	BOOL b_FGIsRunning;
//
//	EnterCriticalSection(&pDis->newImageMutex);
//	b_FGIsRunning = pDis->m_bFGIsRunning;
//	LeaveCriticalSection(&pDis->newImageMutex);
//
//	// Call back function to start the framegrabber
//	if (b_FGIsRunning == TRUE)
//	{
//		// already started
//		pDis->MyLog(L"FGIsRunning Already");
//		return;
//	}
//	else
//	{
//		//m_cFGStartButton.EnableWindow(FALSE);
//
//		if (this->m_hFGThrHandle)
//		{
//			WaitForSingleObject(m_hFGThrHandle, INFINITE);
//			CloseHandle(this->m_hFGThrHandle);
//			this->m_hFGThrHandle = NULL;
//		}
//
//		// start a new thread which openes a frame grabber and grabs continuously
//		m_hFGThrHandle = (HANDLE)_beginthreadex(NULL,  // do not suspend thread  
//			0,      // use same stack size as with main thread 
//			(unsigned(__stdcall *)(void*))&FGRun,  //unsigned ( __stdcall *start_address )( void * ) 
//			this,   // parameters          
//			0,      // start thread; don't suspend it       
//			&m_uiFGThreadId);/* Win32 thread id.; note that this is  */
//
//		// m_cFGStopButton.EnableWindow(TRUE);
//
//		pDis->MyLog(L"FG is Starting Now.");
//	}
//}

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 用于绘制的设备上下文
	
	// TODO: 在此处添加消息处理程序代码

	//UCI::pDis->DisDisplay();

	UCI::pDis->OnMainDisp(NULL, NULL);
	UCI::pDis->OnFGDisp(NULL, NULL);
	
	CWnd::OnPaint();
}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	//SetSystem("use_window_thread", "true");
	//SetWindowAttr("background_color", "gray");

	UCI::pDis = new ST_MYDISP(this);

	// 创建一个 CStatic, 用于显示主窗口图像
	CStatic* pCSMain = new CStatic();
	CRect rect = { 1,1, 300, 300 };
	pCSMain->Create(L"主窗口图像", WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_NOTIFY, rect, this, IDC_MainStatic);
	UCI::pDis->SetPMainStatic(pCSMain);

	//this->initCameraSetting();

	return 0;
}


void CChildView::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	//KillTimer(ID_CAMMER_CAP);

	//UCI::pDis->OnDestroy();

	CWnd::OnClose();
}


BOOL CChildView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	UCI::pDis->DisOnMouseWheel(nFlags, zDelta, pt);

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}


void CChildView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (cx != 0 && cx != this->iLastScreenWidth) {
		this->iLastScreenWidth = cx;
		this->FitImageToClient();
	}
}

LRESULT CChildView::OnFGDisp(WPARAM wParam, LPARAM lParam)
{
	return UCI::pDis->OnFGDisp(wParam, lParam);
}

LRESULT CChildView::OnMainDisp(WPARAM wParam, LPARAM lParam)
{
	return UCI::pDis->OnMainDisp(wParam, lParam);
}


// 初始化相机设置按钮
void CChildView::initRibbon_CameraSetting()
{
	CMFCRibbonBar* pRibbon =
		((CMDIFrameWndEx*)AfxGetMainWnd())->GetRibbonBar();
	ASSERT_VALID(pRibbon);

	CMFCRibbonComboBox* pCameraConType = DYNAMIC_DOWNCAST(
		CMFCRibbonComboBox, pRibbon->FindByID(ID_COMBO_CAMERA_CONNECT_TYPE));
	
	pCameraConType->AddItem(L"MindVision12_X64");
	pCameraConType->AddItem(L"DirectShow");
	//pCameraConType->AddItem(L"GigEVision");
	//pCameraConType->AddItem(L"File");
	//pCameraConType->AddItem(L"DahengCAM");

	
	pCameraConType->SelectItem(0);
	CString str = pCameraConType->GetItem(pCameraConType->GetCurSel());
	UCI::OPTIONS[L"相机协议"] = str;

	//HTuple CamName;
	HTuple CamInfo;
	HTuple CamValues;
	//InfoFramegrabber(asc.m_psz, "info_boards", &CamInfo, &CamValues);
	InfoFramegrabber(CT2A(str).m_psz, "device", &CamInfo, &CamValues);

	int a = (int)CamInfo.Length();     // 这个只有一条信息
	int b = (int)CamValues.Length();   // 

	CString info;
	info.Format(L"%S", CamInfo.SArr()[0]);
	UCI::MyLog(info);

	CMFCRibbonComboBox* pCameraConValue = DYNAMIC_DOWNCAST(
		CMFCRibbonComboBox,
		pRibbon->FindByID(ID_COMBO_CAMERA_NAME));

	for (int i = 0; i < b; i++) {
		CString sCamValues;
		HTuple t0 = CamValues[i];
		sCamValues.Format(L"%S", t0.SArr()[0]);
		pCameraConValue->AddItem(sCamValues);
	}

	pCameraConValue->SelectItem(1); // 默认 DirectShow
	str = CW2A(pCameraConValue->GetItem(pCameraConValue->GetCurSel()));
	UCI::OPTIONS[L"相机名称"] = str;

	// 软触发定时时间
	CMFCRibbonEdit* pE = DYNAMIC_DOWNCAST(
		CMFCRibbonEdit, 
		pRibbon->FindByID(ID_EDIT_CAMERA_TRIG_TIME));

	int t = UCI::OPTIONS[L"软触发定时"];

	CString item;
	item.Format(L"%d", t);
	pE->SetEditText(item);

}

//void CChildView::initCameraSetting()
//{
//	//OPTIONS[L"相机协议"] = "MindVision12_X64";
//	//OPTIONS[L"相机名称"] = "CM500";
//}


void CChildView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//switch (nIDEvent) {
	//case ID_CAMMER_CAP:
	//	UCI::pDis->DisOnTimer();
	//	break;
	//default:
	//	break;
	//}

	CWnd::OnTimer(nIDEvent);
}


void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//if (UCI::pDis->GetAppState() == APP_ATTACH_LINE) {
	//	int a = 0;
	//}

	CWnd::OnRButtonDown(nFlags, point);
}
