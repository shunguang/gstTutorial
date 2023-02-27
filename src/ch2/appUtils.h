#ifndef _APP_UTILS_H_
#define _APP_UTILS_H_

#include "appDefs.h"
namespace app {
	template<typename T>
	std::string num_to_string(const T x, const std::string& msg = "");

	void  dumpLog(const char* x, ...);
	void  dumpLog(const  std::string& x, ...);

	void appExit(const char* fmt, ...);

	void  appAssert(const bool flag, const std::string& msg);
	void  appAssert(const bool flag, const char* file, const int lineNum);

	uint32_t ipConvertStr2Num(const std::string& ip);
	std::string  ipConvertNum2Str(const uint32_t ip);
	bool appCreateDir(const std::string& p);
	bool appFolderExists(const std::string& dirPath);

}
#endif
