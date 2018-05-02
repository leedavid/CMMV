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

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "CMV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace HalconCpp;
using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd()
{
	m_nComboHeight = 0;
	CurOption = nullptr;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_CBN_SELCHANGE(ID_PROP_COMBOX, OnPropCommboxSelChange)
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
	//ON_COMMAND(ID_VIEW_PROPERTIESWND, &CPropertiesWnd::OnViewPropertieswnd)
	//ON_UPDATE_COMMAND_UI(ID_VIEW_PROPERTIESWND, &CPropertiesWnd::OnUpdateViewPropertieswnd)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 消息处理程序

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == NULL || (AfxGetMainWnd() != NULL && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(NULL, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建组合: 
	//const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, ID_PROP_COMBOX))
	{
		TRACE0("未能创建属性组合 \n");
		return -1;      // 未能创建
	}
	
	m_wndObjectCombo.AddString(_T("属性设置"));
	//m_wndObjectCombo.AddString(_T("相机参数"));	
	m_wndObjectCombo.AddString(_T("应用程序"));	

	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("未能创建属性网格\n");
		return -1;      // 未能创建
	}

	SetPropListFont();                             // 设置一下字体

	m_wndPropList.EnableHeaderCtrl(FALSE);		   // 隐藏表头
	m_wndPropList.EnableDescriptionArea();		   // 允许说明栏
	m_wndPropList.SetVSDotNetLook();               // 外观
	m_wndPropList.MarkModifiedProperties();        // 加粗更改的项

	// InitPropList(); 这个是MS的例程

	// 属性表工具条
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 已锁定*/);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 锁定*/);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}



void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}





//void CPropertiesWnd::InitPropListold()
//{
//	SetPropListFont();
//
//	m_wndPropList.EnableHeaderCtrl(FALSE);
//	m_wndPropList.EnableDescriptionArea();
//	m_wndPropList.SetVSDotNetLook();
//	m_wndPropList.MarkModifiedProperties();
//
//	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("外观"));
//
//	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("三维外观"), (_variant_t) false, _T("指定窗口的字体不使用粗体，并且控件将使用三维边框")));
//
//	CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("边框"), _T("对话框外框"), _T("其中之一: “无”、“细”、“可调整大小”或“对话框外框”"));
//	pProp->AddOption(_T("无"));
//	pProp->AddOption(_T("细"));
//	pProp->AddOption(_T("可调整大小"));
//	pProp->AddOption(_T("对话框外框"));
//	pProp->AllowEdit(FALSE);
//
//	pGroup1->AddSubItem(pProp);
//	pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("标题"), (_variant_t) _T("关于"), _T("指定窗口标题栏中显示的文本")));
//
//	m_wndPropList.AddProperty(pGroup1);
//
//	CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("窗口大小"), 0, TRUE);
//
//	pProp = new CMFCPropertyGridProperty(_T("高度"), (_variant_t) 250l, _T("指定窗口的高度"));
//	pProp->EnableSpinControl(TRUE, 50, 300);
//	pSize->AddSubItem(pProp);
//
//	pProp = new CMFCPropertyGridProperty( _T("宽度"), (_variant_t) 150l, _T("指定窗口的宽度"));
//	pProp->EnableSpinControl(TRUE, 50, 200);
//	pSize->AddSubItem(pProp);
//
//	m_wndPropList.AddProperty(pSize);
//
//	CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("字体"));
//
//	LOGFONT lf;
//	CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
//	font->GetLogFont(&lf);
//
//	_tcscpy_s(lf.lfFaceName, _T("宋体, Arial"));
//
//	pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("字体"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("指定窗口的默认字体")));
//	pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("使用系统字体"), (_variant_t) true, _T("指定窗口使用“MS Shell Dlg”字体")));
//
//	m_wndPropList.AddProperty(pGroup2);
//
//	CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("杂项"));
//	pProp = new CMFCPropertyGridProperty(_T("(名称)"), _T("应用程序"));
//	pProp->Enable(FALSE);
//	pGroup3->AddSubItem(pProp);
//
//	CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("窗口颜色"), RGB(210, 192, 254), NULL, _T("指定默认的窗口颜色"));
//	pColorProp->EnableOtherButton(_T("其他..."));
//	pColorProp->EnableAutomaticButton(_T("默认"), ::GetSysColor(COLOR_3DFACE));
//	pGroup3->AddSubItem(pColorProp);
//
//	static const TCHAR szFilter[] = _T("图标文件(*.ico)|*.ico|所有文件(*.*)|*.*||");
//	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("图标"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("指定窗口图标")));
//
//	pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("文件夹"), _T("c:\\")));
//
//	m_wndPropList.AddProperty(pGroup3);
//
//	CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("层次结构"));
//
//	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("第一个子级"));
//	pGroup4->AddSubItem(pGroup41);
//
//	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("第二个子级"));
//	pGroup41->AddSubItem(pGroup411);
//
//	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 1"), (_variant_t) _T("值 1"), _T("此为说明")));
//	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 2"), (_variant_t) _T("值 2"), _T("此为说明")));
//	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("项 3"), (_variant_t) _T("值 3"), _T("此为说明")));
//
//	pGroup4->Expand(FALSE);
//	m_wndPropList.AddProperty(pGroup4);
//}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::OnPropCommboxSelChange()
{
	//int num = this->m_wndObjectCombo.GetCurSel();
	this->InitPropList();
}

void CPropertiesWnd::InitPropList()
{
	int num = this->m_wndObjectCombo.GetCurSel();
	
	this->m_wndPropList.RemoveAll();               // 移除原来的所有数据
	switch (num) {
	case PROPLIST_OTHER:
		InitPropList_CurProp();                   // 显示一组属性表
		break;
	case PROPLIST_GLOBAL:
		InitPropList_Common();                    // 显示全局变量
		break;
	default:
		break;
	}
	this->m_wndPropList.Invalidate();    // 重新刷新一下
}

void CPropertiesWnd::SetCurList(ePROPLIST e)
{
	this->m_wndObjectCombo.SetCurSel(e);
}


// 全局变量设置属性表
void CPropertiesWnd::InitPropList_Common() {

	
	if (CurOption == NULL) return;
	NodeCAMProperty* pn = new NodeCAMProperty(UCI::OPTIONS, PROPRETY_GLOBAL);
	m_wndPropList.AddProperty(pn);
}

void CPropertiesWnd::InitPropList_CurProp()
{
	if (CurOption == NULL) return;
	NodeCAMProperty* pn = new NodeCAMProperty(*CurOption, PROPRETY_ELEMENT);
	m_wndPropList.AddProperty(pn);
}

//// 相机参数列表
//void CPropertiesWnd::InitPropList_Cammer()
//{
//	CMFCPropertyGridProperty* pProp;
//
//	////////////////////////////////////////////////////////////////////////////////////////
//
//	CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("相机参数组选择"));
//	// 取得当前参数名称
//	int camID = UCI::OPTIONS[L"CAMCurGroupID"];          // 当前相机参数
//	UCI::OptionsMap om = UCI::CamOptions[camID-1];
//	std::string sCamGroupName = om[L"CamGroupName"];
//
//	pProp = new CMFCPropertyGridProperty(_T("当前相机参数组"), CA2T(sCamGroupName.c_str()).m_psz,
//		_T("请选择一个相机参数组"), CMAERA_CAM_GROUP);
//
//	for (int i = 0; i < MAX_CAM_PARA_GROUP; i++) {
//		om = UCI::CamOptions[i];
//		std::string sCamGroupName = om[L"CamGroupName"];
//		pProp->AddOption(CA2T(sCamGroupName.c_str()).m_psz);
//	}
//
//	pProp->AllowEdit(FALSE);
//	pGroup1->AddSubItem(pProp);
//
//	m_wndPropList.AddProperty(pGroup1);
//
//	////////////////////////////////////////////////////////////////////////////////////////
//	/// 所有参数组名列表
//
//	om = UCI::CamOptions[camID - 1];  // 当前相机参数
//	NodeCAMProperty* pn = new NodeCAMProperty(om, PROPRETY_CAMMRA);
//	m_wndPropList.AddProperty(pn);
//
//}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}



