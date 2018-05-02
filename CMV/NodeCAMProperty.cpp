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
#include "MyData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

using namespace HalconCpp;
using namespace UCI;
using namespace std;

IMPLEMENT_DYNAMIC(NodeCAMProperty, CMFCPropertyGridProperty)

BOOL NodeCAMProperty::HasButton() const
{
	return(m_dwFlags & AFX_PROP_HAS_LIST) ||
		(m_dwFlags & AFX_PROP_HAS_BUTTON);
}


//enum ePROPERY_TYPE {
//	PROPRETY_GLOBAL,             // 全局属性 
//	PROPRETY_CAMMRA,             // 相机属性
//	PROPRETY_ELEMENT             // 元素属性
//};
// 初始化属性表
NodeCAMProperty::NodeCAMProperty(UCI::OptionsMap & om, ePROPERY_TYPE pType)
	:CMFCPropertyGridProperty(_T("tmp"))
{
	
	switch (pType) {
	case PROPRETY_GLOBAL:
		this->SetName(L"全局变量");
		initGobleProperty(om);
		break;
	case PROPRETY_ELEMENT:
		this->SetName(L"元素属性");
		initProtertyProperty(om);
		break;
	default:
		break;
	}
}
void NodeCAMProperty::initGobleProperty(UCI::OptionsMap & om)
{
	try {

		CMFCPropertyGridProperty* pProp;
		for (size_t idx = 0; idx < om.size(); ++idx) {
			for (auto&it : om) {
				if (it.second.getIdx() == idx) {

					UCI::Option o = it.second;

					bool show = o.getshow();
					if (show == true) {

						CString name = it.first;
						CString type = o.getType();
						CString val = o.getcurrentValue();
						CString note = o.getNote();
						int index = (int)o.getIdx() + 100;

						if (type == L"spin") {
							pProp = new CMFCPropertyGridProperty(name,
								(_variant_t)_ttoi(val), note, index);
							pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
							this->AddSubItem(pProp);
						}
						else if (type == L"double") {
							pProp = new CMFCPropertyGridProperty(name,
								(_variant_t)_ttof(val), note, index);
							this->AddSubItem(pProp);
						}
						else if (type == L"string") {
							pProp = new CMFCPropertyGridProperty(name,
								(_variant_t)val, note, index);

							pProp->AllowEdit(TRUE);

							if (name == L"相机协议") {
								pProp->AddOption(_T("MindVision12_X64"));
								pProp->AddOption(_T("DirectShow"));
								pProp->AllowEdit(FALSE);
							}
							else if (name == L"相机名称") {
								CString sCamInterFace = UCI::OPTIONS[L"相机协议"];
								HTuple CamInfo;
								HTuple CamValues;
								InfoFramegrabber(CT2A(sCamInterFace).m_psz, "device", &CamInfo, &CamValues);
								int b = (int)CamValues.Length();   // 
								for (int i = 0; i < b; i++) {
									HTuple t = CamValues[i];
									CString CSt;
									CSt.Format(L"%S", t.SArr()[0]);
									pProp->AddOption(CSt);
								}
							}						   
							this->AddSubItem(pProp);
						}						
					}
				}
			}
		}
	}
	catch (HalconCpp::HException& except) {
		UCI::sDB.Log(except, L"nitGobleProperty：");

	}
}


// 初始化元素的属性列表
void NodeCAMProperty::initProtertyProperty(UCI::OptionsMap & om)
{
	
	// 每种元素的类型不一样，分别解析一下
	int etype = (om)[L"元素类型"];
	switch (etype) {
	case ELEMENT_LINE:
		initProtertyLine(om);
		break;
	case ELEMENT_CIRCLE:
		initProtertyCircle(om);
		break;
	case ELEMENT_ANGLE:
		initProtertyAngle(om);
		break;
	case ELEMENT_DISTANCE:           // 距离
		initProtertyDistance(om);
		break;
	case ELEMENT_CAMERA:
		initProtertyCamera(om);
		break;
	case ELEMENT_RESULT:
		initProtertyResult(om);
		break;
	case ELEMENT_MM:
		initProtertyMM(om);
		break;
	case ELEMENT_PEAK:
		initProtertyPeek(om);
		break;
	case ELEMENT_BALLOONLENGTH:
		initProtertyBalloonLength(om);
		break;
	case ELEMENT_FUNCTION:
		initProtertyFUNCTION(om);
		break;
	case ELEMENT_MATCH:
		initProtertyMATCH(om);
		break;
	case ELEMENT_MEASURE_RECTANGLE2:
		initProtertyMEASURE_RECTANGLE2(om);
		break;
	case ELEMENT_BALLOON_FLAW:
		initPropertyBalloonFlaw(om);
		break;
	case ELEMENT_ROI:
		initPropertyROI(om);
		break;
	default:
		break;
	}
	
	
}

void NodeCAMProperty::initProtertyCamera(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {


					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(true);    // 允许编辑
					    if (name == L"色彩空间") {							
							pProp->AddOption(L"Gray");
							pProp->AddOption(L"BGR24");							
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyLine(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyCircle(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {
					CString  name = it.first;
					CString  type = o.getType();
					CString  val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"Spoke_Circle_Direction") {							//pProp->AllowEdit(TRUE);
							pProp->AddOption(L"inner");
							pProp->AddOption(L"outer");
						}

						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

// 顶点
void NodeCAMProperty::initProtertyPeek(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

// 球囊长度
void NodeCAMProperty::initProtertyBalloonLength(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {
					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(false);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());	
						if (name == L"USE_Peek1_Indentify") {
							pProp->SetData(USE_Peek1_Element_Name_ID);
						}
						else if (name == L"USE_Peek2_Indentify") {
							pProp->SetData(USE_Peek2_Element_Name_ID);
						}
						else if (name == L"USE_Peek3_Indentify") {
							pProp->SetData(USE_Peek3_Element_Name_ID);
						}
						else if (name == L"USE_Peek4_Indentify") {
							pProp->SetData(USE_Peek4_Element_Name_ID);
						}

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑

						if (name == L"RES_球囊长度") {
							pProp->AllowEdit(false);
						}

						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(TRUE);    // 允许编辑						
				        if (name == L"引用顶点1" // 二直线测量
							|| name == L"引用顶点2"
							|| name == L"引用顶点3"
							|| name == L"引用顶点4")
						{							
							pProp->AllowEdit(false);
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素	
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyAngle(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {


					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(false);    // 不允许编辑
						//pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
                        if (name == L"USE_LINE1_Indentify") {
							pProp->SetData(USE_LINE1_Name_Identify);
						}
						else if (name == L"USE_LINE2_Indentify") {
							pProp->SetData(USE_LINE2_Name_Identify);
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(false);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"USE_LINE1_Parament_Name" // 二直线测量
							|| name == L"USE_LINE2_Parament_Name")
						{
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素		
						}
						this->AddSubItem(pProp);

					}

				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyDistance(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {


					CString  name = it.first;
					CString  type = o.getType();
					CString  val = o.getcurrentValue();
					CString  note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);						
						
						if (name == L"引用元素1 ID") {
							pProp->SetData(USE_Distance1_Name_Identify);
							pProp->AllowEdit(false);    // 不允许编辑
						}
						else if (name == L"引用元素2 ID") {
							pProp->SetData(USE_Distance2_Name_Identify);
							pProp->AllowEdit(false);    // 不允许编辑
						}
						else {
							pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
							pProp->AllowEdit(true);    // 允许编辑
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);

						if (name == L"RES_距离") {
							pProp->AllowEdit(false);    // 不允许编辑
						}
						else {
							pProp->AllowEdit(true);    // 允许编辑
						}						
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
                        if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"引用元素名称1" // 二直线测量
							|| name == L"引用元素名称2")
						{
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素	
						}
						else if (name == L"引用元素1参数名") {
							pProp->SetData(USE_Distance1_Parament_Name_ID);
						}
						else if(name == L"引用元素2参数名") {   // 要选择的元素输出属性
							pProp->SetData(USE_Distance2_Parament_Name_ID);
						}

						this->AddSubItem(pProp);

					}


				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyFUNCTION(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {


					CString  name = it.first;
					CString  type = o.getType();
					CString  val = o.getcurrentValue();
					CString  note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						if (name == L"引用元素1 ID") {
							pProp->SetData(USE_Distance1_Name_Identify);
						}
						else if (name == L"引用元素2 ID") {
							pProp->SetData(USE_Distance2_Name_Identify);
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);

					}


				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyMATCH(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {


					CString  name = it.first;
					CString  type = o.getType();
					CString  val = o.getcurrentValue();
					CString  note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}

						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyResult(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {
					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						if (name == L"超差报警") {
							//pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());							
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"输出的标准值"
							|| name == L"OUT_Elemnt_Upper_Limit"
							|| name == L"OUT_Elemnt_Lower_Limit") {
							//pProp->AllowEdit(TRUE);    // 允许编辑
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
							pProp->SetData(OUT_Element_Note_ID);
						}
						else if (name == L"输出元素名称")		// RESULT_ELEMENT 元素					
						{
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素
						}
						else if (name == L"输出元素的参数") {   // 要选择的元素输出属性
							pProp->SetData(OUT_Element_Name_ID);
						}

						this->AddSubItem(pProp);

					}
				}
			}
		}
	}
}

void NodeCAMProperty::initProtertyMM(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());
						if (name == L"超差报警") {
							//pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());							
						}
						else if (name == L"校正元素的ID X") {
							pProp->SetData(OUT_Element_Name_IdentifyX);
						}
						else if (name == L"校正元素的ID Y") {
							pProp->SetData(OUT_Element_Name_IdentifyY);
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"输出的标准值")
					   {
							//pProp->AllowEdit(TRUE);    // 允许编辑
						}
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
							pProp->SetData(OUT_Element_Note_ID);
						}
						else if (name == L"校正元素 X")		// RESULT_ELEMENT 元素					
						{
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素	
						}
						else if (name == L"校正元素的参数 X") {   // 要选择的元素输出属性
							pProp->SetData(OUT_Element_Name_IDX);
						}
						else if (name == L"校正元素 Y")		// RESULT_ELEMENT 元素					
						{
							insertCurrentPosCanUseElement(pProp, om);   // 查找当前的所有可输出元素	
						}
						else if (name == L"校正元素的参数 Y") {   // 要选择的元素输出属性
							pProp->SetData(OUT_Element_Name_IDY);
						}

						this->AddSubItem(pProp);

					}
				}
			}
		}
	}
}

// 
void NodeCAMProperty::initProtertyMEASURE_RECTANGLE2(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initPropertyBalloonFlaw(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(FALSE);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::initPropertyROI(UCI::OptionsMap & om)
{
	CMFCPropertyGridProperty* pProp;

	for (size_t idx = 0; idx < om.size(); ++idx) {
		for (auto&it : om) {
			if (it.second.getIdx() == idx) {

				UCI::Option o = it.second;

				bool show = o.getshow();
				if (show == true) {

					CString name = it.first;
					CString type = o.getType();
					CString val = o.getcurrentValue();
					CString note = o.getNote();
					int index = (int)o.getIdx() + 100;

					if (type == L"spin") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttoi(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						pProp->EnableSpinControl(TRUE, o.getimin(), o.getimax());

						this->AddSubItem(pProp);
					}
					else if (type == L"double") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)_ttof(val), note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						this->AddSubItem(pProp);
					}
					else if (type == L"string") {
						pProp = new CMFCPropertyGridProperty(name,
							(_variant_t)val, note, index);
						pProp->AllowEdit(true);    // 不允许编辑
						if (name == L"报表列名称")
						{
							pProp->AllowEdit(TRUE);                      // 允许编辑
						}
						else if (name == L"命令行")
						{
							pProp->AllowEdit(TRUE);
						}
						else if (name == L"元素备注")
						{
							pProp->AllowEdit(TRUE);
						}
						this->AddSubItem(pProp);
					}
				}
			}
		}
	}
}

void NodeCAMProperty::insertCurrentPosCanUseElement(CMFCPropertyGridProperty * pProp, UCI::OptionsMap & om)
{
	int emtype = om[L"元素类型"];
	CString omName = om[L"元素名称"];

	switch (emtype) {
	//case ELEMENT_BALLOON_FLAW:
	//{
	//	POSITION pos = UCI::ELE.EleList.GetHeadPosition();
	//	while (pos != NULL) {
	//		UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
	//		CString otName = (*ot)[L"元素名称"];
	//		if (omName == otName)
	//			break;
	//		int  otType = (*ot)[L"元素类型"];
	//		if (otType == ELEMENT_PEAK) {
	//			pProp->AddOption(otName);
	//		}
	//	}
	//}
		//break;
	case ELEMENT_BALLOONLENGTH:
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (omName == otName) 
				break;
			int  otType = (*ot)[L"元素类型"];
			if (otType == ELEMENT_PEAK) {
				pProp->AddOption(otName);
			}
		}
	}
		break;
	case ELEMENT_ANGLE:
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (omName == otName)
				break;
			int  otType = (*ot)[L"元素类型"];
			if (otType == ELEMENT_LINE) {
				pProp->AddOption(otName);
			}
		}
	}
		break;
	case ELEMENT_DISTANCE:
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (omName == otName)
				break;
			int  otType = (*ot)[L"元素类型"];
			if (otType == ELEMENT_LINE || otType == ELEMENT_CIRCLE || otType == ELEMENT_ANGLE) {
				pProp->AddOption(otName);
			}
		}
	}
		break;
	case  ELEMENT_RESULT:
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (omName == otName)
				break;
			int  otType = (*ot)[L"元素类型"];
			if (otType == ELEMENT_LINE
				|| otType == ELEMENT_DISTANCE
				|| otType == ELEMENT_ANGLE
				|| otType == ELEMENT_BALLOONLENGTH
				|| otType == ELEMENT_MEASURE_RECTANGLE2
				|| otType == ELEMENT_BALLOON_FLAW
				|| otType == ELEMENT_CIRCLE) {
				pProp->AddOption(otName);
			}
		}
	}
		break;
	case ELEMENT_MM:
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (omName == otName)
				break;
			int  otType = (*ot)[L"元素类型"];
			if (otType == ELEMENT_RESULT) {
				pProp->AddOption(otName);
			}
		}
	}
		break;
	case ELEMENT_MEASURE_RECTANGLE2:
	{

	}
		break;
	default:
		break;
	}
}

NodeCAMProperty::NodeCAMProperty(int ID, double x, double y, double z)
	:CMFCPropertyGridProperty(_T("tmp"))
{
	CString name(_T("节点"));
	CString tmpStr;
	tmpStr.Format(_T("%i,(%lf,%lf,%lf)"), ID, x, y, z);
	name += tmpStr;
	this->SetName(name);

	CString valueStr;
	valueStr.Format(_T("%lf,%lf,%lf"), x, y, z);
	COleVariant varCoord(valueStr);
	//this->SetValue(var);

	CString idStr;
	idStr.Format(_T("%i"), ID);
	COleVariant varID(idStr);

	this->AddSubItem(new CMFCPropertyGridProperty(_T("节点编号"), varID));
	this->AddSubItem(new CMFCPropertyGridProperty(_T("坐标"), varCoord));
};


