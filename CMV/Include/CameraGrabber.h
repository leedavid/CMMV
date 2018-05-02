#ifndef _MV_CAMERA_GRABBER_H_
#define _MV_CAMERA_GRABBER_H_

#include "CameraDefine.h"
#include "CameraStatus.h"


/******************************************************/
// 函数名   : CameraGrabber_CreateFromDevicePage
// 功能描述 : 弹出相机列表让用户选择要打开的相机
// 参数     : 如果函数执行成功返回函数创建的Grabber
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_CreateFromDevicePage(
	void** Grabber
	);

/******************************************************/
// 函数名   : CameraGrabber_Create
// 功能描述 : 从设备描述信息创建Grabber
// 参数     : Grabber    如果函数执行成功返回函数创建的Grabber对象
//			  pDevInfo	该相机的设备描述信息，由CameraEnumerateDevice函数获得。 
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_Create(
	void** Grabber,
	tSdkCameraDevInfo* pDevInfo
	);

/******************************************************/
// 函数名   : CameraGrabber_Destroy
// 功能描述 : 销毁Grabber
// 参数     : Grabber
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_Destroy(
	void* Grabber
	);

/******************************************************/
// 函数名	: CameraGrabber_SetHWnd
// 功能描述	: 设置预览视频的显示窗口
// 参数		: Grabber
//			  hWnd  窗口句柄
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SetHWnd(
	void* Grabber,
	HWND hWnd
	);

/******************************************************/
// 函数名	: CameraGrabber_StartLive
// 功能描述	: 启动预览
// 参数		: Grabber
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_StartLive(
	void* Grabber
	);

/******************************************************/
// 函数名	: CameraGrabber_StopLive
// 功能描述	: 停止预览
// 参数		: Grabber
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_StopLive(
	void* Grabber
	);

/******************************************************/
// 函数名	: CameraGrabber_SaveImage
// 功能描述	: 抓图
// 参数		: Grabber
//			  Image 返回抓取到的图像（需要调用CameraImage_Destroy释放）
//			  TimeOut 超时时间（毫秒）
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SaveImage(
	void* Grabber,
	void** Image,
	DWORD TimeOut
	);

/******************************************************/
// 函数名	: CameraGrabber_SaveImageAsync
// 功能描述	: 提交一个异步的抓图请求，提交成功后待抓图完成会回调用户设置的完成函数
// 参数		: Grabber
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SaveImageAsync(
	void* Grabber
	);

/******************************************************/
// 函数名	: CameraGrabber_SaveImageAsyncEx
// 功能描述	: 提交一个异步的抓图请求，提交成功后待抓图完成会回调用户设置的完成函数
// 参数		: Grabber
//			  UserData 可使用CameraImage_GetUserData从Image获取此值
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SaveImageAsyncEx(
	void* Grabber,
	void* UserData
	);

/******************************************************/
// 函数名	: CameraGrabber_SetSaveImageCompleteCallback
// 功能描述	: 设置异步方式抓图的完成函数
// 参数		: Grabber
//			  Callback 当有抓图任务完成时被调用
//			  Context 当Callback被调用时，作为参数传入Callback
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SetSaveImageCompleteCallback(
	void* Grabber,
	pfnCameraGrabberSaveImageComplete Callback,
	void* Context
	);

/******************************************************/
// 函数名	: CameraGrabber_SetFrameListener
// 功能描述	: 设置帧监听函数
// 参数		: Grabber
//			  Listener 监听函数，此函数返回0表示丢弃当前帧
//			  Context 当Listener被调用时，作为参数传入Listener
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_SetFrameListener(
	void* Grabber,
	pfnCameraGrabberFrameListener Listener,
	void* Context
	);

/******************************************************/
// 函数名	: CameraGrabber_GetCameraHandle
// 功能描述	: 获取相机句柄
// 参数		: Grabber
//			  hCamera 返回的相机句柄
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_GetCameraHandle(
	void* Grabber,
	CameraHandle *hCamera
	);

/******************************************************/
// 函数名	: CameraGrabber_GetStat
// 功能描述	: 获取帧统计信息
// 参数		: Grabber
//			  stat 返回的统计信息
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_GetStat(
	void* Grabber,
	tSdkGrabberStat *stat
	);

/******************************************************/
// 函数名	: CameraGrabber_GetCameraDevInfo
// 功能描述	: 获取相机DevInfo
// 参数		: Grabber
//			  DevInfo 返回的相机DevInfo
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraGrabber_GetCameraDevInfo(
	void* Grabber,
	tSdkCameraDevInfo *DevInfo
	);




#endif // _MV_CAMERA_GRABBER_H_
