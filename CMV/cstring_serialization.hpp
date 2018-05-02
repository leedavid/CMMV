
#ifndef MFC_CSTRING_SERIALiZATION_INCLUDE
#define MFC_CSTRING_SERIALiZATION_INCLUDE

/*=============================================================================
Copyright (C) Akira Takahashi 2011
Copyright (c) John Paul Pirau 2008

Distributed under the Boost Software License, Version 1.0. (See accompanying
file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

UTF16toUTF8() and UTF8toUTF16() function published here:
http://www.codeproject.com/KB/string/utfConvert.aspx
==============================================================================*/

#include <afx.h>
#include <string>
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/string.hpp>

//namespace mfc_code_conv {
//
//	CStringA UTF16toUTF8(const CStringW& utf16)
//	{
//		CStringA utf8;
//		int len = WideCharToMultiByte(CP_UTF8, 0, utf16, -1, NULL, 0, 0, 0);
//		if (len>1)
//		{
//			char *ptr = utf8.GetBuffer(len - 1);
//			if (ptr) WideCharToMultiByte(CP_UTF8, 0, utf16, -1, ptr, len, 0, 0);
//			utf8.ReleaseBuffer();
//		}
//		return utf8;
//	}
//
//	CStringW UTF8toUTF16(const CStringA& utf8)
//	{
//		CStringW utf16;
//		int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
//		if (len>1)
//		{
//			wchar_t *ptr = utf16.GetBuffer(len - 1);
//			if (ptr) MultiByteToWideChar(CP_UTF8, 0, utf8, -1, ptr, len);
//			utf16.ReleaseBuffer();
//		}
//		return utf16;
//	}
//
//} // namespace mfc_code_conv

namespace boost {
	namespace serialization {

		template <class Archive>
		inline void save(Archive& ar, const CString& s, const unsigned int version)
		{
			//static_cast<void>(version);

			//const CStringA utf8 = ::mfc_code_conv::UTF16toUTF8(s);
			//const std::string ss(utf8);
			//ar & boost::serialization::make_nvp("String", ss);
			std::wstring ss(s);
			ar & ss;

		}

		template<class Archive>
		inline void load(Archive& ar, CString& s, const unsigned int version)
		{
			//static_cast<void>(version);

			//std::string ss;
			//ar & boost::serialization::make_nvp("String", ss);
			//const CStringA utf8 = ss.c_str();

			//s = ::mfc_code_conv::UTF8toUTF16(utf8);
			std::wstring ss;
			ar & ss;
			s = ss.c_str();
		}

		template <class Archive>
		inline void serialize(Archive& ar, CString& s, const unsigned int version)
		{
			boost::serialization::split_free(ar, s, version);
		}

	}
} // namespace boost::serialization


#endif // MFC_CSTRING_SERIALiZATION_INCLUDE