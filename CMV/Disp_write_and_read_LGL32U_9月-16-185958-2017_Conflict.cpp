#include "stdafx.h"
#include "MyData.h"
#include "MyDispLay.h"
#include "MainFrm.h"

using namespace HalconCpp;


bool ST_MYDISP::Write_Image(HalconCpp::HImage Image, CString PathName)
{
	
	// 保存图像
	try {
		int nPos;
		CString ImageType;
		CString stPath;


		nPos = PathName.ReverseFind('.');
		if (nPos > -1) {
			ImageType = PathName.Right(PathName.GetLength() - nPos - 1);
		}
		else {
			ImageType = "bmp";
		}

		nPos = PathName.ReverseFind('\\');
		if (nPos > -1) {
			stPath = PathName.Left(nPos);
		}
		else {
			stPath = UCI::strMoudlePath;
		}

		// 创建路径
		if (!PathIsDirectory(stPath)) {
			SHCreateDirectoryEx(NULL, stPath, NULL);
		}

		Image.WriteImage(CT2A(ImageType).m_psz, 0, CT2A(PathName).m_psz);
	
		return true;
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"ST_MYDISP::Write_Image 保存图像：");
		return false;
	}

	return true;
}








bool ST_MYDISP::GetCameraPoseFromPictre()
{
	return false;
}

void ST_MYDISP::OnDisKeyDown(MSG * pMsg) {

	if (pMsg->wParam == VK_RETURN || pMsg->wParam == VK_SPACE) {

		this->IsSpaceKeyDown = TRUE;

		if (this->bManualMeasureReady == TRUE) {
			this->bManualMeasureReady = FALSE;

			int produtno;
			UCI::sDB.RecordOneProdcutNotUpDateGUI(produtno);

			this->write_message(100, 20, "当前测量数据已保存！");
		}
	}
}




