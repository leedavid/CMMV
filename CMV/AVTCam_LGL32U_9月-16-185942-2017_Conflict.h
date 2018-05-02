#pragma once

#include "Resource.h"
#include "MyCamera.h"

//#define WM_RECEIVE WM_USER+100
#define USE_WINDOWS_CAMERA_HALCON




#ifdef USE_WINDOWS_CAMERA_HALCON

class AVTCam {

	
public:	

	AVTCam(CWnd* PWnd, MYFRAME* pF);   //
	~AVTCam();

	//--------------------------------------------------------------------------------
	// 方法

	

	//HalconCpp::HObject* get_hv_image();

	void set_busy(bool b);
	bool get_busy();

	int get_ImageHeight();
	void set_ImageHeight(int height);	

	int get_ImageWidth();
	void set_ImageWidth(int width);

	int get_camera_index();

	CWnd* get_parent();
	CCamera* get_camera();

	bool IsCaliPictureOk();                                            // 当前标定图像的误差是不是合格
	
	CString GetCamerModel();                                           // 得到相机的标定模型，
	CString GetMoudlePath();                                           // 得到当前软件运行的目录
	CString GetCaliInnerPicPath();                                     // 得到标定 inner 图片的目录
	CString GetCaliPosePicPath();                                      // 得到标定 pose 图片的目录
	CString GetCalibCcdPathFile();      
	CString GetCaliCalPathFile();                                      // camera inner parameters
	CString GetCaliPosePathFile();

	bool Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle);
	bool Read_CalibrationCcd(HalconCpp::HTuple &hv_CalibHandle);

	bool Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, CString Path);
	bool Read_CalibrationCcd(HalconCpp::HTuple &hv_CalibHandle, CString Path);

	bool Write_CaliPose(HalconCpp::HTuple CameraPose); // write the internal camera parameters
	bool Read_CaliPose(HalconCpp::HTuple &CameraPose);
	bool Write_CaliPose(HalconCpp::HTuple CameraPose, CString Path); // write the internal camera parameters
	bool Read_CaliPose(HalconCpp::HTuple &CameraPose, CString Path);

	bool Write_CaliCal(HalconCpp::HTuple CameraParam); // write the internal camera parameters
	bool Read_CaliCal(HalconCpp::HTuple &CameraParam);
	bool Write_CaliCal(HalconCpp::HTuple CameraParam, CString Path); // write the internal camera parameters
	bool Read_CaliCal(HalconCpp::HTuple &CameraParam, CString Path);

	CString GetDescrFileName();

	double GetFocus();                                                 // 得到当前的镜头的焦距
	double GetFsx();                                                   // 得到像素的宽度
	double GetFsy();                                                   // 得到像素的宽度
	double GetdOnePicMaxError();                                       // 得到当前图像的最大允许误差
	double GetCaliPlateThick();                                        // 
	double GetRectificationWidth();

	void SetbOnePIcMaxErrorOK(bool b);

	void SetfError(double f);

	int GetTotalCaliPicture();

	bool open_camera();
	bool close_camera();

	//--------------------------------------------------------------------------------
	



	bool Snap(void);                             // 单帖采集
	bool Grab(void);                             // 连续采集
	bool Freeze(void);                           // 停止连续采集
	bool SetTriggerMode(bool bEnable, int nSrc = 7);
	// 设置触发模式 Mode = 1 找开触发， Mode = 0 关闭触发
	// 设置触发源 nSrc = 1 外触发 nSrc = 7 软触发
	bool SetShutter(int iValue);                 // 设置曝光时间
	bool SetGain(double iValue);                 // 设置增益
	int GetShutter(void);                        // 获取曝光时间
	int GetGain(void);                           // 获取增益
				
	//bool Get_bIsHardTrigger();
	//void Set_bIsHardTrigger(bool b);
									             
	bool Get_m_bRGBmode();
	void Set_m_bRGBmode(bool b);

	void Set_ExposureMode(BOOL ｂ);

	CCamera* GetCam() { return &this->Camera; };

	bool IsOpened();                             // 只读

	void Set_interfaceName(CString str);         // 相机接口
	CString Get_interfaceName();



	void Set_deviceName(CString str);
	//CString Get_deviceName() { return this->Camera.Get_deviceName();};

	int GetParameterInt(CString str) { return this->Camera.GetParameterInt(str); };



private:

	HANDLE m_hThread;                            // 采集线程
	UINT32 Result;                               // 相机 api 函数返回值


	UINT32 iImageWidth;
	UINT32 iImageHeight;
	int m_iCameraIndex;                          // 图像索引

	bool m_bBusy;                                // 相机是否处于忙碌状态
	CWnd * m_pParent;                            // 父窗口指针
	CCamera Camera;                              // firegrab 相机对象

	//-----------------------------------------------------------------------
	// 以下为标定用数据
	CString CamerModel;                          // 面扫描，。。。。
	bool bIsYuanXin;                             // 是不是远芯镜头
	bool bIsSlope;                               // 是不是倾斜
	double fSx;                                  // 单个像元的宽 um
	double fSy;                                  // 单个像元的高 um
	double fFocus;                               // 相机的焦距 mm

	//int id;										// 编号    数库使用	
	//int CammerPort;								// 相机端口
	int nTotalPicture;							 // 总共需要的标定图片数    
	CString DescrFileName;    					 // 标定描述文件名称
	double fError;							     // 图像的误差	 
	CString DescrPsName;						 // 标定 PostScrip 文件名称
	CString DescrDir;							 // 标定描述文件目录
	CString DirInnerPicture;					 // 标定 Inner 图像目录	
	CString DirPosePicture;                      // 标定 pose 图像目录	
	CString CamCaliCcdFile;				         // 相机标定结果文件 .ccd

	CString CamParaCalFile;                      // 
	CString CamPosDatFile;                       // 
	
	double CaliPlateThick;						 // 标定板厚度
	double dOnePicMaxError;						 // 标定板最大允许误差	
	bool bOnePIcMaxErrorOK;						 // 本次标定图像有效
	double RectificationWidth;					 // width of the visible area in world coordinates[mm]

};


#else

#endif



