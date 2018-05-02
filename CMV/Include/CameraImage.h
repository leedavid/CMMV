#ifndef _MV_CAMERA_IMAGE_H_
#define _MV_CAMERA_IMAGE_H_

#include "CameraDefine.h"
#include "CameraStatus.h"


/******************************************************/
// 函数名	: CameraImage_Create
// 功能描述	: 创建一个新的Image
// 参数		: Image
//			  pFrameBuffer 帧数据缓冲区
//			  pFrameHead 帧头
//			  bCopy TRUE: 复制出一份新的帧数据  FALSE: 不复制，直接使用pFrameBuffer指向的缓冲区
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_Create(
	void** Image,
	BYTE *pFrameBuffer, 
	tSdkFrameHead* pFrameHead,
	BOOL bCopy
	);

/******************************************************/
// 函数名	: CameraImage_CreateEmpty
// 功能描述	: 创建一个空的Image
// 参数		: Image
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_CreateEmpty(
	void** Image
	);

/******************************************************/
// 函数名	: CameraImage_Destroy
// 功能描述	: 销毁Image
// 参数		: Image
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_Destroy(
	void* Image
	);

/******************************************************/
// 函数名	: CameraImage_GetData
// 功能描述	: 获取Image数据
// 参数		: Image
//			  DataBuffer 图像数据
//			  Head 图像信息
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_GetData(
	void* Image,
	BYTE** DataBuffer,
	tSdkFrameHead** Head
	);

/******************************************************/
// 函数名	: CameraImage_GetUserData
// 功能描述	: 获取Image的用户自定义数据
// 参数		: Image
//			  UserData 返回用户自定义数据
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_GetUserData(
	void* Image,
	void** UserData
	);

/******************************************************/
// 函数名	: CameraImage_SetUserData
// 功能描述	: 设置Image的用户自定义数据
// 参数		: Image
//			  UserData 用户自定义数据
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_SetUserData(
	void* Image,
	void* UserData
	);

/******************************************************/
// 函数名	: CameraImage_IsEmpty
// 功能描述	: 判断一个Image是否为空
// 参数		: Image
//			  IsEmpty 为空返回:TRUE(1)  否则返回:FALSE(0)
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_IsEmpty(
	void* Image,
	BOOL* IsEmpty
	);

/******************************************************/
// 函数名	: CameraImage_Draw
// 功能描述	: 绘制Image到指定窗口
// 参数		: Image
//			  hWnd 目的窗口
//			  Algorithm 缩放算法  0：快速但质量稍差  1：速度慢但质量好
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_Draw(
	void* Image,
	HWND hWnd,
	int Algorithm
	);

/******************************************************/
// 函数名	: CameraImage_DrawToDC
// 功能描述	: 绘制Image到指定DC
// 参数		: Image
//			  hDC 目的DC
//			  Algorithm 缩放算法  0：快速但质量稍差  1：速度慢但质量好
//			  xDst,yDst: 目标矩形的左上角坐标
//			  cxDst,cyDst: 目标矩形的宽高
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_DrawToDC(
	void* Image,
	HDC hDC,
	int Algorithm,
	int xDst,
	int yDst,
	int cxDst,
	int cyDst
	);

/******************************************************/
// 函数名	: CameraImage_BitBlt
// 功能描述	: 绘制Image到指定窗口（不缩放）
// 参数		: Image
//			  hWnd 目的窗口
//			  xDst,yDst: 目标矩形的左上角坐标
//			  cxDst,cyDst: 目标矩形的宽高
//			  xSrc,ySrc: 图像矩形的左上角坐标
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_BitBlt(
	void* Image,
	HWND hWnd,
	int xDst,
	int yDst,
	int cxDst,
	int cyDst,
	int xSrc,
	int ySrc
	);

/******************************************************/
// 函数名	: CameraImage_BitBltToDC
// 功能描述	: 绘制Image到指定DC（不缩放）
// 参数		: Image
//			  hDC 目的DC
//			  xDst,yDst: 目标矩形的左上角坐标
//			  cxDst,cyDst: 目标矩形的宽高
//			  xSrc,ySrc: 图像矩形的左上角坐标
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_BitBltToDC(
	void* Image,
	HDC hDC,
	int xDst,
	int yDst,
	int cxDst,
	int cyDst,
	int xSrc,
	int ySrc
	);

/******************************************************/
// 函数名	: CameraImage_SaveAsBmp
// 功能描述	: 以bmp格式保存Image
// 参数		: Image
//			  FileName 文件名
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_SaveAsBmp(
	void* Image,
	char const* FileName
	);

/******************************************************/
// 函数名	: CameraImage_SaveAsJpeg
// 功能描述	: 以jpg格式保存Image
// 参数		: Image
//			  FileName 文件名
//			  Quality 保存质量(1-100)，100为质量最佳但文件也最大
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_SaveAsJpeg(
	void* Image,
	char const* FileName,
	BYTE  Quality
	);

/******************************************************/
// 函数名	: CameraImage_SaveAsPng
// 功能描述	: 以png格式保存Image
// 参数		: Image
//			  FileName 文件名
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_SaveAsPng(
	void* Image,
	char const* FileName
	);

/******************************************************/
// 函数名	: CameraImage_SaveAsRaw
// 功能描述	: 保存raw Image
// 参数		: Image
//			  FileName 文件名
//			  Format 0: 8Bit Raw     1: 16Bit Raw
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_SaveAsRaw(
	void* Image,
	char const* FileName,
	int Format
	);

/******************************************************/
// 函数名	: CameraImage_IPicture
// 功能描述	: 从Image创建一个IPicture
// 参数		: Image
//			  Picture 新创建的IPicture
// 返回值   : 成功时，返回CAMERA_STATUS_SUCCESS (0);
//            否则返回非0值的错误码,请参考CameraStatus.h
//            中错误码的定义。
/******************************************************/
MVSDK_API CameraSdkStatus __stdcall CameraImage_IPicture(
	void* Image,
	IPicture** NewPic
	);




#endif // _MV_CAMERA_IMAGE_H_
