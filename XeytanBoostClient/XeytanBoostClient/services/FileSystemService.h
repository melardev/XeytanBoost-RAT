#pragma once
#include "../../../shared/NetLib/models/FileInfo.h"
#include <vector>
#include <boost/system/error_code.hpp>

class FileSystemService
{
public:
	FileSystemService();
	~FileSystemService();
	static const char* driveTypeIntToString(int type);
	static void retrieveRoots(std::vector<FileInfo>& roots, const boost::system::error_code& err);
	
	static void retrieveListDir(const std::wstring& path, std::vector<FileInfo>& dirEntries,
	                            const boost::system::error_code& err);
};
