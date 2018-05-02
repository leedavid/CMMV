#pragma once

#ifndef UCI_H_INCLUDED
#define UCI_H_INCLUDED

#include "Common.h"
#include <fstream>
#include <sstream>
#include <xstring>
#include <afxtempl.h>

#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"

#include <map>

class CMainFrame;
class CProjectTree;
class ST_MYDISP;

namespace UCI {

	class Option;
	class ST_USER;
	class SQLBASE;
	class RIBBON_MENU;
	class stElement;

	/// Custom comparator because UCI options should be case insensitive
	//struct CaseInsensitiveLess {
	//	bool operator() (const std::string&, const std::string&) const;
	//};

	/// Our options container is actually a std::map
	typedef std::map<CString, Option> OptionsMap;


	class RIBBON_MENU {

	public:

		RIBBON_MENU();

		bool Rib_CALI_CAPTURE;                                // 标定图像时，是否可抓图了
		bool bIsInROIdrawing;                                 // 当前是不是在 ROI 绘图中

		eCOMBO_MATCH_ROI_TYPE iCOMBO_MATCH_ROI_TYPE;          // 当前的类型
	};

	class ST_PROJECT
	{
	public:

		ST_PROJECT(int id) { this->ID = id; };

		int ID;
		std::string ProjectName;
		std::string ProjectNote;
		std::string datatime;
	};

	class SQLBASE {


	public:
		//SQLBASE(std::string dbname);

	public:
		void SetDBName(std::string name);
		int MyNewProject(CString name, int& NewProjectID);                     // 在数据库中新建一个测量项目
		int MyProjectRename(CString newName, int NewProjectID);                // 

		//////////////////////////////////////////////////////////////
		/// 元素类数据库操作
		int MyResetProjectElements(int p);                                      // 把当前项目的所有元素删除，并重写入
		int RecordOneProdcut(int& productID);                                   //
		int RecordOneProdcutNotUpDateGUI(int& productID);                                   //
		int GetProductTotalAndGoodBy(int PrID, int start, int& total, int &good);
		int MySaveAllElement();                                                 // 
		int MySaveAllElement(int prID, stElement* e);
		int MySaveOneElement(OptionsMap& o, int);                               // 保存一个元素到元素表中去
		int MySaveOneElement(OptionsMap& o, int, SQLite::Database* pdb);

		int MyLoadElementFromDatabase(int prID);                                // 读出项目中的元素
		int ClearProductResutlByProjectID(int prID); 
		int MyReadSettingFromSqlite(CString  CamName);                          // 读出对应相机的当前全局设置
		int ProjectFillByID(ST_PROJECT* pro);                                   // 根据数据库填充项目信息		
		int MySQLFillProjectTree(CProjectTree * cvt);                           // 
		int MySaveSettingToSqlite(OptionsMap& om);                              // 保存全局参数
	
		int SetOneCamePara(CString Name, UCI::Option o);
		BOOL isCamParaCanOut(CString Name);                                     // 相机的参数可不可以输出
		int SetCamPara(OptionsMap& camo);                                       // 根据当前的相机的参数组的ID，调整相机	
		int DelAllElement(int prID);                                            // 删除所有的元素
		BOOL isCanlogin(CString name, CString pass);
		int GetLastProductTestNumber();
		void PrepareCamDeviceCatalog(int prID);                                  // 更换相机，或新建测量项目时，得准备相应的目录

		int Log(CString log, eLogType e = LOG_LOG);                              // 保存一条日志文件
		int Log(HalconCpp::HException& except, CString str);                     // 读录一个 HALCON 的错误
		int DeleteProject(int prID);                                             // 删除一个项目

		int MyFillUser(CList <CString, CString> *User);
		int GetUserInfoByName(ST_USER* pUser);
		int UpdateUserInfoBy(ST_USER* pUser);
		int AddUser(ST_USER* pUser);
		int DeleteUserByName(ST_USER* pUser);

		//SQLite::Database* db; // (this->dbname, SQLite::OPEN_READWRITE);

	private:

		

		std::string dbname;

		int SaveOneProductResult(int &ProductID);                              // 保存一个产品测量记录
		int UpdateOneProductResult(int ProductID, BOOL result);                // 更新这个产品的测量结果
		int SaveOneElementResult(int ProductID, int index, OptionsMap& ores);

		
		int DeleteOneELementByName(CString name);  
		int DeleteOneGobalParaByName(CString name);

	};




	
	/// Option class implements an option as defined by UCI protocol
	class Option {

		typedef void(*OnChange)(Option&);     // 值改变了，要调用相应的函数去处理一下

	public:
		
		Option(OnChange = nullptr);
		// Option(bool v, OnChange = nullptr);	
		Option(CString sname, const CString v, const CString note = L"Paramter Note",bool show = true, OnChange = nullptr);
		Option(CString sname, int v, int min, int max, const CString note = L"Paramter Note", bool show = true, OnChange = nullptr);
		Option(CString sname, double v, double dmin, double dmax, const CString note = L"Paramter Note", bool show = true, OnChange = nullptr);

		//Option(CString sname, int v, int min, int max, bool iscout, const CString note = L"Paramter Note", bool show = true, OnChange = nullptr);
		//Option(CString sname, const CString v, bool iscout, const CString note = L"Paramter Note", bool show = true, OnChange = nullptr);
		//Option(CString sname, double v, double dmin, double dmax, bool iscout, const CString note = L"Paramter Note", bool show = true, OnChange = nullptr);

		Option& operator=(const CString&);
		Option& operator=(const int&);
		Option& operator=(const double&);
		Option& operator=(const HalconCpp::HTuple&);

		//Option& operator=(const std::string&, const std::string&);
		//Option& operator=(const int&, const std::string&);
		//Option& operator=(const double&, const std::string&);

		void operator<<(const Option&);
		operator int() const;
		operator double() const;
		operator CString() const;	
		operator HalconCpp::HTuple() const;

		
		size_t getIdx() { return this->idx; };
		CString getName() { return this->name; };
		CString getType() { return this->type; };
		CString getcurrentValue() { return this->currentValue; };
		CString getNote() { return this->explain; };
		int getimin() { return this->imin; };
		int getimax() { return this->imax; };
		bool getshow() { return this->bshow; };
		//bool getCamIsOut() { return this->camIsOut; };

		void setNote(const CString str) { this->explain = str; };

		static size_t insert_order;

	private:
		//friend std::ostream& operator<<(std::ostream&, const OptionsMap&);

		
		CString name, defaultValue, currentValue, type, explain;
		int imin, imax;
		double dmin, dmax;
		size_t idx;
		bool bshow;                // 是否显示
		//bool camIsOut = false;     // 是否要输出到相机参数中去
		OnChange on_change;        // 触发函数
	};

	class stElement {

	public:		
		//friend int SQLDATA::MyLoadElementFromDatabase();
		//friend int CProjectView::UpdataByList();

	public:

		OptionsMap* add_One_Element(int iProject, eELEMENT_TYPE);                          // 添加一个元素

		BOOL isELeListOKandDelBad(OptionsMap *o);

		int LoadElementFromDatabase(int prID);                                             // 从数据库中读出当前项目的元素
		int UpdateProjectByList();                                                         // 更新项目树的当前项目
		OptionsMap* initOptionMapElemnet(eELEMENT_TYPE t, int ProjectID);                  // 元素初始化
		int saveOneOptionsMap(OptionsMap& o, int index);                                   // 保存一个元素要数据库中去

		BOOL isHaveSaveElementName(CString str);                                            // 是否有相同的名称
		
		BOOL RemoveOneElement(CString stEleName);    
		OptionsMap *GetOptionsMapByEletype(eELEMENT_TYPE e);

		OptionsMap* GetOptionsMapByEleName(CString name);                                   // 通过元素名称查找元素
		POSITION GetOptionsPosByEleName(CString name);


		// 变量
	public:
		CList <OptionsMap, OptionsMap> EleList;                 // 测量元素   
	private:
		void initOptionMapElemnetLine(OptionsMap& o);
		void initOptionMapElemnetCircle(OptionsMap& o);
		void initOptionMapElemnetResult(OptionsMap& o);
		void initOptionMapElemnetAngle(OptionsMap& o);             // 检测二个直线元素的角度
		void initOptionMapElemnetCamera(OptionsMap& o);            // 初始化一个相机参数
		void initOptionMapElemnetDistance(OptionsMap& o);

		void initOptionMapFUNCTION(OptionsMap& o);
		void initOptionMapMATCH(OptionsMap& o);

		void initOptionMapElemnetMM(OptionsMap& o);
		void initOptionMapElemnetPeek(OptionsMap& o);
		void initOptionMapELEMENT_MEASURE_RECTANGLE2(OptionsMap& o);
		void initOptionMapBalloonFlaw(OptionsMap& o);
		void initOptionMapROI(OptionsMap& o);
		void initOptionMapBalloonLength(OptionsMap& o);
	};	

	void initFirst(CMainFrame* pm);              // 先初始一些基本的变量
	void initOptionsMap(OptionsMap&);

	int MySaveSettingToSqlite();
	int MyReadSettingFromSqlite();

	void GetHTupleFromString(HalconCpp::HTuple&, std::string&);
	/// =====================================================================
	/// 全局函数	

	void Speech(CString str, bool clear = false);
	void Speech(const char* cstr, bool clear = false); //{ Speech(CA2W(cstr).m_psz, clear); };
	void MyLog(CString str, bool isSave = true);
	void MyLog(const char* cst, bool isSave = true);	

	void MyLogStatus(CString str); 
	void MyLogWinHead(CString str);
	void MyLogWinHead(CString name, int id);

	void LoadCaliPoseAndParamter();           // 加载姿态及相机内参
	BOOL CopyCaliPoseAndParamterFile(int oldPr, int newPr);

	int FindSpecialFile(CString stPath,      // 查找当前目录指定后缀的文件数量
		CString stExt, CArray<CString, CString&>* caPathFile, bool isDel = false);

    CString Utf8ToTChar(const std::string& utf8);
	std::string TCharToUtf8(const CString& tchar);

	void InitEmptyProject(int prID);

	/// =====================================================================
	/// 设置变量改变时的处理函数
	void on_camera_para(Option&);

	/// =====================================================================
	/// Halcon 例程
	void action_wafer_dicing();					    // 4.1 半导体晶片切割
	void action_read_serila_number();				// 4.2 读取相机上的系列号
	void action_inspection_of_saw_blades();         // 4.3 读取相机上的系列号
	void action_pring_inspection();                 // 4.4 印刷检查
	void action_BGA_inspection();                   // 4.5 BGA 检测
	void action_sureface_inspection();              // 4.6 表面检测
	void action_park_plug_measuring();				// 4.7 电火花测量	
	void action_mold_flash_detection();             // 4.8 模制品披峰检测
	void action_puched_sheet_detection();           // 4.9 冲孔板检查
	void action_3D_reconstrucion();                 // 4.10 双目立体视觉三维平面重构
	void action_pose_verfication_of_res();          // 4.11 电阻姿态检验
	void action_classification_fabrics();           // 4.12 非织布分类
	

	void reopen_window_fit(HalconCpp::HObject ho_Image, HalconCpp::HTuple hv_MaxExtent, HalconCpp::HTuple hv_Row, HalconCpp::HTuple hv_Col,
		HalconCpp::HTuple *hv_WindowHandle);

	void init_font(HalconCpp::HTuple hv_WindowHandle, HalconCpp::HTuple hv_Font, HalconCpp::HTuple hv_Size);

	void dev_update_all(HalconCpp::HTuple hv_DisplayMode);

	void write_message(HalconCpp::HTuple hv_WindowHandle, HalconCpp::HTuple hv_Row, HalconCpp::HTuple hv_Column, HalconCpp::HTuple hv_String,
		HalconCpp::HTuple hv_WindowCoordinates);

	void get_shape_model_contour_ref(HalconCpp::HObject ho_ModelRegion, HalconCpp::HObject *ho_ModelContoursTrans,
		HalconCpp::HTuple hv_ModelID);

	void gen_arrow_contour_xld(HalconCpp::HObject *ho_Arrow, HalconCpp::HTuple hv_Row1, HalconCpp::HTuple hv_Col1, HalconCpp::HTuple hv_Row2,
		HalconCpp::HTuple hv_Col2, HalconCpp::HTuple hv_Size);

	void plot_tuple(HalconCpp::HTuple hv_WindowHandle, HalconCpp::HTuple hv_Tuple, HalconCpp::HTuple hv_TextXAxis, HalconCpp::HTuple hv_TextYAxis);

	extern BOOL bIsCanOperate(CString Operation);             // 

	extern stElement ELE;	

	extern OptionsMap OPTIONS;                                // 全局变量

	//extern OptionsMap OutCam;                                 // 是否输出到相机中的元素

	extern CMainFrame *pMain;
	extern CString strMoudlePath;                             // AppPath 安装目录
	//extern ST_USER User;                                    // 当前登录用户
	extern bool MSSpeedInitOK;

	extern ST_USER User;
	extern SQLBASE sDB;
	extern RIBBON_MENU RIBmenu;                              // 菜单显示用结构变量          

	extern ST_MYDISP* pDis;                                  // 显示窗口用结构变量;

	extern CString stPictureSavePath;                        // 图像保存的目录
	extern CString stShutterSavePath;                        // 多曝光切换文件保存目录

} // namespace UCI

 






#endif