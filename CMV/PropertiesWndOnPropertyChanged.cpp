
// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// (“Fluent UI”)。该示例仅供参考，
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。  
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。  
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问 
// http://go.microsoft.com/fwlink/?LinkId=238214。
//
// 版权所有(C) Microsoft Corporation
// 保留所有权利。

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "CMV.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace std;
using namespace HalconCpp;

// 全局变量设置改变
LRESULT CPropertiesWnd::OnPropertyChangedGlobal(WPARAM wParam, LPARAM lParam)
{
	CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;

	CString name = pProp->GetName();            // 初改变的参数名称

	int id = int(pProp->GetData());             // 属性的索引值
	COleVariant t1 = pProp->GetValue();         // 改变之后的值 
	t1.ChangeType(t1.vt);

	//COleVariant t2 = pProp->GetOriginalValue();   // 改变之前的值 

	UCI::OptionsMap* om = &UCI::OPTIONS;
	
	if (om->count(name)) {
		switch (t1.vt) {
		case VT_I4:       // 整数
			(*om)[name] = t1.intVal;			
			break;
		case VT_R8:       // 浮点数
			(*om)[name] = t1.dblVal;
			if (name == L"映射宽度"
				|| name == L"标定 Z 轴 角度"
				|| name == L"标定 Y 轴 角度"
				|| name == L"标定 X 轴 角度"
				|| name == L"标定原点 X 坐标"
				|| name == L"标定原点 Y 坐标"
				|| name == L"标定原点 Z 坐标"
				) {
				UCI::OPTIONS[L"标定信息 OK"] = 0;
			}
			break;
		case VT_BSTR:     // 字符串
			(*om)[name] = t1.bstrVal;
			if (name == L"相机名称") 
			{
				// 更新全局变量
				CString CameraName = (*om)[name];
				UCI::sDB.MyReadSettingFromSqlite(CameraName);

				// 更新属性
				this->m_wndPropList.RemoveAll();     // 移除原来的所有数据
				InitPropList_Common();
				this->m_wndPropList.Invalidate();    // 重新刷新一下

				// 重新调用相应的相机参数
				//string lastCammer = UCI::OPTIONS[L"相机名称"];
				//UCI::sDB.MyReadCAMparaFromSqlite(lastCammer);        // 相机参数	
				
				// 重新建立目录
				int curProject = UCI::OPTIONS[L"项目编号"];
				UCI::sDB.PrepareCamDeviceCatalog(curProject);
				
			}			

			break;
		default:
			break;
		}
	}
	return 0;
}



// 其它属性改变
LRESULT CPropertiesWnd::OnPropertyChangedProperty(WPARAM wParam, LPARAM lParam)
{
	try {

		CMFCPropertyGridProperty* pProp = (CMFCPropertyGridProperty*)lParam;

		CString name = pProp->GetName();            // 初改变的参数名称

		int id = int(pProp->GetData());                              // 属性的索引值
		COleVariant t1 = pProp->GetValue();                          // 改变之后的值 
		t1.ChangeType(t1.vt);

		COleVariant t2 = pProp->GetOriginalValue();                  // 改变之前的值 	

		// 得到当前的属性
		UCI::OptionsMap* om = CurOption;       // 

		// UCI::CammerParaChanged = TRUE;
		int etype = (*om)[L"元素类型"];


		if (om->count(name)) {
			switch (t1.vt) {
			case VT_I4:       // 整数
				(*om)[name] = t1.intVal;
				break;
			case VT_R8:       // 浮点数
				(*om)[name] = t1.dblVal;
				break;
			case VT_BSTR:     // 字符串
				(*om)[name] = (t1.bstrVal);

				if (name == L"引用顶点1") {

				}
				else if (name == L"引用顶点2") {

				}
				else if (name == L"引用顶点3") {

				}
				else if (name == L"引用顶点4") {


				}

				else if (name == L"引用元素名称1") {

					CString elename = t1.bstrVal;
					UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(elename);
					CMFCPropertyGridProperty* NpProp;


					// 填充这个选择元素的属性
					NpProp = m_wndPropList.FindItemByData(USE_Distance1_Parament_Name_ID);
					NpProp->RemoveAllOptions();
					NpProp->SetValue((_variant_t)L"");

					// 查找当中可输出的元素变量
					for (auto& it : *oc)
					{
						UCI::Option& o = it.second;
						CString sname = it.first;

						if (sname.Left(4) == L"RES_") {
							NpProp->AddOption(sname);
						}
					}
				}
				else if (name == L"引用元素名称2") {

					CString elename = t1.bstrVal;
					UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(elename);

					CMFCPropertyGridProperty* NpProp;


					// 填充这个选择元素的属性
					NpProp = m_wndPropList.FindItemByData(USE_Distance2_Parament_Name_ID);
					NpProp->RemoveAllOptions();
					NpProp->SetValue((_variant_t)L"");

					// 查找当中可输出的元素变量
					for (auto& it : *oc)
					{
						UCI::Option& o = it.second;
						CString sname = it.first;

						if (sname.Left(4) == L"RES_") {
							NpProp->AddOption(sname);
						}
					}
				}
				else if (name == "输出元素名称") {

					CString elename = t1.bstrVal;
					UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(elename);

					CMFCPropertyGridProperty* NpProp;


					// 填充这个选择元素的属性
					NpProp = m_wndPropList.FindItemByData(OUT_Element_Name_ID);
					NpProp->RemoveAllOptions();
					NpProp->SetValue((_variant_t)L"");

					// 查找当中可输出的元素变量
					for (auto& it : *oc)
					{
						UCI::Option& o = it.second;
						CString sname = it.first;

						if (sname.Left(4) == L"RES_") {
							NpProp->AddOption(sname);
						}
					}
				}
				else if (name == "校正元素 X") {

					//CString elename = (t1.bstrVal);
					//UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(elename);

					//CMFCPropertyGridProperty* NpProp;


					// 填充这个选择元素的属性
					//NpProp = m_wndPropList.FindItemByData(OUT_Element_Name_IDX);
					//NpProp->RemoveAllOptions();
					//NpProp->SetValue((_variant_t)L"");

					//// 查找当中可输出的元素变量
					//for (auto& it : *oc)
					//{
					//	UCI::Option& o = it.second;
					//	CString sname = it.first;

					//	if (sname.Left(4) == L"RES_") {
					//		NpProp->AddOption(sname);
					//	}
					//}
				}
				else if (name == "校正元素 Y") {

					//CString elename = (t1.bstrVal);
					//UCI::OptionsMap* oc = UCI::ELE.GetOptionsMapByEleName(elename);

					//CMFCPropertyGridProperty* NpProp;

					//// 填充这个选择元素的属性
					//NpProp = m_wndPropList.FindItemByData(OUT_Element_Name_IDY);
					//NpProp->RemoveAllOptions();
					//NpProp->SetValue((_variant_t)L"");

					//// 查找当中可输出的元素变量
					//for (auto& it : *oc)
					//{
					//	UCI::Option& o = it.second;
					//	CString sname = it.first;

					//	if (sname.Left(4) == L"RES_") {
					//		NpProp->AddOption(sname);
					//	}
					//}
				}

				break;
			default:
				break;
			}
		}

		if (om->count(name)) {
			// 下面要设置一下相机参数
			if (etype == ELEMENT_CAMERA) {
				if (UCI::pDis->pCAM->GetIsOpen() == FALSE) { // 当前相机是关的，则要打开一下，然后再更改要设置的
					UCI::pDis->DisOpenCamera();
				}
				if (UCI::pDis->pCAM->GetIsOpen()) {
					UCI::sDB.SetOneCamePara(name, (*om)[name]);
				}
			}
		}
	}
	catch (HException& except) {
		UCI::sDB.Log(except, L"OnPropertyChangedProperty(：");
		return 0;
	}


	return 0;
}


LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wParam, LPARAM lParam)
{
	
	// 先得到当前的属性窗口的ID
	int num = this->m_wndObjectCombo.GetCurSel();
	switch (num) {

	case PROPLIST_OTHER:
		return OnPropertyChangedProperty(wParam, lParam);
		break;
	case PROPLIST_GLOBAL:
		return OnPropertyChangedGlobal(wParam, lParam);
		break;
	default:
		break;
	}
	
	return 0;
}