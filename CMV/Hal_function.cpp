#include "stdafx.h"
#include "MainFrm.h"

#include "MyData.h"
#include "MyDispLay.h"

using namespace HalconCpp;


// Procedures 
// Chapter: Matching / Shape-Based
// Short Description: Display the results of Shape-Based Matching. 
void ST_MYDISP::dev_display_shape_matching_results(HShapeModel mhv_Model, HTuple hv_Color, HTuple hv_Row,
	HTuple hv_Column, HTuple hv_Angle, HTuple hv_ScaleR, HTuple hv_ScaleC, HTuple hv_Model)
{

	// Local iconic variables
	HalconCpp::HXLDCont  ho_ModelContours, ho_ContoursAffinTrans;

	// Local control variables
	HTuple  hv_NumMatches, hv_Index, hv_Match;

	HHomMat2D  hv_HomMat2DScale, hv_HomMat2DRotate;
	HHomMat2D hv_HomMat2DIdentity;
	HHomMat2D hv_HomMat2DTranslate;

	

	//This procedure displays the results of Shape-Based Matching.
	//
	hv_NumMatches = hv_Row.TupleLength();
	if (0 != (hv_NumMatches>0))
	{
		if (0 != ((hv_ScaleR.TupleLength()) == 1))
		{
			TupleGenConst(hv_NumMatches, hv_ScaleR, &hv_ScaleR);
		}
		if (0 != ((hv_ScaleC.TupleLength()) == 1))
		{
			TupleGenConst(hv_NumMatches, hv_ScaleC, &hv_ScaleC);
		}
		if (0 != ((hv_Model.TupleLength()) == 0))
		{
			TupleGenConst(hv_NumMatches, 0, &hv_Model);
		}
		else if (0 != ((hv_Model.TupleLength()) == 1))
		{
			TupleGenConst(hv_NumMatches, hv_Model, &hv_Model);
		}
		{
			HTuple end_val15 = 1;
			//HTuple end_val15 = (hv_ModelID.TupleLength()) - 1;
			HTuple step_val15 = 1;
			for (hv_Index = 0; hv_Index.Continue(end_val15, step_val15); hv_Index += step_val15)
			{
				//mhv_Model.GetShapeModelContours()
				//GetShapeModelContours(&ho_ModelContours, HTuple(hv_ModelID[hv_Index]), 1);

				ho_ModelContours = mhv_Model.GetShapeModelContours(1);

				

				//if (HDevWindowStack::IsOpen())
				//	SetColor(HDevWindowStack::GetActive(), HTuple(hv_Color[hv_Index % (hv_Color.TupleLength())]));
				this->m_pMHWinMain->SetColor(HTuple(hv_Color[hv_Index % (hv_Color.TupleLength())]));
				{
					HTuple end_val18 = hv_NumMatches - 1;
					HTuple step_val18 = 1;
					for (hv_Match = 0; hv_Match.Continue(end_val18, step_val18); hv_Match += step_val18)
					{
						if (0 != (hv_Index == HTuple(hv_Model[hv_Match])))
						{
							hv_HomMat2DIdentity.HomMat2dIdentity();
							//HomMat2dIdentity(&hv_HomMat2DIdentity);

							hv_HomMat2DScale = hv_HomMat2DIdentity.HomMat2dScale(HTuple(hv_ScaleR[hv_Match]), HTuple(hv_ScaleC[hv_Match]),
								HTuple(0), HTuple(0));
							//HomMat2dScale(hv_HomMat2DIdentity, HTuple(hv_ScaleR[hv_Match]), HTuple(hv_ScaleC[hv_Match]),
							//	0, 0, &hv_HomMat2DScale);

							hv_HomMat2DRotate = hv_HomMat2DScale.HomMat2dRotate(HTuple(hv_Angle[hv_Match]), HTuple(0), HTuple(0));
							//HomMat2dRotate(hv_HomMat2DScale, HTuple(hv_Angle[hv_Match]), 0, 0, &hv_HomMat2DRotate);

							hv_HomMat2DTranslate = hv_HomMat2DRotate.HomMat2dTranslate(HTuple(hv_Row[hv_Match]), HTuple(hv_Column[hv_Match]));
							//HomMat2dTranslate(hv_HomMat2DRotate, HTuple(hv_Row[hv_Match]), HTuple(hv_Column[hv_Match]),
							//	&hv_HomMat2DTranslate);

							ho_ContoursAffinTrans = ho_ModelContours.AffineTransContourXld(hv_HomMat2DTranslate);
							//AffineTransContourXld(ho_ModelContours, &ho_ContoursAffinTrans, hv_HomMat2DTranslate);

							//if (HDevWindowStack::IsOpen())
							//	DispObj(ho_ContoursAffinTrans, this->hv_HWD); // HDevWindowStack::GetActive());
							this->m_pMHWinMain->DispObj(ho_ContoursAffinTrans);
						}
					}
				}
			}
		}
	}
	return;
}





