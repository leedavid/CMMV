#include "stdafx.h"
#include "MainFrm.h"
#include "ChildView.h"
#include "ProjectView.h"

#include <algorithm>
#include <sstream>

#include "MyData.h"
#include "MyDispLay.h"


using namespace HalconCpp;

using namespace std;
using std::string;

namespace UCI {





	// 从上到下查，如果发现有元素引用了不存在的元素，9将这个元素删除，返回FALSE;
	BOOL stElement::isELeListOKandDelBad(OptionsMap *o)
	{
		POSITION pos = ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			POSITION oldPos = pos;
			OptionsMap *o = &ELE.EleList.GetNext(pos);   // 这儿的POS取值后会自动指向下一个！！
		int etype = (*o)[L"元素类型"];
			switch (etype) {
			case ELEMENT_ANGLE:     // 由二条线组成，
			{
				CString name;
				name = (*o)[L"USE_LINE1_Parament_Name"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
				name = (*o)[L"USE_LINE2_Parament_Name"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
				UCI::ELE.EleList.RemoveAt(oldPos);
				return FALSE;
				}
			}
			break;
			case ELEMENT_DISTANCE:
			{
				CString name;
				name = (*o)[L"引用元素名称1"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
				name = (*o)[L"引用元素名称2"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
				UCI::ELE.EleList.RemoveAt(oldPos);
				return FALSE;
				}
			}
			break;
			case ELEMENT_RESULT:               // 引用了其它元素的结果
			{
				CString name;
				name = (*o)[L"输出元素名称"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
			}
			break;
			case ELEMENT_BALLOONLENGTH:           // 引用了其它元素的结果
			{
				CString name;
				name = (*o)[L"引用顶点1"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
				name = (*o)[L"引用顶点2"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
				name = (*o)[L"引用顶点3"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
				name = (*o)[L"引用顶点4"];
				if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
					UCI::ELE.EleList.RemoveAt(oldPos);
					return FALSE;
				}
			}
			break;
			case ELEMENT_MM:                    // 引用了其它元素的结果
			{
				//CString name;
				//name = (*o)[L"校正元素的参数 X"];
				//if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
				//	UCI::ELE.EleList.RemoveAt(oldPos);
				//	return FALSE;
				//}
				//name = (*o)[L"校正元素的参数 Y"];
				//if (name != L"" && GetOptionsMapByEleName(name) == nullptr) {
				//	UCI::ELE.EleList.RemoveAt(oldPos);
				//	return FALSE;
				//}
				//return FALSE;
			}
			break;
			default:
				break;
			}		
		}
		return TRUE;
	}

	// 删除一个元素

	BOOL stElement::RemoveOneElement(CString stEleName)
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			POSITION prepos = pos;
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString otName = (*ot)[L"元素名称"];
			if (otName == stEleName) {
				UCI::ELE.EleList.RemoveAt(prepos);
				break;
			}
		}

		// 还要删除相关的引用这个元素的其它元素
		pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			OptionsMap *o = &ELE.EleList.GetNext(pos);
			if (isELeListOKandDelBad(o) == FALSE) {    
				pos = ELE.EleList.GetHeadPosition();   // 已经删除了，要重新更新一下
			}
		}		

		return TRUE;
	}

	OptionsMap * stElement::GetOptionsMapByEletype(eELEMENT_TYPE e)
	{

		POSITION pos = EleList.GetHeadPosition();
		while (pos != NULL) {
			OptionsMap *o = &EleList.GetNext(pos);
			//////////////////////////////////////////////////
			// 对每一个元素进行测量	
			int etype = (*o)[L"元素类型"];

			if (etype == e) return o;
		}
		return nullptr;
	}

	OptionsMap * stElement::GetOptionsMapByEleName(CString fname)
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString name = (*ot)[L"元素名称"];
			if (fname == name)
			{
				return ot;
			}
		}		
		return nullptr;
	}

	POSITION stElement::GetOptionsPosByEleName(CString fname)
	{
		POSITION pos = UCI::ELE.EleList.GetHeadPosition();
		while (pos != NULL) {
			POSITION oldpos = pos;
			UCI::OptionsMap *ot = &UCI::ELE.EleList.GetNext(pos);
			CString name = (*ot)[L"元素名称"];
			if (fname == name)
			{
				return oldpos;
			}
		}
		return NULL;
	}

}