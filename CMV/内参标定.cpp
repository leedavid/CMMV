#include "stdafx.h"
#include "MainFrm.h"
#include "ChildView.h"

#include "MyData.h"
#include "MyDispLay.h"


using namespace HalconCpp;
using namespace std;

// 标定内参处理线程
void ST_MYDISP::IPRunCamCaliInner(void *Param)
{
	HANDLE         lphEvents[2];
	HImage         Image;
	HXLDCont       Contours;
	Contours.GenEmptyObj();

	HTuple T1, T2;

	CChildView *pCV = (CChildView *)Param;
	const HWND PostWnd = pCV->GetSafeHwnd();

	ST_MYDISP* pDis = UCI::pDis;

	lphEvents[0] = pDis->pCAM->GetNewImageEvent();
	lphEvents[1] = pDis->m_hFGStopEvent;

	pDis->pCAM->SetbOnePIcMaxErrorOK(false);  // 本次图片无效  



	while (WAIT_OBJECT_0 == WaitForMultipleObjects(2, lphEvents, FALSE, INFINITE))
	{
		pDis->pCAM->GetBufferImage(&Image);

		// 这儿要处理一下图像功能, 这儿不用处理
		//UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_FUNCTION);

		//BOOL old = (*om)[L"USE_map_image"];
		//(*om)[L"USE_map_image"] = FALSE;
		//UCI::pDis->MeasureOneFUNCTION(om, Image, Contours);
		//(*om)[L"USE_map_image"] = old;

		CountSeconds(&T1);
		pDis->DisCamliInnerOnePicture(&Image, &Contours);
		CountSeconds(&T2);

		pDis->m_sIPResult.time_needed = 1000 * (T2[0].D() - T1[0].D());


		HTuple time = pDis->m_sIPResult.time_needed;

		CString str;
		str.Format(_T("内参标定 >当前处理时间为: %4.2f ms"), time[0].D());
		UCI::MyLog(str);

		int delay = UCI::OPTIONS[L"相机标定定时"];
		Sleep(delay);
	}
	return;
}

// 内参取图
void ST_MYDISP::DisCamliInnerOnePicture(HalconCpp::HImage * Image, HalconCpp::HXLDCont*  Contours)
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
	int nPic = this->GetTotalCariInnerPicNum(prID);
	int nTotal = UCI::OPTIONS[L"标定图片数量"];


	if (nPic >= nTotal) {                      // 图片数量已满足，可以计算并保存标定内参。		
		this->DisGetCameraInnerParaFromPic();  // 取得的相机的内部参数。

		this->StopContiouscap();
		this->AppStateChangeTo(APP_READY);
		return;
	}

	try
	{
		hv_TmpCtrl_ReferenceIndex = 0;

		// 标定描述文件	
		CString descFile = this->pCAM->GetCaliDescribePathFile(prID);
		HTuple hv_TmpCtrl_PlateDescription = CT2A(descFile).m_psz;

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
		// 查找图像中的标定板
		m_HCali.FindCalibObject(*Image, 0, 0, hv_Index, hv_TmpCtrl_FindCalObjParNames,
			hv_TmpCtrl_FindCalObjParValues);

		// 设置一下图像的颜色
		this->m_pMHWinMain->SetColor("magenta");

		// 得到标定板的姿态
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
			// 保存图像
			//this->GrabImageERROK = *Image;
			write_message(100, 8, "按下截图按钮保存可保存图像！请尽量选取不同的位置！", 0);

			if (this->PushCaptureDown == 1) {
				this->PushCaptureDown = 0;
				this->SaveOneCaliInnerPicture(prID, Image);   // 保存一副标定的图像  
				this->pCAM->SetbOnePIcMaxErrorOK(false);
			}
			else {
				Sleep(50);
			}

			// 使能按钮
			UCI::RIBmenu.Rib_CALI_CAPTURE = true;

		}
		else {
			this->pCAM->SetbOnePIcMaxErrorOK(false);

			// 使能按钮
			UCI::RIBmenu.Rib_CALI_CAPTURE = false;

			write_message(100, 8, "当前图像误差太大，请检查一下设置或镜头！", 0);
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"正在采集 内参 图像, 发现如下问题：");

		// 使能按钮
		UCI::RIBmenu.Rib_CALI_CAPTURE = false;
	}

	PostMessage(UCI::pMain->m_hWnd, UM_UPDATE_RIBBON, (WPARAM)0, (LPARAM)0);
}

void ST_MYDISP::DisSetInnerAndPosStartPara(
	HalconCpp::HTuple & hv_StartParameters,
	HalconCpp::HTuple & hv_TmpCtrl_FindCalObjParNames, 
	HalconCpp::HTuple & hv_TmpCtrl_FindCalObjParValues)
{

	// 标定初始相机参数 面扫描，多项式
	hv_StartParameters.Clear();     // 
	double focus = UCI::OPTIONS[L"镜头焦距"];
	hv_StartParameters[0] = focus / 1000.0;
	hv_StartParameters[1] = 0;
	hv_StartParameters[2] = 0;
	hv_StartParameters[3] = 0;
	hv_StartParameters[4] = 0;
	hv_StartParameters[5] = 0;

	double fsx = UCI::OPTIONS[L"像素宽度"];
	double fsy = UCI::OPTIONS[L"像素高度"];
	hv_StartParameters[6] = fsx / 1e+006;
	hv_StartParameters[7] = fsy / 1e+006;

	hv_StartParameters[8] = this->GetWidth() / 2;
	hv_StartParameters[9] = this->GetHeight() / 2;
	hv_StartParameters[10] = this->GetWidth();
	hv_StartParameters[11] = this->GetHeight();

	// FindCalObjParNames;

	hv_TmpCtrl_FindCalObjParNames.Clear();
	hv_TmpCtrl_FindCalObjParNames[0] = "gap_tolerance";
	hv_TmpCtrl_FindCalObjParNames[1] = "alpha";
	hv_TmpCtrl_FindCalObjParNames[2] = "max_diam_marks";
	hv_TmpCtrl_FindCalObjParNames[3] = "skip_find_caltab";

	// FindCalObjParValues

	double gap_tolerance = UCI::OPTIONS[L"标定间距容许因子"];
	double alpha = UCI::OPTIONS[L"标定平滑alpha"];
	double max_diam_marks = UCI::OPTIONS[L"标定最大标志点直径"];

	hv_TmpCtrl_FindCalObjParValues.Clear();
	hv_TmpCtrl_FindCalObjParValues[0] = gap_tolerance;
	hv_TmpCtrl_FindCalObjParValues[1] = alpha;
	hv_TmpCtrl_FindCalObjParValues[2] = max_diam_marks;
	hv_TmpCtrl_FindCalObjParValues[3] = "false";
}



// 从图片中取得内参，并保存
void ST_MYDISP::DisGetCameraInnerParaFromPic()
{
	HTuple hv_TmpCtrl_ReferenceIndex;
	HTuple hv_StartParameters;
	HTuple hv_TmpCtrl_FindCalObjParNames;
	HTuple hv_TmpCtrl_FindCalObjParValues;
	HTuple hv_CameraParameters;
	HTuple hv_CameraPose;
	HTuple hv_TmpCtrl_Errors;

	int prID = UCI::OPTIONS[L"项目编号"];
	int nPic = this->GetTotalCariInnerPicNum(prID);
	int nTotal = UCI::OPTIONS[L"标定图片数量"];        // 

	if (nPic < nTotal) {                       // 图片数量不满足
		write_message(8, 8, "标定图像数量太小，尚需" + HTuple(nTotal - nPic) + "个图像", 0);
		return;
	}
	try {

		hv_TmpCtrl_ReferenceIndex = 0;

		// 标定描述文件
		HTuple hv_TmpCtrl_PlateDescription = CT2A(this->pCAM->GetCaliDescribePathFile(prID));

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
		UCI::FindSpecialFile(this->pCAM->GetCaliInnerPicPath(prID), L"*.png", &caFileName);

		int nTpic = (int)caFileName.GetSize();
		for (int i = 0; i < nTpic; i++) {

			//this->Read_Image(Image, caFileName[i]);

			Image.ReadImage(CT2A(caFileName[i]).m_psz);

			//EnterCriticalSection(&this->m_csFGDisp);
			//this->GrabImageDisp = Image;
			//LeaveCriticalSection(&this->m_csFGDisp);		
			this->m_pMHWinMain->DispObj(Image);
			//this->DisDisplay();
			write_message(12, 12, "正在从" + HTuple(i) + "号标定板收集数据。。。", 0);

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

			// 取得相机的内参
			hv_CameraParameters = m_HCali.GetCalibData("camera", 0, "params");

			if (this->pCAM->Write_CaliCal(hv_CameraParameters, prID)) {
				write_message(60, 8, "保存相机内参与外参文件成功了", 0);
			}

			write_message(300, 8, "相机内参：" + hv_CameraParameters, 0);

			double  curErr = hv_TmpCtrl_Errors.D();
			UCI::OPTIONS[L"标定当前误差"] = curErr;

		}
		else {
			write_message(60, 8, "总共" + HTuple(nTotal - nPic) + "个图像,"
				+ "标定失败，标定误差超差:" + hv_TmpCtrl_Errors, 1);

			write_message(120, 8, "请重新采集图像标定！", 0);
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"从图片中取得内参, 发现如下问题：");
	}
}