#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


// Procedures 
// Chapter: Graphics / Text
// Short Description: Set font independent of OS 
void ST_MYDISP::set_display_font(HTuple hv_WindowHandle, HTuple hv_Size, HTuple hv_Font, HTuple hv_Bold,
	HTuple hv_Slant)
{

	// Local iconic variables

	// Local control variables
	HTuple  hv_OS, hv_BufferWindowHandle, hv_Ascent;
	HTuple  hv_Descent, hv_Width, hv_Height, hv_Scale, hv_Exception;
	HTuple  hv_SubFamily, hv_Fonts, hv_SystemFonts, hv_Guess;
	HTuple  hv_I, hv_Index, hv_AllowedFontSizes, hv_Distances;
	HTuple  hv_Indices, hv_FontSelRegexp, hv_FontsCourier;

	//This procedure sets the text font of the current window with
	//the specified attributes.
	//It is assumed that following fonts are installed on the system:
	//Windows: Courier New, Arial Times New Roman
	//Mac OS X: CourierNewPS, Arial, TimesNewRomanPS
	//Linux: courier, helvetica, times
	//Because fonts are displayed smaller on Linux than on Windows,
	//a scaling factor of 1.25 is used the get comparable results.
	//For Linux, only a limited number of font sizes is supported,
	//to get comparable results, it is recommended to use one of the
	//following sizes: 9, 11, 14, 16, 20, 27
	//(which will be mapped internally on Linux systems to 11, 14, 17, 20, 25, 34)
	//
	//Input parameters:
	//WindowHandle: The graphics window for which the font will be set
	//Size: The font size. If Size=-1, the default of 16 is used.
	//Bold: If set to 'true', a bold font is used
	//Slant: If set to 'true', a slanted font is used
	//
	
	// dev_get_preferences(...); only in hdevelop
	// dev_set_preferences(...); only in hdevelop
	if (0 != (HTuple(hv_Size == HTuple()).TupleOr(hv_Size == -1)))
	{
		hv_Size = 16;
	}

		//Set font on Windows systems
		//try
		//{
		//	//Check, if font scaling is switched on
		//	OpenWindow(0, 0, 256, 256, 0, "buffer", "", &hv_BufferWindowHandle);
		//	SetFont(hv_BufferWindowHandle, "-Consolas-16-*-0-*-*-1-");
		//	GetStringExtents(hv_BufferWindowHandle, "test_string", &hv_Ascent, &hv_Descent,
		//		&hv_Width, &hv_Height);
		//	//Expected width is 110
		//	hv_Scale = 110.0 / hv_Width;
		//	hv_Size = (hv_Size*hv_Scale).TupleInt();
		//	CloseWindow(hv_BufferWindowHandle);
		//}
		//// catch (Exception) 
		//catch (HalconCpp::HException &HDevExpDefaultException)
		//{
		//	HDevExpDefaultException.ToHTuple(&hv_Exception);
		//	//throw (Exception)
		//}
		if (0 != (HTuple(hv_Font == HTuple("Courier")).TupleOr(hv_Font == HTuple("courier"))))
		{
			hv_Font = "Courier New";
		}
		else if (0 != (hv_Font == HTuple("mono")))
		{
			hv_Font = "Consolas";
		}
		else if (0 != (hv_Font == HTuple("sans")))
		{
			hv_Font = "Arial";
		}
		else if (0 != (hv_Font == HTuple("serif")))
		{
			hv_Font = "Times New Roman";
		}
		if (0 != (hv_Bold == HTuple("true")))
		{
			hv_Bold = 1;
		}
		else if (0 != (hv_Bold == HTuple("false")))
		{
			hv_Bold = 0;
		}
		else
		{
			hv_Exception = "Wrong value of control parameter Bold";
			throw HalconCpp::HException(hv_Exception);
		}
		if (0 != (hv_Slant == HTuple("true")))
		{
			hv_Slant = 1;
		}
		else if (0 != (hv_Slant == HTuple("false")))
		{
			hv_Slant = 0;
		}
		else
		{
			hv_Exception = "Wrong value of control parameter Slant";
			throw HalconCpp::HException(hv_Exception);
		}
		try
		{
			SetFont(hv_WindowHandle, ((((((("-" + hv_Font) + "-") + hv_Size) + "-*-") + hv_Slant) + "-*-*-") + hv_Bold) + "-");
		}
		// catch (Exception) 
		catch (HalconCpp::HException &HDevExpDefaultException)
		{
			HDevExpDefaultException.ToHTuple(&hv_Exception);
			//throw (Exception)
		}


	// dev_set_preferences(...); only in hdevelop
	return;
}




bool ST_MYDISP::GetRefMapFromSaveFile()
{
	this->DisOnOneShot();

	int prID = UCI::OPTIONS[L"项目编号"];

	// 读出相机内参
	if (!this->pCAM->Read_CaliCal(hv_CamParameters,prID)) {
		this->write_message(8, 8, "读取相机内参文件失败!", true);
		return false;
	}

	if (!this->pCAM->Read_CaliPose(hv_CamPose,prID)) {
		this->write_message(8, 8, "读取相机姿态文件失败", true);
		return false;
	}

	double RectificationWidth = 75.0 / 1000.0;  // mm
	try {

		/*	SetOriginPose(this->hv_CamPose,
		-0.5*RectificationWidth,
		-0.4*RectificationWidth,
		0,
		&this->hv_RectifiPose);*/

		SetOriginPose(this->hv_CamPose,
			0,
			0,
			0,
			&this->hv_RectifiPose);

		// HObject  ho_TmpObj_RectificationMap;

		int hv_width = UCI::OPTIONS[L"图像宽度"];
		int hv_height = UCI::OPTIONS[L"图像高度"];


		GenImageToWorldPlaneMap(&this->ho_RectMap,
			this->hv_CamParameters,
			this->hv_RectifiPose,
			hv_width,
			hv_height,
			hv_width,
			hv_height,
			RectificationWidth / hv_width,
			"bilinear"
		);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"GetRefMapFromSaveFile：");

		return false;
	}

	this->m_bRectMapOK = true;  // 

	return true;
}





// MAP
void ST_MYDISP::DisOnButtonGenRectifyMap()
{
	
	// 先读一个图, 初始化一下
	this->DisOnOneShot();
	
	if (!this->GetRefMapFromSaveFile()) return;

	this->m_pMHWinMain->DispObj(this->ho_RectMap);
	this->write_message(200, 8, "内参:" + hv_CamParameters, true);
	this->write_message(200, 400, "CamPose 姿态:" + hv_CamPose, true);
	this->write_message(600, 400, "Recti  姿态:" + hv_RectifiPose, true);
}



void ST_MYDISP::DisOnOpenImage()
{
	
	
	TCHAR szFilter[] = _T("图像文件|*.bmp;*.jpg;*.jpeg;*.png|所有文件(*.*)|*.*||");

	// 构造打开文件对话框   
	CFileDialog fileDlg(true, _T("打开图像文件"), NULL, 0, szFilter, NULL);

	CString strFilePath;

	// 显示打开文件对话框   
	if (IDOK == fileDlg.DoModal()) {

		strFilePath = fileDlg.GetPathName();
	}
	else {
		return;
	}
	
	ReadImageFromFile(strFilePath);
	SetPartHvWnd();

	// 调整一下缩略图的显示区域

	

	//DisDisplay();
	write_message(12, 12, CT2A(strFilePath).m_psz, true);

	// 设置一下窗口的标题
	// this->pCView->SetWindowText(strFilePath);
}

void ST_MYDISP::DisOnSaveCurrentDISPtoFile()
{
	
	CString format = UCI::OPTIONS[L"相像保存格式"];

	CString strName;
	if (this->SaveImgToFile(UCI::stPictureSavePath, format, strName)) {
		write_message(12, 12, "保存图片成功！", true);
		write_message(32, 12, CT2A(strName).m_psz, true);

		UCI::MyLog(strName);
	}
	else {
		write_message(12, 12, "保存图片失败！", true);
	}
}

void ST_MYDISP::DisOnOpenCamera()
{
	if (this->pCAM->GetIsOpen()){
		this->DisCloseCamera();
	}
	else {
		this->DisOpenCamera();
	}

}

// 鼠标滚轮事件， 放大，缩小， 取灰度值，还原图像大小
void ST_MYDISP::DisOnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	try {
		CRect WndRect;
		double Scale = 0.1;    // 缩放步长
		double MaxScale = 40;  // 最大放大系数

		Hlong ImagePtX, ImagePtY;
		Hlong Row0_1, Col0_1, Row1_1, Col1_1;

		CString str;

		//CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
		//::GetWindowRect(this->hWnd, &WndRect);

		this->PMainStatic->GetWindowRect(&WndRect);

		//// 修正一下 WndRect, CClientRect 要小一些
		////WndRect.left += 2;             
		////WndRect.right -= 2;
		////WndRect.top += 2;
		this->m_pMHWinMain->ClearWindow();

		double dRatio = UCI::OPTIONS[L"图像宽长比"];

		int raHeight = int(WndRect.Width()* dRatio);
		WndRect.bottom = WndRect.top + raHeight;

		if (!WndRect.PtInRect(pt)) return;

		// 判断鼠标右键是不是按下了，则恢复图像的显示区域
		if (MK_RBUTTON == nFlags) {

			int hv_width = UCI::OPTIONS[L"图像宽度"];
			int hv_height = UCI::OPTIONS[L"图像高度"];

			


			this->DisDisplay(CRect(0, 0, hv_width - 1, hv_height - 1));

			// 显示一下
			PostMessage(this->pCView->GetSafeHwnd(), WM_IPDISP, NULL, NULL);

			return;
		}
		// 判断鼠标左键是不是按下了，则显示一下当前的灰度值
		if (MK_LBUTTON == nFlags) {
			HTuple x, y;
			HTuple Grey;
			HObject Cross;

			x = 1.0*(pt.x - WndRect.left)*
				(1.0*(this->rcImage.right - this->rcImage.left) / WndRect.Width())
				+ this->rcImage.left;
			y = 1.0*(pt.y - WndRect.top)*
				(1.0*(this->rcImage.bottom - this->rcImage.top) / WndRect.Height())
				+ this->rcImage.top;

			x = x.TupleRound();
			y = y.TupleRound();

			int ix = x.I();
			int iy = y.I();



			HTuple  hv_Red, hv_Green, hv_Blue;
			this->m_pMHWinMain->GetRgb(&hv_Red, &hv_Green, &hv_Blue);
			//GetRgb(hv_WindowHandle, &hv_Red, &hv_Green, &hv_Blue);

			Grey = this->m_sIPResult.result_img.GetGrayval(y, x);

			this->m_pMHWinMain->SetColor("blue");

			HalconCpp::HXLDCont cont;
			cont.GenCrossContourXld(y, x, HTuple(50.0), HTuple(0.0));
			this->m_pMHWinMain->DispObj(cont);

			write_message(y, x,
				"(x=" + x + ",y=" + y + ")grayval=" + Grey, 0);


			this->m_pMHWinMain->SetRgb(hv_Red, hv_Green, hv_Blue);
			//SetRgb(hv_WindowHandle, hv_Red, hv_Green, hv_Blue);


			return;
		}

		// 向上滑动滚轮，图像缩小。 以当前鼠标坐标点为基准
		if (zDelta > 0) {

			ImagePtX = Hlong(this->rcImage.left
				+ double(pt.x - WndRect.left) / (WndRect.Width())
				*(this->rcImage.right - this->rcImage.left));
			ImagePtY = Hlong(this->rcImage.top
				+ double(pt.y - WndRect.top) / (WndRect.Height())
				*(this->rcImage.bottom - this->rcImage.top));

			Row0_1 = Hlong(ImagePtY - 1 / (1 - Scale)*(ImagePtY - this->rcImage.top));
			Row1_1 = Hlong(ImagePtY - 1 / (1 - Scale)*(ImagePtY - this->rcImage.bottom));
			Col0_1 = Hlong(ImagePtX - 1 / (1 - Scale)*(ImagePtX - this->rcImage.left));
			Col1_1 = Hlong(ImagePtX - 1 / (1 - Scale)*(ImagePtX - this->rcImage.right));


			// 限定缩小范围
			if (fabs(Col1_1 - Col0_1) / this->GetWidth() <= 2) {
				this->DisDisplay(CRect((int)Col0_1, (int)Row0_1, (int)Col1_1, (int)Row1_1));

				// 显示一下
				PostMessage(this->pCView->GetSafeHwnd(), WM_IPDISP, NULL, NULL);
			}
		}
		else { // 向下滑动滚轮，图像放大。 以当前鼠标坐标点为基准
			ImagePtX = Hlong(this->rcImage.left
				+ double(pt.x - WndRect.left) / (WndRect.Width())
				*(this->rcImage.right - this->rcImage.left));
			ImagePtY = Hlong(this->rcImage.top
				+ double(pt.y - WndRect.top) / (WndRect.Height())
				*(this->rcImage.bottom - this->rcImage.top));

			Row0_1 = Hlong(ImagePtY - 1 / (1 + Scale)*(ImagePtY - this->rcImage.top));
			Row1_1 = Hlong(ImagePtY - 1 / (1 + Scale)*(ImagePtY - this->rcImage.bottom));
			Col0_1 = Hlong(ImagePtX - 1 / (1 + Scale)*(ImagePtX - this->rcImage.left));
			Col1_1 = Hlong(ImagePtX - 1 / (1 + Scale)*(ImagePtX - this->rcImage.right));

			// 限制放大范围
			if (this->GetWidth() / fabs(Col1_1 - Col0_1) <= MaxScale) {
				this->DisDisplay(CRect((int)Col0_1, (int)Row0_1, (int)Col1_1, (int)Row1_1));

				// 显示一下
				PostMessage(this->pCView->GetSafeHwnd(), WM_IPDISP, NULL, NULL);
			}
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"DisOnMouseWheel()：");
	}

}

void ST_MYDISP::DisOnMouseLeftDown(UINT nFlags, CPoint point)
{
	
}

void ST_MYDISP::DisOnMouseMove(UINT nFlags, CPoint point)
{
	//this->pImg->bMustPaint = true;	

	//this->ptMouseLdownNow = point;


	// 这个只有在主框架上写
	//CMainFrame *pMain = (CMainFrame *)AfxGetMainWnd();
	//CClientDC DC(pMain);

	//CPen pen(PS_SOLID, 1, RGB(255, 0, 0));	
	//CPen *oldpen = pDC->SelectObject(&pen);


	//int len = 10;
	//pDC->MoveTo(point.x - len, point.y - len);
	//pDC->LineTo(point.x + len, point.y + len);
	//pDC->MoveTo(point.x - len, point.y + len);
	//pDC->LineTo(point.x + len, point.y - len);

	// pDC->SelectObject(oldpen);
}

void ST_MYDISP::DisOnRButtonDown(UINT nFlags, CPoint point)
{
}

void ST_MYDISP::DisOnRButtonUp(UINT nFlags, CPoint point)
{
	//ClearWindow(this->hv_HWD);
	//// 恢复原来的比例
	//CRect rc = { 0,0,this->pImg->iWidth,this->pImg->iHeight };
	//this->pImg->ChangeImageCRect(rc);
	//this->SetPartHvWnd();
	//this->bMouseLBdown = false;
}



// 相机采集一张照片
void ST_MYDISP::DisOnOneShot()
{
	DisOpenCamera();


	this->pCAM->OneShot();

	EnterCriticalSection(&this->m_csFGDisp);
	this->pCAM->GetBufferImage(&this->GrabImageDisp);
	LeaveCriticalSection(&this->m_csFGDisp);

	EnterCriticalSection(&m_csIPDisp);      
	m_sIPResult.result_img = this->GrabImageDisp;   
	LeaveCriticalSection(&m_csIPDisp);      

	this->OnFGDisp(NULL, NULL);   // 用来表示显示那个
	this->OnMainDisp(NULL, NULL);

}

// 相机采集多张照片,不同的曝光,增益
void ST_MYDISP::DisOnCamCapMulti()
{
	DisOpenCamera();

	HImage      GrabImage;

	// TODO: 在此添加命令处理程序代码
	HTuple hv_Start, hv_End;
	CString str;

	// 设置采集总帧数
	int nFrameNum = 7;

	// 记录开始计时
	CountSeconds(&hv_Start);

	// 循环采集，保存及显示
	for (int i = 0; i < nFrameNum; i++) {

		//// 设置曝光
		//this->pCAM->SetShutter(500 * (i)+1);
		//// 设置增益
		//this->pCAM->SetGain(5 * i);

		// 采集一帧图像
		this->DisOnOneShot();

		CString format = UCI::OPTIONS[L"相像保存格式"];

		CString strName;
		bool res = SaveImgToFile(UCI::stShutterSavePath, format, strName);

		if (res) {
			UCI::MyLog(strName);
			UCI::MyLog("保存图片成功了！");
		}

		Sleep(10);
		
	}
	// 记录结束时间
	CountSeconds(&hv_End);

	str.Format(L"连续曝光并采集了 %d 帧图像， 总时间(s) %f",
		nFrameNum, (hv_End - hv_Start).D());
	UCI::MyLog(str);
}



// 开始连续采集图像 
void ST_MYDISP::StartContiouscap()
{
	BOOL b_FGIsRunning = this->pCAM->GetIsFGRuning();

	// Call back function to start the framegrabber
	if (b_FGIsRunning == TRUE)
	{
		// already started		
		MyLog(L"当前已经在采集图像了!");  //BEEP_ERROR;
		return;
	}
	else {

		if (this->m_hFGThrHandle)
		{
			WaitForSingleObject(m_hFGThrHandle, INFINITE);
			CloseHandle(this->m_hFGThrHandle);
			this->m_hFGThrHandle = NULL;
		}

		// start a new thread which openes a frame grabber and grabs continuously
		this->m_hFGThrHandle = (HANDLE)_beginthreadex(NULL,  // do not suspend thread  
			0,      // use same stack size as with main thread 
			(unsigned(__stdcall *)(void*))&FGCapContinues,  //unsigned ( __stdcall *start_address )( void * ) 
			this->pCView, //this->,   // parameters          
			0,      // start thread; don't suspend it       
			&this->m_uiFGThreadId);/* Win32 thread id.; note that this is  */

		MyLog(L"开始连续采集图像中...");
	}
}

void ST_MYDISP::StopContiouscap()
{
	BOOL b_FGIsRunning;

	//EnterCriticalSection(&newImageMutex);
	b_FGIsRunning = this->pCAM->GetIsFGRuning();
	//LeaveCriticalSection(&newImageMutex);

	if (b_FGIsRunning)
	{
		// send stop signal to running framegrabber
		SetEvent(m_hFGStopEvent);

		//m_cFGStopButton.EnableWindow(FALSE);
		//m_cFGStartButton.EnableWindow(TRUE);
		//BEEP_OK;
		MyLog(L"StopContiouscap ok.");  //BEEP_ERROR;
	}
	else
	{
		// nothing to stop
		//BEEP_ERROR;
		MyLog(L"nothing to stop...");  //BEEP_ERROR;
	}
}



// 连续采集图像按钮
void ST_MYDISP::DisOnCamContinueCap()
{


	if (this->mAppState == App_ContinueingCap) {
		this->StopContiouscap();
		this->AppStateChangeTo(APP_READY);
	}
	else if (this->mAppState == APP_READY || mAppState == APP_LOGGING_OK) {

		DisOpenCamera();

		// 使用当前项目的照相参数
		UCI::OptionsMap *camo = &UCI::ELE.EleList.GetHead();
		UCI::sDB.SetCamPara(*camo);
		
		this->AppStateChangeTo(App_ContinueingCap);
		this->StartContiouscap();		
	}


}

void ST_MYDISP::DisOnAutoStop()
{
	if (this->mAppState == App_AutoMeasureing) {
		this->StopContiouscap();
		this->AppStateChangeTo(APP_READY);
	}
}

void ST_MYDISP::DisOnButCaliShowInfo()
{
	
	HPose hv_pose;
	int prID = UCI::OPTIONS[L"项目编号"];
	HTuple hv_CameraParameters;
	
	if (this->pCAM->Read_CaliPose(hv_pose, prID)) {
		write_message(40, 200, "相机姿态：" + hv_pose, 0);
	}
	else {
		write_message(40, 200, "相机姿态没有数据！");
	}

	if (this->pCAM->Read_CaliCal(hv_CameraParameters, prID)) {
		write_message(600, 200, "相机内参：" + hv_CameraParameters, 0);
	}
	else {
		write_message(600, 200, "相机内参没有数据！");
	}

}




