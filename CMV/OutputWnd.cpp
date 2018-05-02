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

#include "stdafx.h"

#include "OutputWnd.h"
#include "Resource.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// COutputBar

COutputWnd::COutputWnd()
{
}

COutputWnd::~COutputWnd()
{
}

BEGIN_MESSAGE_MAP(COutputWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

int COutputWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建选项卡窗口: 
	if (!m_wndTabs.Create(CMFCTabCtrl::STYLE_FLAT, rectDummy, this, 1))
	{
		TRACE0("未能创建输出选项卡窗口\n");
		return -1;      // 未能创建
	}

	// 创建输出窗格: 
	const DWORD dwStyle = LBS_NOINTEGRALHEIGHT | WS_CHILD | WS_VISIBLE | WS_HSCROLL | WS_VSCROLL;

	if (!m_wndOutputBuild.Create(dwStyle, rectDummy, &m_wndTabs, 2) ||
		!m_wndOutputDebug.Create(dwStyle, rectDummy, &m_wndTabs, 3) ||
		!m_wndOutputFind.Create(dwStyle, rectDummy, &m_wndTabs, 4))
	{
		TRACE0("未能创建输出窗口\n");
		return -1;      // 未能创建
	}

	if (!m_wndDataList.Create(WS_CHILD | WS_VISIBLE | WS_BORDER | LVS_REPORT, rectDummy,
		&m_wndTabs, 5)) 
	{
		TRACE0("未能创建ListCtrl窗口\n");
		return -1;      // 未能创建
	}
	else
	{
		m_wndDataList.InsertColumn(0, L"编号", LVCFMT_RIGHT, 60);
		m_wndDataList.InsertColumn(1, L"名称", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(2, L"测量值", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(3, L"标准值", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(4, L"下限", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(5, L"上限", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(6, L"判定结果", LVCFMT_RIGHT, 100);
		m_wndDataList.InsertColumn(7, L"备注", LVCFMT_RIGHT, 300);

		m_wndDataList.SetExtendedStyle(LVS_EX_GRIDLINES);

		//m_wndDataList.DeleteAllItems();
	}

	UpdateFonts();

	CString strTabName;
	BOOL bNameValid;

	// 将列表窗口附加到选项卡: 
	bNameValid = strTabName.LoadString(IDS_BUILD_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputBuild, strTabName, (UINT)0);

	bNameValid = strTabName.LoadString(IDS_OUTRESULT_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndDataList, strTabName, (UINT)1);
	
	bNameValid = strTabName.LoadString(IDS_DEBUG_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputDebug, strTabName, (UINT)2);
	bNameValid = strTabName.LoadString(IDS_FIND_TAB);
	ASSERT(bNameValid);
	m_wndTabs.AddTab(&m_wndOutputFind, L"其它", (UINT)3);

	// 使用一些虚拟文本填写输出选项卡(无需复杂数据)
	FillBuildWindow();
	FillDebugWindow();
	FillFindWindow();

	return 0;
}

void COutputWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);

	// 选项卡控件应覆盖整个工作区: 
	m_wndTabs.SetWindowPos (NULL, -1, -1, cx, cy, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void COutputWnd::AdjustHorzScroll(CListBox& wndListBox)
{
	CClientDC dc(this);
	CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

	int cxExtentMax = 0;

	for (int i = 0; i < wndListBox.GetCount(); i ++)
	{
		CString strItem;
		wndListBox.GetText(i, strItem);

		cxExtentMax = max(cxExtentMax, (int)dc.GetTextExtent(strItem).cx);
	}

	wndListBox.SetHorizontalExtent(cxExtentMax);
	dc.SelectObject(pOldFont);
}

void COutputWnd::FillBuildWindow()
{
	m_wndOutputBuild.AddString(_T("程序初始化成功！"));
	//m_wndOutputBuild.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputBuild.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillDebugWindow()
{
	//m_wndOutputDebug.AddString(_T("调试输出正显示在此处。"));
	//m_wndOutputDebug.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputDebug.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::FillFindWindow()
{
	//m_wndOutputFind.AddString(_T("查找输出正显示在此处。"));
	//m_wndOutputFind.AddString(_T("输出正显示在列表视图的行中"));
	//m_wndOutputFind.AddString(_T("但您可以根据需要更改其显示方式..."));
}

void COutputWnd::UpdateFonts()
{
	m_wndOutputBuild.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputDebug.SetFont(&afxGlobalData.fontRegular);
	m_wndOutputFind.SetFont(&afxGlobalData.fontRegular);
}

void COutputWnd::insertOneResult(UCI::OptionsMap * o)
{	
	CListCtrl* CL = &m_wndDataList;

	int column = 1;
	CString str;

	int count = CL->GetItemCount();

    str.Format(L"%d", count+1);
	CL->InsertItem(count, str);

	CString name = (*o)[L"报表列名称"];
	CL->SetItemText(count, column++, name);

    double dRes = (*o)[L"输出的测量值"];
	str.Format(L"%.3lf", dRes);
	CL->SetItemText(count, column++, str);

	dRes = (*o)[L"输出的标准值"];
	str.Format(L"%.3lf", dRes);
	CL->SetItemText(count, column++, str);

	double dLow = (*o)[L"输出的下公差"];
	str.Format(L"%.3lf", dRes + dLow);
	CL->SetItemText(count, column++, str);

	double dUpper = (*o)[L"输出的上公差"];
	str.Format(L"%.3lf", dRes + dUpper);
	CL->SetItemText(count, column++, str);

	CString stRes = (*o)[L"输出测量结论"];
	CL->SetItemText(count, column++, stRes);

	CString stNote = (*o)[L"元素备注"];
	CL->SetItemText(count, column++, stNote);
}

//TCHAR * ColTitle[num] = { L"编号",L"名称",L"测量值",L"标准值",L"上公差",L"下公差",L"判定结果",L"备注" };
//int width[num] = { 60,60,100,100,100,100,100,200 };

void COutputWnd::OnViewOutput()
{
	if (this->IsVisible() == FALSE) {
		this->ShowPane(TRUE, FALSE, TRUE);
	}
	else {
		this->ShowPane(FALSE, FALSE, TRUE);
	}
}


/////////////////////////////////////////////////////////////////////////////
// COutputList1

COutputList::COutputList()
{
}

COutputList::~COutputList()
{
}

BEGIN_MESSAGE_MAP(COutputList, CListBox)
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	//ON_COMMAND(ID_VIEW_OUTPUTWND, OnViewOutput)
	ON_WM_WINDOWPOSCHANGING()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// COutputList 消息处理程序

void COutputList::OnContextMenu(CWnd* /*pCView*/, CPoint point)
{
	CMenu menu;
	menu.LoadMenu(IDR_OUTPUT_POPUP);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
		CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

		if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
			return;

		((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
		UpdateDialogControls(this, FALSE);
	}

	SetFocus();
}

void COutputList::OnEditCopy()
{
	MessageBox(_T("复制输出"));
}

void COutputList::OnEditClear()
{
	MessageBox(_T("清除输出"));
}

