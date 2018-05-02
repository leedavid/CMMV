#include "stdafx.h"
#include "CExcelOperation.h"
#include "MyData.h"
#include "MyDispLay.h"

using namespace UCI;
using namespace std;

CExcelOperation::CExcelOperation()
{
}

//CExcelOperation::CExcelOperation()
//{
//	m_lpDisp = NULL;
//	m_lMaxColNum = -1;
//	m_lMaxRowNum = -1;
//	m_lStartCol = -1;
//	m_lStartRow = -1; 
//	m_pfExcelDealCB = NULL;
//}
CExcelOperation::~CExcelOperation()
{
	//this->closeExcelFile();
}

CString CExcelOperation::IndexToString(int row, int col)
{
	
	CString strResult;
	if (col > 26)
	{
		strResult.Format(_T("%c%c%d"), 'A' + (col - 1) / 26 - 1, 'A' + (col - 1) % 26, row);
	}
	else
	{
		strResult.Format(_T("%c%d"), 'A' + (col - 1) % 26, row);
	}
	return strResult;
}

int CExcelOperation::InitExcel()
{
	
	return 0;
	
	if (oExcel.m_lpDispatch == NULL) {
		oExcel.CreateDispatch(L"Excel.Application");
	}

	COleVariant covOptional(DISP_E_PARAMNOTFOUND, VT_ERROR);

	// Show Excel to the user.
	oExcel.put_Visible(TRUE);
	oExcel.put_UserControl(TRUE);

	// Add a new workbook and get the first worksheet in that book.
	oBooks = oExcel.get_Workbooks();
	oBook = oBooks.Add(covOptional);
	oSheets = oBook.get_Worksheets();
	oSheet = oSheets.get_Item(COleVariant((short)1));

	// Get a range of data.
	oRange = oSheet.get_Range(COleVariant(L"A1"), covOptional);
	oRange = oRange.get_Resize(COleVariant((short)5), COleVariant((short)5));

	COleSafeArray saRet;          // 这是一个数组
	DWORD numElements[2];
	numElements[0] = 5;
	numElements[1] = 5;

	bool isString = false;

	long index[2];
	// Create a BSTR or double safe array.
	if (isString)
		saRet.Create(VT_BSTR, 2, numElements);
	else
		saRet.Create(VT_R8, 2, numElements);

	// Fill the array with data.
	for (int iRow = 1; iRow <= 5; iRow++) {
		for (int iCol = 1; iCol <= 5; iCol++) {
			index[0] = iRow - 1;
			index[1] = iCol - 1;
			if (isString) {
				CString szTemp;
				szTemp.Format(L"%d|%d", iRow, iCol);
				BSTR bstr = szTemp.AllocSysString();
				saRet.PutElement(index, bstr);
				SysFreeString(bstr);
			}
			else {
				double d = iRow * iCol;
				saRet.PutElement(index, &d);
			}
		}
	}
	// Send the array to Excel.
	//oRange.put_Value(covOptional, COleVariant(saRet));

	oRange.put_Value(covOptional, COleVariant(L"进行"));

	COleVariant  covTrue((short)TRUE), covFalse((short)FALSE);
	oRange.ReleaseDispatch();
	oSheet.ReleaseDispatch();
	oSheets.ReleaseDispatch();
	oBook.ReleaseDispatch();
	oBooks.ReleaseDispatch();
	oExcel.Quit();
	oExcel.ReleaseDispatch();

	// 最后两行代码顺序不要反了，否则不能彻底关闭Excel进程，这是关键 
	// http://www.voidcn.com/article/p-njswywsv-bas.html

	// 分别向每个单元格添加数据
	// Excel2000 用函数SetValue()即可, Excel2003用函数SetValue2()

	return 0;
}

BOOL CExcelOperation::exportProductExcel(int iStartPrid)
{
	if (oExcel.m_lpDispatch == NULL) {
		oExcel.CreateDispatch(L"Excel.Application");
	}

	COleVariant covOptional(DISP_E_PARAMNOTFOUND, VT_ERROR);
	COleVariant covTrue((short)TRUE), covFalse((short)FALSE);

	// Show Excel to the user.
	oExcel.put_Visible(TRUE);
	oExcel.put_UserControl(TRUE);

	// Add a new workbook and get the first worksheet in that book.
	oBooks = oExcel.get_Workbooks();
	oBook = oBooks.Add(covOptional);
	oSheets = oBook.get_Worksheets();
	oSheet = oSheets.get_Item(COleVariant((short)1));
	CFont0 font;

	VARIANT varUnit;
	VariantInit(&varUnit);
	varUnit.vt = VT_I4;
	

	// Get a range of data.
	oRange = oSheet.get_Range(COleVariant(L"B2"), COleVariant(L"J2"));
	oRange.Merge(covOptional);

	
	oRange.put_Value(covOptional, COleVariant(L"江苏常美医疗器械有限公司"));	
	font = oRange.get_Font();
	font.put_Name(COleVariant(_T("黑体")));
	varUnit.lVal = 16;
	font.put_Size(varUnit);

	interior = oRange.get_Interior();
	interior.put_Color(_variant_t(RGB(189, 215, 238)));


	varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
	oRange.put_HorizontalAlignment(varUnit);

	oRange = oRange.get_Resize(COleVariant((short)1), COleVariant((short)1));

	int IStartRow = 7;   // 开始的行
	int IStartColumn = 2; // 空一格
	int iStartNumber = UCI::OPTIONS[L"ProdutctExportStartNo"];     // 
	int prID = UCI::OPTIONS[L"项目编号"];

	try {
		std::string dbname = CT2A(UCI::strMoudlePath).m_psz;
		dbname += "\\CMdata.db";
		
		SQLite::Database db(dbname);   // 只读

		SQLite::Statement query(db, "SELECT * FROM PRODUCT_RESULT WHERE ID >= ? AND ProjectID = ?");
		query.bind(1, iStartNumber);
		query.bind(2, prID);

		string batchNo;

		int row = IStartRow;
		int col = IStartColumn;

		while (query.executeStep()) {   // 发现了产品

			int productID = query.getColumn(0);			
			int result = query.getColumn(4);
			string stDate = query.getColumn(5);

			if (row == IStartRow) {                          // 写表头， 批号等
				batchNo = query.getColumn(3).getString();

				int NROW = 4;
				int NCOL = 2;				

				// 产品名称
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), covOptional);
				oRange.put_Value2(COleVariant(L"产品名称："));
				varUnit.lVal = -4131;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));


				UCI::ST_PROJECT pr(prID);
				UCI::sDB.ProjectFillByID(&pr);				

				NCOL++;
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), COleVariant(IndexToString(NROW, NCOL+1)));
				oRange.Merge(covOptional);
				oRange.put_Value2(COleVariant(Utf8ToTChar(pr.ProjectName)));
				varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));

				NCOL += 2;
				// 批号
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), covOptional);
				oRange.put_Value2(COleVariant(L"生产批号："));
				varUnit.lVal = -4131;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));

				NCOL++;
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), COleVariant(IndexToString(NROW, NCOL + 1)));
				oRange.Merge(covOptional);
				oRange.put_Value2(COleVariant(CA2T(batchNo.c_str()).m_psz));
				varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));


				// 检验员
				NCOL+=2;
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), covOptional);
				oRange.put_Value2(COleVariant(L"质检人员："));
				varUnit.lVal = -4131;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));

				NCOL++;
				oRange = oSheet.get_Range(COleVariant(IndexToString(NROW, NCOL)), COleVariant(IndexToString(NROW, NCOL + 1)));
				oRange.Merge(covOptional);
				CString logname = UCI::OPTIONS[L"UserLogName"];
				oRange.put_Value2(COleVariant(logname));
				varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(210, 210, 210)));

			}

			CString csResult = L"OK";
			if (result == 0) {
				csResult = L"NG";
			}

			SQLite::Statement query2(db, "SELECT * FROM ELEMENT_RESULT WHERE ProResID = ? ORDER BY EleIndex ASC");
			query2.bind(1, productID);


			if (row == IStartRow) {  // 同时写入标题, 与内容
				oRange = oSheet.get_Range(COleVariant(IndexToString(row-1, col)), covOptional);
				oRange.put_Value2(COleVariant(L"No."));
				varUnit.lVal = -4152;// -4108水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);
				font = oRange.get_Font();
				font.put_Bold(covTrue);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(180, 198, 231)));
			}

			// 写入行号
			oRange = oSheet.get_Range(COleVariant(IndexToString(row, col)), covOptional);			
			oRange.put_Value2(COleVariant((long long)(row- IStartRow+1)));
			col++;

			while (query2.executeStep()) {   // 发现了测量元素

				double dRes = query2.getColumn(4);

				double dStand = query2.getColumn(5);
				double dUpper = query2.getColumn(6);
				double dLower = query2.getColumn(7);

				double dlimit_up = dStand + dUpper;
				double dlimit_low = dStand + dLower;

				if (row == IStartRow) {  // 同时写入标题, 与内容
					oRange = oSheet.get_Range(COleVariant(IndexToString(row - 1, col)), covOptional);
					CString stName = Utf8ToTChar(query2.getColumn(3).getString());
					oRange.put_Value2(COleVariant(stName));

					font = oRange.get_Font();
					font.put_Bold(covTrue);						

					interior = oRange.get_Interior();
					interior.put_Color(_variant_t(RGB(180, 198, 231)));
					//interior.put_ColorIndex(COleVariant((long)3));

					//varUnit.lVal = 5;
					//font.put_Background(varUnit);	

					varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
					oRange.put_HorizontalAlignment(varUnit);
				}
				
				oRange = oSheet.get_Range(COleVariant(IndexToString(row, col)), covOptional);
				oRange.put_Value2(COleVariant(dRes));

				// 设置一下颜色
				if (dRes > dlimit_up) {
					interior = oRange.get_Interior();
					interior.put_Color(_variant_t(RGB(255, 150, 150)));
				}
				else if (dRes < dlimit_low) {
					interior = oRange.get_Interior();
					interior.put_Color(_variant_t(RGB(150, 150, 255)));
				}
				else {
					interior = oRange.get_Interior();
					interior.put_Color(_variant_t(RGB(255, 255, 255)));
				}
				

				col++;
			}// 发现了测量元素

			if (row == IStartRow) {  // 同时写入标题, 与内容
				oRange = oSheet.get_Range(COleVariant(IndexToString(row - 1, col+2)), covOptional);
				oRange.put_Value2(COleVariant(L"测量日期"));
				varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
				oRange.put_HorizontalAlignment(varUnit);
				font = oRange.get_Font();
				font.put_Bold(covTrue);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(180, 198, 231)));

				oRange.put_ColumnWidth(_variant_t((long)16));

				oRange = oSheet.get_Range(COleVariant(IndexToString(row - 1, col+1)), covOptional);
				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(180, 198, 231)));

				oRange = oSheet.get_Range(COleVariant(IndexToString(row - 1, col)), covOptional);
				oRange.put_Value2(COleVariant(L"测量结果"));
				font = oRange.get_Font();
				font.put_Bold(covTrue);

				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(180, 198, 231)));

			}

			// 写入测量时间


			oRange = oSheet.get_Range(COleVariant(IndexToString(row, col+2)), covOptional);	
			oRange.put_Value2(COleVariant(CA2T(stDate.c_str()).m_psz));
			
			// 写入结果 		
			oRange = oSheet.get_Range(COleVariant(IndexToString(row, col)), covOptional);
			oRange.put_Value2(COleVariant(csResult));
			varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
			oRange.put_HorizontalAlignment(varUnit);

			if (result == 0) {
				//font = oRange.get_Font();
				//font.put_ColorIndex(COleVariant((long)3));
				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(255, 0, 0)));
			}
			else {
				interior = oRange.get_Interior();
				interior.put_Color(_variant_t(RGB(91, 155, 213)));
			}

			col = IStartColumn;
			row++;		
		} // 发现了产品

		//IndexToString(row, col))
		row += 1;
		oShapes = oSheet.get_Shapes();
		oRange = oSheet.get_Range(COleVariant(IndexToString(row, 2)), COleVariant(IndexToString(row+5, 10)));
		oRange.Merge(covOptional);	

		oRange.put_RowHeight(_variant_t((long)40));
		//oRange.put_(_variant_t((long)16));

		double picHeight = oRange.get_Height().dblVal;
		double picWidth = picHeight*(2592.0f / 1944.0f);
		
		CString strPic = UCI::pDis->GetMatchPathFile(prID);    // 得到匹配文件
		oShapes.AddPicture2(
			strPic,
			false,
			true,
			(float)oRange.get_Left().dblVal+100,
			(float)oRange.get_Top().dblVal,
			(float)picWidth,
			(float)picHeight,
			true
		);

		row += 7;
		// 写入最后一行
		oRange = oSheet.get_Range(COleVariant(IndexToString(row,  2)), COleVariant(IndexToString(row, 10)));
		oRange.Merge(covOptional);		

		oRange.put_Value(covOptional, COleVariant(L"江苏常美 版权所有"));
		font = oRange.get_Font();
		font.put_Name(COleVariant(_T("黑体")));
		varUnit.lVal = 16;
		font.put_Size(varUnit);
		varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
		oRange.put_HorizontalAlignment(varUnit);

		interior = oRange.get_Interior();
		interior.put_Color(_variant_t(RGB(189, 215, 238)));

		row += 1;
		CTime time;
		time = CTime::GetCurrentTime();
		CString date = time.Format(L"%Y-%m-%d %H:%M:%S %W-%A");

		oRange = oSheet.get_Range(COleVariant(IndexToString(row, 2)), COleVariant(IndexToString(row, 10)));
		oRange.Merge(covOptional);

		oRange.put_Value(covOptional, COleVariant(date + L"  by LGL"));
		font = oRange.get_Font();
		font.put_Name(COleVariant(_T("黑体")));
		varUnit.lVal = 8;
		font.put_Size(varUnit);
		varUnit.lVal = -4108;//水平：-4131靠左；-4152靠右。垂直：-4160靠上；-4107靠下。
		oRange.put_HorizontalAlignment(varUnit);

		interior = oRange.get_Interior();
		interior.put_Color(_variant_t(RGB(172, 185, 202)));
	}
	catch (std::exception& e) {
		std::string err = "SQLite exception: ";
		err += e.what();
		MyLog(err.c_str());
	}



	//COleVariant  covTrue((short)TRUE), covFalse((short)FALSE);
	oRange.ReleaseDispatch();
	oSheet.ReleaseDispatch();
	oSheets.ReleaseDispatch();
	oBook.ReleaseDispatch();
	oBooks.ReleaseDispatch();
	oExcel.Quit();
	oExcel.ReleaseDispatch();
	
	return TRUE;
}

// https://support.microsoft.com/en-us/help/308407/how-to-automate-excel-from-mfc-and-visual-c-2005-or-visual-c--net-to-f

//int CExcelOperation::openExcelFile(CString strFileName)
//{
//	int iRet = -1; 
//	m_strErrMsg = "No Error";
//	CRange rangeUsedCells;
//	CRange rangeTempCells;
//	COleVariant covResult;
//	COleVariant covOption((long)DISP_E_PARAMNOTFOUND, VT_ERROR);
//	do 
//	{
//		if (strFileName.IsEmpty())
//		{
//			m_strErrMsg = "Error, strFileName is empty!";
//			break;
//		}
//
//		if (!m_appExcelServer.CreateDispatch(L"Excel.Application",NULL)) {
//			m_strErrMsg = "Error, open Excel Application Server fail!";
//			break;
//		}
//
//		m_wbsExcelBooks = m_appExcelServer.get_Workbooks();
//		// m_wbsExcelBooks.AttachDispatch(m_appExcelServer.get_Workbooks());
//        
//		m_lpDisp = m_wbsExcelBooks.Open(strFileName,covOption, covOption, covOption, covOption, covOption, 
//			                            covOption, covOption, covOption, covOption, covOption, covOption, 
//							            covOption, covOption, covOption);
//
//		if (m_lpDisp == NULL)
//		{
//			break;
//		}
//		m_wbExcelBook.AttachDispatch(m_lpDisp);
//		m_lpDisp = m_wbExcelBook.get_ActiveSheet();
//		m_wsSheet.AttachDispatch(m_lpDisp);
//		m_rangeBasicCells.AttachDispatch(m_wsSheet.get_Cells());
//
//		rangeUsedCells.AttachDispatch(m_wsSheet.get_UsedRange());
//		rangeTempCells.AttachDispatch(rangeUsedCells.get_Columns());
//		m_lMaxColNum = rangeTempCells.get_Count();
//		rangeTempCells.AttachDispatch(rangeUsedCells.get_Rows());
//		m_lMaxRowNum = rangeTempCells.get_Count();
//		m_lStartCol = rangeUsedCells.get_Column();
//		m_lStartRow = rangeUsedCells.get_Row();
//
//		rangeUsedCells.ReleaseDispatch();
//		rangeTempCells.ReleaseDispatch();
//
//		iRet = 0;
//	} while(0);
//
//	// 
//	CString strExcelVersion = m_appExcelServer.get_Version();
//	UCI::MyLog(strExcelVersion);
//
//	return iRet;
//}
//
//int CExcelOperation::closeExcelFile()
//{
//	m_rangeBasicCells.ReleaseDispatch();
//	m_wsSheet.ReleaseDispatch();
//	m_wbExcelBook.ReleaseDispatch();
//	//m_wbsExcelBooks.Close();
//	m_wbsExcelBooks.ReleaseDispatch();
//	m_appExcelServer.Quit();
//	m_appExcelServer.ReleaseDispatch();
//	return 0;
//}
//
//int CExcelOperation::startDealExcel()
//{
//	int iRet = -1; 
//	m_strErrMsg = "No Error";
//
//	do 
//	{
//		if (m_pfExcelDealCB == NULL)
//		{
//			m_strErrMsg = "Error: CallBack is NULL!";
//			break;
//		}
//		m_pfExcelDealCB(this);
//		//AfxBeginThread((AFX_THREADPROC)m_pfExcelDealCB,this,THREAD_PRIORITY_NORMAL);
//
//		iRet = 0;
//	} while(0);
//	return iRet;
//}

////////////////////////////////////////////////////////////////////////  
///Function:    IndexToString  
///Description: 得到的单元格在EXCEL中的定位名称字符串  
///Input:       int row 单元格所在行  
///             int col 单元格所在列  
///Return:      CString 单元格在EXCEL中的定位名称字符串  
////////////////////////////////////////////////////////////////////////  



//CString CExcelOperation::GetRowHeight(int row)
//{
//	int col = 1;
//	m_rangeBasicCells = sheet.GetRange(COleVariant(IndexToString(row, col)), COleVariant(IndexToString(row, col)));
//	VARIANT height = m_rangeBasicCells.GetRowHeight();
//	CString strheight;
//	strheight.Format(CString((LPCSTR)(_bstr_t)(_variant_t)height));
//	return strheight;
//
//
//	m_wsSheet.get_Range()
//}
