// DialogUserLog.cpp : 实现文件
//

#include "stdafx.h"
#include "CMV.h"
#include "DialogUserLog.h"
#include "DlgUserMange.h"
#include "afxdialogex.h"
#include "MyData.h"
#include "MainFrm.h"


// CDialogUserLog 对话框

IMPLEMENT_DYNAMIC(CDialogUserLog, CDialogEx)

CDialogUserLog::CDialogUserLog(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_LOGING, pParent)
{

}

CDialogUserLog::~CDialogUserLog()
{
}

void CDialogUserLog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_USER_NAME, ID_UserName);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, ID_Password);
}


BEGIN_MESSAGE_MAP(CDialogUserLog, CDialogEx)
	//ON_WM_CREATE()
	ON_BN_CLICKED(IDOK, &CDialogUserLog::OnBnClickedOk)
	//ON_BN_SETFOCUS(IDOK, &CDialogUserLog::OnBnSetfocusOk)
	//ON_BN_CLICKED(IDOK_USER_Manage, &CDialogUserLog::OnBnClickedUserManage)
END_MESSAGE_MAP()


// CDialogUserLog 消息处理程序

//
//int CDialogUserLog::OnCreate(LPCREATESTRUCT lpCreateStruct)
//{
//	if (CDialogEx::OnCreate(lpCreateStruct) == -1)
//		return -1;
//
//	// TODO:  在此添加您专用的创建代码
//	//this->ID_UserName = (CEdit*)GetDlgItem(IDC_EDIT_USER_NAME);
//	//this->ID_Password = (CEdit*)GetDlgItem(IDC_EDIT_PASSWORD);
//
//
//
//	return 0;
//}


void CDialogUserLog::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	CString username, pass;
	this->ID_UserName.GetWindowText(username);
	this->ID_Password.GetWindowText(pass);

	if (UCI::sDB.isCanlogin(username, pass)) {
		UCI::User.Name = username;
		UCI::User.Password = pass;
		UCI::User.islogin = true;

		UCI::MyLog(username + L" 登录成功！");

		UCI::MyLogStatus(username + L" 登录成功！");

		UCI::OPTIONS[L"UserLogName"] = username;

		CTime time;
		time = CTime::GetCurrentTime();
		CString date = time.Format(L"%Y-%m-%d %H:%M:%S %W-%A");

		UCI::OPTIONS[L"UserLogDateTime"] = date;

		date = time.Format(L"%Y-%m-%d");

		UCI::OPTIONS[L"ProdutctBatchNumber"] = CString(L"CM"+date);

		int last = UCI::sDB.GetLastProductTestNumber();

		UCI::OPTIONS[L"ProdutctExportStartNo"] = last+1;

		UCI::pMain->StatusSetLogName(username);

		UCI::pDis->AppStateChangeTo(APP_READY);
	}
	else {

		MessageBox(L"对不起，口令不准确！");
		UCI::MyLog(username + L" 登录失败！");
		return;
	}
	
	CDialogEx::OnOK();
}


//void CDialogUserLog::OnBnSetfocusOk()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	std::string lastloguser = UCI::OPTIONS[L"UserLogName"];
//	this->ID_UserName.SetWindowText(CA2T(lastloguser.c_str()).m_psz);
//}


BOOL CDialogUserLog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CString lastloguser = UCI::OPTIONS[L"UserLogName"];
	this->ID_UserName.SetWindowText(lastloguser);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


//void CDialogUserLog::OnBnClickedUserManage()
//{
//	// TODO: 在此添加控件通知处理程序代码
//	DlgUserManage dlg(this);
//
//	if (dlg.DoModal() == IDOK) {
//
//		//CString userName;
//		//CString passWord;
//		//dlg.ID_UserName.GetWindowText(userName);
//		//dlg.ID_Password.GetWindowText(passWord);
//
//
//	}
//}
