#pragma once
#include <string>
#include <map>

struct BasicSysInfo;

class SystemService
{
public:
	SystemService();
	~SystemService();
	static std::string getOSName();
	static std::wstring getUsername();
	static std::string getPcName();

	static void getBasicSystemInfo(std::string& pcName, std::wstring& userName, std::string& osName);

	static std::map<std::string, std::string> getEnvironmentVariables();
};

