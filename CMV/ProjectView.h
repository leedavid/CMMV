
#pragma once

#include "ProjectTree.h"
#include "MyDispLay.h"

class CFileViewToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CProjectView : public CDockablePane
{
// 构造
public:
	CProjectView();

	//UCI::OptionsMap* getCurProperyMap() { return this->m_wndProjectTree.CurProp; };
	//void setCurProperyMap(UCI::OptionsMap* o) { this->m_wndProjectTree.CurProp = o; };


	/*int GetCurElementPropertyIndex() { return this->m_wndProjectTree.curElementPropertyIndex; };
	void SetCurElementPropertyIndex(int cur) { this->m_wndProjectTree.curElementPropertyIndex = cur; };*/

	HTREEITEM GetCurrentSelHTREEITEM();        // 返回当前选择的叶节点
	//int GetCurrentSelElementID();              // 得到当前选择的元素的ID

	void AdjustLayout();
	void OnChangeVisualStyle();

	int EmptyProjectTree(int prID);           // 清空项目树
	int UpdataProjectTreeByList();
	UCI::OptionsMap* AddOneElementToProjectTree(UCI::OptionsMap* o);

// 特性
protected:

	CProjectTree m_wndProjectTree;
	CImageList m_ProjectViewImages;
	CFileViewToolBar m_wndToolBar;

public:
	void FillProjectTreeView();

// 实现
public:
	virtual ~CProjectView();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnProjectOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnProjectRename();

	//void OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult);
	//void OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult);

	afx_msg void OnProjectTreePaste();
};

