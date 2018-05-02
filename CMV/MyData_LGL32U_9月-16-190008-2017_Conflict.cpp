#include "stdafx.h"
#include "MainFrm.h"
#include "ChildView.h"
#include "ProjectView.h"
#include "AVTCam.h"
#include <algorithm>
#include <sstream>

#include "MyData.h"
#include "MyDispLay.h"
#include "afxdialogex.h"

using namespace HalconCpp;

using namespace std;
using std::string;




namespace UCI {

	// 全局变量
	//OptionsMap *CurProp;                        
	//OptionsMap CamOptions[MAX_CAM_PARA_GROUP];   // 相机的参数变量, 目前最多10个不同的参数
	OptionsMap OPTIONS;                          // Global object
	//OptionsMap OutCam;

	CMainFrame *pMain;
	ST_MYDISP* pDis;                         // 显示窗口用变量;
	CString strMoudlePath;                   // AppPath 安装目录
	bool MSSpeedInitOK = false;	

	CString stPictureSavePath;               // 图像保存的目录
	CString stShutterSavePath;               // 多曝光切换文件保存目录

	RIBBON_MENU RIBmenu;                     // RIBBON 菜单按钮
	SQLBASE sDB;
	ST_USER User;
	//BOOL CammerParaChanged;
	stElement ELE;                           // 
	//CArray<stROI_ELEMENT, stROI_ELEMENT> MyELEMENT;      //
	/// 'On change' actions, triggered by an option's value change
	//void on_clear_hash(const Option&) { Search::clear(); }
	//void on_hash_size(const Option& o) { TT.resize(o); }
	//void on_logger(const Option& o) { start_logger(o); }
	//void on_eval(const Option&) { Eval::init(); }
	//void on_threads(const Option&) { Threads.read_uci_options(); }

	/// 变量名称大小写不敏感处理
	/// Our case insensitive less() function as required by UCI protocol
	//bool CaseInsensitiveLess::operator() (const string& s1, const string& s2) const {

	//	return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(),
	//		[](char c1, char c2) { return tolower(c1) < tolower(c2); });
	//}


	//UCI::initFirst();
	//UCI::pMain = this;
	//UCI::sDB.SetDBName("CMdata.db");
	//UCI::init(Options);
	
	

	void initFirst(CMainFrame* pmain) {

		TCHAR modulePath[MAX_PATH];
		GetModuleFileName(NULL, modulePath, MAX_PATH);
		CString _strMoudlePath(modulePath);
		_strMoudlePath = _strMoudlePath.Left(_strMoudlePath.ReverseFind('\\'));
		strMoudlePath = _strMoudlePath;

		stPictureSavePath = strMoudlePath + L"\\" + L"Picture Save";		        // 图像保存目录
		stShutterSavePath = strMoudlePath + L"\\" + L"Multi Shutter";

		UCI::pMain = pmain;
		UCI::sDB.SetDBName("CMdata.db");
		UCI::initOptionsMap(OPTIONS);

		// 读出最后一次的相机型号
		CString INIfile = UCI::strMoudlePath + L"\\config.ini";
		CString lastCammer;
		GetPrivateProfileString(_T("SETTING"), L"Last_Camer", L"CM500",
			lastCammer.GetBuffer(64), 64, INIfile);

		UCI::sDB.MyReadSettingFromSqlite(lastCammer);        // 全局设定		

		int prID = UCI::OPTIONS[L"项目编号"];			
		ELE.LoadElementFromDatabase(prID);                // 从数据库中加载元素

		
	}
		
	void Speech(CString str, bool clear)
	{
		pMain->Speech(str, clear);
	}

	void MyLog(CString str, bool isSave)
	{
		//pMain->LogOut(str, isSave);

		CString *p_str = new CString(str);
		PostMessage(UCI::pMain->m_hWnd, UM_UPDATESTATUS, (WPARAM)isSave, (LPARAM)(p_str));
		
	}

	void Speech(const char* cstr, bool clear)
	{ 
		Speech(CA2W(cstr).m_psz, clear); 
	};

	void MyLog(const char * cstr, bool isSave )
	{
		CString str = CA2W(cstr);
		MyLog(str, isSave);		
	}

	void MyLogStatus(CString str)
	{
		pMain->StatusCommon(str);
	}

	void MyLogWinHead(CString str)
	{
		pMain->LogWinHead(str);
	}	

	void MyLogWinHead(CString name, int id)
	{
		CString st;
		st.Format(_T("当前项目名称：< %s > 项目编号：< %d >"), name, id);
		MyLogWinHead(st);
	}
	void LoadCaliPoseAndParamter()
	{
		pDis->LoadCaliPoseAndParamter();
	}

	BOOL CopyCaliPoseAndParamterFile(int oldPr, int newPr)
	{
		return pDis->CopyCaliPoseAndParamterFile(oldPr, newPr);
	}



	CString Utf8ToTChar(const std::string & utf8)
	{
		// ----------------------------------------
		// Convert UTF-8 to Unicode(UTF-16).
		// UTF-8をUnicode(UTF-16)に変換する
		// ----------------------------------------
		// First, get the size of the buffer to store Unicode chars.
		// まず、Unicode文字列を格納するバッファサイズを取得する
		INT bufsize = ::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
		WCHAR* utf16 = new WCHAR[bufsize];

		// Then, convert UTF-8 to Unicode.
		// UTF-8をUnicodeに変換する
		::MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, utf16, bufsize);

		// ----------------------------------------
		// Convert Unicode(UTF-16) to TCHAR
		// Unicode(UTF-16)をTCHARに変換する
		// ----------------------------------------
		// Convert Unicode(WCHAR) to TCHAR(MBCS or Unicode).
		// WCHARからCStringWまたはCStringAに変換する(プロジェクトの設定による)

		//int len = wcslen(utf16);

		CString ret(utf16, int(::wcslen(utf16)));
		delete[] utf16;

		return ret;
	}

	std::string TCharToUtf8(const CString& tchar)
	{
		// ----------------------------------------
		// Convert TCHAR to Unicode(UTF-16).
		// TCHARをUnicode(UTF-16)に変換する
		// ----------------------------------------
		// Convert TCHAR to Unicode.
		// TCHARをUniocodeに変換する
		CStringW utf16(tchar);

		// ----------------------------------------
		// Convert Unicode(UTF-16) to UTF-8.
		// Unicode(UTF-16)をUTF-8に変換する
		// ----------------------------------------
		// First, get the size of the buffer to store UTF-8 chars.
		// まず、UTF-8文字列を格納するバッファサイズを取得する
		INT bufsize = ::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, NULL, NULL);
		CHAR* utf8 = new CHAR[bufsize];

		// Then, convert Unicode to UTF-8.
		// 次にUTF-8をUnicodeに変換する
		::WideCharToMultiByte(CP_UTF8, 0, utf16, -1, utf8, bufsize, NULL, NULL);

		// At the end, create the std::string that contains the UTF-8 chars.
		// std::stringにいれて返す
		std::string ret(utf8, ::strlen(utf8));
		delete[] utf8;

		return ret;
	}

	// 添加三个初始元素
	void InitEmptyProject(int curProject)
	{
		UCI::ELE.add_One_Element(curProject, ELEMENT_CAMERA);
		UCI::ELE.add_One_Element(curProject, ELEMENT_FUNCTION);
		UCI::ELE.add_One_Element(curProject, ELEMENT_MATCH);
	}


	//                              //目标路径            后
	int FindSpecialFile(CString target_file,
		CString stExt, CArray<CString, CString&> *caPathFile, bool isDel)
	{
		int found_count = 0; //查找计数器  

		target_file += L"\\" + stExt;

		CFileFind find;
		BOOL ret = find.FindFile(target_file); //查找文件  
		while (ret)
		{
			ret = find.FindNextFile();
			if (find.IsDots()) //. 或 .. 文件  
			{
			}
			else if (find.IsDirectory()) //子目录  
			{
			}
			else //file  
			{
				found_count++; //搜索计数增加  

				CString stName = find.GetFilePath();
				CFile   TempFile;

				if (caPathFile != NULL) {
					caPathFile->Add(stName);
				}

				if (isDel) {
					TempFile.Remove(stName);
				}
			}
		}

		find.Close(); //关闭搜索  

		return(found_count); //返回搜索到数目  
	}
	
	/// Option class constructors and conversion operators

	Option::Option(CString sname, const CString v, const CString note, bool show, OnChange f) : type(L"string"), imin(0), imax(0), on_change(f)
	{
		name = sname;
		defaultValue = currentValue = v;
		explain = note;
		bshow = show;
	}


	Option::Option(OnChange f) : type(L"button"), imin(0), imax(0), on_change(f)
	{}

	Option::Option(CString sname, int v, int minv, int maxv, const CString note, bool show, OnChange f) : type(L"spin"), imin(minv), imax(maxv), on_change(f)
	{
		name = sname;

		CString strv;
		strv.Format(L"%d", v);

		defaultValue = currentValue = strv;
		explain = note;
		bshow = show;
	}

	Option::Option(CString sname, double v, double dminv, double dmaxv, const CString note, bool show, OnChange f) : type(L"double"), dmin(dminv), dmax(dmaxv), on_change(f)
	{
		name = sname;

		CString strv;
		strv.Format(L"%f", v);

		defaultValue = currentValue = strv;
		explain = note;
		bshow = show;
	}
	


	Option::operator int() const {
		assert(type == L"spin");		

		return (type == L"spin" ? _ttoi(currentValue) : currentValue == L"true");
	}

	Option::operator double() const
	{
		assert(type == L"double");
		return _ttof(currentValue);
	}

	Option::operator CString() const {
		assert(type == L"string");
		return currentValue;
	}

	Option::operator HalconCpp::HTuple() const
	{
		assert(type == L"string");

		HalconCpp::HTuple hv;


		int prev_pos = 0; 
		int pos = 0;

		hv.Clear();

		int a = currentValue.Find(L" ");

		while ((pos = currentValue.Find(L" ", pos))!= -1) {
			CString substring = currentValue.Mid(prev_pos, pos - prev_pos);
			double d = _ttof(substring);
			hv.Append(d);
			prev_pos = ++pos;
		}	
		
		return hv;
	}

	/// operator<<() inits options and assigns idx in the correct printing order

	void Option::operator<<(const Option& o) {
		*this = o;
		idx = insert_order++;
	}

	/// operator=() updates currentValue and triggers on_change() action. It's up to
	/// the GUI to check for option's limits, but we could receive the new value from
	/// the user by console window, so let's check the bounds anyway.

	Option& Option::operator=(const CString& v) {

		assert(!type.IsEmpty());	

		if ((type == "spin" && (_ttoi(v) < imin || _ttoi(v) > imax))
			|| (type == "double" && (_ttof(v) < dmin || _ttof(v) > dmax)))
			return *this;

		currentValue = v;

		if (on_change)
			on_change(*this);

		return *this;
	}

	Option& Option::operator=(const int& v) {
		assert(!type.IsEmpty());

		CString str;
		str.Format(L"%d", v);

		currentValue = str;

		if (on_change)
			on_change(*this);

		return *this;
	}

	Option& Option::operator=(const double& v) {
		assert(!type.IsEmpty());

		CString str;
		str.Format(L"%f", v);

		currentValue = str;

		if (on_change)
			on_change(*this);

		return *this;
	}

	Option & Option::operator=(const HalconCpp::HTuple & hv)
	{
		assert(!type.IsEmpty());

		currentValue.Empty();	

		for (int i = 0; i < hv.Length(); i++) {
			double d = hv[i].D();

			CString str;
			str.Format(L"%f", d);

			currentValue += (str + L" ");
		}

		if (on_change)
			on_change(*this);

		return *this;
	}


	// 相机参数有改变了， 直接发送到相机中去
	void on_camera_para(Option &o)
	{
		if (UCI::pDis == nullptr) return;
		
		if (UCI::pDis->pCAM->GetIsOpen()) {

			CString name = o.getName();
			CString val = o.getcurrentValue();
			CString type = o.getType();

			if (UCI::sDB.isCamParaCanOut(name)) {  // 这个参数可输出

				if (type == L"spin") {
					int v = _ttoi(val);
					UCI::pDis->pCAM->SetParameterHTuple(name, v);
				}
				else if (type == L"double") {
					double v = _ttof(val);
					UCI::pDis->pCAM->SetParameterHTuple(name, v);
				}
				else if (type == L"string") {
					UCI::pDis->pCAM->SetParameterHTuple(name, CT2A(val).m_psz);
				}
			}
		}
	}



	void GetHTupleFromString(HalconCpp::HTuple &t, std::string &s)
	{
		std::string::size_type prev_pos = 0, pos = 0;

		t.Clear();

		while ((pos = s.find(' ', pos)) != std::string::npos)
		{
			std::string substring(s.substr(prev_pos, pos - prev_pos));

			double d = stof(substring);
			t.Append(d);
			prev_pos = ++pos;
		}
	}

	BOOL bIsCanOperate(CString Operation)
	{
		if (UCI::User.islogin == FALSE) {
			MessageBox(0,L"请登录后再使用此功能!",0, MB_OK);
			return FALSE;
		}
		return TRUE;
	}


	inline RIBBON_MENU::RIBBON_MENU() {

		this->Rib_CALI_CAPTURE = false;
		this->bIsInROIdrawing = false;

		this->iCOMBO_MATCH_ROI_TYPE = ROI_UNITE;  // 并集
	}





	OptionsMap * stElement::add_One_Element(int iProject, eELEMENT_TYPE t)
	{
		OptionsMap* o = initOptionMapElemnet(t, iProject);

		o = pMain->MProjectTreeAddElement(o);   // 3. 添加到项目树中去
		pMain->SetPropertyCurOptionsMap(o);
		pMain->ProPertyUpdate();

		return o;
	}

	int stElement::LoadElementFromDatabase(int prID)
	{
		return sDB.MyLoadElementFromDatabase(prID);    


	}

	int stElement::UpdateProjectByList()
	{
		return pMain->MProjectTreeFillByPrID();
	}



	void SQLBASE::SetDBName(std::string name)
	{
		dbname = CT2A(UCI::strMoudlePath).m_psz;
		dbname += "\\";
		dbname += name;
	}




}





bool ST_MYDISP::SaveImgToFile(CString stSavePath, 
	CString stFormat, CString & stFileName)
{
	
	if (!PathIsDirectory(stSavePath)) {		
		SHCreateDirectoryEx(NULL, stSavePath, NULL);
	}
	
	stFileName = stSavePath + L"\\";
	CTime t = CTime::GetCurrentTime();
	CString s = t.Format(L"%Y%m%d%H%M%S");
	stFileName += s + L"." + stFormat;   // 以png格式保存图像

	//HObject ho_new;

	//// 多线程时要当心一下
	HImage GrabImage;
	EnterCriticalSection(&m_csFGDisp);      // CriticalSect
	GrabImage = this->GrabImageDisp;
	LeaveCriticalSection(&m_csFGDisp);      // CriticalSect
	
	try {
		GrabImage.WriteImage(CT2A(stFormat).m_psz, 0,
			CT2A(stFileName).m_psz);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"ST_MYDISP::SaveImgToFile：");
	}

	return true;
}

bool ST_MYDISP::SaveImgToROI()
{
	try {
		int prid = UCI::OPTIONS[L"项目编号"];
		CString str = UCI::strMoudlePath;

		CString format = UCI::OPTIONS[L"相像保存格式"];

		//str = str + "\\project\\" + to_string(prid) + "\\ROI\\roimatch." + format;
		//return str;

		CString stDeviceName = UCI::OPTIONS[L"相机名称"];

		CString strTmp;
		strTmp.Format(L"%d", prid);

		CString stSavePath = str + L"\\project\\" + strTmp
			+ L"\\ROI\\" + stDeviceName;


		if (!PathIsDirectory(stSavePath)) {
			SHCreateDirectoryEx(NULL, stSavePath, NULL);
		}

		CString stFileName = stSavePath + L"\\roimatch."
			+ format;

		//HImage HImage;
		//EnterCriticalSection(&m_csIPDisp);      // CriticalSect
		//GrabImage = this->GrabImageDisp;
		//HImage = this->GrabImageDisp;               // 保存已map过的图像
		//LeaveCriticalSection(&m_csIPDisp);      // CriticalSect


		// 保存原图
		this->GrabImageDisp.WriteImage(CT2A(format).m_psz, 0,
			CT2A(stFileName).m_psz);

		MyLog(L"保存图片成功了：" + stFileName);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"ST_MYDISP::SaveImgToROI()：");

		return false;
	}

	return true;
}

bool ST_MYDISP::ReadImageFromFile(CString stFileName)
{
	CT2A asc(stFileName);
	try {
		HTuple hv_ImageWidth, hv_ImageHeight;

		EnterCriticalSection(&m_csFGDisp);      // CriticalSect
		this->GrabImageDisp.ReadImage(asc.m_psz);
		this->GrabImageDisp.GetImageSize(&hv_ImageWidth, &hv_ImageHeight);
		LeaveCriticalSection(&m_csFGDisp);      // CriticalSect

		
		EnterCriticalSection(&this->m_csIPDisp);
		this->m_sIPResult.result_img = this->GrabImageDisp;
		LeaveCriticalSection(&this->m_csIPDisp);

		int iHeight = hv_ImageHeight.I();
		int iWidth = hv_ImageWidth.I();

		int oldW = UCI::OPTIONS[L"图像宽度"];
		int oldH = UCI::OPTIONS[L"图像高度"];

		if (iHeight != oldH || iWidth != iWidth) {

			UCI::OPTIONS[L"ORGwhByCamera"] = 0;
			UCI::OPTIONS[L"图像宽度"] = iWidth;
			UCI::OPTIONS[L"图像高度"] = iHeight;
		}
		

		CRect cr = { 0,0,iWidth - 1,iHeight - 1 };
		this->ChangeImageCRect(cr);

		this->m_pHWinFG->SetPart(0, 0, iHeight-1, iWidth-1);
		this->m_pMHWinMain->SetPart(0, 0, iHeight - 1, iWidth - 1);
	
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"ST_MYDISP::ReadImageFromFile：");

		return false;
	}

}

bool ST_MYDISP::LoadDefaultPicture()
{
	CString str = UCI::strMoudlePath + L"\\start.png";
	this->ReadImageFromFile(str);
	SetPartHvWnd();
	
	

	return true;
}

void ST_MYDISP::ChangeImageCRect(CRect rect)
{
	rect.NormalizeRect();
	if (rect.Width() < 10) return;

	double dRatio = UCI::OPTIONS[L"图像宽长比"];
	rect.bottom = rect.top + int(rect.Width()*dRatio);
	this->rcImage = rect;
}

void CPPExpDefaultExceptionHandler(const HalconCpp::HException & except)
{
	CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();

	CString str;

	// 获取 HALCON 错误信息
	str.Format(L"出错算子： s%, 错误信息： %s, ErrorCode: %d", 
		except.ProcName(), except.ErrorMessage(), except.ErrorCode());

	AfxMessageBox(str);

	throw except;
}

