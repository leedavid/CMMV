#pragma once
#include "afxwin.h"


// CDialogUserLog 对话框

class CDialogUserLog : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogUserLog)

public:
	CDialogUserLog(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDialogUserLog();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGING };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CEdit ID_UserName;
	CEdit ID_Password;
	//afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedOk();
	//afx_msg void OnBnSetfocusOk();
	virtual BOOL OnInitDialog();
	//afx_msg void OnBnClickedUserManage();
};
