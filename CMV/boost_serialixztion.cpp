
#include "stdafx.h"
#include "MyData.h"





	//序列化，输出到字符串
//std::ostringstream ossOut(ostringstream::out);   //把对象写到字符串输出流中
//boost::archive::text_oarchive oa(ossOut);
//TestClass objTestClass;
//oa << objTestClass;
//string strTrans = ossOut.str();
//……
////反序列化，从字符串输入
//istringstream ossIn(strTrans);      //从字符串输入流中读入数据
//boost::archive::text_iarchive ia(ossIn);
//TestClass newObjTestClass;
//ia >> newObjTestClass;