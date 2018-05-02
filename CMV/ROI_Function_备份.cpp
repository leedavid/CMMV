#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;
using namespace std;
using namespace Gdiplus;


// 算子
BOOL ST_MYDISP::MeasureOneFUNCTION2(UCI::OptionsMap * o, HImage & hImg, HXLDCont &Cont)
{
	try {

		HImage ImageTmp = hImg;
		HRegion RegionTmp;

		// 先校正图像
		BOOL bIsMap = (*o)[L"USE_map_image"];
		if (bIsMap) {
			BOOL bLoadCalibrateOK = UCI::OPTIONS[L"标定信息 OK"];
			if (bLoadCalibrateOK == FALSE) {

				this->LoadCaliPoseAndParamter();
				bLoadCalibrateOK = UCI::OPTIONS[L"标定信息 OK"];
				if (bLoadCalibrateOK == FALSE)
				{
					write_message(200, 20, "加载标定数据没有成功，请先标定相机");
					UCI::MyLog(L"MeasureOneFUNCTION()： 加载映射图像失败！");
					return FALSE;
				}
			}

			// 校正图像
			ImageTmp = ImageTmp.MapImage(this->ho_RectMap);

			EnterCriticalSection(&m_csIPDisp);
			this->m_sIPResult.result_img = ImageTmp;
			LeaveCriticalSection(&m_csIPDisp);
		}

		//ImageTmp = ImageTmp.InvertImage();
		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(ImageTmp);

		BOOL bIsUSE_emphasize = (*o)[L"USE_emphasize"];
		if (bIsUSE_emphasize) {

			int emphasize_MaskWidth = (*o)[L"emphasize_MaskWidth"];
			int emphasize_MaskHeight = (*o)[L"emphasize_MaskHeight"];
			double emphasize_Factor = (*o)[L"emphasize_Factor"];

			ImageTmp = ImageTmp.Emphasize(emphasize_MaskWidth, emphasize_MaskHeight, emphasize_Factor);
		}

		//	this->m_pMHWinMain->ClearWindow();
		//	this->m_pMHWinMain->DispObj(ImageTmp);

		BOOL bIsUSE_threshold = (*o)[L"USE_threshold"];
		if (bIsUSE_threshold) {

			double threshold_MinGray = (*o)[L"threshold_MinGray"];
			double threshold_MaxGray = (*o)[L"threshold_MaxGray"];

			RegionTmp = ImageTmp.Threshold(threshold_MinGray, threshold_MaxGray);
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// 膨胀
		BOOL isUSE_dilation_circle = (*o)[L"USE_dilation_circle"];
		if (isUSE_dilation_circle) {
			double dilation_circle_radius = (*o)[L"dilation_circle_radius"];

			RegionTmp = RegionTmp.DilationCircle(dilation_circle_radius);
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// fill_up
		BOOL USE_fill_up = (*o)[L"USE_fill_up"];
		if (USE_fill_up) {
			RegionTmp = RegionTmp.FillUp();
		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// 收缩
		BOOL USE_erosion_circle = (*o)[L"USE_erosion_circle"];
		if (USE_erosion_circle) {
			double erosion_circle_radius = (*o)[L"erosion_circle_radius"];

			RegionTmp = RegionTmp.ErosionCircle(erosion_circle_radius);


		}

		//this->m_pMHWinMain->ClearWindow();
		//this->m_pMHWinMain->DispObj(RegionTmp);

		// 把不是想要的部分部分搞成白色的。



		//= hImg.Difference(hImg);

		//	this->m_pMHWinMain->ClearWindow();
		//	this->m_pMHWinMain->DispObj(RegionBack);
		//	HImage reTmp;

		BOOL bIsUSE_reduce_domain = (*o)[L"USE_reduce_domain"];
		if (bIsUSE_reduce_domain) {

			HRegion RegionBack;
			Difference(hImg, RegionTmp, &RegionBack);
			RegionBack.OverpaintRegion(ImageTmp, 0, "fill");

			BOOL USE_reduce_domain_Gray = (*o)[L"USE_reduce_domain_Gray"];
			if (USE_reduce_domain_Gray) {
				RegionTmp.OverpaintRegion(ImageTmp, 255, "fill");
			}
		}

		hImg = ImageTmp;
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"MeasureOneFUNCTION()：");
		return FALSE;
	}
	return TRUE;
}