

#include "stdafx.h"
#include "mainfrm.h"
#include "ProjectView.h"
#include "Resource.h"
#include "CMV.h"

#include "MyData.h"
#include "MyDispLay.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CFileView

CProjectView::CProjectView()
{
}

CProjectView::~CProjectView()
{
}

BEGIN_MESSAGE_MAP(CProjectView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnProjectOpen)
	//ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	//ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)           // 删除一个项目或删除一个测量元素
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	// ON_COMMAND(ID_PROJECT_RENAME, &CFileView::OnProjectRename)

	//ON_NOTIFY_REFLECT(TVN_BEGINLABELEDIT, OnBeginlabeledit)
	//ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, OnEndlabeledit)

	ON_COMMAND(ID_ProjectTreePaste, &CProjectView::OnProjectTreePaste)
END_MESSAGE_MAP()




////处理消息 TVN_BEGINLABELEDIT 
//void CParentWnd::OnBeginEditTree(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	TV_DISPINFO* pTVDI = (TV_DISPINFO*)pNMHDR;
//	if (pTVDI->item.lParam == 0);
//	//判断是否取消该操作
//	*pResult = 1;
//	else *pResult = 0;
//}
////处理消息 TVN_BEGINLABELEDIT 
//void CParentWnd::OnBeginEditTree(NMHDR* pNMHDR, LRESULT* pResult)
//{
//	TV_DISPINFO* pTVDI = (TV_DISPINFO*)pNMHDR;
//	if (pTVDI->item.pszText == NULL);
//	//判断是否已经取消取消编辑 
//	m_tree.SetItemText(pTVDI->item.hItem, pTVDI->pszText);
//	//重置显示字符 
//	*pResult = 0;
//}

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar 消息处理程序

int CProjectView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 创建视图: 
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS 
		| TVS_EDITLABELS;

	if (!m_wndProjectTree.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("未能创建文件视图\n");
		return -1;      // 未能创建
	}

	// 加载视图图像: 
	m_ProjectViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));

	//m_ProjectViewImages.Create(IDB_MEA_ELE_LARGE, 16, 0, RGB(255, 0, 255));
	m_wndProjectTree.SetImageList(&m_ProjectViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* 已锁定*/);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// 所有命令将通过此控件路由，而不是通过主框架路由: 
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	
	// 填入一些静态树视图数据(此处只需填入虚拟代码，而不是复杂的数据)
	FillProjectTreeView();
	AdjustLayout();

	return 0;
}

void CProjectView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CProjectView::FillProjectTreeView()
{
	UCI::sDB.MySQLFillProjectTree(&m_wndProjectTree);
}

void CProjectView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndProjectTree;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);
		return;
	}

	if (point != CPoint(-1, -1))
	{
		// 选择已单击的项: 
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

HTREEITEM CProjectView::GetCurrentSelHTREEITEM()
{
	return this->m_wndProjectTree.GetSelectedItem();
	//return HTREEITEM();
}

// 返回当前选择的元素ID
//int CProjectView::GetCurrentSelElementID()
//{
//	HTREEITEM hCurrentSel = this->m_wndProjectTree.GetSelectedItem();
//
//	if (hCurrentSel == NULL) {
//		return -1;         // 没有选择
//	}
//
//	HTREEITEM parent = m_wndProjectTree.GetParentItem(hCurrentSel);
//	if (parent == NULL) {   // 这个是根项目
//		return -1;
//	}	
//	CString stParentName = m_wndProjectTree.GetItemText(parent);
//	if (stParentName == L"测量项目") {
//			return -1;     // 这个项目
//	}
//	
//	return (int)this->m_wndProjectTree.GetItemData(hCurrentSel);
//
//}

void CProjectView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndProjectTree.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CProjectView::OnProperties()
{
	AfxMessageBox(_T("属性...."));

}

// 打开一个测量项目
void CProjectView::OnProjectOpen()
{
	HTREEITEM it = m_wndProjectTree.GetSelectedItem();

	CProjectTree* pTree = &this->m_wndProjectTree;

	HTREEITEM parent = pTree->GetParentItem(it);
	if (parent == nullptr) {   // 这个是根目录
		return;
	}

	if (pTree->GetParentItem(parent) == nullptr) {         // 这个是项目

		CString projectName = pTree->GetItemText(it);      // 项目名称

		int pold = UCI::OPTIONS[L"项目编号"];

		int prID = (int)pTree->GetItemData(it);              // 项目编号

		if (pold == prID) {
			return;
		}

		UCI::MyLogWinHead(projectName, prID);
		UCI::OPTIONS[L"项目编号"] = prID;

		UCI::ELE.LoadElementFromDatabase(prID);                // 从数据库中加载元素

		UCI::CopyCaliPoseAndParamterFile(pold, prID);          // 拷贝原来的相机姿态及内参到现在的文件夹来

		//UCI::LoadCaliPoseAndParamter();                        // 加载姿态及相机内参


		if (UCI::ELE.EleList.GetCount() == 0) {                 // 加入三个初始元素，相机，算子，匹配
			UCI::InitEmptyProject(prID);
		}
		UCI::pMain->MProjectTreeFillByPrID();                   // 初始化项目树
	}
}

void CProjectView::OnFileOpenWith()
{
	// TODO: 在此处添加命令处理程序代码
}

void CProjectView::OnDummyCompile()
{
	// TODO: 在此处添加命令处理程序代码
}

void CProjectView::OnEditCut()
{
	// TODO: 在此处添加命令处理程序代码
}

void CProjectView::OnEditCopy()
{
	// TODO: 在此处添加命令处理程序代码
	this->m_wndProjectTree.m_CurrentSelectItem = m_wndProjectTree.GetSelectedItem();

	// UCI::MyLog(L"当前已复制了一个项目！");
}


void CProjectView::OnProjectRename()
{
	// TODO: 在此添加命令处理程序代码
	//AfxMessageBox(_T("属性...."));

	HTREEITEM hItem = m_wndProjectTree.GetSelectedItem();

	//CString te = m_wndProjectView.GetItemText(hItem);

	//int a = 0;
	m_wndProjectTree.SetItemText(hItem, L"重命名");
}

// 删除一个项目,或删除一个元素
void CProjectView::OnEditClear()
{
	if (!UCI::pDis->TestCurrentAppState(APP_LOGGING_OK)) return;
	
	HTREEITEM hCurrentSel = m_wndProjectTree.GetSelectedItem();

	int prID = UCI::OPTIONS[L"项目编号"];

	if (m_wndProjectTree.GetChildItem(hCurrentSel) == NULL) {                                 // 

		// 没有展开的项目是也空的哇！！
		HTREEITEM parent = m_wndProjectTree.GetParentItem(hCurrentSel);
		if (parent != NULL) {
			CString stParentName = m_wndProjectTree.GetItemText(parent);
			if (stParentName == L"测量项目") {
				//UCI::MyLog(L"项目暂时不能删除！");

				int delPrID = (int)m_wndProjectTree.GetItemData(hCurrentSel);
				if (delPrID == prID) {
					MessageBox(L"当前项目不能删除！");
					return;
				}

				// 警告一下
				int res = ::MessageBox(NULL,
					L"点击 确定  删除！\n"
					"点击 取消  返回。",
					L"您正在删除选定的项目！",
					MB_OKCANCEL | MB_ICONSTOP);
				if (res == IDCANCEL) return;

				UCI::sDB.DeleteProject(delPrID);
				return; 
			}
		}

		// 得到元素的ELE编号
		CString stCurSelName = m_wndProjectTree.GetItemText(hCurrentSel);

		UCI::ELE.RemoveOneElement(stCurSelName);                // 删除一个元素

		this->UpdataProjectTreeByList();		
	}
}

void CProjectView::OnPaint()
{
	CPaintDC dc(this); // 用于绘制的设备上下文

	CRect rectTree;
	m_wndProjectTree.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CProjectView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndProjectTree.SetFocus();
}

void CProjectView::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* 锁定*/);

	m_ProjectViewImages.DeleteImageList();

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

	m_ProjectViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ProjectViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndProjectTree.SetImageList(&m_ProjectViewImages, TVSIL_NORMAL);
}

int CProjectView::EmptyProjectTree(int prID)
{
	// 查找当前项目树的节点
	HTREEITEM hProject = m_wndProjectTree.findProjectItemByPrID(prID);
	this->m_wndProjectTree.DelAllByPrID(prID);
	return TRUE;
}

// 刷新一个项目的所有节点
int CProjectView::UpdataProjectTreeByList()
{

	int prID = UCI::OPTIONS[L"项目编号"];

	// 查找当前项目树的节点
	HTREEITEM hProject = m_wndProjectTree.findProjectItemByPrID(prID);

	if (hProject != NULL) {

		this->m_wndProjectTree.DelAllByPrID(prID);

		//

		// 插入所有该项目的元素
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *o = &UCI::ELE.EleList.GetNext(pos);
			this->m_wndProjectTree.AddOneElement(o, NULL);
		}
	}
	
	return 0;
}

UCI::OptionsMap* CProjectView::AddOneElementToProjectTree(UCI::OptionsMap * o)
{
	// 1. 先到到当前选择的节点
	HTREEITEM hCurrentSel = this->m_wndProjectTree.GetSelectedItem();
	if (hCurrentSel != NULL) {
		HTREEITEM hParent = this->m_wndProjectTree.GetParentItem(hCurrentSel);
		if (hParent != NULL) {
			CString name = this->m_wndProjectTree.GetItemText(hParent);
			if (name == L"测量项目") { // 这是一个项目节点
			}
			else {  // 这是一个元素节点
				//POSITION pos = (POSITION)this->m_wndProjectTree.GetItemData(hCurrentSel);
				return this->m_wndProjectTree.AddOneElement(o, hCurrentSel);
			}
		}
	}


	return this->m_wndProjectTree.AddOneElement(o,NULL);
		

}

// 粘贴节点
void CProjectView::OnProjectTreePaste() // 粘贴一个节点，有可能是项目，有可能是一个元素
{
	// TODO: 在此添加命令处理程序代码
	if (this->m_wndProjectTree.m_CurrentSelectItem != NULL) {

		HTREEITEM it = this->m_wndProjectTree.m_CurrentSelectItem;
		CProjectTree* pTree = &this->m_wndProjectTree;

		HTREEITEM parent = pTree->GetParentItem(it);

		if (parent == nullptr) {   // 根目录不能复制
			return; 
		}	

		CString parentName = pTree->GetItemText(parent);
		if (parentName == L"测量项目") { // 项目节点
			
			CString stOld = pTree->GetItemText(it);

			int nImage, nSelectedImage;
			pTree->GetItemImage(it, nImage, nSelectedImage);

			DWORD_PTR id = pTree->GetItemData(it);

			UINT mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

			CString newName = stOld + L"复制";

			// 然后在数据库插入一个项目
			int NewProjectID;
			UCI::sDB.MyNewProject(newName, NewProjectID);

			pTree->InsertItem(mask, newName, nImage, nSelectedImage, 0, 0, NewProjectID, parent, it);

			// 把现在的元素保存到新项目中去
			UCI::stElement newELE;

			POSITION pos = UCI::ELE.EleList.GetHeadPosition();
			while (pos != NULL) {
				UCI::OptionsMap ot = UCI::ELE.EleList.GetNext(pos);				
				ot[L"项目编号"] = NewProjectID;
				newELE.EleList.AddTail(ot);
			}

			UCI::sDB.MySaveAllElement(NewProjectID, &newELE);
		}
		else {  // 元素节点

		}

	}
}
