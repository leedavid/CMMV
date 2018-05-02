// DlgUserMange.cpp : 实现文件
//

#include "stdafx.h"
#include "CMV.h"
#include "DlgUserMange.h"
#include "afxdialogex.h"
#include "MyData.h"

// DlgUserMange 对话框

IMPLEMENT_DYNAMIC(DlgUserManage, CDialogEx)

DlgUserManage::DlgUserManage(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_User_Manage, pParent)
{

}

DlgUserManage::~DlgUserManage()
{
}

void DlgUserManage::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_UserList, CbUserList);
	DDX_Control(pDX, IDC_User_Name, CeUserName);
	DDX_Control(pDX, IDC_User_RegTime, CeRegTime);
	DDX_Control(pDX, IDC_User_LastLogTime, CeLogTime);
	DDX_Control(pDX, IDC_User_Note, CeUserNote);
	DDX_Control(pDX, IDC_CHECK_isAdmin, CbIsAdminstrator);
	DDX_Control(pDX, IDC_CHECK_isInpector, CbIsInspector);
	DDX_Control(pDX, IDC_Org_Password, CeOrgPassword);
	DDX_Control(pDX, IDC_New_Password, CeNewPass);
	DDX_Control(pDX, IDC_Repeat_Password, CeRepeatPass);
}


BEGIN_MESSAGE_MAP(DlgUserManage, CDialogEx)
	ON_BN_CLICKED(IDOK, &DlgUserManage::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_UserList, &DlgUserManage::OnCbnSelchangeComboUserlist)
	ON_BN_CLICKED(ID_ChangePass, &DlgUserManage::OnBnClickedChangepass)
	ON_BN_CLICKED(IDOK_ADD_User, &DlgUserManage::OnBnClickedAddUser)
	ON_BN_CLICKED(ID_Delete_User, &DlgUserManage::OnBnClickedDeleteUser)
END_MESSAGE_MAP()


// DlgUserMange 消息处理程序


void DlgUserManage::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码



	CDialogEx::OnOK();
}


BOOL DlgUserManage::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	CList<CString, CString> User;

	UCI::sDB.MyFillUser(&User);    // 得到用户列表


	POSITION pos = User.GetHeadPosition();
	while (pos != NULL) {
		CString stName = User.GetNext(pos);
		this->CbUserList.AddString(stName);
	}

	// 


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void DlgUserManage::OnCbnSelchangeComboUserlist()
{
	// TODO: 在此添加控件通知处理程序代码
	UCI::ST_USER stUser;
	this->CbUserList.GetLBText(this->CbUserList.GetCurSel(), stUser.Name);	

	UCI::sDB.GetUserInfoByName(&stUser);

	this->CeUserName.SetWindowText(stUser.Name);

	this->CeLogTime.SetWindowText(stUser.stLastLogTime);
	this->CeRegTime.SetWindowText(stUser.stRegTime);
	this->CeUserNote.SetWindowText(stUser.Note);

	if (stUser.Permission > 1) {
		this->CbIsAdminstrator.SetCheck(TRUE);
	}
}


void DlgUserManage::OnBnClickedChangepass()
{
	UCI::ST_USER stUser;

	// 更改用户口令或备注
	CString stUserName;
	this->CeUserName.GetWindowText(stUser.Name);

	// 看一下用户存在不？ SQLITE_OK;
	

	if (SQLITE_OK != UCI::sDB.GetUserInfoByName(&stUser)) {
		MessageBox(L"对不起，用户不存在！");
		return;
	}

	CString Password;
	this->CeOrgPassword.GetWindowText(Password);

	if (Password != stUser.Password) {
		MessageBox(L"对不起，原始口令不准确！");
		return;
	}

	CString NewPass;
	CString RepeatPass;
	this->CeNewPass.GetWindowText(NewPass);
	this->CeRepeatPass.GetWindowText(RepeatPass);

	if (NewPass.GetLength() > 0) {
		if (NewPass != RepeatPass) {
			MessageBox(L"对不起，新口令与重复口令不一致！");
			return;
		}
	}

	stUser.Password = NewPass;
	this->CeUserNote.GetWindowText(stUser.Note);

	if (SQLITE_OK == UCI::sDB.UpdateUserInfoBy(&stUser)) {
		MessageBox(L"更新用户信息成功！");
	}
	else {
		MessageBox(L"更新用户信息失败！");
	}

}

// 添加用户
void DlgUserManage::OnBnClickedAddUser()
{
	// 判断是不是有相同用户名了
	UCI::ST_USER stUser;

	// 更改用户口令或备注
	this->CeUserName.GetWindowText(stUser.Name);

	if (SQLITE_OK == UCI::sDB.GetUserInfoByName(&stUser)) {
		MessageBox(L"相同的用户已注册，请更换用户名！");
		return;
	}

	CString NewPass;
	CString RepeatPass;
	this->CeNewPass.GetWindowText(NewPass);
	this->CeRepeatPass.GetWindowText(RepeatPass);

	if (NewPass.GetLength() == 0) {
		MessageBox(L"请输入口令！");
		return;
	}
	if (RepeatPass.GetLength() == 0) {
		MessageBox(L"请重复口令！");
		return;
	}


	if (NewPass != RepeatPass) {
		MessageBox(L"二个口令不一致，请重新输入口令！");
		return;
	}
	
	stUser.Password = NewPass;
	stUser.Permission = this->CbIsAdminstrator.GetCheck();
	this->CeUserNote.GetWindowText(stUser.Note);

	if (SQLITE_OK == UCI::sDB.AddUser(&stUser)) {
		MessageBox(L"增加用户成功！");
	}
	else {
		MessageBox(L"增加用户失败！");
	}
}

// 删除用户
void DlgUserManage::OnBnClickedDeleteUser()
{
	// TODO: 在此添加控件通知处理程序代码

	UCI::ST_USER stUser;

	// 更改用户口令或备注
	this->CeUserName.GetWindowText(stUser.Name);

	if (SQLITE_OK == UCI::sDB.DeleteUserByName(&stUser)) {
		MessageBox(L"删除用户成功！");
	}
	else {
		MessageBox(L"删除用户失败！");
	}
}
