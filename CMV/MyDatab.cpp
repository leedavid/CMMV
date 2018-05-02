#include "stdafx.h"

#include "MyData.h"
#include "SQLiteCpp/SQLiteCpp.h"
#include "SQLiteCpp/VariadicBind.h"

#ifdef SQLITECPP_ENABLE_ASSERT_HANDLER
namespace SQLite
{
	/// definition of the assertion handler enabled when SQLITECPP_ENABLE_ASSERT_HANDLER is defined in the project (CMakeList.txt)
	void assertion_failed(const char* apFile, const long apLine, const char* apFunc, const char* apExpr, const char* apMsg)
	{
		// Print a message to the standard error output stream, and abort the program.
		std::cerr << apFile << ":" << apLine << ":" << " error: assertion failed (" << apExpr << ") in " << apFunc << "() with message \"" << apMsg << "\"\n";
		std::abort();
	}
}
#endif



namespace UCI
{
	
}