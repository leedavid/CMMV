#pragma once
#include "afxwin.h"




// DlgUserMange 对话框

class DlgUserManage : public CDialogEx
{
	DECLARE_DYNAMIC(DlgUserManage)

public:
	DlgUserManage(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~DlgUserManage();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_User_Manage };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeComboUserlist();
	CComboBox CbUserList;
	CEdit CeUserName;
	CEdit CeRegTime;
	CEdit CeLogTime;
	CEdit CeUserNote;
	CButton CbIsAdminstrator;
	CButton CbIsInspector;
	CEdit CeOrgPassword;
	CEdit CeNewPass;
	CEdit CeRepeatPass;
	afx_msg void OnBnClickedChangepass();
	afx_msg void OnBnClickedAddUser();
	afx_msg void OnBnClickedDeleteUser();
};
