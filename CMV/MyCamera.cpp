#include "stdafx.h"
#include "Resource.h"
#include "MyCamera.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;

CCamera::CCamera()
{
	this->bIsOpened = false;
}

CCamera::~CCamera()
{
	// 相机析构
}


//
//UINT32 CCamera::GetParameter(UINT16 Which, UINT32 * pValue)
//{
//
//	HalconCpp::HTuple hv_Value;
//	
//	switch (Which) {
//	case FGP_XSIZE:
//		GetFramegrabberParam(hv_AcqHandle, "image_width", &hv_Value);
//		break;
//	case FGP_YSIZE:
//		GetFramegrabberParam(hv_AcqHandle, "image_height", &hv_Value);
//		break;
//	default:
//		break;
//	}
//
//
//	*pValue = hv_Value.I();
//
//	return UINT32();
//}


bool CCamera::SetParameterHTuple(CString str, HalconCpp::HTuple ht)
{
	try {
		SetFramegrabberParam(hv_AcqHandle, CT2A(str).m_psz, ht);
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"SetParameterHTuple()：");

		return false;
	}
}

int CCamera::GetParameterInt(CString  sPara)
{
	HalconCpp::HTuple hv_Value;	
	GetFramegrabberParam(hv_AcqHandle, CT2A(sPara).m_psz, &hv_Value);
	return hv_Value.I();
}
double CCamera::GetParameterDouble(CString str)
{
	HalconCpp::HTuple hv_Value;
	GetFramegrabberParam(hv_AcqHandle, CT2A(str).m_psz, &hv_Value);
	return hv_Value.D();
}
CString CCamera::GetParameterString(CString str)
{

	HalconCpp::HTuple hv_Value;
	GetFramegrabberParam(hv_AcqHandle, CT2A(str).m_psz, &hv_Value);

	CString s;
	s.Format(L"%S", hv_Value.SArr()[0]);
	return s;
}


bool CCamera::Get_bIsOpened()
{
	return this->bIsOpened;
}

void CCamera::Set_bIsOpened(bool b)
{
	this->bIsOpened = b;
}



//void CCamera::Set_interfaceName(CString str)
//{
//	this->interfaceName = str;
//}
//
//CString CCamera::Get_interfaceName()
//{
//	return this->interfaceName;
//}



void CCamera::Set_Gain(double d)
{
	SetFramegrabberParam(hv_AcqHandle, "图像增益", d);
}


//BOOL CCamera::Get_ExposureMode()
//{
//	return this->m_iExposureMode;
//}
//
//void CCamera::Set_ExposureMode(BOOL b)
//{
//	SetFramegrabberParam(hv_AcqHandle, "exposure_mode", b);
//	this->m_iExposureMode = b;
//}

void CCamera::Set_Shutter(int i)
{
	SetFramegrabberParam(hv_AcqHandle, "爆光时间", i);
}









