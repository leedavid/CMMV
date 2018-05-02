#include "stdafx.h"
#include "MyData.h"
#include "CCameraCPP.h"

using namespace HalconCpp;
using namespace std;

CCameraCPP::CCameraCPP()
{
	m_pFG = NULL;
	bIsOpened = FALSE;

	this->m_bFGIsRunning = FALSE;

	//this->iWidth = -1;
	//this->iHeight = -1;


	// 相机标定

	bOnePIcMaxErrorOK = false;              // 标定图像的误差OK

	InitializeCriticalSection(&newImageMutex);
	newImageEven = CreateEventW(NULL, FALSE, FALSE, NULL);

	ResetEvent(newImageEven);

}

CCameraCPP::~CCameraCPP()
{
	if (m_pFG != NULL) delete m_pFG;

	DeleteCriticalSection(&newImageMutex);
	CloseHandle(this->newImageEven);
}



BOOL CCameraCPP::GetIsOpen()
{
	return this->bIsOpened;
}

//void CCameraCPP::SetShutter(int i)
//{
//	this->m_pFG->SetFramegrabberParam("爆光时间", i);
//}
//
//void CCameraCPP::SetGain(double g)
//{
//	this->m_pFG->SetFramegrabberParam("图像增益", g);
//}

//int CCameraCPP::GetWidth()
//{
//	return this->iWidth;
//}
//
//void CCameraCPP::SetWidth(int i)
//{
//	this->iWidth = i;
//}
//
//int CCameraCPP::GetHeight()
//{
//	return this->iHeight;
//}
//
//void CCameraCPP::SetHeight(int i)
//{
//	this->iHeight = i;
//}




int CCameraCPP::GetParameterInt(CString sPara)
{
	HalconCpp::HTuple hv_Value;
	//this->m_pFG->GetFramegrabberParam(CT2A(sPara).m_psz, &hv_Value);
	hv_Value = this->m_pFG->GetFramegrabberParam(CT2A(sPara).m_psz);
	return hv_Value.I();
}

CString CCameraCPP::GetParameterString(CString sPara)
{
	CString s;
	try {
		HalconCpp::HTuple hv_Value;
		hv_Value = this->m_pFG->GetFramegrabberParam(CT2A(sPara).m_psz);
		s.Format(L"%S", hv_Value.SArr()[0]);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"GetParameterString()：");

	}
	return s;
}

double CCameraCPP::GetParameterDouble(CString sPara)
{
	HalconCpp::HTuple hv_Value;
	try {
		hv_Value = this->m_pFG->GetFramegrabberParam(CT2A(sPara).m_psz);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"GetParameterDouble()：");

	}
	return hv_Value.D();
}


void CCameraCPP::SetParameterHTuple(std::string str, HalconCpp::HTuple ht)
{
	try {
		this->m_pFG->SetFramegrabberParam(str.c_str(), ht);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"SetParameterHTuple()：");

	}
}

HalconCpp::HFramegrabber * CCameraCPP::GetPFG()
{
	return this->m_pFG;
}

BOOL CCameraCPP::Connect(CString str)
{

	CString interfaceName = UCI::OPTIONS[L"相机协议"];
	CString deviceName = UCI::OPTIONS[L"相机名称"];
	try {
		if (str != L"NotFiles") {
			this->m_pFG = new HFramegrabber("File",             /* Name of Frame Grabber Interface */
				1, 1,               /* Image Resolution                */
				0, 0,               /* Image Size                      */
				0, 0,               /* Start Row and Column            */
				"default",          /* Fields                          */
				-1,                 /* Bits per channel                */
				"default",          /* Color space                     */
				-1.0,               /* Gain                            */
				"false",            /* External trigger                */
				CT2A(str).m_psz,    /* Camera type                     */
				"0",                /* Device                          */
				-1,                 /* Port                            */
				-1                  /* Line in                         */
			);
			this->OneShot();
		}
		else if (interfaceName == L"DirectShow") {

			this->m_pFG = new HFramegrabber(CT2A(interfaceName).m_psz,  /* Name of Frame Grabber Interface */
				1, 1,							/* Image Resolution                */
				0, 0,                           /* Image Size                      */
				0, 0,                           /* Start Row and Column            */
				"default",                      /* Fields                          */
				8,								/* Bits per channel                */
				"rgb",                          /* Color space                     */
				-1.0,                           /* Gain                            */
				"false",                        /* External trigger                */
				"default",                      /* Camera type                     */
				CT2A(deviceName).m_psz,                     /* Device                          */
				0,                              /* Port                            */
				-1                              /* Line in                         */
			);


		}
		else if (interfaceName == L"MindVision12_X64") {

			//if (deviceName == "CM500") {

			this->m_pFG = new HFramegrabber(CT2A(interfaceName).m_psz,   /* Name of Frame Grabber Interface */
				1, 1,							/* Image Resolution                */
				0, 0,                           /* Image Size                      */
				0, 0,                           /* Start Row and Column            */
				"progressive",                  /* Fields                          */
				8,								/* Bits per channel                */
				"Gray",                         /* Color space                     */
				-1.0,                           /* Gain                            */
				"false",                        /* External trigger                */
				"auto",                         /* Camera type                     */
				CT2A(deviceName).m_psz,         /* Device                          */
				0,                              /* Port                            */
				-1                              /* Line in                         */
			);

			//}
			//else if (deviceName == "CM-300") {
			//	this->m_pFG = new HFramegrabber(interfaceName.c_str(),  /* Name of Frame Grabber Interface */
			//		1, 1,							/* Image Resolution                */
			//		0, 0,                           /* Image Size                      */
			//		0, 0,                           /* Start Row and Column            */
			//		"progressive",                  /* Fields                          */
			//		8,								/* Bits per channel                */
			//		"Gray",                         /* Color space                     */
			//		-1.0,                           /* Gain                            */
			//		"false",                        /* External trigger                */
			//		"auto",                         /* Camera type                     */
			//		deviceName.c_str(),         /* Device                          */
			//		0,                              /* Port                            */
			//		-1                              /* Line in                         */
			//	);
			//}
			// 初始化相机
			//OpenFramegrabber("MindVision12_X64",
			//	1, 1, 0, 0, 0, 0, "progressive",
			//	8, "BGR24", -1, "false", "auto", CT2A(deviceName).m_psz, 0, -1, &hv_AcqHandle);

			//OpenFramegrabber("MindVision12_X64",
			//	1, 1, 0, 0, 0, 0, "progressive",
			//	8, "Gray", -1, "false", "auto", CT2A(deviceName).m_psz, 0, -1, &hv_AcqHandle);

			// open_framegrabber ('MindVision12_X64', 1, 1, 0, 0, 0, 0, 
			// 'progressive', 8, 'Gray', -1, 'false', 'auto', 'CM500', 0, -1, AcqHandle)
		}
		else {
			return FALSE;
		}

		HalconCpp::HTuple hv_Value;

		hv_Value = this->m_pFG->GetFramegrabberParam("image_width");
		UCI::OPTIONS[L"图像宽度"] = hv_Value.I();


		hv_Value = this->m_pFG->GetFramegrabberParam("image_height");
		UCI::OPTIONS[L"图像高度"] = hv_Value.I();
		
		//UCI::ELE.PrePareHoGrayImg();

		int w = UCI::OPTIONS[L"图像宽度"];
		int h = UCI::OPTIONS[L"图像高度"];

		// 相机的参数也要更新一下。
		UCI::OptionsMap *om = UCI::ELE.GetOptionsMapByEletype(ELEMENT_CAMERA);
		(*om)[L"采样宽度"] = w;
		(*om)[L"采样高度"] = h;

		UCI::OPTIONS[L"ORGwhByCamera"] = 1;

		
	//	this->m_pFG->GrabImage();

		this->m_pFG->GrabImageStart(-1);	

		int delay = UCI::OPTIONS[L"首张延时"];
		Sleep(delay);
		//HImage  GrabImage;
		//GrabImage = this->m_pFG->GrabImageAsync(-1.0f);

		this->bIsOpened = true;

	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Connect(CString str): ");

		return FALSE;
	}

	//if (this->bIsOpened) {
	//	Sleep(1500);
	//	HImage  GrabImage;
	//	GrabImage = this->m_pFG->GrabImageAsync(-1.0f);
	//	


	//	//HImage  GrabImage;
	//	Sleep(1500);
	//	GrabImage = this->m_pFG->GrabImageAsync(-1.0f);
	//	Sleep(1500);
	//	//GrabImage = this->m_pFG->GrabImageAsync(-1.0f);
	//}

	return TRUE;
}

BOOL CCameraCPP::close_camera()
{
	if (this->m_pFG != NULL) {
		delete this->m_pFG;
		this->bIsOpened = false;
	}
	return TRUE;
}

//bool CCameraCPP::IsCaliPictureOk()
//{
//	return this->bOnePIcMaxErrorOK;
//}


CString CCameraCPP::GetCaliInnerPicPath(int prid)
{

	CString stDeviceName = UCI::OPTIONS[L"相机名称"];
	CString str = UCI::strMoudlePath;

	CString res = (str + L"\\project\\" + CA2T(to_string(prid).c_str()).m_psz
		+ L"\\inner\\" + stDeviceName);
	return res;
}

CString CCameraCPP::GetCaliPosePicPath(int prid)
{
	CString  stDeviceName = UCI::OPTIONS[L"相机名称"];
	CString str = UCI::strMoudlePath;

	CString res = (str + L"\\project\\" + CA2T(to_string(prid).c_str()).m_psz
		+ L"\\pos\\" + stDeviceName);
	return res;
}

CString CCameraCPP::GetCalibCcdPathFile(int prID)
{

	CString str = UCI::OPTIONS[L"CaliFileSaveName"];
	CString res = (this->GetCaliInnerPicPath(prID) + L"\\"
		+ str + L".ccd");
	return res;
}

CString  CCameraCPP::GetCaliCalPathFile(int prID)
{

	CString str = UCI::OPTIONS[L"CaliFileSaveName"];

	//CString ss = this->GetCaliInnerPicPath(prID);

	//CString res = ss + "\\" + str + ".cal";

	CString res = (this->GetCaliInnerPicPath(prID) + L"\\"
		+ str + L".cal");
	return res;
}

CString  CCameraCPP::GetCaliPoseDataPathFile(int prID)
{
	CString str = UCI::OPTIONS[L"CaliFileSaveName"];
	CString res = (this->GetCaliPosePicPath(prID) + L"\\"
		+ str + L".dat");
	return res;
}

CString CCameraCPP::GetCaliPostScriptsPathFile(int prID)
{
	CString str = UCI::OPTIONS[L"CaliFileSaveName"];
	CString res = (this->GetCaliPosePicPath(prID) + L"\\"
		+ str + L".ps");
	return res;
}



CString CCameraCPP::GetCaliDescribePathFile(int prID)
{
	CString name = UCI::OPTIONS[L"标定描述文件"];
	CString str = UCI::strMoudlePath;
	CString res = (str + L"\\caltab\\" + name);
	return res;
}


bool CCameraCPP::Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, int prID)
{
	return this->Write_CalibrationCcd(hv_CalibHandle, GetCalibCcdPathFile(prID));
}

bool CCameraCPP::Read_CalibrationCcd(HalconCpp::HTuple & hv_CalibHandle, int prID)
{
	return this->Read_CalibrationCcd(hv_CalibHandle, GetCalibCcdPathFile(prID));
}

bool CCameraCPP::Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, std::string Path)
{
	return Write_CalibrationCcd(hv_CalibHandle, CA2T(Path.c_str()).m_psz);
}

bool CCameraCPP::Read_CalibrationCcd(HalconCpp::HTuple & hv_CalibHandle, std::string Path)
{
	return Read_CalibrationCcd(hv_CalibHandle, CA2T(Path.c_str()).m_psz);

}

bool CCameraCPP::Write_CalibrationCcd(HalconCpp::HTuple hv_CalibHandle, CString Path)
{
	try {
		WriteCalibData(hv_CalibHandle, CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Write_CalibrationCcd：");

		return false;
	}
}

bool CCameraCPP::Read_CalibrationCcd(HalconCpp::HTuple & hv_CalibHandle, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		ReadCalibData(CT2A(Path).m_psz, &hv_CalibHandle);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Read_CalibrationCcd：");

		return false;
	}
}

bool CCameraCPP::Write_CaliPose(HPose pose, int prID)
{
	return this->Write_CaliPose(pose, GetCaliPoseDataPathFile(prID));
}

bool CCameraCPP::Write_CaliPose(HalconCpp::HPose CameraPose, std::string Path)
{
	return Write_CaliPose(CameraPose, CA2T(Path.c_str()).m_psz);
}

bool CCameraCPP::Write_CaliPose(HalconCpp::HPose CameraPose, CString Path)
{
	try {
		CameraPose.WritePose(CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Write_CaliPose：");

		return false;
	}
}

bool CCameraCPP::Read_CaliPose(HalconCpp::HPose & CameraPose, int prID)
{
	return this->Read_CaliPose(CameraPose, GetCaliPoseDataPathFile(prID));
}

bool CCameraCPP::Read_CaliPose(HalconCpp::HPose & CameraPose, std::string Path)
{
	return Read_CaliPose(CameraPose, CA2T(Path.c_str()).m_psz);
}

bool CCameraCPP::Read_CaliPose(HalconCpp::HPose & CameraPose, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		CameraPose.ReadPose(CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Read_CaliPose：");

		return false;
	}
}

bool CCameraCPP::Write_CaliCal(HalconCpp::HTuple CameraParam, int prID)
{
	return this->Write_CaliCal(CameraParam, this->GetCaliCalPathFile(prID));
}

bool CCameraCPP::Read_CaliCal(HalconCpp::HTuple & CameraParam, int prID)
{
	return this->Read_CaliCal(CameraParam, this->GetCaliCalPathFile(prID));
}

bool CCameraCPP::Write_CaliCal(HalconCpp::HTuple CameraParam, std::string Path)
{
	return Write_CaliCal(CameraParam, CA2T(Path.c_str()).m_psz);
}

bool CCameraCPP::Read_CaliCal(HalconCpp::HTuple & CameraParam, std::string Path)
{
	return Read_CaliCal(CameraParam, CA2T(Path.c_str()).m_psz);
}

bool CCameraCPP::Write_CaliCal(HalconCpp::HTuple CameraParam, CString Path)
{
	try {
		HalconCpp::WriteCamPar(CameraParam, CT2A(Path).m_psz);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Write_CaliCal：");

		return false;
	}
}

bool CCameraCPP::Read_CaliCal(HalconCpp::HTuple & CameraParam, CString Path)
{
	if (!PathFileExists(Path)) {
		return false;
	}

	try {
		HalconCpp::ReadCamPar(CT2A(Path).m_psz, &CameraParam);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"Read_CaliCal：");

		return false;
	}
}


void CCameraCPP::SetbOnePIcMaxErrorOK(bool b)
{
	this->bOnePIcMaxErrorOK = b;
}

void CCameraCPP::SetfError(double f)
{
	this->fError = f;
}



void CCameraCPP::OneShot()
{	

	try 
	{
		//Sleep(1000);
		HImage  GrabImage;
		GrabImage = this->m_pFG->GrabImageAsync(-1);
		// Image handoff
		EnterCriticalSection(&this->newImageMutex);
		if (this->imgList.size() < MAX_BUFFERS) {
			this->imgList.push_back(GrabImage);
		}
		LeaveCriticalSection(&this->newImageMutex);
		SetEvent(this->newImageEven);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"OneShot()：");

	}
}

HBOOL CCameraCPP::GetIsFGRuning()
{
	HBOOL b;
	EnterCriticalSection(&newImageMutex);
	b = this->m_bFGIsRunning;
	LeaveCriticalSection(&newImageMutex);
	return b;
}

void CCameraCPP::SetFGRuning(HBOOL b)
{
	EnterCriticalSection(&this->newImageMutex);
	this->m_bFGIsRunning = b;
	LeaveCriticalSection(&this->newImageMutex);
}

void CCameraCPP::GetBufferImage(HalconCpp::HImage * img)
{
	//unsigned       idx;
	// get the new image buffer

	// HalconCpp::HImage img;

	try {

		EnterCriticalSection(&newImageMutex);

		if (imgList.size() > 0) {
			*img = this->imgList.front();
			this->imgList.pop_front();
		}
		LeaveCriticalSection(&newImageMutex);

		ResetEvent(this->newImageEven);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"GetBufferImage()：");

	}

	return;

}

void CCameraCPP::GrabOneImageAsync(HalconCpp::HImage* img)
{
	//HImage      GrabImage;
	try {
		*img = this->m_pFG->GrabImageAsync(-1);

		// Image handoff
		EnterCriticalSection(&this->newImageMutex);
		if (this->imgList.size() < MAX_BUFFERS) {
			this->imgList.push_back(*img);
		}
		LeaveCriticalSection(&this->newImageMutex);

		SetEvent(this->newImageEven);
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"GrabOneImageAsync()：");

	}

	return;
}

void CCameraCPP::ResetIPNewImageEvent()
{
	ResetEvent(this->newImageEven);
}


HANDLE CCameraCPP::GetNewImageEvent()
{
	return this->newImageEven;
}
