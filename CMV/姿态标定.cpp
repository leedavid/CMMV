#include "stdafx.h"
#include "MainFrm.h"
#include "ChildView.h"

#include "MyData.h"
#include "MyDispLay.h"


using namespace HalconCpp;
using namespace std;


// 标定外参处理线程
void ST_MYDISP::IPRunCamCaliPose(void *Param)
{
	HANDLE         lphEvents[2];
	HImage         Image;
	HXLDCont       Contours;
	Contours.GenEmptyObj();

	HTuple T1, T2;

	CChildView *pCV = (CChildView *)Param;
	const HWND PostWnd = pCV->GetSafeHwnd();

	//ST_MYDISP* pDis = pCV->pDis;

	lphEvents[0] = UCI::pDis->pCAM->GetNewImageEvent();
	lphEvents[1] = UCI::pDis->m_hFGStopEvent;

	UCI::pDis->pCAM->SetbOnePIcMaxErrorOK(false);  // 本次图片无效  



	while (WAIT_OBJECT_0 == WaitForMultipleObjects(2, lphEvents, FALSE, INFINITE))
	{
		UCI::pDis->pCAM->GetBufferImage(&Image);

		// 这儿要处理一下图像功能
		//UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_FUNCTION);
		//BOOL old = (*om)[L"USE_map_image"];
		//(*om)[L"USE_map_image"] = FALSE;
		//UCI::pDis->MeasureOneFUNCTION(om, Image, Contours);
		//(*om)[L"USE_map_image"] = old;

		CountSeconds(&T1);
		UCI::pDis->DisCamliPoseOnePicture(&Image, &Contours);
		CountSeconds(&T2);

		UCI::pDis->m_sIPResult.time_needed = 1000 * (T2[0].D() - T1[0].D());


		HTuple time = UCI::pDis->m_sIPResult.time_needed;
		//sVal.Format(_T("外参姿态标定 >当前处理时间为: %4.2f ms"), time[0].D())

		//CString *sVal = new CString();
		//sVal->Format(_T("外参姿态标定 >当前处理时间为: %4.2f ms"), time[0].D());

		CString str;
		str.Format(_T("外参姿态标定 >当前处理时间为: %4.2f ms"), time[0].D());
		UCI::MyLog(str);

	}
	return;
}

// 姿态取图
void ST_MYDISP::DisCamliPoseOnePicture(HalconCpp::HImage * Image, HalconCpp::HXLDCont*  Contours)
{
	HTuple hv_TmpCtrl_ReferenceIndex;
	HTuple hv_StartParameters;
	HTuple hv_TmpCtrl_FindCalObjParNames;
	HTuple hv_TmpCtrl_FindCalObjParValues;
	//HTuple hv_CalibHandle;
	HTuple hv_CameraParameters;
	HTuple hv_CameraPose;
	HTuple hv_TmpCtrl_Errors;
	HTuple hv_Index;

	Contours->GenEmptyObj();
	int prID = UCI::OPTIONS[L"项目编号"];
	int nPic = this->GetTotalCariPosePicNum(prID);
	int nTotal = 1;         // pose 只要一张

	if (nPic >= nTotal) {                      // 图片数量已满足，可以计算并保存标定内参。		
		this->DisGetCameraPoseParaFromPic();   // 取得的相机的外部参数。

		this->StopContiouscap();

		Sleep(100);
		this->AppStateChangeTo(APP_READY);
		return;
	}

	this->pCAM->SetbOnePIcMaxErrorOK(false);   // 本次图片无效  
	try
	{
		hv_TmpCtrl_ReferenceIndex = 0;

		// 标定描述文件	
		HTuple hv_TmpCtrl_PlateDescription = CT2A(this->pCAM->GetCaliDescribePathFile(prID)).m_psz;

		// 标定初始相机参数 
		DisSetInnerAndPosStartPara(hv_StartParameters,
			hv_TmpCtrl_FindCalObjParNames,
			hv_TmpCtrl_FindCalObjParValues);

		// 建立标定句柄
		HCalibData m_HCali;
		m_HCali.CreateCalibData("calibration_object", 1, 1);
		// 设置标定初始参数
		CString calimodel = UCI::OPTIONS[L"CamCalibrateModel"];
		m_HCali.SetCalibDataCamParam(0,
			CT2A(calimodel).m_psz,
			hv_StartParameters);
		// 设置标定描述文件
		m_HCali.SetCalibDataCalibObject(0, hv_TmpCtrl_PlateDescription);

		hv_Index = 0;
		// 查找图像中的标定图标
		m_HCali.FindCalibObject(*Image, 0, 0, hv_Index, hv_TmpCtrl_FindCalObjParNames,
			hv_TmpCtrl_FindCalObjParValues);

		// 设置一下图像的颜色
		this->m_pMHWinMain->SetColor("magenta");
		HPose hv_pose = m_HCali.GetCalibDataObservPose(0, 0, hv_Index);

		// 显示图像
		this->m_pMHWinMain->DispObj(*Image);
		// 显示标定板圆坐标
		this->m_pMHWinMain->DispCaltab(hv_TmpCtrl_PlateDescription,
			hv_StartParameters, hv_pose, 1.0);

		// 显示标定板外形轮廓
		this->m_pMHWinMain->SetColor("red");
		this->m_pMHWinMain->SetLineWidth(2);

		*Contours = m_HCali.GetCalibDataObservContours("caltab", 0, 0, 0);

		this->m_pMHWinMain->DispObj(*Contours);

		// 计算标定数据
		hv_TmpCtrl_Errors = m_HCali.CalibrateCameras();

		write_message(8, 8, "正在采集标定图像，当前误差:" + hv_TmpCtrl_Errors
			+ "像素，已经采集：" + HTuple(nPic)
			+ "个，尚需" + HTuple(nTotal - nPic) + "个图像", 1);

		double picError = UCI::OPTIONS[L"标定充许误差"];
		if (hv_TmpCtrl_Errors <  picError) {
			this->pCAM->SetbOnePIcMaxErrorOK(true);
			// 保存姿态图像
			//this->GrabImageERROK = *Image;
			write_message(100, 8, "按下截图按钮保存可保存图像！请尽量选取不同的位置！", 0);

			// 使能按钮 UCI::RIBmenu.Rib_CALI_CAPTURE
			UCI::RIBmenu.Rib_CALI_CAPTURE = true;

			if (this->PushCaptureDown == 1) {
				this->PushCaptureDown = 0;
				this->SaveOneCaliPosePicture(prID, Image);   // 保存一副标定的图像  
				this->pCAM->SetbOnePIcMaxErrorOK(false);
			}
			else {
				Sleep(50);
			}
		}
		else {
			this->pCAM->SetbOnePIcMaxErrorOK(false);
			write_message(100, 8, "当前图像误差太大，请检查一下设置或镜头！", 1);

			// 使能按钮
			UCI::RIBmenu.Rib_CALI_CAPTURE = false;
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"正在采集 姿态 图像, 发现如下问题：");

		// 使能按钮
		UCI::RIBmenu.Rib_CALI_CAPTURE = false;
	}

	PostMessage(UCI::pMain->m_hWnd, UM_UPDATE_RIBBON, (WPARAM)0, (LPARAM)0);
}

// 从图片中取得姿态，并保存
void ST_MYDISP::DisGetCameraPoseParaFromPic()
{
	HTuple hv_TmpCtrl_ReferenceIndex;
	HTuple hv_StartParameters;
	HTuple hv_TmpCtrl_FindCalObjParNames;
	HTuple hv_TmpCtrl_FindCalObjParValues;
	// HTuple hv_CalibHandle;
	HTuple hv_CameraParameters;
	//HTuple hv_CameraPose;
	HTuple hv_TmpCtrl_Errors;
	//HTuple hv_Index;
	int prID = UCI::OPTIONS[L"项目编号"];
	int nPic = this->GetTotalCariPosePicNum(prID);
	int nTotal = 1;        // 

						   //this->DisDisplay();                        // 显示一下当前的图片
	if (nPic < nTotal) {                       // 图片数量不满足
		write_message(8, 8, "标定姿态图像数量太小，尚需" + HTuple(nTotal - nPic) + "个图像", 1);
		return;
	}
	try {

		hv_TmpCtrl_ReferenceIndex = 0;

		// 标定描述文件
		HTuple hv_TmpCtrl_PlateDescription = CT2A(this->pCAM->GetCaliDescribePathFile(prID)).m_psz;

		// 标定初始相机参数 面扫描，多项式
		DisSetInnerAndPosStartPara(hv_StartParameters,
			hv_TmpCtrl_FindCalObjParNames,
			hv_TmpCtrl_FindCalObjParValues);

		// 建立标定句柄
		HCalibData m_HCali;
		m_HCali.CreateCalibData("calibration_object", 1, 1);

		// 设置标定初始参数
		CString calimodel = UCI::OPTIONS[L"CamCalibrateModel"];
		m_HCali.SetCalibDataCamParam(0,
			CT2A(calimodel).m_psz,
			hv_StartParameters);

		// 设置标定描述文件
		m_HCali.SetCalibDataCalibObject(0, hv_TmpCtrl_PlateDescription);

		HalconCpp::HImage Image;

		CArray<CString, CString&> caFileName;     // 取得所有的文件名
		UCI::FindSpecialFile(this->pCAM->GetCaliPosePicPath(prID), L"*.png", &caFileName);

		int nTpic = (int)caFileName.GetSize();
		for (int i = 0; i < nTpic; i++) {

			//this->Read_Image(Image, caFileName[i]);

			//HTuple h, w;
			//Image.GetImageSize(&w, &h);

			//int h1 = h.I();
			//int w1 = w.I();

			Image.ReadImage(CT2A(caFileName[i]).m_psz);

			//EnterCriticalSection(&this->m_csFGDisp);
			//this->GrabImageDisp = Image;
			//LeaveCriticalSection(&this->m_csFGDisp);
			this->m_pMHWinMain->DispObj(Image);
			//this->DisDisplay();
			write_message(12, 12, "正在从" + HTuple(i) + "号标定板收集数据。。。", 1);

			// 查找图像中的标定图标
			m_HCali.FindCalibObject(Image, 0, 0, i, hv_TmpCtrl_FindCalObjParNames,
				hv_TmpCtrl_FindCalObjParValues);
		}

		// 设置一下图像的颜色
		this->m_pMHWinMain->SetColor("green");

		// 取得标定的坐标信息
		HPose hv_pose = m_HCali.GetCalibDataObservPose(0, 0, nTpic - 1);

		// 显示标定板圆坐标
		this->m_pMHWinMain->DispCaltab(hv_TmpCtrl_PlateDescription,
			hv_StartParameters, hv_pose, 1.0);

		// 取得标定板外形轮廓
		HalconCpp::HXLDCont  Contours
			= m_HCali.GetCalibDataObservContours("caltab", 0, 0, nTpic - 1);

		// 显示标定板外形轮廓
		this->m_pMHWinMain->SetColor("green");
		this->m_pMHWinMain->SetLineWidth(2);
		this->m_pMHWinMain->DispObj(Contours);

		// 计算标定数据
		hv_TmpCtrl_Errors = m_HCali.CalibrateCameras();

		double picError = UCI::OPTIONS[L"标定充许误差"];
		if (hv_TmpCtrl_Errors < picError) {
			write_message(8, 8, "总共标定了" + HTuple(nTpic) + "个图像,"
				+ "标定成功，标定误差:" + hv_TmpCtrl_Errors, 1);

			// 取得相机的姿态
			//m_HCali.GetCalibDataObservPose

			HPose hv_pose = m_HCali.GetCalibDataObservPose(0, 0, nTpic - 1);

			//hv_CameraPose = m_HCali.GetCalibData("calib_obj_pose", (Hlong)HTuple(0).TupleConcat(hv_TmpCtrl_ReferenceIndex), "pose");

			//
			////GetCalibData(hv_CalibHandle, "calib_obj_pose", HTuple(0).TupleConcat(hv_TmpCtrl_ReferenceIndex),
			////	"pose", &hv_CameraPose);

			//// write_message(600, 600, "相机姿态pre：" + hv_CameraPose, 0);

			////Calibration 01: Adjust origin for plate thickness
			//SetOriginPose(hv_CameraPose, 0.0, 0.0, this->pCAM->GetCaliPlateThick(), &hv_CameraPose);

			if (this->pCAM->Write_CaliPose(hv_pose, prID)) {
				write_message(160, 8, "保存相机外参文件成功了", 1);
			}

			write_message(600, 32, "相机姿态：" + hv_pose, 0);

			Sleep(500);
		}
		else {
			write_message(8, 8, "总共" + HTuple(nTotal - nPic) + "个图像,"
				+ "标定姿态失败，标定误差超差:" + hv_TmpCtrl_Errors, 1);

			write_message(32, 8, "请重新采集图像标定！", 0);
		}

		//Calibration 01: Clear calibration model when done
		//ClearCalibData(hv_CalibHandle);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"从图片中取得姿态, 发现如下问题：");
	}

}