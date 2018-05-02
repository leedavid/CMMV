#pragma once

#include "afxcmn.h"
#include "afxwin.h"
#include <HalconCpp.h>
#include "FireGrab.h"
#include <list>

#define MAX_BUFFERS 2
//#define INC_BUFFER( IDX)  ((IDX) < (MAX_BUFFERS-1)) ? (IDX)++ : (IDX)=0

/////////////////////////////////////////////////////////////////////////////
// ResultContainer - data structure to share the result data between the
//                   worker threads and the display thread. 
/////////////////////////////////////////////////////////////////////////////
struct ResultContainer
{
	//HalconCpp::HImage   disp_img;
	//HalconCpp::HXLDCont disp_cont;
	HalconCpp::HImage   result_img;
	HalconCpp::HXLDCont result_cont;
	HalconCpp::HTuple   time_needed;
	HalconCpp::HTuple   result_handle;
	HalconCpp::HTuple   decoded_data;
};

class CChildView;


class CCameraCPP
{
public:

	CCameraCPP();
	~CCameraCPP();


	BOOL GetIsOpen();


	int GetParameterInt(CString sPara);
	CString GetParameterString(CString sPara);
	double GetParameterDouble(CString sPara);

	void SetParameterHTuple(std::string str, HalconCpp::HTuple ht);

	void SetParameterHTuple(CString str, HalconCpp::HTuple ht) {
		SetParameterHTuple(CT2A(str).m_psz, ht);
	};

	HalconCpp::HFramegrabber* GetPFG();

	BOOL Connect(CString str = L"NotFiles");                           // 如果无参数就是调用摄像头
	BOOL close_camera();


	CString  GetCaliInnerPicPath(int pr);                                 // 得到标定 inner 图片的目录
	CString  GetCaliPosePicPath(int pr);                                  // 得到标定 pose 图片的目录

	CString  GetCalibCcdPathFile(int pr);
	CString  GetCaliCalPathFile(int pr);                                   // camera inner parameters
	CString  GetCaliPoseDataPathFile(int pr);
	CString  GetCaliDescribePathFile(int pr);
	CString  GetCaliPostScriptsPathFile(int pr);


	bool Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, int pr);
	bool Read_CalibrationCcd(HalconCpp::HTuple &hv_CalibHandle, int pr);


	bool Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, std::string  Path);
	bool Read_CalibrationCcd(HalconCpp::HTuple &hv_CalibHandle, std::string  Path);

	bool Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, CString Path);
	bool Read_CalibrationCcd(HalconCpp::HTuple &hv_CalibHandle, CString Path);

	bool Write_CaliPose(HalconCpp::HPose pose, int prID);
	bool Write_CaliPose(HalconCpp::HPose CameraPose, std::string Path); // write the internal camera parameters
	bool Write_CaliPose(HalconCpp::HPose CameraPose, CString Path); // write the internal camera parameters

	bool Read_CaliPose(HalconCpp::HPose& pose, int prID);
	bool Read_CaliPose(HalconCpp::HPose &CameraPose, std::string Path);
	bool Read_CaliPose(HalconCpp::HPose &CameraPose, CString Path);

	bool Write_CaliCal(HalconCpp::HTuple CameraParam, int prID); // write the internal camera parameters
	bool Read_CaliCal(HalconCpp::HTuple &CameraParam, int prID);

	bool Write_CaliCal(HalconCpp::HTuple CameraParam, std::string Path); // write the internal camera parameters
	bool Read_CaliCal(HalconCpp::HTuple &CameraParam, std::string Path);
	bool Write_CaliCal(HalconCpp::HTuple CameraParam, CString Path); // write the internal camera parameters
	bool Read_CaliCal(HalconCpp::HTuple &CameraParam, CString Path);



	void SetbOnePIcMaxErrorOK(bool b);

	void SetfError(double f);

	void OneShot();     // 软触发模式采集一帖图像（设置曝光和增益马上生效，不需要延时）

	HBOOL GetIsFGRuning();
	void SetFGRuning(HBOOL b);

	;
	void GetBufferImage(HalconCpp::HImage*);
	void GrabOneImageAsync(HalconCpp::HImage *);  // 异步采集图像
	void ResetIPNewImageEvent();

	HANDLE GetNewImageEvent();


private:

	std::list<HalconCpp::HImage> imgList;        // 采集的图像缓冲  

	CRITICAL_SECTION       newImageMutex;        /* Local mutex to protect the access on the image buffer  */
	HANDLE                 newImageEven;         /* Event to tell the IP thread of a new image        */
	double fError;							     // 图像的误差	 
	bool bOnePIcMaxErrorOK;						 // 本次标定图像有效
	double RectificationWidth;					 // width of the visible area in world coordinates[mm]

												 //-----------------------------------------------------------------------
												 // 以上是相机的公共参数
	HBOOL     m_bFGIsRunning;                    // 相机正在采集图像
	HalconCpp::HFramegrabber  *m_pFG;            // Frame grabber 
	BOOL    bIsOpened;                           // 相机是否打开了.



};
