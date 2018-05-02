#include "stdafx.h"
#include "mainfrm.h"
#include "PictureView.h"
#include "Resource.h"
#include "CMV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif


/////////////////////////////////////////////////////////////////////////////
// CFileView

CPictureView::CPictureView()
{
}

CPictureView::~CPictureView()
{
	//delete pIPWinStatic;
	//delete pFGWinStatic;
}

BEGIN_MESSAGE_MAP(CPictureView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//ON_COMMAND(ID_PROPERTIES, OnProperties)
	//ON_COMMAND(ID_OPEN, OnFileOpen)
	//ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	//ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	//ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	//ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	//ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CPictureView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	

	CRect rect;
	GetClientRect(&rect);



	//CRect rectDummy;
	//rectDummy.SetRectEmpty();

	//// 创建视图: 
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	//if (!m_wndFileView.Create(dwViewStyle, rectDummy, this, 4))
	//{
	//	TRACE0("未能创建图片缩略视图\n");
	//	return -1;      // 未能创建
	//}

	//// 加载视图图像: 
	//m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	//m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	//m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	//m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	//FillFileView();
	AdjustLayout();

	return 0;
}

void CPictureView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPictureView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndFileView;
	//ASSERT_VALID(pWndTree);

	//if (pWnd != pWndTree)
	//{
	//	CDockablePane::OnContextMenu(pWnd, point);
	//	return;
	//}

	//if (point != CPoint(-1, -1))
	//{
	//	// 选择已单击的项: 
	//	CPoint ptTree = point;
	//	pWndTree->ScreenToClient(&ptTree);

	//	UINT flags = 0;
	//	HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
	//	if (hTreeItem != NULL)
	//	{
	//		pWndTree->SelectItem(hTreeItem);
	//	}
	//}

	//pWndTree->SetFocus();
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CPictureView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	if (UCI::pDis == NULL) return;

	//CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	//ST_MYDISP *pDis = UCI::pDis;

	double dRatio = UCI::OPTIONS[L"图像宽长比"];

	if (dRatio > 2.0f) return;

	if (UCI::pDis->GetpFGWinStatic() == NULL) return;

	CRect rect = rectClient;

	int gap = 2;
	int gapBetweenPic = 10;

	rect.top += gap;
	rect.left += gap;
	rect.right -= gap;

	
	
	int width = rect.Width();

	//double dRatio = UCI::OPTIONS[L"图像宽长比"];

	int height = (int)(width * dRatio);

	if (height < 50) return;

	rect.bottom = height;
	rect.NormalizeRect();	

	UCI::pDis->GetpFGWinStatic()->MoveWindow(rect, TRUE);

	UCI::pDis->m_pHWinFG->SetWindowExtents(0, 0, rect.Width(), rect.Height());
//	UCI::pDis->m_pHWinFG->SetPart(0, 0, rect.Width()-1, rect.Height()-1);

	Invalidate(TRUE);   // 刷新一下本窗口	
}

void CPictureView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectClient;
	GetClientRect(rectClient);

	dc.FillRect(rectClient, &afxGlobalData.brWindow);

	//CRect rectTree;
	//m_wndFileView.GetWindowRect(rectTree);
	//ScreenToClient(rectTree);

	//rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectClient, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CPictureView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	//m_wndFileView.SetFocus();
}

void CPictureView::OnChangeVisualStyle()
{
	//m_wndToolBar.CleanUpLockedImages();
	//m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	//m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("无法加载位图: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

}

void CPictureView::InitPictureStatic(ST_MYDISP * pD)
{

	CStatic* pFG = new CStatic();

	CRect rect = { 0,0,200,200 };

	pFG->Create(L"缩略图像", WS_CHILD | WS_VISIBLE | SS_GRAYRECT | SS_NOTIFY, rect, this, IDC_FGWinStatic);

	pD->SetpFGWinStatic(pFG);

}
