#include "stdafx.h"
#include "Myhead.h"
#include "MainFrm.h"

#include "sapi.h"


using namespace HalconCpp;
using namespace std;




void ST_MYDISP::DisBalloonOffileMold()   // 球囊离线建模
{
	
	UCI::OPTIONS["CAMCurGroupID"] = 1;

	try {
		// 拍一张图片
		HalconCpp::HImage   Image;

		this->DisOnOneShot();
		EnterCriticalSection(&this->m_csFGDisp);
		Image = this->GrabImageDisp;
		LeaveCriticalSection(&this->m_csFGDisp);

		SetSystem("border_shape_models", "true");
		// 清空原来的模板
		this->hv_Model.Clear();

		this->write_message(20, 20, "画模板区域,用来定位球囊,完成后点击鼠标右键确认", true);

		double Row, Column, Phi, Length1, Length2;
		HalconCpp::HImage ho_ImageReduced;
		// 画区域
		this->m_pMHWinMain->DrawRectangle2(&Row, &Column, &Phi, &Length1, &Length2);
		//DrawRectangle2(this->hv_HWD, &Row, &Column, &Phi, &Length1, &Length2);		

		// 产生区域
		GenRectangle2(&this->ho_ModelRegion, Row, Column, Phi, Length1, Length2);
		// 裁剪模板区图像
		ho_ImageReduced = Image.ReduceDomain(this->ho_ModelRegion);

		this->hv_Model.CreateShapeModel(
			ho_ImageReduced,
			"auto",
			HTuple(-180).TupleRad(),
			HTuple(360).TupleRad(),
			"auto",
			"auto",
			"use_polarity",
			"auto",
			"auto"
		);

		// 再在原图上查找确认一下.
		HTuple Row3, Column3, Angle1, Score1;

		this->hv_Model.FindShapeModel(
			Image,
			HTuple(-180).TupleRad(),
			HTuple(360).TupleRad(),
			0.5,
			1,
			0.5,
			"least_squares",
			0,
			0.9,
			&Row3,
			&Column3,
			&Angle1,
			&Score1
		);

		// 显示一下查找到的模板 
		dev_display_shape_matching_results(
			this->hv_Model,
			"red",
			Row3,
			Column3,
			Angle1,
			1,
			1,
			0);

		if (Score1 < 0.9) {
			write_message(20, 20, "模板定义不准确! 请重新绘制模板区域 ", true);
			return;
		}

		// 创建直线1, 球囊外径的一条边
		// 3.  创建直线 1 检测 ROI
		// DispObj(this->ho_pImage, this->hv_HWD);

		this->m_pMHWinMain->SetColor("green");

		HXLDCont Regions;
		double Row1, Column1, Row2, Column2;

		draw_rake(&Regions, 
			10,
			30,
			15,
			&Row1,
			&Column1,
			&Row2,
			&Column2);
		
		this->m_pMHWinMain->DispObj(Regions);

		HTuple Line1ROIRow, Line1ROICol;

		Line1ROIRow.Clear();
		Line1ROIRow.Append(Row1);
		Line1ROIRow.Append(Row2);

		Line1ROICol.Clear();
		Line1ROICol.Append(Column1);
		Line1ROICol.Append(Column2);

		// 4. 
		this->m_pMHWinMain->SetColor("red");
		draw_rake(&Regions,
			10,
			30,
			15,
			&Row1,
			&Column1,
			&Row2,
			&Column2);

		this->m_pMHWinMain->DispObj(Regions);

		//this->m_pMHWinMain->DrawEllipse

		HTuple Line2ROIRow, Line2ROICol;

		Line2ROIRow.Clear();
		Line2ROIRow.Append(Row1);
		Line2ROIRow.Append(Row2);

		Line2ROICol.Clear();
		Line2ROICol.Append(Column1);
		Line2ROICol.Append(Column2);

		// 保存参数 
		WriteRegion(this->ho_ModelRegion, "ModelROI.reg");

		//if (this->hv_ModelID > -1) {
		//	WriteShapeModel(this->hv_ModelID, "Model.shm");
		//	ClearShapeModel(this->hv_ModelID);
		//	this->hv_ModelID = -1;
		//}
		this->hv_Model.WriteShapeModel("Model.shm");
		this->hv_Model.Clear();

		// 保存直线 1 ROI 参数
		WriteTuple(Line1ROIRow, "Line1ROIRow.tup");
		WriteTuple(Line1ROICol, "Line1ROICol.tup");

		// 保存直线 2 ROI 参数
		WriteTuple(Line2ROIRow, "Line2ROIRow.tup");
		WriteTuple(Line2ROICol, "Line2ROICol.tup");

		write_message(100, 20, "球囊定位模板已建立成功. 亲! 您可以开始检测球囊了!");
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
	}
}

void ST_MYDISP::DisOnBalloonMeasure()
{
	try {
		ReadRegion(&this->ho_ModelRegion, "ModelROI.reg");
		this->hv_Model.ReadShapeModel("Model.shm");
		//ReadShapeModel("Model.shm", &this->hv_ModelID);

		// 读取直线1 ROI 参数
		HTuple Line1ROIRow, Line1ROICol;
		ReadTuple("Line1ROIRow.tup", &Line1ROIRow);
		ReadTuple("Line1ROICol.tup", &Line1ROICol);

		// 读取直线2 ROI 参数
		HTuple Line2ROIRow, Line2ROICol;
		ReadTuple("Line2ROIRow.tup", &Line2ROIRow);
		ReadTuple("Line2ROICol.tup", &Line2ROICol);

		// 计算模板区域中心
		HTuple Area, Row0, Column0;
		AreaCenter(this->ho_ModelRegion, &Area, &Row0, &Column0);

		this->DisOnOneShot();   // 拍一张图片
		HalconCpp::HImage result_img;
		EnterCriticalSection(&this->m_csFGDisp);
		result_img = this->GrabImageDisp;
		LeaveCriticalSection(&this->m_csFGDisp);

		HTuple Row, Column, Angle, Score;

		this->hv_Model.FindShapeModel(
			result_img,
			HTuple(-180).TupleRad(),
			HTuple(360).TupleRad(),
			0.5,
			1,
			0.5,
			"least_squares",
			0,
			0.9,
			&Row,
			&Column,
			&Angle,
			&Score
		);	

		if (Row.TupleLength() < 1 || Score<0.8) {
			write_message(20, 20, "亲, 没有发现球囊, 坏蛋, 你把球囊藏在哪儿了?", true);
			return;
		}

		// 4. 找到物体, 求仿射矩阵, 对 ROI 特征点进行仿射变换

		HTuple HomMat2D;
		VectorAngleToRigid(Row0, Column0, 0, Row, Column, Angle, &HomMat2D);

		// 对直线和圆 ROI 区域特征点进行仿射变换

		// 直线 1 ROI 参数
		HTuple NewLine1ROIRow, NewLine1ROICol;
		AffineTransPoint2d(HomMat2D, Line1ROIRow, Line1ROICol,
			&NewLine1ROIRow, &NewLine1ROICol);

		// 直线 2 ROI 参数
	    HTuple NewLine2ROIRow, NewLine2ROICol;
		AffineTransPoint2d(HomMat2D, Line2ROIRow, Line2ROICol,
		&NewLine2ROIRow, &NewLine2ROICol);


		// 5. 查找直线1, 直线2的边缘并拟合

		// 拟合直线 1
		// 找边缘
		HXLDCont Regions1;
		HTuple ResultRow1, ResultColumn1;
		rake(
			result_img,
			&Regions1,
			10,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewLine1ROIRow[0],
			NewLine1ROICol[0],
			NewLine1ROIRow[1],
			NewLine1ROICol[1],
			&ResultRow1,
			&ResultColumn1);


		// 拟合
		HXLDCont Line1;
		HTuple 	Line1Row11, Line1Column11, Line1Row21, Line1Column21;
		pts_to_best_line(
			&Line1,
			ResultRow1,
			ResultColumn1,
			2,
			&Line1Row11,
			&Line1Column11,
			&Line1Row21,
			&Line1Column21);

		// 拟合直线 2
		// 找边缘
		HObject Regions2;
		HTuple ResultRow2, ResultColumn2;
		rake(
			result_img,
			&Regions1,
			10,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewLine2ROIRow[0],
			NewLine2ROICol[0],
			NewLine2ROIRow[1],
			NewLine2ROICol[1],
			&ResultRow2,
			&ResultColumn2);

		// 拟合
		HXLDCont Line2;
		HTuple 	Line2Row11, Line2Column11, Line2Row21, Line2Column21;
		pts_to_best_line(
			&Line2,
			ResultRow2,
			ResultColumn2,
			2,
			&Line2Row11,
			&Line2Column11,
			&Line2Row21,
			&Line2Column21);

		// 求直线1和直线2的距离
		// 求直线2的中点, 用中点到直线1的距离作为2条直线的距离
		HTuple Line2CenterY = (Line2Row11 + Line2Row21)*0.5;
		HTuple Line2CenterX = (Line2Column11 + Line2Column21)*0.5;
		HTuple Line2ToLine1Distance;

		DistancePl(Line2CenterY, Line2CenterX,
			Line1Row11, Line1Column11,
			Line1Row21, Line1Column21,
			&Line2ToLine1Distance);

		// 显示图像和结果
		this->m_pMHWinMain->SetLineWidth(2);
		this->m_pMHWinMain->DispObj(result_img);

		// 显示直线1
		this->m_pMHWinMain->SetColor("red");
		this->m_pMHWinMain->DispObj(Line1);

		// 显示直线2
		this->m_pMHWinMain->SetColor("green");
		this->m_pMHWinMain->DispObj(Line2);

		write_message(300, 20, "亲, 球囊直径是:" + Line2ToLine1Distance, true);

	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
	}
}

void ST_MYDISP::DisOnButOffLineMold()
{

	//CLSID CLSID_SpVoice;    // class ID for the SAPI SpVoice object
	//LPDISPATCH pDisp;       // dispatch interface for the class
	//ISpeechVoice voice;     // use the MFC Class Wizard generated wrapper

	//CLSIDFromProgID(L"SAPI.SpVoice", &CLSID; _SpVoice);
	//voice.CreateDispatch(CLSID_SpVoice);
	//pDisp = voice.m_lpDispatch;

	UCI::OPTIONS["CAMCurGroupID"] = 1;
	
	// 拍一张图片
	this->DisOnOneShot(); 
	HalconCpp::HImage result_img;
	EnterCriticalSection(&this->m_csFGDisp);
	result_img = this->GrabImageDisp;
	LeaveCriticalSection(&this->m_csFGDisp);

	//// Display the results: contours on original image
	//EnterCriticalSection(&UCI::pDis->m_csIPDisp);      // CriticalSect		
	//UCI::pDis->m_sIPResult.result_img = result_img;
	////UCI::pDis->m_sIPResult.result_cont = Contours;
	//LeaveCriticalSection(&UCI::pDis->m_csIPDisp);      // CriticalSect
	//PostMessage(this->pCView->GetSafeHwnd(), WM_IPDISP, NULL, NULL);

	this->m_pMHWinMain->DispObj(result_img);

	//
	SetSystem("border_shape_models", "true");

	// 清空原来的模板
	this->hv_Model.Clear();

	this->write_message(20, 20, "画模板区域,点击鼠标右键确认", true);

	double Row, Column, Phi, Length1, Length2;
	HalconCpp::HImage ho_ImageReduced;
	// 画区域
	this->m_pMHWinMain->DrawRectangle2(&Row, &Column, &Phi, &Length1, &Length2);
	// 产生区域
	GenRectangle2(&this->ho_ModelRegion, Row, Column, Phi, Length1, Length2);
	// 裁剪模板区图像
	ho_ImageReduced = result_img.ReduceDomain(this->ho_ModelRegion);
	//ReduceDomain(this->ho_Image, this->ho_ModelRegion, &ho_ImageReduced);

	HShapeModel hsModel;

	hsModel.CreateShapeModel(
		ho_ImageReduced,
		"auto",
		HTuple(-180).TupleRad(),
		HTuple(360).TupleRad(),
		"auto",
		"auto",
		"use_polarity",
		"auto",
		"auto"
	);

	// 再在原图上查找确认一下.
	HTuple Row3, Column3, Angle1, Score1;

	hsModel.FindShapeModel(
		result_img,
		HTuple(-180).TupleRad(),
		HTuple(360).TupleRad(),
		0.5,
		1,
		0.5,
		"least_squares",
		0,
		0.9,
		&Row3,
		&Column3,
		&Angle1,
		&Score1
	);

	// 显示一下查找到的模板 
	dev_display_shape_matching_results(
		hsModel,
		"red",
		Row3,
		Column3,
		Angle1,
		1,
		1,
		0);

	if (Score1 < 0.9) {
		write_message(20, 20, "模板定义不准确! ",true);
		return;
	}

	// 创建直线 1 检测 ROI
	this->m_pMHWinMain->SetColor("green");
	
	HXLDCont Regions;
	double Row1, Column1, Row2, Column2;
	draw_rake(&Regions, 
		10,
		30,
		15,
		&Row1,
		&Column1,
		&Row2,
		&Column2);

	this->m_pMHWinMain->DispObj(Regions);

	HTuple Line1ROIRow, Line1ROICol;

	Line1ROIRow.Clear();
	Line1ROIRow.Append(Row1);
	Line1ROIRow.Append(Row2);

	Line1ROICol.Clear();
	Line1ROICol.Append(Column1);
	Line1ROICol.Append(Column2);

	// 创建圆检测 ROI
	HXLDCont Regions1;
	HTuple ROIRows, ROICols, Direct;
	draw_spoke(
		//result_img,
		&Regions1,
		30,
		30,
		15,
		&ROIRows,
		&ROICols,
		&Direct
	);

	HTuple CircleROIRow, CircleROICol;
	CircleROIRow = ROIRows;
	CircleROICol = ROICols;

	this->m_pMHWinMain->DispObj(Regions1);

	// 保存参数
	WriteRegion(this->ho_ModelRegion, "ModelROI.reg");

	//if (ModelID > -1) {
	//	WriteShapeModel(ModelID, "Model.shm");
	//	ClearShapeModel(ModelID);
	//	ModelID = -1;
	//}
	hsModel.WriteShapeModel("Model.shm");
	hsModel.Clear();

	// 保存直线 1 ROI 参数
	WriteTuple(Line1ROIRow, "Line1ROIRow.tup");
	WriteTuple(Line1ROICol, "Line1ROICol.tup");


	// 保留圆 ROI 参数
	WriteTuple(CircleROIRow, "CircleROIRow.tup");
	WriteTuple(CircleROICol, "CircleROICol.tup");       
	WriteTuple(Direct, "CircleDirect.tup");             // 圆的方向

	//
	write_message(100, 20, "离线建模成功了!", true);
}



void ST_MYDISP::DisOnButOnlineInspect() {
	// 读取模板区域
	HObject ModelROI;
	HTuple ModelID;

	try {
		ReadRegion(&ModelROI, "ModelROI.reg");
		//ReadShapeModel("Model.shm", &ModelID);
		this->hv_Model.ReadShapeModel("Model.shm");
		
		// 读取直线1 ROI 参数
		HTuple Line1ROIRow, Line1ROICol;
		ReadTuple("Line1ROIRow.tup", &Line1ROIRow);
		ReadTuple("Line1ROICol.tup", &Line1ROICol);

		// 读取圆 ROI 参数
		HTuple CircleROIRow, CircleROICol;
		ReadTuple("CircleROIRow.tup", &CircleROIRow);
		ReadTuple("CircleROICol.tup", &CircleROICol);

		SetSystem("border_shape_models", "true");

		// 计算模板区域中心
		HTuple Area, Row0, Column0;
		AreaCenter(ModelROI, &Area, &Row0, &Column0);

		this->DisOnOneShot();   // 拍一张图片
		HalconCpp::HImage result_img;
		EnterCriticalSection(&this->m_csFGDisp);
		result_img = this->GrabImageDisp;
		LeaveCriticalSection(&this->m_csFGDisp);

		this->m_pMHWinMain->DispObj(result_img);

		HTuple Row, Column, Angle, Score;
		this->hv_Model.FindShapeModel(
			result_img,
			HTuple(-180).TupleRad(),
			HTuple(360).TupleRad(),
			0.5,
			1,
			0.5,
			"least_squares",
			0,
			0.9,
			&Row,
			&Column,
			&Angle,
			&Score);		

		if (Row.TupleLength() < 1) {
			write_message(20, 20, "没有发现对象, 请重新打开对象", true);
			return;
		}

		// 下面开始计算
		

		// 4. 找到物体, 求仿射矩阵, 对 ROI 特征点进行仿射变换

		HTuple HomMat2D;
		VectorAngleToRigid(Row0, Column0, 0, Row, Column, Angle, &HomMat2D);

		// 对直线和圆 ROI 区域特征点进行仿射变换

		// 直线 1 ROI 参数
		HTuple NewLine1ROIRow, NewLine1ROICol;
		AffineTransPoint2d(HomMat2D, Line1ROIRow, Line1ROICol,
			&NewLine1ROIRow, &NewLine1ROICol);

		// 直线 2 ROI 参数
		//HTuple NewLine2ROIRow, NewLine2ROICol;
		//AffineTransPoint2d(HomMat2D, Line2ROIRow, Line2ROICol,
		//	&NewLine2ROIRow, &NewLine2ROICol);

		// 圆 ROI 参数
		HTuple NewCircleROIRow, NewCircleROICol;
		AffineTransPoint2d(HomMat2D, CircleROIRow, CircleROICol,
			&NewCircleROIRow, &NewCircleROICol);

		// 5. 查找直线1, 直线2的边缘并拟合

		// 拟合直线 1
		// 找边缘
		HXLDCont Regions1;
		HTuple ResultRow1, ResultColumn1;
		rake(
			result_img,
			&Regions1,
			10,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewLine1ROIRow[0],
			NewLine1ROICol[0],
			NewLine1ROIRow[1],
			NewLine1ROICol[1],
			&ResultRow1,
			&ResultColumn1);


		// 拟合
		HXLDCont Line1;
		HTuple 	Line1Row11, Line1Column11, Line1Row21, Line1Column21;
		pts_to_best_line(
			&Line1,
			ResultRow1,
			ResultColumn1,
			2,
			&Line1Row11,
			&Line1Column11,
			&Line1Row21,
			&Line1Column21);

		// 拟合圆
		// 找边缘
		HXLDCont Regions3;
		HTuple ResultRow3, ResultColumn3, ArcType;
		spoke(
			result_img,
			&Regions3,
			30,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewCircleROIRow,
			NewCircleROICol,
			"inner",
			&ResultRow3,
			&ResultColumn3,
			&ArcType);

		// 拟合
		HXLDCont Circle1;
		HTuple RowCenter, ColCenter, Radius, StartPhi, EndPhi, PointOrder;
		pts_to_best_circle(
			&Circle1,
			ResultRow3,
			ResultColumn3,
			3,
			"circle",
			&RowCenter,
			&ColCenter,
			&Radius,
			&StartPhi,
			&EndPhi,
			&PointOrder);

		// 产生圆心
		HObject Cross;
		GenCrossContourXld(&Cross, RowCenter, ColCenter, 16, 0);

		// 7. 测量, 并根据像素当量, 把测量结果从图像坐标转化到物理体系空间

		// 求直线1和直线2的距离
		// 求直线2的中点, 用中点到直线1的距离作为2条直线的距离
		//HTuple Line2CenterY = (Line2Row11 + Line2Row21)*0.5;
		//HTuple Line2CenterX = (Line2Column11 + Line2Column21)*0.5;
		HTuple Line2ToLine1Distance;

		// 求圆心到直线的距离
		DistancePl(RowCenter, ColCenter,
			Line1Row11, Line1Column11,
			Line1Row21, Line1Column21,
			&Line2ToLine1Distance);

		// 把距离从图像空间转化到物理空间,单位 mm
		HTuple Res = 1.0;
		Line2ToLine1Distance = Line2ToLine1Distance * Res;

		//// 求直线2到直线1的夹角,单位: 弧度
		//HTuple Line2ToLine1Angle;
		//AngleLl(
		//	Line2Row11,
		//	Line2Column11,
		//	Line2Row21,
		//	Line2Column21,
		//	Line1Row11,
		//	Line1Column11,
		//	Line1Row21,
		//	Line1Column21,
		//	&Line2ToLine1Angle);

		//// 求直线1到直线2的夹角,单位: 弧度
		//HTuple Line1ToLine2Angle;
		//AngleLl(
		//	Line1Row11,
		//	Line1Column11,
		//	Line1Row21,
		//	Line1Column21,
		//	Line2Row11,
		//	Line2Column11,
		//	Line2Row21,
		//	Line2Column21,
		//	&Line1ToLine2Angle);

		// 圆的半径, 从图像空间转化到物理空间 单位:mm
		HTuple RadiusWord = Radius*Res;

		// 显示图像和结果
		this->m_pMHWinMain->SetLineWidth(3);
		this->m_pMHWinMain->DispObj(result_img);

		// 显示直线1
		this->m_pMHWinMain->SetColor("red");
		this->m_pMHWinMain->DispObj(Line1);

		// 显示直线2
		//SetColor(this->hv_HWD, "green");
		//DispObj(Line2, this->hv_HWD);

		// 显示圆和圆心
		this->m_pMHWinMain->SetColor("blue");
		this->m_pMHWinMain->DispObj(Circle1);
		this->m_pMHWinMain->DispObj(Cross);

		write_message(80, 20, "找到对象,得分:" + Score, true);
		// 显示测量结果
		write_message(200, 20, "圆心到直线1的距离为(mm)" + Line2ToLine1Distance, false);
		//write_message(240, 20, "直线2到直线1的夹角 弧度:" + Line2ToLine1Angle, true);
		//write_message(280, 20, "直线1到直线2的夹角 弧度:" + Line1ToLine2Angle, true);
		write_message(320, 20, "圆半径 mm:" + RadiusWord,false);

		if (ModelID > -1) {
			ClearShapeModel(ModelID);
		}


	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
	}
}

void ST_MYDISP::DisOnButOnlineInspect_3_86()
{
	// 读取模板区域
	HObject ModelROI;
	//HTuple ModelID;

	try {

		ReadRegion(&ModelROI, "ModelROI.reg");

		this->hv_Model.ReadShapeModel("Model.shm");
		//ReadShapeModel("Model.shm", &ModelID);

		// 读取直线1 ROI 参数
		HTuple Line1ROIRow, Line1ROICol;
		ReadTuple("Line1ROIRow.tup", &Line1ROIRow);
		ReadTuple("Line1ROICol.tup", &Line1ROICol);

		// 读取直线2 ROI 参数
		HTuple Line2ROIRow, Line2ROICol;
		ReadTuple("Line2ROIRow.tup", &Line2ROIRow);
		ReadTuple("Line2ROICol.tup", &Line2ROICol);

		// 读取圆 ROI 参数
		HTuple CircleROIRow, CircleROICol;
		ReadTuple("CircleROIRow.tup", &CircleROIRow);
		ReadTuple("CircleROICol.tup", &CircleROICol);

		SetSystem("border_shape_models", "true");

		// 计算模板区域中心
		HTuple Area, Row0, Column0;
		AreaCenter(ModelROI, &Area, &Row0, &Column0);

		DisOnOpenImage();
		HalconCpp::HImage result_img;
		EnterCriticalSection(&this->m_csFGDisp);
		result_img = this->GrabImageDisp;
		LeaveCriticalSection(&this->m_csFGDisp);

		HTuple Row, Column, Angle, Score;
		this->hv_Model.FindShapeModel(
			result_img,
			HTuple(-180).TupleRad(),
			HTuple(360).TupleRad(),
			0.5,
			1,
			0.5,
			"least_squares",
			0,
			0.9,
			&Row,
			&Column,
			&Angle,
			&Score);		

		if (Row.TupleLength() < 1) {
			write_message(20, 20, "没有发现对象, 请重新打开对象", true);
			return;
		}

		write_message(80, 20, "找到对象,得分:" + Score, true);

		// 4. 找到物体, 求仿射矩阵, 对 ROI 特征点进行仿射变换

		HTuple HomMat2D;
		VectorAngleToRigid(Row0, Column0, 0, Row, Column, Angle, &HomMat2D);

		// 对直线和圆 ROI 区域特征点进行仿射变换
		
		// 直线 1 ROI 参数
		HTuple NewLine1ROIRow, NewLine1ROICol;
		AffineTransPoint2d(HomMat2D, Line1ROIRow, Line1ROICol,
			&NewLine1ROIRow, &NewLine1ROICol);

		// 直线 2 ROI 参数
		HTuple NewLine2ROIRow, NewLine2ROICol;
		AffineTransPoint2d(HomMat2D, Line2ROIRow, Line2ROICol,
			&NewLine2ROIRow, &NewLine2ROICol);

		// 圆 ROI 参数
		HTuple NewCircleROIRow, NewCircleROICol;
		AffineTransPoint2d(HomMat2D, CircleROIRow, CircleROICol,
			&NewCircleROIRow, &NewCircleROICol);

		// 5. 查找直线1, 直线2的边缘并拟合

		// 拟合直线 1
		// 找边缘
		HXLDCont Regions1;
		HTuple ResultRow1, ResultColumn1;
		rake(
			result_img,
			&Regions1,
			10,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewLine1ROIRow[0],
			NewLine1ROICol[0],
			NewLine1ROIRow[1],
			NewLine1ROICol[1],
			&ResultRow1,
			&ResultColumn1);


		// 拟合
		HXLDCont Line1;
		HTuple 	Line1Row11, Line1Column11, Line1Row21, Line1Column21;
		pts_to_best_line(
			&Line1,
			ResultRow1,
			ResultColumn1,
			2,
			&Line1Row11,
			&Line1Column11,
			&Line1Row21,
			&Line1Column21);

		// 拟合直线 2
		// 找边缘
		HXLDCont Regions2;
		HTuple ResultRow2, ResultColumn2;
		rake(
			result_img,
			&Regions2,
			10,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewLine2ROIRow[0],
			NewLine2ROICol[0],
			NewLine2ROIRow[1],
			NewLine2ROICol[1],
			&ResultRow2,
			&ResultColumn2);

		// 拟合
		HXLDCont Line2;
		HTuple 	Line2Row11, Line2Column11, Line2Row21, Line2Column21;
		pts_to_best_line(
			&Line2,
			ResultRow2,
			ResultColumn2,
			2,
			&Line2Row11,
			&Line2Column11,
			&Line2Row21,
			&Line2Column21);

		// 拟合圆
		// 找边缘
		HXLDCont Regions3;
		HTuple ResultRow3, ResultColumn3, ArcType;
		spoke(
			result_img,
			&Regions3,
			30,
			30,
			15,
			1,
			20,
			"all",
			"max",
			NewCircleROIRow,
			NewCircleROICol,
			"inner",
			&ResultRow3,
			&ResultColumn3,
			&ArcType);

		// 拟合
		HXLDCont Circle1;
		HTuple RowCenter, ColCenter, Radius, StartPhi, EndPhi, PointOrder;
		pts_to_best_circle(
			&Circle1,
			ResultRow3,
			ResultColumn3,
			3,
			"circle",
			&RowCenter,
			&ColCenter,
			&Radius,
			&StartPhi,
			&EndPhi,
			&PointOrder);

		// 产生圆心
		HObject Cross;
		GenCrossContourXld(&Cross, RowCenter, ColCenter, 16, 0);

		// 7. 测量, 并根据像素当量, 把测量结果从图像坐标转化到物理体系空间

		// 求直线1和直线2的距离
		// 求直线2的中点, 用中点到直线1的距离作为2条直线的距离
		HTuple Line2CenterY = (Line2Row11 + Line2Row21)*0.5;
		HTuple Line2CenterX = (Line2Column11 + Line2Column21)*0.5;
		HTuple Line2ToLine1Distance;

		DistancePl(Line2CenterY, Line2CenterX,
			Line1Row11, Line1Column11,
			Line1Row21, Line1Column21,
			&Line2ToLine1Distance);

		// 把距离从图像空间转化到物理空间,单位 mm
		HTuple Res = 1.0;
		Line2ToLine1Distance = Line2ToLine1Distance * Res;
		
		// 求直线2到直线1的夹角,单位: 弧度
		HTuple Line2ToLine1Angle;
		AngleLl(
			Line2Row11,
			Line2Column11,
			Line2Row21,
			Line2Column21,
			Line1Row11,
			Line1Column11,
			Line1Row21,
			Line1Column21,
			&Line2ToLine1Angle);

		// 求直线1到直线2的夹角,单位: 弧度
		HTuple Line1ToLine2Angle;
		AngleLl(
			Line1Row11,
			Line1Column11,
			Line1Row21,
			Line1Column21,
			Line2Row11,
			Line2Column11,
			Line2Row21,
			Line2Column21,
			&Line1ToLine2Angle);

		// 圆的半径, 从图像空间转化到物理空间 单位:mm
		HTuple RadiusWord = Radius*Res;

		// 显示图像和结果
		this->m_pMHWinMain->SetLineWidth(3);
		this->m_pMHWinMain->DispObj(result_img);

		// 显示直线1
		this->m_pMHWinMain->SetColor("red");
		this->m_pMHWinMain->DispObj(Line1);

		// 显示直线2
		this->m_pMHWinMain->SetColor("green");
		this->m_pMHWinMain->DispObj(Line2);

		// 显示圆和圆心
		this->m_pMHWinMain->SetColor("blue");
		this->m_pMHWinMain->DispObj(Circle1);
		this->m_pMHWinMain->DispObj(Cross);

		// 显示测量结果
		write_message(200, 20, "直线2到直线1的距离为(mm)" + Line2ToLine1Distance, true);
		write_message(240, 20, "直线2到直线1的夹角 弧度:" + Line2ToLine1Angle, true);
		write_message(280, 20, "直线1到直线2的夹角 弧度:" + Line1ToLine2Angle, true);
		write_message(320, 20, "圆半径 mm:" + RadiusWord);

		//if (ModelID > -1) {
		//	ClearShapeModel(ModelID);
		//}
		this->hv_Model.Clear();
		
	}
	catch (HalconCpp::HException& except) {
		this->CPPExpDefaultExceptionHandler(except);
	}

}
