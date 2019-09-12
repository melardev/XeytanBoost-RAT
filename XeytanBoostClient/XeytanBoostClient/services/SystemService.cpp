#include "SystemService.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include <boost/process.hpp>
#include "../appdefs.h"
#include <string>

#if defined(XEYTAN_WINDOWS)
#include <Windows.h>
#endif


SystemService::SystemService()
{
}


SystemService::~SystemService()
{
}

std::string SystemService::getOSName()
{
#ifdef XEYTAN_WINDOWS
	return "Windows";
#endif
	return "Unknown";
}

std::wstring SystemService::getUsername()
{
#if defined(XEYTAN_WINDOWS)

	// wchar_t pcNameBuffer[255];
	// size_t pcNameLen = 255;
	// _wgetenv_s(&pcNameLen, pcNameBuffer, L"COMPUTERNAME");

	wchar_t userNameBuffer[255];
	DWORD userNameLen = 255;
	GetUserNameW(userNameBuffer, &userNameLen);
	return userNameBuffer;
#endif
}

std::string SystemService::getPcName()
{
	boost::process::native_environment processEnv = boost::this_process::environment();
	// std::string computerName = processEnv["COMPUTERNAME"].to_string();
	// std::wstring computerNameW = boost::locale::conv::to_utf<wchar_t>(computerName, "utf-8");

	// gethostname is cross platform
	char computerName[255];
	gethostname(computerName, 255);
	return computerName;
}


void SystemService::getBasicSystemInfo(std::string& pcName, std::wstring& userName, std::string& osName)
{
	boost::process::native_environment processEnv = boost::this_process::environment();
	// std::string computerName = processEnv["COMPUTERNAME"].to_string();
	// std::wstring computerNameW = boost::locale::conv::to_utf<wchar_t>(computerName, "utf-8");

	// gethostname is cross platform
	char computerName[255];
	gethostname(computerName, 255);
	pcName.append(computerName);

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)

	// wchar_t pcNameBuffer[255];
	// size_t pcNameLen = 255;
	// _wgetenv_s(&pcNameLen, pcNameBuffer, L"COMPUTERNAME");

	wchar_t userNameBuffer[255];
	DWORD userNameLen = 255;
	GetUserNameW(userNameBuffer, &userNameLen);
	userName.append(userNameBuffer);

	osName.append("Windows");
#elif defined(linux)
	getpwuid();
	// getenv("USERNAME"); Linux


#elif defined(MAC_OSX)
	char* username = 0;
	username = getenv("USER")
		if (username != NULL)
			userName.append(username);
#endif
}

std::map<std::string, std::string> SystemService::getEnvironmentVariables()
{
	// TODO: Migrate implementation to unicode
	std::map<std::string, std::string> envVariables;

	boost::process::native_environment processEnv = boost::this_process::environment();
	// Unicode:
	// boost::process::wnative_environment wprocessEnv = boost::this_process::wenvironment();
	
	for (unsigned int i = 0; i < processEnv.size(); i++)
	{
		boost::process::native_environment::entry_type envVariable = *(processEnv.begin() + i);
		const std::string key = envVariable.get_name();
		const std::string value = envVariable.to_string();

		// The below is the same as envVariables.insert(std::make_pair( key, value ));
		envVariables.insert({key, value});
	}

	return envVariables;
}
