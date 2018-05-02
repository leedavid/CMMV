#pragma once
#include "AVTCam.h"
#include "sqlite3.h"

//#define MAX_CAM_PARA_GROUP 10

enum eCOMBO_MATCH_ROI_TYPE 
{
	ROI_UNITE,
	ROI_AND,
	ROI_SUB,
	ROI_XOR
};

enum eLogType
{
	LOG_LOG,
	LOG_HALCON_ERR,
	LOG_DATABASE_ERR,
	LOG_OTHER
};

enum eAppState
{
	APP_READY,                // 原始状态
	APP_LOGGING_OK,             // 登录成功了
	App_CamInner,             // 当前是标定相机 Inner Parameters 状态
	App_CamPose,              // 当前是标定相机 POSE 状态
	App_MoldMatching,         // 当前正在匹配
	App_ElementAdding,        // 当前正在添加元素
	App_ContinueingCap,       // 正在连续采集图片状态
	APP_MANUAL_MEASURING,      // 手动测量一次
	App_AutoMeasureing,       // 程序正在自动测量

	APP_ATTACH_LINE,          // 当前附加了直线了
	APP_ATTACH_CIRCLE,          // 当前附加了直线了
	APP_ATTACH_PEAK,          // 当前附加了直线了
	APP_ATTACH_MEASURE_RECTANGLE2,
	APP_ATTACH_BALLOON_FLAW,
	APP_ATTACH_ROI,

	App_Stat_End
};

static CString STATE_NAME[] =
{
	L"未登录",
	L"登录成功",
	L"内参标定",
	L"姿态标定",
	L"模型匹配",
	L"添加元素",
	L"连续采集",
	L"手动测量",
	L"自动测量"
};

enum ePROPERY_TYPE 
{
	PROPRETY_GLOBAL,             // 全局属性 
	//PROPRETY_CAMMRA,             // 相机属性
	PROPRETY_ELEMENT             // 元素属性
};



enum eELEMENT_TYPE
{
	ELEMENT_LINE,                     // 找直线      元素
	ELEMENT_CIRCLE,                   // 找圆        元素
	ELEMENT_RECT,                     // 找矩形      元素
	ELEMENT_ELLIPSE,                  // 找椭圆      元素
	ELEMENT_ANGLE,                    // 角度        元素
	ELEMENT_DISTANCE,                 // 距离
	ELEMENT_RESULT,                   // 输出报表    元素
	ELEMENT_CAMERA,                   // 相机参数    元素
	ELEMENT_MM,                       // 毫米
	ELEMENT_PEAK,                     // 找顶点元素
	ELEMENT_BALLOONLENGTH,            // 四顶点找球囊长度函数
	ELEMENT_FUNCTION,                 // 算子处理
	ELEMENT_MATCH,                    // 匹配元素
	ELEMENT_MEASURE_RECTANGLE2,       // 一维测量，矩形
	ELEMENT_BALLOON_FLAW,             // 球囊表面缺陷
	ELEMENT_ROI,                      // 全局ROI
	ELEMNET_END
};

static CString EleName[] = 
{
	L"直线",
	L"圆",
	L"矩形",
	L"椭圆",
	L"角度",
	L"距离",
	L"结果",
	L"相机",
	L"校正",
	L"顶点",
	L"球囊长度",
	L"算子",
	L"匹配",
	L"一维矩形",
	L"球囊表面缺陷",
	L"ROI"
	L"其它"
};

static int ELEMTEN_PIC_INDEX_NOSEL[]
{
	2,
	3,
	4,
	4,
	6,
	5,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	3
};

static int ELEMTEN_PIC_INDEX_SEL[]
{
	2,
	3,
	4,
	4,
	6,
	5,
	7,
	8,
	9,
	10,
	11,
	12,
	13,
	14,
	15,
	16,
	3
};

typedef struct CheckError
{
	int iErrorType;
	CString strErrorInfo;
}s_CheckError;


enum ePROP_ID
{
	OUT_Element_Name_ID = 1000,
	OUT_Element_Note_ID,
	OUT_Element_Name_Identify,
	USE_LINE1_Name_Identify,
	USE_LINE2_Name_Identify,
	USE_Distance1_Name_Identify,
	USE_Distance2_Name_Identify,
	USE_Distance1_Parament_Name_ID,
	USE_Distance2_Parament_Name_ID,
	OUT_Element_Name_IdentifyX,
	OUT_Element_Name_IdentifyY,
	OUT_Element_Name_IDX,
	OUT_Element_Name_IDY,

	USE_Peek1_Element_Name_ID,
	USE_Peek2_Element_Name_ID,
	USE_Peek3_Element_Name_ID,
	USE_Peek4_Element_Name_ID,

	ePROP_END
};

//enum ePROP_NAME
//{
//	CMAERA_CAM_GROUP = 1000
//};

enum ePROPLIST {
	PROPLIST_OTHER,
	//PROPLIST_CAMERA,	
	PROPLIST_GLOBAL,

	ePROPLIST_END
};

namespace UCI {

	class ST_USER
	{
	public:

		ST_USER() { islogin = false; };
	public:
		int ID;                                 //  
		CString Name;
		CString Password;
		int Permission;                         // 权限
		CString Note;
		BOOL islogin;                           // 是否登录成功

		CString stRegTime;
		CString stLastLogTime;

		//BOOL isCanlogin(CString name, CString pass) { return UCI::sDB.isCanlogin(name, pass); };
	};
}


