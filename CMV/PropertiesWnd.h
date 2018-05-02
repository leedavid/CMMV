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

#pragma once

#include "MyData.h"

//class UCI::Option;
//class UCI::OptionsMap;

//#define OUT_Element_Name_ID               999
//#define OUT_Element_Note_ID               998
//#define OUT_Element_Name_Identify         997
//#define USE_LINE1_Name_Identify           996
//#define USE_LINE2_Name_Identify           995
//
//#define USE_Distance1_Name_Identify       994
//#define USE_Distance2_Name_Identify       993
//
//#define "引用元素1参数名"_ID    993
//#define USE_Distance2_Parament_Name_ID    991




class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
// 构造
public:
	CPropertiesWnd();

	void AdjustLayout();

// 特性
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;

// 实现
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnExpandAllProperties();
	afx_msg void OnUpdateExpandAllProperties(CCmdUI* pCmdUI);
	afx_msg void OnSortProperties();
	afx_msg void OnUpdateSortProperties(CCmdUI* pCmdUI);



	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	afx_msg void OnPropCommboxSelChange();
	DECLARE_MESSAGE_MAP()


	//void InitPropListold();
	
	//void InitPropList_Cammer();    // 相机参数调协
	void InitPropList_Common();    // 全局变量设置
	void InitPropList_CurProp();   // 当前属性显示

	void SetPropListFont();

	int m_nComboHeight;

private:

	LRESULT OnPropertyChangedGlobal(WPARAM wParam, LPARAM lParam);
	//LRESULT OnPropertyChangedCamera(WPARAM wParam, LPARAM lParam);
	LRESULT OnPropertyChangedProperty(WPARAM wParam, LPARAM lParam);

public:

	void InitPropList();

	void SetCurList(ePROPLIST e);

	UCI::OptionsMap* CurOption;          // 当前属性表的元素

	//afx_msg void OnViewPropertieswnd();
	//afx_msg void OnUpdateViewPropertieswnd(CCmdUI *pCmdUI);
};


#define AFX_PROP_HAS_LIST		0x0001  
#define AFX_PROP_HAS_BUTTON		0x0002  
#define AFX_PROP_HAS_SPIN		0x0004 

// 派生自己属性项
class NodeCAMProperty : public  CMFCPropertyGridProperty
{

	DECLARE_DYNAMIC(NodeCAMProperty)

public:

	// 相机参数节点 ePROPERY_TYPE
	// NodeCAMProperty(UCI::OptionsMap& om, CString groupName);

	NodeCAMProperty(UCI::OptionsMap& om, ePROPERY_TYPE pType);

	
private:
	void initGobleProperty(UCI::OptionsMap& om);
	//void initCameraProperty(UCI::OptionsMap& om);
	void initProtertyProperty(UCI::OptionsMap& om);

	void initProtertyCamera(UCI::OptionsMap& om);
	void initProtertyLine(UCI::OptionsMap& om);
	void initProtertyCircle(UCI::OptionsMap& om);
	void initProtertyPeek(UCI::OptionsMap& om);
	void initProtertyBalloonLength(UCI::OptionsMap& om);
	void initProtertyAngle(UCI::OptionsMap& om);
	void initProtertyDistance(UCI::OptionsMap& om);
	void initProtertyFUNCTION(UCI::OptionsMap& om);
	void initProtertyMATCH(UCI::OptionsMap& om);
	void initProtertyResult(UCI::OptionsMap& om);
	void initProtertyMM(UCI::OptionsMap& om);
	void initProtertyMEASURE_RECTANGLE2(UCI::OptionsMap& om);
	void initPropertyBalloonFlaw(UCI::OptionsMap& om);
	void initPropertyROI(UCI::OptionsMap& om);

	void insertCurrentPosCanUseElement(CMFCPropertyGridProperty* pProp, UCI::OptionsMap& om);   // 插入当前元素前面可引用的元素


	//节点ID号,节点坐标
	NodeCAMProperty(int ID, double x, double y, double z);
	BOOL HasButton() const;
};
