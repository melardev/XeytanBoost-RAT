#include "FileSystemService.h"
#include <iostream>
#include <boost/filesystem/operations.hpp>
#include "../appdefs.h"
#if defined(XEYTAN_WINDOWS)
#include <Windows.h>
#endif

FileSystemService::FileSystemService()
{
}

FileSystemService::~FileSystemService()
{
}


const char* FileSystemService::driveTypeIntToString(int type)
{
	switch (type)
	{
	case DRIVE_FIXED:
		return "fixed";
	case DRIVE_CDROM:
		return "cdrom";
	case DRIVE_REMOVABLE:
		return "removable";
	case DRIVE_UNKNOWN:
		return "unknown";
	case DRIVE_NO_ROOT_DIR:
		return "unmounted";
	case DRIVE_REMOTE:
		return "remote";
	case DRIVE_RAMDISK:
		return "ramdisk";
	default:
		return "?";
	}
}

std::wstring string2Wstring(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

void FileSystemService::retrieveRoots(std::vector<FileInfo>& roots, const boost::system::error_code& err)
{
#if true || defined(XEYTAN_WINDOWS)

	int64_t num_bytes;

	char driveLetters[255];

	char* currentLetter = driveLetters;

	char opts[20];

	char fsType[MAX_PATH + 1] = {0};
	DWORD pflags = 0;

	num_bytes = GetLogicalDriveStringsA(254, currentLetter);

	if (num_bytes == 0)
	{
		return;
	}

	while (*currentLetter != 0)
	{
		FileInfo info;

		opts[0] = 0;
		fsType[0] = 0;


		int driveType = GetDriveTypeA(currentLetter);

		BOOL success = GetVolumeInformationA(
			currentLetter, NULL, _ARRAYSIZE(currentLetter),
			NULL, NULL, &pflags, fsType,
			_ARRAYSIZE(fsType));

		if (success)
		{
			if (pflags & FILE_READ_ONLY_VOLUME)
				strcat_s(opts, _countof(opts), "ro");
			else
				strcat_s(opts, _countof(opts), "rw");
			if (pflags & FILE_VOLUME_IS_COMPRESSED)
				strcat_s(opts, _countof(opts), ",compressed");
		}
		else
		{
			// We might get here in case of a floppy hard drive, in
			// which case the error is (21, "device not ready").
			// Let's pretend it didn't happen as we already have
			// the drive name and type ('removable').
			strcat_s(opts, _countof(opts), "");
		}

		if (strlen(opts) > 0)
			strcat_s(opts, _countof(opts), ",");

		strcat_s(opts, _countof(opts), driveTypeIntToString(driveType));

		info.filePath = string2Wstring(std::string(currentLetter) + " - " + opts);
		roots.push_back(info);

		currentLetter = strchr(currentLetter, 0) + 1;
	}

#elif defined(XEYTAN_LINUX)

#endif
}

void FileSystemService::retrieveListDir(const std::wstring& path, std::vector<FileInfo>& dirEntries,
                                        const boost::system::error_code& err)
{
	try
	{
		if (boost::filesystem::exists(path))
		{
			if (boost::filesystem::is_directory(path))
			{
				for (boost::filesystem::directory_entry& file : boost::filesystem::directory_iterator(path))
				{
					FileInfo f = {};
					f.filePath = file.path().wstring();
					if (boost::filesystem::is_regular_file(file))
					{
						f.lastUpdateTime = boost::filesystem::last_write_time(file);
						f.size = boost::filesystem::file_size(file);
					}
					else
					{
						f.size = -1;
					}


					dirEntries.push_back(f);
				}
			}
			else
			{
				// TODO: Error handling
			}
		}
		else
		{
			std::wcout << path << " does not exist" << std::endl;
		}
	}
	catch (const boost::filesystem::filesystem_error& ex)
	{
		std::cout << ex.what() << std::endl;
	}
}
