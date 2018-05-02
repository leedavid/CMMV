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

	size_t Option::insert_order = 0;
	
	////////////////////////////////////////////////////////////////////////////////////
	/// 初始化全局变量的缺省值
	void initOptionsMap(OptionsMap& o) {

		Option::insert_order = 0;       // 项目排序显示用

		o[L"项目编号"] << Option(L"项目编号", 1, 1, 200000,                             // 当前的项目编号
			L"project ID", false);	

		//o[L"当前属性ID"] << Option(L"当前属性ID", 1, 1, 200000,                         // 当前的在属性列表中显示的ID
		//	L"当前属性ID", true);

		o[L"图像宽长比"] << Option(L"dRadio", 600.0f/800.0f, 0.5f, 2.0f,                            // 图像窗口比例
			L"Picture Ration height/width [0.5, 2.0] default 0.75");													

		o[L"软触发定时"] << Option(L"iSoftTimeCamCapture", 20, 10, 2000,                         // 软触发的定时时间
			L"In soft trigger mode,Camera capture one picture time [10,2000] defautl 20ms");   

		o[L"触发方式"] << Option(L" trigger_mode", 0, 0, 2,                                      // 触发方式
			L"trigger mode 0: continues 1: soft trigger 2: hard trigger", 
			true,on_camera_para);

		o[L"相机协议"] << Option(L"CAMinterfaceName",L"MindVision12_X64",                      // 接口名称 File, DirectShow ...
			L"Camer interface: MindVision12_X64 or DirectShow");  

		o[L"相机名称"] << Option(L"CAMDeviceName", L"CM300",                                    // 当前选择的相机名称
			L"Camera DeviceName");  

		o[L"相像保存格式"] << Option(L"PictureSaveFormat", L"png",                              // 图像保存格式
			L"Picture Save Format",false);

		//o[L"CAMCurGroupID"] << Option(L"CAMCurGroupID", 1, 1, 10,                               // 当前的相机参数组号	
		//	"CAMCurGroup number", false);      

		// 测量结果
		o[L"GLO_Measure_OK"] << Option(L"GLO_Measure_OK", 0, 0, 1,                               // 最后一次测量结果
			L"Last Measure Is OK", false);

		o[L"GLO_Measure_Always_OK"] << Option(L"GLO_Measure_Always_OK", 0, 0, 1,                 // 最后一次测量结果
			L"Last Measure Always Is OK", false);

		o[L"转换矩阵成功"] << Option(L"GLO_HomMat2D_OK", 0, 0, 1,                                // 找到模型，并建立了转换矩阵了。
			L"Fine model and already creat HomMat2D Is OK", true);

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		// 内参标定全局变量
		o[L"标定信息 OK"] << Option(L"LoadCalibrateOK", 0, 0, 1,                                 // 加载标定信息OK了
			L"Load Calibrate Pose and Parameters OK", true);

		o[L"匹配模板 OK"] << Option(L"Load_ROI_OK", 0, 0, 1,                                     // 加载模型匹配信息OK了
			L"加载模型匹配信息OK了", true);
		
		o[L"相机标定定时"] << Option(L"iSoftTimeCamCalibrate", 50, 10, 20000,                    // 标定采样定时时间
			L"In soft trigger mode,Camera Calibrate timer [10,20000] defautl 50ms");

		o[L"CamCalibratePath"] << Option(L"CamCalibratePath", L"Calibration File",                // 相机标定存储路径
			L"Camera Calibrate used file store path",false);

		o[L"CamCalibrateModel"] << Option(L"CamCalibrateModel", L"area_scan_polynomial",          // 相机标定模型
			L"CamCalibrate Model area_scan_polynomial, ",false);

		o[L"远心镜头"] << Option(L"LensTelecentric", 0, 0, 1,                                   // 是否远心镜头
			L"is camera lens telecentric: 0: no, 1: yes"	);

		o[L"倾斜镜头"] << Option(L"LensSlope", 0, 0, 1,                                         // 是否倾斜镜头
			L"is camera lens slope: 0: no, 1: yes");

		o[L"像素宽度"] << Option(L"LensFSx", 2.2f, 1.00f, 200.0f,                                 // 像素宽度
			L"lens pixel x size [1.0, 200.0] um");

		o[L"像素高度"] << Option(L"LensFSy", 2.2f, 1.00f, 200.0f,                                 // 像素高度
			L"lens pixel y size [1.0, 200.0] um");

		o[L"镜头焦距"] << Option(L"LensFocus", 8.0f, 1.00f, 200.0f,                             // 镜头焦距
			L"lens Focus [1.0, 200.0] mm");
		
		o[L"标定板厚度"] << Option(L"CaliPlateThick", 0.001f, 0.000001f, 10.0f,               // 标定板厚度
			L"Calibrate Plate Thicnkness [0.000001, 10.0] m");

		o[L"映射宽度"] << Option(L"RectificationWidth", 75.0f, 0.1f, 1000.0f,                      // 映射宽度
			L"RectificationWidth [0.1, 1000.0] mm");

		o[L"标定图片数量"] << Option(L"CaliTotalPicture", 8, 5, 20,                            // 最多标定图片数量
			L"CaliTotalPicture: [5, 20], default: 8");

		o[L"标定充许误差"] << Option(L"CaliPicMaxError", 0.55f, 0.01f, 10.0f,                   // 标定充许误差
			L"CaliPicMaxError [0.01, 10.0] pix default: 0.55");

		o[L"标定当前误差"] << Option(L"CaliCurrentError", 0.0f, 0.001f, 10.0f,                  // 标定当前误差
			L"CaliPic Current Error [0.01, 10.0] pix default: 0.55");

		o[L"标定间距容许因子"] << Option(L"标定间距容许因子", 1.0f, 0.1f, 2.0f,                  
			L"标定gap_tolerance default 1.0f min 0.1 max 2.0");
		
		o[L"标定平滑alpha"] << Option(L"标定平滑alpha", 0.50f, 0.1f, 2.0f,                  
			L"标定alpha default 1.0f min 0.1 max 2.0");
		
		o[L"标定最大标志点直径"] << Option(L"标定最大标志点直径", 350.0f, 1.0f, 500.0f,                  
			L"标定最大标志点直径 default 1.0f min 1.0 max 500.0");

		///////////////////////////////////////////////////////////////////////////////////////////////////////
		/// 标定文件名设置
		o[L"图像宽度"] << Option(L"ORGwidth", 2592, 200, INT_MAX,
			L"width [100,2592] pixel");                                                           // "图像宽度"
		o[L"图像高度"] << Option(L"ORGheight", 1944, 200, INT_MAX,
			L"height [100,1944] pixel");

		o[L"尺寸用灰度"] << Option(L"尺寸用灰度", 250, 0, 255,
			L"测量尺寸时，填充图像时使用的灰度");

		o[L"ORGwhByCamera"] << Option(L"ORGwhByCamera", 0, 0, 1,
			L"ORG width and height is get from Camera ", false);

		o[L"CaliFileSaveName"] << Option(L"CaliFileSaveName", L"calitabsave",                       // 相机标定过程文件
			L"CamCalibrate save file name ",false);

		o[L"标定描述文件"] << Option(L"CaliDescripteName", L"caltab_30mm.descr",                   // 相机标定描述文件
			L"CamCalibrate describe file name ");

		//o[L"单个像素长度"] << Option(L"OnePixelDis", 0.013f, 0.00000001f, 1.0f,                     // 设置标定后的图像一个像素代表的实际距离（米）
		//	L"Cali One Pixel Distane [0.00000001, 1.0] m default: 0.00013");

		o[L"标定原点 X 坐标"] << Option(L"CaliOriginPoseX", -0.5f, -2.0f, 2.0f,                // 转换原点坐标，正常情况下，标定板中心为原点，把标定板中心向右下角移动 
			L"Cali One CaliOriginPose X to Width [FLT_MIN, FLT_MAX] m default: -0.04");

		o[L"标定原点 Y 坐标"] << Option(L"CaliOriginPoseY", -0.375f, -2.0f, 2.0f,                 // 转换原点坐标，正常情况下，标定板中心为原点，把标定板中心向右下角移动 
			L"Cali One CaliOriginPose Y of Height [FLT_MIN, FLT_MAX] m default: -0.03");

		o[L"标定原点 Z 坐标"] << Option(L"CaliOriginPoseZ",  0.00f, -100.9f, 100.1f,                  // 转换原点坐标，正常情况下，标定板中心为原点，把标定板中心向右下角移动 
			L"Cali One CaliOriginPose Z [FLT_MIN, FLT_MAX] m default: -0.03");

		
		o[L"标定 X 轴 角度"] << Option(L"CalibRotateAngle X", 0.0f, -360.0f, 360.0f,            // insert(poseCalib, poseCalib[5]-90, 5, poseCalibRot) 
			L"Calib Rotate Angle X [-180.0, 180.0] degree default: 0.0");                          // http://112.124.6.111/read-6712.html

		o[L"标定 Y 轴 角度"] << Option(L"CalibRotateAngle Y", 0.0f, -360.0f, 360.0f,           // insert(poseCalib, poseCalib[5]-90, 5, poseCalibRot) 
			L"Calib Rotate Angle Y [-180.0, 180.0] degree default: 0.0");                          // http://112.124.6.111/read-6712.html

		o[L"标定 Z 轴 角度"] << Option(L"CalibRotateAngle Z", 180.0f, -360.0f, 360.0f,          // insert(poseCalib, poseCalib[5]-90, 5, poseCalibRot) 
			L"Calib Rotate Angle Z [-180.0, 180.0] degree default: 0.0");                          // http://112.124.6.111/read-6712.html

		o[L"映射宽度"] << Option(L"RectificationWidth", 40.0f, 5.0f, FLT_MAX,                    // insert(poseCalib, poseCalib[5]-90, 5, poseCalibRot) 
			L"RectificationWidth [0.01F, FLT_MAX] degree default: 40.0");

		o[L"单个像素长度mm"] << Option(L"OnePixelScalemm", 0.030f, 0.0001f, FLT_MAX,               // 最终一个像素代表的尺寸
			L"OnePixel Scale mm [0.0001, FLT_MAX] degree default: 0.030");


	
		///////////////////////////////////////////////////////////////////////////////////////////////////////////
		/// 其它杂项
		o[L"TestUseStorePicture"] << Option(L"TestUseStorePicture", 0, 0, 1,                                                         
			L"Test Use prestored Picture", true);

		o[L"TestUseStorePicName"] << Option(L"TestUseStorePicName", L"roimatch.png",                                                 
			L"Test Use prestored Picture Name ", true);

		o[L"ElementDoubleClickColor"] << Option(L"ElementDoubleClickColor", L"magenta",                                              
			L"Suggested values: black, #ffb529 ", true);

		o[L"ElementDoubleClickLineWidth"] << Option(L"ElementDoubleClickLineWidth", 3, 1, 2000,                                      
			L"Restriction: (Width >= 1) && (Width <= 2000) ", true);

		o[L"ElementDoubleClickDistanceColor"] << Option(L"ElementDoubleClickDistanceColor", L"red",                                  
			L"Suggested values: black, #ffb529 ", true);

		o[L"ElementDoubleClickDistancWidth"] << Option(L"ElementDoubleClickDistancWidth", 3, 1, 2000,                                
			L"Restriction: (Width >= 1) && (Width <= 2000) ", true);

		o[L"ElementDoubleClickResultColor"] << Option(L"ElementDoubleClickResultColor", L"medium slate blue",                        
			L"Suggested values: black,  #ffb529 ", true);

		o[L"ElementDoubleClickResultWidth"] << Option(L"ElementDoubleClickResultWidth", 3, 1, 2000,                                  
			L"Restriction: (Width >= 1) && (Width <= 2000) ", true);

		o[L"ElementDimColor"] << Option(L"ElementDimColor", L"white",                                             
			L"Suggested values: #ffb529 ", true);

		o[L"ElementDimWidth"] << Option(L"ElementDimWidth", 1, 1, 2000,                                           
			L"Restriction: (Width >= 1) && (Width <= 2000) ", true);

		o[L"ElementHDLXColor"] << Option(L"ElementHDLXColor", L"green",                                           
			L"Suggested values:black ffb529", true);

		o[L"ElementHDLXWidth"] << Option(L"ElementHDLXWidth", 1, 1, 2000,                                         
			L"Restriction: (Width >= 1) && (Width <= 2000) ", true);

		o[L"ElementArrowSize"] << Option(L"ElementArrowSize",5.0f, 1.0f, 2000.0f,                                 
			L"Element Arrow Size display ", true);

		o[L"UserID"] << Option(L"UserID", 1, 1, INT_MAX,                                                            // 
			L"Current logged User iD ", false); 

		o[L"UserLogName"] << Option(L"UserLogName", L"NoUser",                                                      // 用户登录姓名
			L"UserLogName ", true);

		o[L"UserLogDateTime"] << Option(L"UserLogDateTime", L"20170816 08:00",                                       // 用户登录时间
			L"UserLogDateTime ", true);

		o[L"ProdutctBatchNumber"] << Option(L"ProdutctBatchNumber", L"20170816",                                     // 产品批号
			L"ProdutctBatchNumber", true);

		o[L"ProdutctExportStartNo"] << Option(L"ProdutctExportStartNo", 1, 1, INT_MAX,                             // 导出到EXCEL的产品开始编号 
			L"Produtct Export Start Number", true);

		o[L"ProdutctOKAutoSave"] << Option(L"ProdutctOKAutoSave", 1, 0, 1,                                       // 导出到EXCEL的产品开始编号 
			L"Produtct OK AutoSave", true);

		o[L"AutoMeasureDelayTime"] << Option(L"AutoMeasureDelayTime", 50, 20, 2000,                              // 
			L"Auto Measure Delay Time ms", true);

		o[L"首张延时"] << Option(L"首张延时", 600, 10, 5000,                                                      // 
			L"照相机拍第一张时的延时",
			true);

		o[L"显示原图"] << Option(L"显示原图", 0, 0, 1,                                                            // 
			L"主窗口是不是显示采集的原图",
			true);

		o[L"连续采集效果使用"] << Option(L"连续采集效果使用", 1, 0, 1,                                            // 
			L"连续采集图像, 应用相关效果算子处理后，再显示图像",
			true);

		o[L"Dummy"] << Option(L"Dummy", L"Dummy",                                                                  // 使用保存的图片的文件名
			L"Dummy why not show ", false);
		
	}	
		
	//  直线 
	void stElement::initOptionMapElemnetLine(OptionsMap& o)
	{
		
		// 输入的数据
		o[L"ROI_Row1"] << Option(L"ROI_Row1", 0.0f, -FLT_MAX, FLT_MAX,                                  // 元素的Rows数据
			L"ROI_Row1", false);
		o[L"ROI_Column1"] << Option(L"ROI_Column1", 0.0f, -FLT_MAX, FLT_MAX,                                  // 元素的Rows数据
			L"ROI_Column1", false);
		o[L"ROI_Row2"] << Option(L"ROI_Row2", 0.0f, -FLT_MAX, FLT_MAX,                                  // 元素的Rows数据
			L"ROI_Row2", false);
		o[L"ROI_Column2"] << Option(L"ROI_Column2", 0.0f, -FLT_MAX, FLT_MAX,                                  // 元素的Rows数据
			L"ROI_Column2", false);

			
		
		o[L"Rake直线检测点数"] << Option(L"Rake_Line_Elements", 10, 1, 1000,                                              // CreateShapeModel double AngleStart
		    L"Rake Line Elements number [1 20] default 10 ", true);

		o[L"Rake直线检测高度"] << Option(L"Rake_Line_Detect_Height", 30.0f, 1.0f, 500.0f,                             // CreateShapeModel double AngleStart
			L"Rake_Line_Detect_Height [1.0f, 500.0f] default 30.0 ", true);

		o[L"Rake直线检测宽度"] << Option(L"Rake_Line_Detect_Width", 15.0f, 1.0f, 500.0f,                                // CreateShapeModel double AngleStart
			L"Rake_Line_Detect_Height [1.0f, 500.0f] default 15.0 ", true);

		o[L"Rake直线检测Sigma"] << Option(L"Rake_Line_Detect_Sigma", 1.0f, 0.0f, 500.0f,                                // CreateShapeModel double AngleStart
			L"Rake_Line_Detect_Sigma [0.0f, 500.0f] default 1.0 ", true);

		o[L"Rake直线检测阈值"] << Option(L"Rake_Line_Detect_Threshold", 20.0f, 1.0f, 500.0f,                        // CreateShapeModel double AngleStart
			L"Rake_Line_Detect_Threshold [1.0f, 500.0f] default 20.0 ", true);

		o[L"Rake_Line_Detect_Transition"] << Option(L"Rake_Line_Detect_Transition", L"all",                                    // 元素的命令宏
			L"Rake_Line_Detect_Transition", true);

		o[L"Rake_Line_Detect_Select"] << Option(L"Rake_Line_Detect_Select", L"first",                                           // 元素的命令宏
			L"Rake_Line_Detect_Select", true);

		o[L"直线拟合最小点数"] << Option(L"pts_to_best_line_ActiveNum", 2, 2, 1000,                                 // CreateShapeModel double AngleStart
			L"pts_to_best_line_ActiveNum [2 1000] default 2 ", true);

		// 存储直线的测量信息
		o[L"_RES_Line_Row1"] << Option(L"RES_Line_Row1", 0.0f, -FLT_MAX, FLT_MAX,                                               // CreateShapeModel double AngleStart
			L"RES_Line_Row1 [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"_RES_Line_Row2"] << Option(L"RES_Line_Row2", 0.0f, -FLT_MAX, FLT_MAX,                                               // CreateShapeModel double AngleStart
			L"RES_Line_Row2 [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"_RES_Line_Col1"] << Option(L"RES_Line_Col1", 0.0f, -FLT_MAX, FLT_MAX,                                                // CreateShapeModel double AngleStart
			L"RES_Line_Col1 [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"_RES_Line_Col2"] << Option(L"RES_Line_Col2", 0.0f, -FLT_MAX, FLT_MAX,                                               // CreateShapeModel double AngleStart
			L"RES_Line_Col2 [FLT_MIN, FLT_MAX] default 0.0 ", true);
	}

	//  圆
	void stElement::initOptionMapElemnetCircle(OptionsMap&  o)
	{
		
		// ROI 本身存储信息

		o[L"HTupleArcOrCircle"] << Option(L"HTupleArcOrCircle", 0,0,1,                            // 是圆，还是弧 0: 
			L"HTupleArcOrCircle 是圆，还是弧 0: 圆, 1: 弧", true);

		o[L"ROI_RowC"] << Option(L"ROI_RowC", 0.0f, -FLT_MAX, FLT_MAX,                              // 圆心坐标 Y
			L"hv_RowC [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_ColumnC"] << Option(L"ROI_ColumnC", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"hv_ColumnC [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_Radius"] << Option(L"ROI_Radius", 0.0f, -FLT_MAX, FLT_MAX,                           //
			L"hv_Radius [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_StartPhi"] << Option(L"ROI_StartPhi", 0.0f, -FLT_MAX, FLT_MAX,                        //
			L"hv_StartPhi [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_EndPhi"] << Option(L"ROI_EndPhi", 0.0f, -FLT_MAX, FLT_MAX,                           //
			L"hv_EndPhi [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_PointOrder"] << Option(L"ROI_PointOrder", L"",                                       // 
			L"hv_PointOrder [FLT_MIN, FLT_MAX] default 0.0 ", true);

		
		o[L"Spoke_十字尺寸"] << Option(L"Spoke_十字尺寸", 10, 1, 1000,                                             // CreateShapeModel double AngleStart
			L"Spoke_十字尺寸 显示大小", true);

		// 检测设置信息		
		o[L"Spoke_Circle_Direction"] << Option(L"Spoke_Circle_Direction", L"inner",                                             // 元素的方向
			L"Direction of ROI default inner, inner, outer", true);

		o[L"Spoke_Circle_Elements"] << Option(L"Spoke_Circle_Elements", 30, 3, 1000,                                             // CreateShapeModel double AngleStart
			L"Spoke Circle Elements number [1 20] default 10", true);

		o[L"Spoke_Circle_Detect_Height"] << Option(L"Spoke_Circle_Detect_Height", 50.0f, 1.0f, 500.0f,                            // CreateShapeModel double AngleStart
			L"Spoke Circle Elements number [1.0f, 500.0f] default 30.0 ", true);

		o[L"Spoke_Circle_Detect_Width"] << Option(L"Spoke_Circle_Detect_Width", 35.0f, 1.0f, 500.0f,                             // CreateShapeModel double AngleStart
			L"Spoke Circle Elements number [1.0f, 500.0f] default 15.0 ", true);

		o[L"Spoke_Circle_Detect_Sigma"] << Option(L"Spoke_Circle_Detect_Sigma", 1.0f, 0.0f, 500.0f,                                // CreateShapeModel double AngleStart
			L"Spoke_Circle_Detect_Sigma [0.0f, 500.0f] default 1.0 ", true);

		o[L"Spoke_Circle_Detect_Threshold"] << Option(L"Spoke_Circle_Detect_Threshold", 20.0f, 1.0f, 500.0f,                        // CreateShapeModel double AngleStart
			L"Spoke_Circle_Detect_Threshold [1.0f, 500.0f] default 20.0 ", true);

		o[L"Spoke_Circle_Detect_Transition"] << Option(L"Spoke_Circle_Detect_Transition", L"all",                                    
			L"Spoke_Circle_Detect_Transition", true);

		o[L"Spoke_Circle_Detect_Select"] << Option(L"Spoke_Circle_Detect_Select", L"first",                                          
			L"Spoke_Circle_Detect_Select", true);

		o[L"Circle_Fit_Min_Points_Num"] << Option(L"Circle_Fit_Min_Points_Num", 10, 5, 1000,                                         // 拟合圆最少点数
			L"Circle_Fit_Min_Points_Num [2 1000] default 5 ", true);


		// 存储圆的测量信息
		o[L"RES_圆半径"] << Option(L"RES_圆半径", 0.0f, -FLT_MAX, FLT_MAX,                                                         // 圆半径
			L"RES_Circle_Radius [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_圆直径"] << Option(L"RES_圆直径", 0.0f, -FLT_MAX, FLT_MAX,                                                         // 圆半径
			L"RES_Circle_Diameter [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_Circle_Center_Row"] << Option(L"RES_Circle_Center_Row", 0.0f, -FLT_MAX, FLT_MAX,                                   // 圆心坐标 X
			L"RES_Circle_Row [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_Circle_Center_Column"] << Option(L"RES_Circle_Center_Column", 0.0f, -FLT_MAX, FLT_MAX,                             // 圆心坐标 Y
			L"RES_Circle_Column [FLT_MIN, FLT_MAX] default 0.0 ", true);
		

		o[L"RES_Circle_StartPhi"] << Option(L"RES_Circle_StartPhi", 0.0f, -FLT_MAX, FLT_MAX,                                        // 圆的起始角
			L"RES_Circle_StartPhi [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_Circle_EndPhi"] << Option(L"RES_Circle_EndPhi", 0.0f, -FLT_MAX, FLT_MAX,                                            // 圆的结束角
			L"RES_Circle_EndPhi [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_Circle_Type"] << Option(L"RES_Circle_Type", L"",                                                                    // 圆的类型，圆，弧
			L"RES_Circle_Type: Circle, Arc ", true);

		o[L"RES_Circle_PointOrders"] << Option(L"RES_Circle_PointOrders", L"",                                                      // 元素的Rows数据
			L"RES_Circle_PointOrders: negative, positive ", true);
	}
	
	// 结果
	void stElement::initOptionMapElemnetResult(OptionsMap& o)
	{
		o[L"输出元素名称"] << Option(L"OUT_Elemnt_Name", L"",                                           //  元素的名称
			L"输出元素名称", true);
		
		o[L"输出元素的参数"] << Option(L"OUT_Elemnt_Parament_Name", L"",                                 // 元素的参数名
			L"输出元素的参数", true);

		o[L"输出的测量值"] << Option(L" OUT_Elemnt_Data", 0.0f, -FLT_MAX, FLT_MAX,                      // 测量值
			L"OUT_Elemnt_Parament_Data", true);

		o[L"输出的标准值"] << Option(L" OUT_Elemnt_Standard", 0.0f, -FLT_MAX, FLT_MAX,                      // 标准值
			L"OUT_Elemnt_Parament_Standard", true);

		o[L"输出的上公差"] << Option(L"OUT_Elemnt_Upper_Tol", 0.5f, -FLT_MAX, FLT_MAX,                     // 上限
			L"OUT_Elemnt_Upper_Tolerance", true);

		o[L"输出的下公差"] << Option(L"OUT_Elemnt_Lower_Tol", -0.5f, -FLT_MAX, FLT_MAX,                     // 下限
			L"OUT_Elemnt_Lower_Tolerance", true);

		o[L"输出测量结论"] << Option(L"OUT_Elemnt_Result", L"",                                        // 结论, OK, 下超限，上超限
			L"OUT_Elemnt_Result: OK, Lower, Upper", true);

		o[L"超差报警"] << Option(L"OUT_Elemnt_Is_Alarm", 0, 0, 1,                                      // 超差是不是报警
			L"超差报警", true);
	}

	// 角度
	void stElement::initOptionMapElemnetAngle(OptionsMap & o)
	{
		
		o[L"USE_LINE1_Parament_Name"] << Option(L"USE_LINE1_Parament_Name", L"",                                 // 使用直线1名称
			L"USE_LINE1_Parament_Name", true);

		//o[L"USE_LINE1_Indentify"] << Option(L"USE_LINE1_Indentify", 0, 0, INT_MAX,                              // 使用直线1 ID
		//	L"USE_LINE1_Indentify", true);

		o[L"USE_LINE2_Parament_Name"] << Option(L"USE_LINE2_Parament_Name", L"",                                 // 使用直线2名称
			L"USE_LINE2_Parament_Name", true);

		//o[L"USE_LINE2_Indentify"] << Option(L"USE_LINE2_Indentify", 0, 0, INT_MAX,                              // 使用直线2 ID
		//	L"USE_LINE2_Indentify", true);

		o[L"RES_2LINE_Angle_Rad"] << Option(L"RES_2LINE_Angle_Rad", 0.0f, -FLT_MAX, FLT_MAX,                     // 二条直线的角度
			L"RES_2LINE_Angle_Rad [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_夹角"] << Option(L"RES_夹角", 0.0f, -FLT_MAX, FLT_MAX,                                           // 二条直线的角度
			L"RES_2LINE_Angle_Deg [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"RES_Intersection_Row"] << Option(L"RES_Intersection_Row", 0.0f, -FLT_MAX, FLT_MAX,                   // 二条直线的交点的 ROW
			L"RES_Intersection_Row ", true);

		o[L"RES_Intersection_Column"] << Option(L"RES_Intersection_Column", 0.0f, -FLT_MAX, FLT_MAX,             // 二条直线的交点的 Column
			L"RES_Intersection_Column ", true);

		o[L"RES_Intersection_Points"] << Option(L"RES_Intersection_Points", L"",                                 // 二条直线的交点的 Points
			L"RES_Intersection_Points", true);
	}

	// 相机
	void stElement::initOptionMapElemnetCamera(OptionsMap & o)
	{
		(o)[L"备注"] << Option(L"备注", L"Camera Parameter group description",     // 相机参数组说明
			L"Camera Parameter group description");

		(o)[L"图像采样超时"] << Option(L"grab_timeout", 300, 50, 100000,                    // 收集一副图像超时设置 ms
			L"grab_timeout [50, 100000] ms default 50ms",
			true);
		(o)[L"爆光时间"] << Option(L"shutter", 20000, 50, 1000000,                           // 爆光时间 us
			L"shutter time  [50, 1000000] μs default 20000 20ms",
			true );
		(o)[L"对比度"] << Option(L"contrast", 100, 1, 200,                                // 对比度
			L"LUT diagram contrast [1, 200] defalut 100",
			true );
		(o)[L"gamma"] << Option(L"gamma", 100, 1, 1000,                                     // gamma
			L"LUT diagram gamma [1,1000] default 100",
			true );
		(o)[L"爆光模式"] << Option(L"exposure_mode", 0, 0, 1,                              // 爆光方式
			L"exposure mode 0: manual, 1: automatic",
			true );
		(o)[L"帧速率"] << Option(L"frame_speed", 0, 0, 2,                                  // 帧速率
			L"frame speed 0: low speed, 1: mid speed, 2: high speed",
			true );
		(o)[L"anti_flick"] << Option(L"anti_flick", 0, 0, 2,                                // 抗屏闪
			L"anti_flick 0：not use, 1: anti 50HZ, 2: anti 60HZ",
			false );
		(o)[L"resolution"] << Option(L"resolution", 0, 0, 255,                              // 分辨率
			L"resolution [0, 255] default 0", false );
		(o)[L"resolution_mod"] << Option(L"resolution_mod", L"Normal",                       // 分辨方式 Normal BINSUM2X BINAVR2X Uknown
			L"resolution mod [Normal,BINSUM2X BINAVR2X Uknown] default Normal",
			false );
		(o)[L"offsetX"] << Option(L"offsetX", 0, 0, 2592,                                   // 
			L"offsetX [0, 2592] default 0",
			true );                             // 
		(o)[L"offsetY"] << Option(L"offsetY", 0, 0, 1944,
			L"offsetY [0, 1944] default 0",
			true );                             // 
		(o)[L"采样宽度"] << Option(L"width", 2592, 100, INT_MAX,
			L"width [100,2592] pixel",
			true );                                 // 
		(o)[L"采样高度"] << Option(L"height", 1944, 100, INT_MAX,
			L"height [100,1944] pixel",
			true );                                // 

		(o)[L"色温"] << Option(L"color_temperature", 0, 0, 3,                                 // 色温
			L"color temperature 0: auto 1: D65 2: 5500K (pure white) 3: cloudy sky in door",
			true );
		(o)[L"红色增益"] << Option(L"red_gain", 1.00f, 0.10f, 4.0f,                             // 红色增益
			L"red gain [0.0, 3.99]",
			true );
		(o)[L"绿色增益"] << Option(L"green_gain", 1.00f, 0.10f, 4.0f,                         // 绿色增益
			L"green gain [0.0, 3.99]",
			true );
		(o)[L"蓝色增益"] << Option(L"blue_gain", 1.00f, 0.10f, 4.0f,                           // 蓝色增益
			L"blue gain [0.0, 3.99]",
			true );
		(o)[L"饱和度"] << Option(L"saturation", 0, 0, 200,                                  // 饱和度
			L"saturation [0, 200]",
			true );
		(o)[L"图像增益"] << Option(L"gain", 1.25f, 1.25f, 50.0f,                                   // 增益
			L"gain [1.25, 50.0]",
			true );
		(o)[L"色彩空间"] << Option(L"color_space", L"Gray",                                  // 色彩空间 BGR24
			L"color space [Gray, BGR24]",
			true );
		(o)[L"采样延时"] << Option(L"采样延时", 0, 0, 2000,                                  
			L"采样前延时 [0, 1200] ms", true);
	}

	// 距离
	void stElement::initOptionMapElemnetDistance(OptionsMap & o)
	{
		
		//o[L"IsDisplayDimension"] << Option(L"IsDisplayDimension", 1, 0, 1,                                             // 是否显示尺寸数值
		//	L"IsDisplayDimension", false);

		
		o[L"引用元素名称1"] << Option(L"USE_Distance1_Element_Name", L"",                                     // 使用直线1名称
			L"引用元素名称1", true);

		o[L"引用元素1参数名"] << Option(L" USE_Distance1_Parament_Name", L"",                                 // 使用直线1名称
			L"引用元素1参数名", true);

		//o[L"引用元素1 ID"] << Option(L" USE_Distance1_Indentify", 0, 0, INT_MAX,                              // 使用直线1 ID
		//	L"引用元素1 ID", true);

		o[L"引用元素名称2"] << Option(L"USE_Distance2_Element_Name", L"",                                     // 使用直线2名称
			L"引用元素名称2", true);
		
		o[L"引用元素2参数名"] << Option(L"USE_Distance2_Parament_Name",L"",                                   // 使用直线2名称
			L"引用元素2参数名", true);

		//o[L"引用元素2 ID"] << Option(L"USE_Distance2_Indentify", 0, 0, INT_MAX,                               // 使用直线2 ID
		//	L"引用元素2 ID", true);

		// 距离总是二个点
		o[L"距离Row1"] << Option(L"距离Row1", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Row1", false);
		o[L"距离Col1"] << Option(L"距离Col1", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Col1", false);

		o[L"距离Row2"] << Option(L"距离Row2", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Row1", false);
		o[L"距离Col2"] << Option(L"距离Col2", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Col2", false);

		o[L"尺寸显示偏移X"] << Option(L"尺寸显示偏移X", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移X", true);
		o[L"尺寸显示偏移Y"] << Option(L"尺寸显示偏移Y", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移Y", true);


		o[L"尺寸箭头长度"] << Option(L"尺寸箭头长度", 20.0f, 2.0f, FLT_MAX,                                      // gen_arrow_contour_xld
			L"Arrow_HeadLength Default Value: 5 建议值: [2,3,5,10,20]", true);

		o[L"尺寸箭头宽度"] << Option(L"尺寸箭头宽度", 20.0f, 2.0f, FLT_MAX,                                      // gen_arrow_contour_xld
			L"The width of the arrow head in pixels Default Value: 5 建议值: [2,3,5,10,20]", true);

		o[L"RES_距离"] << Option(L"RES_Distance", 0.0f, -FLT_MAX, FLT_MAX,                                            // 二条直线的距离
			L"RES_Distance [FLT_MIN, FLT_MAX] default 0.0 ", true);

	

	}

	// 算子
	void stElement::initOptionMapFUNCTION(OptionsMap & o)
	{		

		//==========================================
		
		
		//==========================================
		o[L"USE_mean_image"] << Option(L"USE_mean_image", 1, 0, 1,                                              // FindShapeModleMinScore
			L"Smooth by averaging. ");

		(o)[L"mean_image_MaskWidth"] << Option(L"mean_image_MaskWidth", 9, 1, 501,                           // 
			L"Width of filter mask. Default value: 9 Suggested values: 3, 5, 7, 9, 11, 15, 23, 31, 43, 61, 101",
			true);

		(o)[L"mean_image_MaskHeight"] << Option(L"mean_image_MaskHeight", 9, 1, 501,                           // 
			L"Height of filter mask. Default value: 9 Suggested values: 3, 5, 7, 9, 11, 15, 23, 31, 43, 61, 101",
			true);


		//==========================================
		//o[L"USE_scale_image"] << Option(L"USE_scale_image", 0, 0, 1,                                          // FindShapeModleMinScore
		//	L"使用图像灰度拉伸功能 Scale the gray values of an image scale_image(Image : ImageScaled : Mult, Add : ) g1 := g * Mult + Add");

		//(o)[L"scale_image_Mult"] << Option(L"scale_image_Mult", 0.01f, 0.001f, 1.0f,                           // 
		//	L"Scale factor. Default value: 0.01 Suggested values: 0.001, 0.003, 0.005, 0.008, 0.01, 0.02, 0.03, 0.05, 0.08, 0.1, 0.5, 1.0",
		//	true);

		//(o)[L"scale_image_Add"] << Option(L"scale_image_Add", 50.0f, 0.01f, 500.0f,                           // 
		//	L"scale_image_Add. Default value: 0 Suggested values: 0, 10, 50, 100, 200, 500",
		//	true);
		//==========================================
		o[L"USE_equ_histo_image"] << Option(L"USE_equ_histo_image", 1, 0, 1,                                              // FindShapeModleMinScore
			L"Histogram linearisation of images The operator equ_histo_image primarily serves for optical processing of images for a human viewer. For example, the (local) contrast spreading can lead to a detection of fictitious edges.");
		

		//==========================================
		o[L"USE_emphasize"] << Option(L"USE_emphasize", 1, 0, 1,                                              // FindShapeModleMinScore
			L"使用图像增强功能 Enhance contrast of the image. emphasize(Image : ImageEmphasize : MaskWidth, MaskHeight, Factor : ) res := round((orig - mean) * Factor) + orig");

		(o)[L"emphasize_MaskWidth"] << Option(L"emphasize_MaskWidth", 7, 3, 201,                           // 
			L"Width of low pass mask.efault value: 7 Suggested values: 3, 5, 7, 9, 11, 15, 21, 25, 31, 39  Typical range of values: 3 ≤ MaskWidth ≤ 201 Minimum increment: 2",
			true);
		
		(o)[L"emphasize_MaskHeight"] << Option(L"emphasize_MaskHeight", 7, 3, 201,                           // 
			L"Width of low pass mask.efault value: 7 Suggested values: 3, 5, 7, 9, 11, 15, 21, 25, 31, 39  Typical range of values: 3 ≤ MaskHeight ≤ 201 Minimum increment: 2",
			true);

		(o)[L"emphasize_Factor"] << Option(L"emphasize_Factor", 1.0f, 0.01f, 20.0f,                           // 
			L"Intensity of contrast emphasis. Default value: 1.0 Suggested values: 0.3, 0.5, 0.7, 1.0, 1.4, 1.8, 2.0 Minimum increment: 0.01",
			true);

		


		//==========================================
		o[L"USE_threshold"] << Option(L"USE_threshold", 0, 0, 1,                                              // FindShapeModleMinScore
			L"USE_threshold Segment an image using global threshold. threshold(Image : Region : MinGray, MaxGray : ) ");

		(o)[L"threshold_MinGray"] << Option(L"threshold_MinGray", 180.0f, 0.00f, 255.0f,                           // 
			L"Lower threshold for the gray values. Default value: 128.0 Suggested values: 0.0, 10.0, 30.0, 64.0, 128.0, 200.0, 220.0, 255.0",
			true);

		(o)[L"threshold_MaxGray"] << Option(L"threshold_MaxGray", 255.0f, 0.00f, 255.0f,                           // 
			L"Lower threshold for the gray values. Default value: 128.0 Suggested values: 0.0, 10.0, 30.0, 64.0, 128.0, 200.0, 220.0, 255.0",
			true);

		
		o[L"USE_dilation_circle"] << Option(L"USE_dilation_circle", 0, 0, 1,                                              // FindShapeModleMinScore
			L"USE_dilation_circle");
		(o)[L"dilation_circle_radius"] << Option(L"dilation_circle_radius", 3.5f, 1.50f, 511.0f,                           // 
			L"Radius of the circular structuring element.. efault value: 3.5 uggested values: 1.5, 2.5, 3.5, 4.5, 5.5, 7.5, 9.5, 12.5, 15.5, 19.5, 25.5, 33.5, 45.5, 60.5, 110.5",
			true);

		o[L"USE_fill_up"] << Option(L"USE_fill_up", 0, 0, 1,                                              // FindShapeModleMinScore
			L"USE_fill_up fill_up fills up holes in regions. The number of regions remains unchanged. The neighborhood type is set via set_system(neighborhood,<4/8>) (default: 8-neighborhood).");

		o[L"USE_erosion_circle"] << Option(L"USE_erosion_circle", 0, 0, 1,                                              // FindShapeModleMinScore
			L"USE_erosion_circle");
		(o)[L"erosion_circle_radius"] << Option(L"erosion_circle_radius", 3.5f, 1.50f, 511.0f,                           // 
			L"Radius of the circular structuring element.. efault value: 3.5 uggested values: 1.5, 2.5, 3.5, 4.5, 5.5, 7.5, 9.5, 12.5, 15.5, 19.5, 25.5, 33.5, 45.5, 60.5, 110.5",
			true);


		//==========================================
		o[L"USE_reduce_domain"] << Option(L"USE_reduce_domain", 0, 0, 1,                                      // 
			L"USE_reduce_domain reduce_domain(Image, Region : ImageReduced : : )");

		o[L"USE_reduce_domain_Gray"] << Option(L"USE_reduce_domain_Gray", 0, 0, 1,                           // 
			L"USE_reduce_domain_Gray image to replace the product to measure dimension");
		
		
	}

	// 匹配元素
	void stElement::initOptionMapMATCH(OptionsMap & o)
	{
		
		o[L"CreateShapeModleNumLevels"] << Option(L"NumLevels", L"auto",                                              // FindShapeModleMinScore
			L"FindShapeModle NumLevels Default value: auto  List of values: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 auto");
		
		o[L"CreateShapeModleAngleStart"] << Option(L"AngleStart", -0.39f, -3.14f, 3.14f,                             // CreateShapeModel double AngleStart
			L"CreateShapeModle double AngleStart RAD Default value: -0.39 Suggested values: -3.14, -1.57, -0.78, -0.39, -0.20, 0.0 ");
		
		o[L"CreateShapeModleAngleExtend"] << Option(L"AngleExtend", 0.78f, 0.0f, 6.29f,                               // CreateShapeModel double AngleExtend
			L"CreateShapeModle double AngleExtend Default value: 0.78 Suggested values: 6.29, 3.14, 1.57, 0.78, 0.39, 0.0 ");

		o[L"CreateShapeModleAngleStep"] << Option(L"AngleStep", L"auto",                                               // CreateShapeModel double AngleExtend
			L"CreateShapeModle double AngleStep Default value: auto Suggested values: 0.0175, 0.0349, 0.0524, 0.0698, 0.0873 Restriction: (AngleStep >= 0) && (AngleStep <= (pi / 16))");

		o[L"CreateShapeModleOptimization"] << Option(L"Optimization", L"auto",                                         // FindShapeModleMinScore
			L"Kind of optimization and optionally method used for generating the model. List of values: auto, no_pregeneration, none, point_reduction_high, point_reduction_low, point_reduction_medium, pregeneration");

		o[L"CreateShapeModleMetric"] << Option(L"Metric", L"use_polarity",                                         // FindShapeModleMinScore
			L"Match metric. Default value: use_polarityList of values: ignore_color_polarity, ignore_global_polarity, ignore_local_polarity, use_polarity");

		o[L"CreateShapeModleContrast"] << Option(L"Contrast", L"auto",                                         // FindShapeModleMinScore
			L"Threshold or hysteresis thresholds for the contrast of the object in the template image and optionally minimum size of the object parts. Default value: auto Suggested values: auto, auto_contrast, auto_contrast_hyst, auto_min_size, 10, 20, 30, 40, 60, 80, 100, 120, 140, 160");
		
		o[L"CreateShapeModleMinContrast"] << Option(L"MinContrast", L"auto",                                         // FindShapeModleMinScore
			L"Minimum contrast of the objects in the search images. Default value: auto Suggested values: auto, 1, 2, 3, 5, 7, 10, 20, 30, 40 estriction: MinContrast < Contrast");


		o[L"FindShapeModleAngleStart"] << Option(L"AngleStart", -0.39f, -3.14f, 3.14f,                             // CreateShapeModel double AngleStart
			L"FindShapeModle double AngleStart RAD Default value: -0.39 Suggested values: -3.14, -1.57, -0.78, -0.39, -0.20, 0.0 ");

		o[L"FindShapeModleAngleExtend"] << Option(L"AngleExtend", 0.78f, 0.0f, 6.29f,                               // CreateShapeModel double AngleExtend
			L"FindShapeModle double AngleExtend Default value: 0.78 Suggested values: 6.29, 3.14, 1.57, 0.78, 0.39, 0.0 ");

		o[L"FindShapeModleMinScore"] << Option(L"MinScore", 0.85f, 0.30f, 1.0f,                                      // FindShapeModleMinScore
			L"FindShapeModle MinScore  Default value: 0.5 Suggested values: 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0");

		o[L"FindShapeModleNumMatches"] << Option(L"NumMatches", 1, 0, 20,                                              // FindShapeModleMinScore
			L"FindShapeModleNumMatches  Default value: 1 Suggested values: 0, 1, 2, 3, 4, 5, 10, 20");

		o[L"FindShapeModleMaxOverlap"] << Option(L"MaxOverlap", 0.5f, 0.0f, 1.00f,                                      // FindShapeModleMinScore
			L"FindShapeModle MaxOverlap double  Default value: 0.5 Suggested values: 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0");

		o[L"FindShapeModleSubPixel"] << Option(L"SubPixel", L"least_squares",                                         // FindShapeModleMinScore
			L"FindShapeModle least_squares Default value: least_squares List of values: interpolation, least_squares, least_squares_high, least_squares_very_high, max_deformation 1, max_deformation 2, max_deformation 3, max_deformation 4, max_deformation 5, max_deformation 6, none");

		o[L"FindShapeModleNumLevels"] << Option(L"NumLevels", 0, 0, 10,                                              // FindShapeModleMinScore
			L"FindShapeModle NumLevels Default value: 0  List of values: 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10");

		o[L"FindShapeModleGreediness"] << Option(L"Greediness", 0.9f, 0.0f, 1.0f,                                              // FindShapeModleMinScore
			L"FindShapeModle Default value: 0.9 Suggested values: 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0 Greediness of the search heuristic (0: safe but slow; 1: fast but matches may be missed).");
		
		o[L"DispShapeColor"] << Option(L"DispShapeColor", L"red",                                         // FindShapeModleMinScore
			L"dev_display_shape_matching_results color");

		o[L"RES_FindShapeRow"] << Option(L"RES_FindShapeRow", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeRow");

		o[L"RES_FindShapeColumn"] << Option(L"RES_FindShapeColumn", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeColumn");

		o[L"RES_FindShapeAngle"] << Option(L"RES_FindShapeAngle", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeAngle");

		o[L"RES_FindShapeScore"] << Option(L"RES_FindShapScore", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeScore");

		o[L"RES_FindShapeCenterRow"] << Option(L"RES_FindShapeCenterRow", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeCenterRow");

		o[L"RES_FindShapeCenterColumn"] << Option(L"RES_FindShapeCenterColumn", L"",                                         // FindShapeModleMinScore
			L"RES_FindShapeCenterColumn");

	}

	// 校正 mm 
	void stElement::initOptionMapElemnetMM(OptionsMap & o)
	{
		o[L"校正元素 X"] << Option(L"OUT_Elemnt_NameX", L"",                                                 //  元素的名称
			L"校正元素 X", true);

		//o[L"校正元素的参数 X"] << Option(L"OUT_Elemnt_Parament_NameX", L"",                               // 元素的参数名
		//	L"输出元素的参数", true);

		//o[L"校正元素的ID X"] << Option(L"OUT_Elemnt_IndentifyX", 0, 0, INT_MAX,                             // 检测元素的唯一编号
		//	L"校正元素的ID X", true);

		//o[L"校正元素的测量值 X"] << Option(L"OUT_Elemnt_DataX", 0.0f, -FLT_MAX, FLT_MAX,                             // 测量值
		//	L"OUT_Elemnt_Parament_DataX", true);

		//o[L"校正元素的标准值 X"] << Option(L"OUT_Elemnt_StandardX", 0.0f, -FLT_MAX, FLT_MAX,                     // 标准值
		//	L"OUT_Elemnt_Parament_StandardX", true);

		o[L"校正元素 Y"] << Option(L" OUT_Elemnt_NameY", L"",                                                 //  元素的名称
			L"校正元素 Y", true);

		//o[L"校正元素的参数 Y"] << Option(L"OUT_Elemnt_Parament_NameY", L"",                               // 元素的参数名
		//	L"输出元素的参数", true);

		//o[L"校正元素的ID Y"] << Option(L"OUT_Elemnt_IndentifyY", 0, 0, INT_MAX,                            // 检测元素的唯一编号
		//	L"校正元素的ID Y", true);

		//o[L"校正元素的测量值 Y"] << Option(L"OUT_Elemnt_DataY", 0.0f, -FLT_MAX, FLT_MAX,                            // 测量值
		//	L"OUT_Elemnt_Parament_DataY", true);

		//o[L"校正元素的标准值 Y"] << Option(L"OUT_Elemnt_StandardY", 0.0f, -FLT_MAX, FLT_MAX,                    // 标准值
		//	L"OUT_Elemnt_Parament_StandardY", true);
	}

	// 顶点元素
	void stElement::initOptionMapElemnetPeek(OptionsMap & o)
	{
		o[L"Peak_Rectangle2_Row"] << Option(L"Peak_Rectangle2_Row", 0.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak Rectangle2 Row  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Peak_Rectangle2_Colum"] << Option(L"Peak_Rectangle2_Colum", 0.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak Rectangle2 Colum  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Peak_Rectangle2_Phi"] << Option(L"Peak_Rectangle2_Phi", 0.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak Rectangle2 Phi  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Peak_Rectangle2_Length1"] << Option(L"Peak_Rectangle2_Length1", 0.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak Rectangle2 Length1  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Peak_Rectangle2_Length2"] << Option(L"Peak_Rectangle2_Length2", 0.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak Rectangle2 Length2  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Peak_DetectWidth"] << Option(L"Peak_DetectWidth", 2.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak_DetectWidth  [FLT_MIN, FLT_MAX] default 2.0 ", true);

		o[L"Peak_Sigma"] << Option(L"Peak_Sigma", 0.41f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak_Sigma [FLT_MIN, FLT_MAX] default 0.41 ", true);

		o[L"Peak_Threshold"] << Option(L"Peak_Threshold", 20.0f, -FLT_MAX, FLT_MAX,                                        // 
			L"Peak_Threshold[FLT_MIN, FLT_MAX] default 20.0 ", true);


		// 存储结果,顶点可能有多个
		o[L"Peak_cross_size"] << Option(L"Peak_cross_size", 20.0f, -FLT_MAX, FLT_MAX,
			L"顶点十字线的尺寸", true);

		o[L"Peak_cross_angle"] << Option(L"Peak_cross_angle", 0.785398f, -FLT_MAX, FLT_MAX,
			L"顶点十字线的角度 Default value: 0.785398 Suggested values: 0.0, 0.785398", true);

		// 存储结果,顶点可能有多个
		o[L"Peak_Transition"] << Option(L"Peak_Transition", L"positive",                                                     
			L"Peak ransition negative: 黑顶点 positive：白顶点", true);

		o[L"Peak_Select"] << Option(L"Peak_Select", L"first",
			L"Peak Select", true);
		

		// 存储结果,顶点可能有多个
		//o[L"nRES_hv_EdgesY"] << Option(L"RES_hv_EdgesY", "",                                                      // 元素的结果所有边缘
		//	"RES_hv EdgesY ", true);

		//o[L"nRES_hv_EdgesX"] << Option(L"RES_hv_EdgesX", "",                                                      // 元素的结果数据
		//	"RES_hv EdgesX ", true);

		o[L"RES_hv_ResultRow"] << Option(L"RES_hv_ResultRow", L"",                                                  // 元素的结果数据
			L"RES_hv ResultRow ", true);

		o[L"RES_hv_ResultColumn"] << Option(L"RES_hv_ResultColumn", L"",                                            // 元素的结果数据
			L"RES_hv ResultColumn ", true);

	}

	// 一维测量
	void stElement::initOptionMapELEMENT_MEASURE_RECTANGLE2(OptionsMap & o)
	{
		// 输入
		
		o[L"Measure_Rectangle2_Row"] << Option(L"Measure_Rectangle2_Row", 20.0f, 0.0f, FLT_MAX,                  // Row coordinate of the center of the rectangle.
			L"Measure Rectangle2 Row  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Measure_Rectangle2_Column"] << Option(L"Measure_Rectangle2_Column", 20.0f, 0.0f, FLT_MAX,           // Column coordinate of the center of the rectangle.
			L"Measure Rectangle2 Column  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Measure_Rectangle2_Phi"] << Option(L"Measure_Rectangle2_Phi", 0.0f, -FLT_MAX, FLT_MAX,           // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Measure Rectangle2 Phi  Typical range of values: -1.178097 ≤ Phi ≤ 1.178097 (lin)", true);

		o[L"Measure_Rectangle2_Length1"] << Option(L"Measure_Rectangle2_Length1", 20.0f, 1.0f, FLT_MAX,           // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Measure Rectangle2 Length1  Restriction: Length1 >= 1.0", true);

		o[L"Measure_Rectangle2_Length2"] << Option(L"Measure_Rectangle2_Length2", 10.0f, 1.0f, FLT_MAX,           // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Measure Rectangle2 Length2  Restriction: Length1 >= 1.0", true);

		o[L"Measure_Rectangle2_Interpolation"] << Option(L"Measure_Rectangle2_Interpolation", L"bilinear",            // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Measure Rectangle2 Interpolation  bicubic, bilinear, nearest_neighbor", true);

		// 结果
		// measure_pairs 输入
		o[L"measure_pairs_Sigma"] << Option(L"measure_pairs_Sigma", 1.0f, 0.4f, 100.0f,           // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Sigma of gaussian smoothing.Restriction: Sigma >= 0.4 ", true);
		
		o[L"measure_pairs_Threshold"] << Option(L"measure_pairs_Threshold", 30.0f, 1.0f, 255.0f,           // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Minimum edge amplitude.Default value: 30.0 Suggested values: 5.0, 10.0, 20.0, 30.0, 40.0, 50.0, 60.0, 70.0, 90.0, 110.0  ", true);
		
		o[L"measure_pairs_Transition"] << Option(L"measure_pairs_Transition", L"all",            // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Type of gray value transition that determines how edges are grouped to edge pairs. List of values: all, all_strongest, negative, negative_strongest, positive, positive_strongest", true);
	
		o[L"measure_pairs_Select"] << Option(L"measure_pairs_Select", L"all",            // Angle of longitudinal axis of the rectangle to horizontal (radians).
			L"Selection of edge pairs. Default value: all. List of values: all, first, last", true);

		// 输出
		o[L"measure_pairs_RowEdgeFirst"] << Option(L"measure_pairs_RowEdgeFirst", L"",            
			L"Row coordinate of the center of the first edge.", true);

		o[L"measure_pairs_ColumnEdgeFirst"] << Option(L"measure_pairs_ColumnEdgeFirst", L"",      
			L"Column coordinate of the center of the first edge.", true);

		o[L"measure_pairs_AmplitudeFirst"] << Option(L"measure_pairs_AmplitudeFirst", L"",        
			L"Edge amplitude of the first edge (with sign).", true);

		o[L"measure_pairs_RowEdgeSecond"] << Option(L"measure_pairs_RowEdgeSecond", L"",
			L"Row coordinate of the center of the second edge.", true);

		o[L"measure_pairs_ColumnEdgeSecond"] << Option(L"measure_pairs_ColumnEdgeSecond", L"",
			L"Column coordinate of the center of the second edge.", true);

		o[L"measure_pairs_AmplitudeSecond"] << Option(L"measure_pairs_AmplitudeSecond", L"",
			L"Edge amplitude of the second edge (with sign).", true);

		o[L"RES_measure_pairs_IntraDistance"] << Option(L"RES_measure_pairs_IntraDistance", L"",
			L"Distance between edges of an edge pair.", true);

		o[L"RES_measure_pairs_InterDistance"] << Option(L"RES_measure_pairs_InterDistance", L"",
			L"Distance between consecutive edge pairs.", true);

		o[L"RES_一维总长"] << Option(L"RES_一维总长", 0.0f, -FLT_MAX, FLT_MAX,
			L"RES_measure_pairs_Balloon.", true);

		o[L"尺寸显示偏移X"] << Option(L"尺寸显示偏移X", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移X", true);
		o[L"尺寸显示偏移Y"] << Option(L"尺寸显示偏移Y", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移Y", true);
	}

	// 球囊缺陷 
	void stElement::initOptionMapBalloonFlaw(OptionsMap & o)
	{
		// 输入
		o[L"Flaw_Rectangle2_Row"] << Option(L"Flaw_Rectangle2_Row", 400.0f, 0.0f, FLT_MAX,                 
			L"Flaw Rectangle2 Row  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Flaw_Rectangle2_Column"] << Option(L"Flaw_Rectangle2_Column", 400.0f, 0.0f, FLT_MAX,           
			L"Flaw Rectangle2 Column  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"Flaw_Rectangle2_Phi"] << Option(L"Flaw_Rectangle2_Phi", 0.0f, -FLT_MAX, FLT_MAX,              
			L"Typical range of values: -1.178097 ≤ Phi ≤ 1.178097 (lin)", true);

		o[L"Flaw_Rectangle2_Length1"] << Option(L"Flaw_Rectangle2_Length1", 400.0f, 10.0f, FLT_MAX,         
			L"Restriction: Length1 >= 1.0", true);

		o[L"Flaw_Rectangle2_Length2"] << Option(L"Flaw_Rectangle2_Length2", 300.0f, 10.0f, FLT_MAX,         
			L"Restriction: Length1 >= 1.0", true);

		// mean_image 均值
		o[L"Flaw_mean_image_MaskWidth"] << Option(L"Flaw_mean_image_MaskWidth", 43, 1, 501,
			L"Typical, Default value: 9, range of values: 1 ≤ MaskHeight ≤ 501", true);

		o[L"Flaw_mean_image_MaskHeight"] << Option(L"Flaw_mean_image_MaskHeight", 43, 1, 501,
			L"Typical, Default value: 9, range of values: 1 ≤ MaskHeight ≤ 501", true);

		// dyn_threshold 动态域值
		o[L"Flaw_dyn_threshold_Offset"] << Option(L"Flaw_dyn_threshold_Offset", 50.0, -255.0f, 255.0f,
			L"Default value: 5.0, Suggested values: 1.0, 3.0, 5.0, 7.0, 10.0, 20.0, 30.0 Typical range of values: -255.0 ≤ Offset ≤ 255.0 (lin) ", true);

		o[L"Flaw_dyn_threshold_LightDark"] << Option(L"Flaw_dyn_threshold_LightDark", L"dark",
			L"Extract light, dark or similar areas? Default value: light, List of values: dark, equal, light, not_equal  ", true);

		// closing_circle
		o[L"Flaw_closing_circle_Radius"] << Option(L"Flaw_closing_circle_Radius", 3.5f, 0.5f, 511.0f,
			L"Radius of the circular structuring element.  Default value: 3.5 Suggested values: 1.5, 2.5, 3.5, 4.5, 5.5, 7.5, 9.5, 12.5, 15.5, 19.5, 25.5, 33.5, 45.5, 60.5, 110.5", true);

		// select_shape
		o[L"Flaw_select_shape_Features"] << Option(L"Flaw_select_shape_Features", L"area",
			L"Shape features to be checked. Default value: area, List of values: anisometry, area, area_holes, bulkiness, circularity, column, column1, column2, compactness, connect_num, contlength, convexity, dist_deviation, dist_mean, euler_number, height, holes_num, inner_height, inner_radius, inner_width, max_diameter, moments_i1, ", true);
		
		o[L"Flaw_select_shape_Operation"] << Option(L"Flaw_select_shape_Operation", L"and",
			L"Linkage type of the individual features.Default value: and, List of values: and, or ", true);


		o[L"Flaw_select_shape_Min"] << Option(L"Flaw_select_shape_Min", 15.0f, 1.0f, FLT_MAX,
			L"Lower limits of the features or min.", true);

		o[L"Flaw_select_shape_Max"] << Option(L"Flaw_select_shape_Max", 99999.0f, 1.0f, FLT_MAX,
			L"Lower limits of the features or max.", true);


		o[L"RES_缺陷数量"] << Option(L"RES_缺陷数量", 0.0f, 0.0f, FLT_MAX,
			L"输出：RES_缺陷数量", true);

		o[L"RES_缺陷最大面积"] << Option(L"RES_缺陷最大面积", 0.0f, 0.0f, FLT_MAX,
			L"RES_缺陷最大面积", true);

		o[L"RES_缺陷最小面积"] << Option(L"RES_缺陷最小面积", 0.0f, 0.0f, FLT_MAX,
			L"RES_缺陷最小面积", true);
	}

	// ROI
	void stElement::initOptionMapROI(OptionsMap & o)
	{
		
		// 先校正才能有ROI
		o[L"USE_map_image"] << Option(L"USE_map_image", 1, 0, 1,                               // 上面先处理，再校正
			L"使用相机的校正信息对图像进行畸变校正.");
		
		// 输入
		o[L"ROI_Rectangle2_Row"] << Option(L"ROI_Rectangle2_Row", 400.0f, 0.0f, FLT_MAX,
			L"ROI Rectangle2 Row  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_Rectangle2_Column"] << Option(L"ROI_Rectangle2_Column", 400.0f, 0.0f, FLT_MAX,
			L"ROI Rectangle2 Column  [FLT_MIN, FLT_MAX] default 0.0 ", true);

		o[L"ROI_Rectangle2_Phi"] << Option(L"ROI_Rectangle2_Phi", 0.0f, -FLT_MAX, FLT_MAX,
			L"Typical range of values: -1.178097 ≤ Phi ≤ 1.178097 (lin)", true);

		o[L"ROI_Rectangle2_Length1"] << Option(L"ROI_Rectangle2_Length1", 200.0f, 1.0f, FLT_MAX,
			L"Restriction: Length1 >= 1.0", true);

		o[L"ROI_Rectangle2_Length2"] << Option(L"ROI_Rectangle2_Length2", 200.0f, 1.0f, FLT_MAX,
			L"Restriction: Length1 >= 1.0", true);
	}

	// 球囊长度
	void stElement::initOptionMapBalloonLength(OptionsMap & o)
	{
		o[L"IsDisplayDimension"] << Option(L"IsDisplayDimension", 1, 0, 1,                                     // 是否显示尺寸数值
			L"IsDisplayDimension", false);

		o[L"引用顶点1"] << Option(L"引用顶点1", L"",                                  // 使用顶点1名称
			L"引用顶点1", true);

		//o[L"USE_Peek1_Indentify"] << Option(L"USE_Peek1_Indentify", 0, 0, INT_MAX,                              // 使用顶点1 ID
		//	L"USE_Peek1_Indentify", true);
		
		o[L"引用顶点2"] << Option(L"引用顶点2", L"",                                  // 使用顶点1名称
			L"引用顶点2", true);

		//o[L"USE_Peek2_Indentify"] << Option(L"USE_Peek2_Indentify", 0, 0, INT_MAX,                              // 使用顶点1 ID
		//	L"USE_Peek2_Indentify", true);

		o[L"引用顶点3"] << Option(L"引用顶点3", L"",                                  // 使用顶点1名称
			L"引用顶点3", true);

		//o[L"USE_Peek3_Indentify"] << Option(L"USE_Peek3_Indentify", 0, 0, INT_MAX,                              // 使用顶点1 ID
		//	L"USE_Peek3_Indentify", true);

		o[L"引用顶点4"] << Option(L"引用顶点4", L"",                                  // 使用顶点4名称
			L"引用顶点4", true);

		//o[L"USE_Peek4_Indentify"] << Option(L"USE_Peek4_Indentify", 0, 0, INT_MAX,                              // 使用顶点4 ID
		//	L"USE_Peek4_Indentify", true);

		o[L"尺寸显示偏移X"] << Option(L"尺寸显示偏移X", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移X", true);
		o[L"尺寸显示偏移Y"] << Option(L"尺寸显示偏移Y", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"尺寸显示偏移Y", true);

		o[L"Balloon距离Row1"] << Option(L"距离Row1", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Row1", false);
		o[L"Balloon距离Col1"] << Option(L"距离Col1", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Col1", false);

		o[L"Balloon距离Row2"] << Option(L"距离Row2", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Row1", false);
		o[L"Balloon距离Col2"] << Option(L"距离Col2", 0.0f, -FLT_MAX, FLT_MAX,                        // 
			L"距离Col2", false);

		o[L"尺寸箭头长度"] << Option(L"尺寸箭头长度", 10.0f, 2.0f, FLT_MAX,                                      // gen_arrow_contour_xld
			L"Arrow_HeadLength Default Value: 5 建议值: [2,3,5,10,20]", true);

		o[L"尺寸箭头宽度"] << Option(L"尺寸箭头宽度", 10.0f, 2.0f, FLT_MAX,                                      // gen_arrow_contour_xld
			L"The width of the arrow head in pixels Default Value: 5 建议值: [2,3,5,10,20]", true);

		o[L"RES_球囊长度"] << Option(L"RES_Balloon_Length", 0.0f, -FLT_MAX, FLT_MAX,                            // 球囊的长度
			L"RES_Balloon_Length [FLT_MIN, FLT_MAX] default 0.0 ", true);

	}

	////////////////////////////////////////////////////////////////////////////////////
	// 初始化一个元素变量
	OptionsMap* stElement::initOptionMapElemnet(eELEMENT_TYPE t, int iPID)
	{
		
		OptionsMap o;
		Option::insert_order = 0;

		
		// 不用唯一编号，只用唯一名称
		CString name = L""; 
		for (int i = 1; i < INT_MAX; i++) {
			name.Format(L"%s %d", EleName[t], i);
			if (!UCI::ELE.isHaveSaveElementName(name)) {
				break;
			}
		}

		//int index = (int)ELE.EleList.GetSize() + 1;

		// 初始化一些公共变量                                                   
		o[L"项目编号"] << Option(L"项目编号", iPID, 1, INT_MAX,                         // 当前组的所属的项目编号
			L"Current Project ID", false);

		o[L"元素类型"] << Option(L"元素类型", t, 0, ELEMNET_END - 1,                    // 当前参数元素类型
			L"Current Elemnet Type",false);
		
		o[L"元素名称"] << Option(L"元素名称", name,                                     // 元素的名称
			L"Element Name", true);	
		

		o[L"元素已匹配"] << Option(L"元素已匹配", 0, 0, 1,                           // 元素的已找到
			L"Element is Finded", false);

		switch (t) {
		case ELEMENT_LINE:      // 直线
			initOptionMapElemnetLine(o);
			break;
		case ELEMENT_CIRCLE:    // 圆 
			initOptionMapElemnetCircle(o);
			break;

		case ELEMENT_ELLIPSE:   // 椭圆

			break;
		case ELEMENT_ANGLE:     // 角度
			initOptionMapElemnetAngle(o);
			break;
		case ELEMENT_RESULT:    // 输出结果
		{
			o[L"报表列名称"] << Option(L"报表列名称", name,             // 要输出的元素的名称
				L"输出到报表时，最终显示的名称", true);
		}

			initOptionMapElemnetResult(o);	
			break;

		case ELEMENT_MM:
			initOptionMapElemnetMM(o);
			break;
		case ELEMENT_DISTANCE:
			initOptionMapElemnetDistance(o);
			break;
		case ELEMENT_CAMERA:
			initOptionMapElemnetCamera(o);
			break;
		case ELEMENT_PEAK:
			initOptionMapElemnetPeek(o);
			break;
		case ELEMENT_BALLOONLENGTH:
			initOptionMapBalloonLength(o);
			break;
		case ELEMENT_FUNCTION:
			initOptionMapFUNCTION(o);
			break;
		case ELEMENT_MATCH:
			initOptionMapMATCH(o);
			break;
		case ELEMENT_MEASURE_RECTANGLE2:
			initOptionMapELEMENT_MEASURE_RECTANGLE2(o);
			break;
		case ELEMENT_BALLOON_FLAW:
			initOptionMapBalloonFlaw(o);
			break;
		case ELEMENT_ROI:
			initOptionMapROI(o);
			break;
		default:			
			break;
		}

		o[L"命令行"] << Option(L"命令行", L"none", L"Command Macro", true);                           // 元素的命令宏
		o[L"元素备注"] << Option(L"元素备注", L"note", L"Element Note", true);                        // 元素的备注
		
		ELE.EleList.AddTail(o);
		return &ELE.EleList.GetTail();
	}

	
}




