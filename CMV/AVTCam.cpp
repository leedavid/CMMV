#include "stdafx.h"
#include "AVTCam.h"

// 连续采集图像

DWORD WINAPI ContinousGrab(LPVOID pParam)
{
	// FGFRAME Frame;
	UINT32 error;

	AVTCam* pOwner = (AVTCam*)pParam;   // 
	pOwner->set_busy(TRUE);             // 设置相机忙碌状态

	MYFRAME* pmyF = pOwner->GetPFrame();

	// 开始采集
	pOwner->get_camera()->StartDevice();       // 
	while (pOwner->get_busy()) {

		// 获取一帖图像
		error = pOwner->get_camera()->GetFrame(pmyF, 200);  // 立刻采集

		// 判断采图返回值是否正确，采图是否为空
		if (error == FCE_NOERROR){ // && Frame.pData != NULL) {

			// 对图像进行拷贝
			// memcpy(pOwner->pImage, Frame.PData, pOwner->iImageHeight*pOwner->iImageWidth);
			// HalconCpp::CopyImage(Frame.ho_FRAME, pOwner->get_hv_image());

			// 使用消息机制将图像传出
			if (pOwner->m_fcallback != NULL) {
				pOwner->m_fcallback(pOwner->get_parent(), pOwner->get_camera_index());
			}
			// 清空图像，准备接收下一帖图像
			// error = pOwner->get_camera()->PutFrame(&Frame);
		}
		else {
			// 将CPU睡眠 10ms
			Sleep(10);
		}
	}

	pOwner->get_camera()->StopDevice();

	return 0;
}


AVTCam::AVTCam(CWnd * PWnd, MYFRAME* pF)
{	
	//NodeCnt = 0;
	m_bBusy = false;
	m_iCameraIndex = -1;
	//pImage = NULL;
	m_fcallback = NULL;
	Result = FCE_NOERROR;	

	m_pParent = PWnd;
	this->pmyFrame = pF;

	// 相机标定
	nTotalPicture = 8;						// 总共需要的标定图片数    
	dOnePicMaxError = 0.55;					// 标定板最大允许像素误差
	bOnePIcMaxErrorOK = false;              // 标定图像的误差OK

	CaliPlateThick = 0.001;					// 标定板厚度
	RectificationWidth = 75.0;				// width of the visible area in world coordinates[mm]
											// 目录
	DescrDir = L"Calibration File";		    // 标定目录
	DirInnerPicture = L"7x72caltabInner";	// DescrFileName + id
	DirPosePicture = L"7x72caltabPose";     // 
	DescrFileName = L"7x72caltab.descr";    // 
	DescrPsName = L"7x72caltab.ps";			// 标定 PostScrip 文件名称

	CamCaliCcdFile = L"7x72caltab.ccd";     // 相机标定文件，保存了上次的标定信息

	CamParaCalFile = L"7x72caltab.cal";     // 
	CamPosDatFile = L"7x72caltab.dat";      // 

	CamerModel = L"area_scan_polynomial";
	bIsYuanXin = false;
	bIsSlope = false;
	fSx = 8.3;
	fSy = 8.3;
	fFocus = 8;
}

AVTCam::~AVTCam()
{
	if (this->m_bBusy) {
		this->m_bBusy = false;
		Sleep(100);
	}
	
	SetTriggerMode(false);
	Camera.StopDevice();
	Camera.CloseCapture();

	// 释放图像缓存

	//if (pImage != NULL) {
	//	delete pImage;
	//	pImage = NULL;
	//}
}

void AVTCam::CPPExpDefaultExceptionHandler(const HalconCpp::HException & except)
{
	CString err = CA2W(except.ProcName().Text());
	err += CA2W(except.ErrorMessage());
	AfxMessageBox(err);
}

//HalconCpp::HObject * AVTCam::get_hv_image()
//{
//	return this->ho_pImage;
//}

void AVTCam::set_busy(bool b)
{
	this->m_bBusy = b;
}

bool AVTCam::get_busy()
{
	return this->m_bBusy;
}

int AVTCam::get_ImageHeight()
{
	return (int)this->iImageHeight;
}

int AVTCam::get_ImageWidth()
{
	return (int)this->iImageWidth;
}

void AVTCam::set_ImageWidth(int width)
{
	this->iImageWidth = width;
}

int AVTCam::get_camera_index()
{
	return this->m_iCameraIndex;
}

CWnd * AVTCam::get_parent()
{
	return this->m_pParent;
}

void AVTCam::set_ImageHeight(int height)
{
	this->iImageHeight = height;
}

CCamera * AVTCam::get_camera()
{
	return &this->Camera;
}

bool AVTCam::IsCaliPictureOk()
{
	return this->bOnePIcMaxErrorOK;
}

CString AVTCam::GetCamerModel()
{
	return this->CamerModel;
}

CString AVTCam::GetMoudlePath()
{
	TCHAR modulePath[MAX_PATH];
	GetModuleFileName(NULL, modulePath, MAX_PATH);
	CString _strMoudlePath(modulePath);
	_strMoudlePath = _strMoudlePath.Left(_strMoudlePath.ReverseFind('\\'));

	return _strMoudlePath;
}

CString AVTCam::GetCaliInnerPicPath()
{
	CString str = this->GetMoudlePath() + L"\\"
		+ this->DescrDir + L"\\" + this->DirInnerPicture;
	return str;
}

CString AVTCam::GetCaliPosePicPath()
{
	CString str = this->GetMoudlePath() + L"\\"
		+ this->DescrDir + L"\\" + this->DirPosePicture;
	return str;
}

CString AVTCam::GetCalibCcdPathFile()
{
	return  this->GetCaliInnerPicPath() +
		L"\\" + this->CamCaliCcdFile;
}

CString AVTCam::GetCaliCalPathFile()
{
	return  this->GetCaliInnerPicPath() +
		L"\\" + this->CamParaCalFile;
}

CString AVTCam::GetCaliPosePathFile()
{
	return  this->GetCaliPosePicPath() +
		L"\\" + this->CamPosDatFile;
}

bool AVTCam::Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle)
{
	return this->Write_CalibrationCcd(hv_CalibHandle, GetCalibCcdPathFile());
}

bool AVTCam::Read_CalibrationCcd(HalconCpp::HTuple & hv_CalibHandle)
{
	return this->Read_CalibrationCcd(hv_CalibHandle, GetCalibCcdPathFile());
}

bool AVTCam::Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, CString Path)
{
	try {
		WriteCalibData(hv_CalibHandle, CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}

bool AVTCam::Read_CalibrationCcd(HalconCpp::HTuple & hv_CalibHandle, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		ReadCalibData(CT2A(Path).m_psz, &hv_CalibHandle);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}



bool AVTCam::Write_CaliPose(HalconCpp::HTuple CameraPose)
{
	return this->Write_CaliPose(CameraPose, GetCaliPosePathFile());
}

bool AVTCam::Read_CaliPose(HalconCpp::HTuple & CameraPose)
{
	return this->Read_CaliPose(CameraPose, GetCaliPosePathFile());
}

bool AVTCam::Write_CaliPose(HalconCpp::HTuple CameraPose, CString Path)
{
	try {
		HalconCpp::WritePose(CameraPose, CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}

bool AVTCam::Read_CaliPose(HalconCpp::HTuple & CameraPose, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		HalconCpp::ReadPose(CT2A(Path).m_psz, &CameraPose);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}

bool AVTCam::Write_CaliCal(HalconCpp::HTuple CameraParam)
{
	return this->Write_CaliCal(CameraParam, this->GetCaliCalPathFile());
}

bool AVTCam::Read_CaliCal(HalconCpp::HTuple & CameraParam)
{
	return this->Read_CaliCal(CameraParam, this->GetCaliCalPathFile());
}

bool AVTCam::Write_CaliCal(HalconCpp::HTuple CameraParam, CString Path)
{
	try {
		HalconCpp::WriteCamPar(CameraParam, CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}

bool AVTCam::Read_CaliCal(HalconCpp::HTuple &CameraParam, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		HalconCpp::ReadCamPar(CT2A(Path).m_psz, &CameraParam);
		return true;
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
		return false;
	}
}

CString AVTCam::GetDescrFileName()
{
	return this->GetMoudlePath() + "\\" + DescrDir + "\\" + DescrFileName;
}

double AVTCam::GetFocus()
{
	return this->fFocus;
}

double AVTCam::GetFsx()
{
	return this->fSx;
}

double AVTCam::GetFsy()
{
	return this->fSy;
}

double AVTCam::GetdOnePicMaxError()
{
	return this->dOnePicMaxError;
}

double AVTCam::GetCaliPlateThick()
{
	return this->CaliPlateThick;
}

double AVTCam::GetRectificationWidth()
{
	return this->RectificationWidth;
}

void AVTCam::SetfError(double f)
{
	this->fError = f;
}


int AVTCam::GetTotalCaliPicture()
{
	return this->nTotalPicture;
}

void AVTCam::SetbOnePIcMaxErrorOK(bool b)
{
	this->bOnePIcMaxErrorOK = b;
}

// 打开相机
bool AVTCam::open_camera()
{	
	if (this->IsOpened()) {
		AfxMessageBox(L"相机已打开，不必再打开了"); 
		return true;
	}

	// 这儿准备连接词

	if (this->InitialCamera()) {
		return true;
	}
	else {
		AfxMessageBox(L"当前相机无法打开！");
		return false;
	}	
}

bool AVTCam::InitialCamera()
{
	Result = FCE_NOERROR;
	
	m_fcallback = NULL;

	// 连接相机
	// if (Result == FCE_NOERROR && NodeCnt) {
	if (Result == FCE_NOERROR) {
		Result = Camera.Connect();
	}
	else if (Result == HALER_BUSY || this->IsOpened()) {
		AfxMessageBox(_T("相机工作中！"));
		return false;
	}
	else {
		AfxMessageBox(_T("相机初始化失败！"));
		return false;
	}

	// 设置触发模式
	// SetTriggerMode(TRUE);

	// 获取图像宽度
	Camera.GetParameter(FGP_XSIZE, &iImageWidth);
	Camera.GetParameter(FGP_YSIZE, &iImageHeight);


	// 开启相机
	Camera.StartDevice();

	return true;
}

bool AVTCam::close_camera(){

	if (Camera.CloseCapture() == FCE_NOERROR) {
		return true;
	}
	return false;
}

bool AVTCam::OneShotA()
{
	if (m_bBusy)    // 当前正在连续拍摄
	{
		return false;
	}

	// 获取一帧图像，最多等待 1000ms,
	Result = Camera.GetFrame(this->GetPFrame(),1000);


	if (FCE_NOERROR == Result) {
		return true;
	}
	else {
		return false;
	}	
}


// 单帖采集
bool AVTCam::Snap(void) {

	// 开始采集
	Camera.StartDevice();

	// 检测相机序号是否大于等于0， 检测相机是否处于忙碌状态
	if (!m_bBusy) {
		// 清空采集到的图像
		// Camera.DiscardFrames();

		//FGFRAME Frame;

		// 获取一帖图像，最多等待 1000ms, 
		Result = Camera.GetFrame(this->GetPFrame(),1000);

		if (Result == FCE_NOERROR) {
			// 清除缓存，等待接收下一帧图像
			// 如果回调函数不为空，执行回调函数，进行图像处理
			if (m_fcallback != NULL) {
				m_fcallback(m_pParent, m_iCameraIndex);
			}
			return true;
		}
		else {
			return false;
		}
	}
	else {
		AfxMessageBox(_T("请确认相机状态！"));
		return false;
	}
}

// 连续图像采集

bool AVTCam::Grab(void)
{
	// 检测相机是否处于忙碌状态
	if (m_bBusy) {
		return true;
	}
	if (!this->IsOpened()) {
		AfxMessageBox(_T("相机不存在！"));
		return true;
	}

	// 启动相机采集线程进行连续采集
	m_hThread = CreateThread(NULL, 0, ContinousGrab, this, 0, NULL);

	// 如果出错，则提示信息并且返回
	// if (FAILED((HRESULT)(m_hThread))) {
	if (m_hThread < 0) {
		AfxMessageBox(_T("建立grap线程失败！"));
		return false;
	}
	return true;
}

bool AVTCam::Freeze(void)
{
	m_bBusy = false;
	Sleep(20);
	return true;
}

bool AVTCam::SetTriggerMode(bool bEnable, int nSrc)
{
	//UINT32 nOn;
	//// 是否使用触发模式
	//if (bEnable) {
	//	nOn = 1;
	//}
	//else {
	//	nOn = 0;
	//}

	//// 0=ext.trigger off 1= ext.trigger on
	//UINT32 nPolarity = 0; // 0=低电平触发 1=高电平触发
	////UINT32 nSrc = 1;		// 触发源设置
	////UINT32 nSrc = 7;	    // 触发源设置 1=外触发 7=软触发
	//UINT32 nMode = 0;       // 0=边缘触发 1=电平触发
	//UINT32 nParm = 0;       // 预留 不使用
	//UINT32 TriggeValue = 0;

	////TriggeValue = MAKETRIGGER(nOn, nPolarity, nSrc, nMode, nParm);
	//Camera.SetParameter(FGP_TRIGGER, TriggeValue);

	// 设置防抖延时，防止误触发。
	//if (nSrc == 1) {
	//	Camera.WriteRegister(0xf1000840, 0x8000000A);
	//}

	return true;
}

bool AVTCam::SetShutter(int iValue)
{
	Camera.Set_Shutter(iValue);
	return true;
}

bool AVTCam::SetGain(double gain)
{
	Camera.Set_Gain(gain);
	return true;
}

int AVTCam::GetShutter(void)
{
	UINT32 iValue = 0;
	Camera.GetParameter(FGP_SHUTTER, &iValue);
	return iValue;
}

int AVTCam::GetGain(void)
{
	UINT32 iValue = 0;
	Camera.GetParameter(FGP_GAIN, &iValue);
	return iValue;
}
//
//bool AVTCam::Get_bIsHardTrigger()
//{
//	return this->bIsHardTrigger;
//}
//
//void AVTCam::Set_bIsHardTrigger(bool b)
//{
//	this->bIsHardTrigger = b;
//}

bool AVTCam::Get_m_bRGBmode()
{
	return this->Camera.Get_m_bRGBmode();
}

void AVTCam::Set_m_bRGBmode(bool b)
{
	this->Camera.Set_m_bRGBmode(b);
}

//BOOL AVTCam::Get_ExposureMode()
//{
//	return this->Camera.Get_ExposureMode();
//}

void AVTCam::Set_ExposureMode(BOOL ｂ)
{
	this->Camera.Set_ExposureMode(ｂ);
}

bool AVTCam::IsOpened()
{
	return this->Camera.Get_bIsOpened();
}

void AVTCam::Set_interfaceName(CString str)
{
	this->Camera.Set_interfaceName(str);
}

CString AVTCam::Get_interfaceName()
{
	return this->Camera.Get_interfaceName(); 
}

void AVTCam::Set_deviceName(CString str)
{
	this->Camera.Set_deviceName(str);
}





