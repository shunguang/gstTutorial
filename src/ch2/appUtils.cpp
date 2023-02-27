#include "appUtils.h"

template<typename T>
std::string  app::num_to_string(const T x, const std::string& msg)
{
	return msg + std::to_string(x);
}

void app::dumpLog(const std::string &fmt, ...)
{
	dumpLog(fmt.c_str());
}

void app::dumpLog(const char* fmt, ...)
{
	//todo: remove this LOG_MAX_MSG_LEN, using dynamic allocation idea
	char buffer[20148];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buffer, 2048, fmt, args);
	va_end(args);
	std::cout << buffer << std::endl;
}

void app::appExit(const char* fmt, ...)
{
	dumpLog(fmt);
	exit(-1);
}


void app::appAssert(const bool flag, const std::string& msg)
{
	if (!flag) {
		dumpLog(msg.c_str());
		exit(1);
	}
}
void  app::appAssert(const bool flag, const char* file, const int lineNum)
{
	if (!flag) {
		dumpLog("Assert fail in File [%s], line[%d]", file, lineNum);
		exit(1);
	}
}

std::string  app::ipConvertNum2Str(const uint32_t ip)
{
	char buf[16];
	unsigned char bytes[4];
	bytes[0] = ip & 0xFF;
	bytes[1] = (ip >> 8) & 0xFF;
	bytes[2] = (ip >> 16) & 0xFF;
	bytes[3] = (ip >> 24) & 0xFF;
	snprintf(buf, 16, "%d.%d.%d.%d", bytes[3], bytes[2], bytes[1], bytes[0]);
	return std::string(buf);
}

uint32_t app::ipConvertStr2Num(const std::string& ip)
{
	int a, b, c, d;

	sscanf(ip.data(), "%d.%d.%d.%d", &a, &b, &c, &d);
	assert(a <= 255);
	assert(b <= 255);
	assert(c <= 255);
	assert(d <= 255);

	uint32_t x = d;
	x |= (c << 8);
	x |= (b << 16);
	x |= (a << 24);

	return x;
}


bool app::appCreateDir(const std::string& p)
{
	std::filesystem::path p0(p);
	if (!std::filesystem::exists(p0)) {
		if (!std::filesystem::create_directories(p0)) {
			printf("appCreateDir(): cannot create root folder:%s", p0.string().c_str());
			return false;
		}
	}
	return true;
}

#if _WIN32
#include <io.h>  //_access_s
#endif
bool app::appFolderExists(const std::string& dirPath)
{
#if _WIN32
	if (_access_s(dirPath.c_str(), 0) == 0) {
#else
	if (access(dirPath.c_str(), 0) == 0) {
#endif
		struct stat status;
		stat(dirPath.c_str(), &status);
		if (status.st_mode & S_IFDIR) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}

