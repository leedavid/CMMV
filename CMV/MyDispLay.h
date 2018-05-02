#pragma once

#include"MyData.h"
#include <list>
#include "CExcelOperation.h"
#include "CCameraCPP.h"

class struct_CROSS_LINE
{
public:
	int x;
	int y;
	COLORREF color;
	bool isUse;
};


class ST_MYDISP
{
public:
	ST_MYDISP(CWnd* cwnd);  // CChildView
	~ST_MYDISP();


	/////////////////////////////////////////////////////////////////////////////
	// Declaration of the thread functions
	void static IPRun(void*);
	void static FGRun(void*);

	void static IPRunCamCaliInner(void*);      // 标定内参线程
	void static IPRunCamCaliPose(void*);       // 标定外参线程

	void static IPContinous(void*);            // 连续采集图像时的处理

	void static IPAutoMeasure(void*);          // 自动测量处理线程

	void AutoMeasureOnePicture(HalconCpp::HImage& img, HalconCpp::HXLDCont& con);

	void DisplayOtherInfo();

	void static FGCapContinues(void*);

	void OnDestroy();

	void InitThreadCommunication();

	double testTime;                                 // 一次检测用时


	HalconCpp::HWindow   *m_pHWinFG;                  /* HALCON frame grabber window    */
	HalconCpp::HWindow   *m_pMHWinMain;               // HALCON 显示主窗口

	CRITICAL_SECTION       m_csFGDisp;               /* Local mutex to protect the access on the fg result     */
	CRITICAL_SECTION       m_csIPDisp;               /* Local mutex to protect the access on the ip result     */

	HANDLE                 m_hFGStopEvent;           /* Event handle to signal the end of grabbing             */

	HalconCpp::HTuple hv_DrawID;                     // 附加对象全局变量
													 //HalconCpp::HTuple hv_DrawID2;                    // 附加对象全局变量2

	CCameraCPP* pCAM;

	void ResetAllCStaticPart();                              // 相机连接成功后, 设置一下所有的显示控件的显示区域
	void ResetAllCStaticWinExtends();                        // 设置halcon显示的图像的大小


	void AppStateChangeTo(eAppState e);                      // 改变程序状态

	HalconCpp::HImage  GrabImageDisp;                        // 来用显示的直接采集到的图像
															 //HalconCpp::HImage  GrabImageERROK;                       // 标定图像误差合格的图像
	struct ResultContainer m_sIPResult;
	// ======================================================================== 	
	// OnFGDisp() - The OnFGDisp method also allows derived classes to handle the 
	//             event without attaching a delegate. This is the preferred 
	//			   technique for handling the event in a derived class.
	//

	LRESULT OnFGDisp(WPARAM wParam, LPARAM lParam);

	// ======================================================================== 	
	// OnMainDisp() - The OnFGDisp method also allows derived classes to handle the 
	//             event without attaching a delegate. This is the preferred 
	//			   technique for handling the event in a derived class.
	//
	LRESULT OnMainDisp(WPARAM wParam, LPARAM lParam);


	void set_display_font(HalconCpp::HTuple hv_WindowHandle, HalconCpp::HTuple hv_Size,
		HalconCpp::HTuple hv_Font, HalconCpp::HTuple hv_Bold,
		HalconCpp::HTuple hv_Slant);

	// Start continuous grabbing
	void OnButtonFGStart();
	void OnButtonFGStop();

	BOOL LoadCaliPoseAndParamter();                         // 加载姿态及相机内参
	BOOL CopyCaliPoseAndParamterFile(int oldPr, int newPr);

	BOOL OnDisMeasureOnePic(UCI::OptionsMap * o, HalconCpp::HImage* Img);      // 用o参数采集一个图，并校正

	
	void OnDisButMatchLoadPic();
	void OnDisButLoadPic();

	void DisOnButCaliCorrect();                            // 根据标定板校正图像


	void DisOnMatchTakeOne();

	void DisDrawAllRoi(HalconCpp::HXLDCont& hv_Cont);       // 画出所有ROI HXLD
	void DisDrawOneRoi(UCI::OptionsMap *o, HalconCpp::HXLDCont& hv_Cont);                // 画出一个元素的ROI

	void DisOnEleOutReport();                     // 报表元素输出
	void DisOnMeaEleMillimeter();                 // 元素转换mm
	void DisOnEleCameraAdd();                     // 添加相机元素

	void EditOneElement(UCI::OptionsMap *o);      // 编辑一个元素
	void EditOneElementLine(UCI::OptionsMap *o);
	void EditOneElementCIRCLE(UCI::OptionsMap *o);
	void EditOneElementPEAK(UCI::OptionsMap *o);
	void EditOneMEASURE_RECTANGLE2(UCI::OptionsMap *o);
	void EditOneBalloonFlaw(UCI::OptionsMap *o);
	void EditOneROI(UCI::OptionsMap *o);

	void DisOnMatchDelAllRoi();

	CString GetMatchPathFile(int pr);               // 得到当前项目的匹配图片文件名
	CString GetShapeModelPathFile(int pr);          // 得到 shm 的文件名
	CString GetShapeModelRegionPathFile(int pr);    // 得到 建立 ShapeModel的Region文件名

	int GetHeight() {
		int h = UCI::OPTIONS[L"图像高度"]; return h;
	};

	int GetWidth() {
		int w = UCI::OPTIONS[L"图像宽度"]; return w;
	};

	eAppState GetAppState() { return this->mAppState; };

private:
	HANDLE             m_hFGThrHandle;              /* Win32 thread handle        */
	unsigned int       m_uiFGThreadId;              /* Win 32 thread identifier   */

	HANDLE m_hIPThrHandle;
	unsigned int m_uiIPThreadId;

	BOOL bManualMeasureReady;
	BOOL bManualMeasureResutl;

private:

	BOOL MeasureOneElement(UCI::OptionsMap *o, HalconCpp::HImage &hImg, HalconCpp::HXLDCont&);     // 测量一个元素　　　

	BOOL MeasureOneLine(UCI::OptionsMap *o, HalconCpp::HImage &hImg, HalconCpp::HXLDCont&);        // 测量一个直线　　
	BOOL MeasureOneCircle(UCI::OptionsMap *o, HalconCpp::HImage &hImg, HalconCpp::HXLDCont&);      // 测量一个圆　
	BOOL MeasureOneAngle(UCI::OptionsMap *o, HalconCpp::HImage &hImg, HalconCpp::HXLDCont&);
	BOOL MeasureOneDistance(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);

	BOOL MeasureOneCamera(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);       // 测量一个相机元素 
	BOOL MeasureOneROI(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);          // 测量一个ROI元素 

	BOOL MeasureOneFUNCTION(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);       // 
	BOOL MeasureOneFUNCTION2(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);       // 
	BOOL MeasureOneMATCH(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);       // 
	BOOL MeasureOneELEMENT_MEASURE_RECTANGLE2(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);
	BOOL MeasureOneELEMENT_MEASURE_BalloonFlaw(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);
	//BOOL MeasureOneELEMENT_MEASURE_ROI(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);

	BOOL MeasureOneResult(UCI::OptionsMap *o, HalconCpp::HXLDCont&);
	BOOL MeasureOneMM(UCI::OptionsMap *o, HalconCpp::HXLDCont&);
	BOOL MeasureOnePeak(UCI::OptionsMap *o, HalconCpp::HImage&hImg, HalconCpp::HXLDCont&);
	BOOL MeasureOneBalloonLenght(UCI::OptionsMap *o, HalconCpp::HXLDCont&);

	// 显示尺寸用
	void p_disp_dimensions(HalconCpp::HTuple hv_RowEdgeFirst,
		HalconCpp::HTuple hv_ColumnEdgeFirst,
		HalconCpp::HTuple hv_RowEdgeSecond,
		HalconCpp::HTuple hv_ColumnEdgeSecond,
		HalconCpp::HTuple hv_IntraDistance,
		HalconCpp::HTuple hv_InterDistance,
		HalconCpp::HTuple hv_Phi,
		HalconCpp::HTuple hv_Length, double scalemm,
		HalconCpp::HXLDCont&);

	// Displays a marker, which indicates the position of an edge 
	void p_disp_edge_marker(HalconCpp::HTuple hv_Rows,
		HalconCpp::HTuple hv_Cols,
		HalconCpp::HTuple hv_Phi,
		HalconCpp::HTuple hv_Length,
		HalconCpp::HTuple hv_Color,
		HalconCpp::HTuple hv_LineWidth,
		HalconCpp::HXLDCont&);

	void p_disp_text_right_of_center(HalconCpp::HTuple hv_Text,
		HalconCpp::HTuple hv_RowFirst,
		HalconCpp::HTuple hv_ColFirst,
		HalconCpp::HTuple hv_RowSecond,
		HalconCpp::HTuple hv_ColSecond,
		HalconCpp::HTuple hv_Phi,
		HalconCpp::HTuple hv_Distance, double scalemm,
		HalconCpp::HXLDCont&);

	void p_disp_text_left_of_center(HalconCpp::HTuple hv_Text,
		HalconCpp::HTuple hv_RowFirst,
		HalconCpp::HTuple hv_ColFirst,
		HalconCpp::HTuple hv_RowSecond,
		HalconCpp::HTuple hv_ColSecond,
		HalconCpp::HTuple hv_Phi,
		HalconCpp::HTuple hv_Distance, double scalemm,
		HalconCpp::HXLDCont&);


	CStatic*	pFGWinStatic;
	CStatic*    PMainStatic;

	//////////////////////////////////////////////////////////////////////////////
	/// 
	HalconCpp::HImage ho_RectMap;	         //== 畸变映射图像
	HalconCpp::HShapeModel hv_Model;         //==  定位模板
	HalconCpp::HRegion ho_ModelRegion;       //==  模块区域
	HalconCpp::HHomMat2D HomMat2D;           //==  ROI 变换矩阵
	double HomMatAngle;                      // ROI 的变换角度
	HalconCpp::HTuple hv_RectifiPose;        //== 映射姿态
	HalconCpp::HPose hv_CamPose;             //== 相机姿态 
	HalconCpp::HTuple hv_CamParameters;      //== 相机内部参数	
											 //HalconCpp::HXLDCont hv_Cont;             //== 当前测试用的显示轮廓

	bool m_bRectMapOK;                       //== 畸变映射图像及其它参数OK了


	CWnd* pCView;                             // 窗口变量
	HWND hWnd;								  // 当前的窗口句柄
	CRect rcImage;							  // 图像的显示区域

	struct_CROSS_LINE CrossLine[3];           // 十字参考线

	eAppState mAppState;                      // 当前视图所处的状态	
											  //int productNumber;                        // 当前产品编号 
											  //int elementResultIndex;                   // 当前元素测量结果编号

	int  PushCaptureDown;                      // 标定内参或外参时按下的了一次	
	BOOL IsSpaceKeyDown = 0;                       // 键盘按下了一次
												   //=====================================================================
												   // 以下为 测量用变量


public:

	CStatic* GetpFGWinStatic() { return this->pFGWinStatic; };
	CStatic* GetpMainStatic() { return this->PMainStatic; };

	void SetpFGWinStatic(CStatic* pcs) { this->pFGWinStatic = pcs; };
	void SetPMainStatic(CStatic* pcs) { this->PMainStatic = pcs; };

	void DisCamliInnerOnePicture(HalconCpp::HImage *img, HalconCpp::HXLDCont*  Contours);
	void DisCamliPoseOnePicture(HalconCpp::HImage *img, HalconCpp::HXLDCont*  Contours);

	void StartContiouscap();
	void StopContiouscap();

	void StartAutoMeasureCap();    // 开始连续采集测量


	void MyLog(CString str) { return UCI::MyLog(str); };
	void MyLog(std::string str) { return MyLog(CA2T(str.c_str()).m_psz); };

	bool SaveImgToFile(CString stSavePath, CString stFormat, CString& st);

	bool SaveImgToROI();

	bool ReadImageFromFile(CString stFileName);
	bool LoadDefaultPicture();
	void ChangeImageCRect(CRect cr);

	bool SaveOneCaliInnerPicture(int pr, HalconCpp::HImage *img);             // 保存一副
	bool SaveOneCaliPosePicture(int pr, HalconCpp::HImage *img);
	CString  GetTimeMsToString();                                             // 得到当前时间的ms数的字符串


	int GetTotalCariInnerPicNum(int pr);                                      // 返加当前采集图像是的数量
	int GetTotalCariPosePicNum(int pr);                                        // 

	void DelTotalCariInnerPic(int pr);                // 删除标定图像文件	
	void DelTotalCariPosePic(int pr);                 // 删除标定图像文件	


													  ///--------------------------------------------------------
	bool Write_Image(HalconCpp::HImage Image, CString Path);

	bool GetCameraPoseFromPictre();

	void OnDisKeyDown(MSG* pMsg);                      // 按键处理程序
	void OnRButtonDown(MSG* pMsg);                     // 鼠标右键处理
	void OnRButtonDownAPP_ATTACH_LINE();
	void OnRButtonDownAPP_ATTACH_CIRCLE();
	void OnRButtonDownAPP_ATTACH_PEAK();
	void OnRButtonDownAPP_ATTACH_MEASURE_RECTANGLE2();
	void OnRButtonDownAPP_ATTACH_BALLOON_FLAW();
	void OnRButtonDownAPP_ATTACH_ROI();



	//---------------------------------------------------------------------------
	// 以下为函数

	void DisDisplay(CRect rect);                               // 更改图像的显示区域，再显示一下


	void SetPartHvWnd();

	void DisSetInnerAndPosStartPara(HalconCpp::HTuple& hv_StartParameters,
		HalconCpp::HTuple& hv_TmpCtrl_FindCalObjParNames,
		HalconCpp::HTuple& hv_TmpCtrl_FindCalObjParValues);

	void DisGetCameraInnerParaFromPic();                        // 从标定的图像中取得相机的内参
	void DisGetCameraPoseParaFromPic();                         // 从标定的图像中取得相机的内参


	void write_message(HalconCpp::HTuple hv_Row,
		HalconCpp::HTuple hv_Column,
		HalconCpp::HTuple hv_String,
		bool isclear = false);

	bool IsCanLoadPicture();                                    // 当前能不能打开相机
	void DisOpenCamera();										// 打开摄像机
	void DisCloseCamera();										// 关闭摄像机

	bool GetRefMapFromSaveFile();                               // 

																//==================================================================================
																// ROI

	void draw_rake(
		HalconCpp::HXLDCont *ho_Regions,
		HalconCpp::HTuple hv_Elements,
		HalconCpp::HTuple hv_DetectHeight,
		HalconCpp::HTuple hv_DetectWidth,
		double *hv_Row1,
		double *hv_Column1,
		double *hv_Row2,
		double *hv_Column2);


	void rake(
		HalconCpp::HImage ho_Image,
		HalconCpp::HXLDCont *ho_Regions,
		HalconCpp::HTuple hv_Elements,
		HalconCpp::HTuple hv_DetectHeight,
		HalconCpp::HTuple hv_DetectWidth,
		HalconCpp::HTuple hv_Sigma,
		HalconCpp::HTuple hv_Threshold,
		HalconCpp::HTuple hv_Transition,
		HalconCpp::HTuple hv_Select,
		HalconCpp::HTuple hv_Row1,
		HalconCpp::HTuple hv_Column1,
		HalconCpp::HTuple hv_Row2,
		HalconCpp::HTuple hv_Column2,
		HalconCpp::HTuple *hv_ResultRow,
		HalconCpp::HTuple *hv_ResultColumn);

	void pts_to_best_line(
		HalconCpp::HXLDCont *ho_Line,
		HalconCpp::HTuple hv_Rows,
		HalconCpp::HTuple hv_Cols,
		HalconCpp::HTuple hv_ActiveNum,
		HalconCpp::HTuple *hv_Row1,
		HalconCpp::HTuple *hv_Column1,
		HalconCpp::HTuple *hv_Row2,
		HalconCpp::HTuple *hv_Column2);

	void draw_spoke(UCI::OptionsMap* o, HalconCpp::HXLDCont&);

	void draw_spoke(
		HalconCpp::HXLDCont *ho_Regions,
		HalconCpp::HTuple hv_Elements,
		HalconCpp::HTuple hv_DetectHeight,
		HalconCpp::HTuple hv_DetectWidth,
		HalconCpp::HTuple *hv_ROIRows,
		HalconCpp::HTuple *hv_ROICols,
		HalconCpp::HTuple *hv_Direct);



	void spoke(
		HalconCpp::HImage ho_Image,
		HalconCpp::HXLDCont *ho_Regions,
		HalconCpp::HTuple hv_Elements,
		HalconCpp::HTuple hv_DetectHeight,
		HalconCpp::HTuple hv_DetectWidth,
		HalconCpp::HTuple hv_Sigma,
		HalconCpp::HTuple hv_Threshold,
		HalconCpp::HTuple hv_Transition,
		HalconCpp::HTuple hv_Select,
		HalconCpp::HTuple hv_ROIRows,
		HalconCpp::HTuple hv_ROICols,
		HalconCpp::HTuple hv_Direct,
		HalconCpp::HTuple *hv_ResultRow,
		HalconCpp::HTuple *hv_ResultColumn,
		HalconCpp::HTuple *hv_ArcType);

	void pts_to_best_circle(
		HalconCpp::HXLDCont *ho_Circle,
		HalconCpp::HTuple hv_Rows,
		HalconCpp::HTuple hv_Cols,
		HalconCpp::HTuple hv_ActiveNum,
		HalconCpp::HTuple hv_ArcType,
		HalconCpp::HTuple *hv_RowCenter,
		HalconCpp::HTuple *hv_ColCenter,
		HalconCpp::HTuple *hv_Radius,
		HalconCpp::HTuple *hv_StartPhi,
		HalconCpp::HTuple *hv_EndPhi,
		HalconCpp::HTuple *hv_PointOrder);

	void gen_arrow_contour_xld(
		HalconCpp::HXLDCont *ho_Arrow,
		HalconCpp::HTuple hv_Row1,
		HalconCpp::HTuple hv_Column1,
		HalconCpp::HTuple hv_Row2,
		HalconCpp::HTuple hv_Column2,
		HalconCpp::HTuple hv_HeadLength,
		HalconCpp::HTuple hv_HeadWidth);

	void gen_double_arrow_contour_xld(
		HalconCpp::HXLDCont *ho_Arrow,
		HalconCpp::HTuple hv_Row1,
		HalconCpp::HTuple hv_Column1,
		HalconCpp::HTuple hv_Row2,
		HalconCpp::HTuple hv_Column2,
		HalconCpp::HTuple hv_HeadLength,
		HalconCpp::HTuple hv_HeadWidth);

	//////////////////////////////////////////////////////////////
	/// dis ROI
	void DisOnRoiDrawRectangle2();
	void DisOnRoiDrawCircle();
	void DisOnRoiDrawEllipse();
	void DisOnRoiDrawRectangle();
	void DisOnRoiDrawAnyLine();
	void DisOnRoiInfo();
	void DisOnRoiFindModel();
	void DisOnButMatchSaveModel();
	void DisOnElementMeasureManual();
	void DisOnAddFindPeekElement();             // 添加找顶点元素
	void DisOnAddMEASURE_RECTANGLE2();                 // 1维测量元素
	void DisOnMeaEleEllispe();

	void DisFontLarge(CString str, float x, float y, float size, Gdiplus::SolidBrush &br);

	void DisElementDimension(UCI::OptionsMap *o);      // 显示元素的尺寸
	void DisElemnetDimension(UCI::OptionsMap *o, double dis, double midrow, double midcol);

	void DisOnElementDeleteAll();

	void peak(
		HalconCpp::HImage ho_Image,
		HalconCpp::HTuple hv_Row,
		HalconCpp::HTuple hv_Coloumn,
		HalconCpp::HTuple hv_Phi,
		HalconCpp::HTuple hv_Length1,
		HalconCpp::HTuple hv_Length2,
		HalconCpp::HTuple hv_DetectWidth,
		HalconCpp::HTuple hv_Sigma,
		HalconCpp::HTuple hv_Threshold,
		HalconCpp::HTuple hv_Transition,
		HalconCpp::HTuple hv_Select,
		HalconCpp::HTuple *hv_EdgeRows,
		HalconCpp::HTuple *hv_EdgeColumns,
		HalconCpp::HTuple *hv_ResultRow,
		HalconCpp::HTuple *hv_ResultColumn);

	//////////////////////////////////////////////////////////////
	// DIS　ROI　ELEMENT 元素
	void DisOnEleLineAdd();
	void DisOnEleCircleAdd();
	void DisOnEleAngle();
	void DisOnEleDistance();
	void DisMeaEleFunction();
	void DisMeaEleMatching();
	void DisOnMeaEleBalloonLength();
	void DisOnMeaEleBalloonFlaw();  // 球囊表面缺陷检测
	void DisOnMeaEleAddRoi();       // 全局ROI

	void DisOnAutoStart();                     // 自动检测开始


	BOOL TestCurrentAppState(eAppState s);


	void DisProcessROIRegion(HalconCpp::HRegion&, HalconCpp::HRegion&);
	void DisProcessROImodel(HalconCpp::HImage&, HalconCpp::HRegion&);


	void DisOnButtonGenRectifyMap();

	void DisOnSaveOnCaliPicture(int pr);                        // 保存一副标定图像
																// 界面命令接口
	void DisOnOpenImage();                                      // 打开一副图像
	void DisOnSaveCurrentDISPtoFile();                          // 保存当前的图像
	void DisOnOpenCamera();									    // 打开相机命令
	void DisOnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void DisOnMouseLeftDown(UINT nFlags, CPoint point);
	void DisOnMouseMove(UINT nFlags, CPoint point);
	void DisOnRButtonDown(UINT nFlags, CPoint point);
	void DisOnRButtonUp(UINT nFlags, CPoint pointC);

	void DisOnOneShot();
	void DisOnCamCapMulti();
	void DisOnCamContinueCap();
	void DisOnAutoStop();

	void DisOnButCaliShowInfo(); // 显示当前的标定信息

	void DisOnButCamCaliInner();					            // 相机标定内参命令
	void DisOnButCamCaliPose();                                 // 相机标定pose命令     


	void dev_display_shape_matching_results(
		HalconCpp::HShapeModel hv_ModelID,
		HalconCpp::HTuple hv_Color,
		HalconCpp::HTuple hv_Row,
		HalconCpp::HTuple hv_Column,
		HalconCpp::HTuple hv_Angle,
		HalconCpp::HTuple hv_ScaleR,
		HalconCpp::HTuple hv_ScaleC,
		HalconCpp::HTuple hv_Model);

	//=============================================================================
	// HALCON 通用函数						            


};
