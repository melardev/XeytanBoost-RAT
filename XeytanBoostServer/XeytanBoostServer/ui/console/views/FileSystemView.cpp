#include "FileSystemView.h"
#include <boost/algorithm/string/predicate.hpp>
#include <sstream>
#include <iostream>
#include <boost/foreach.hpp>


FileSystemView::FileSystemView(const boost::shared_ptr<ConsoleUiMediator>& mediator)
{
}

FileSystemView::~FileSystemView()
{
}

void FileSystemView::printBanner() const
{
}

void FileSystemView::changeDirectory(const std::wstring& path)
{
	boost::filesystem::path p(path);
	if (p.is_absolute())
	{
		currentPath = p;
		return;
	}

	if (!currentPath.empty())
		currentPath = (currentPath / path).lexically_normal();
	else
		currentPath = path;
}

bool FileSystemView::onInstruction(std::vector<std::string>& instructionTokens)
{
	if (!instructionTokens.empty())
	{
		const std::string& firstToken = instructionTokens[0];
		if (boost::iequals("cd", firstToken) && instructionTokens.size() > 1)
		{
			std::string& path = instructionTokens.at(1);
			std::wostringstream oss;
			oss << path.c_str();
			changeDirectory(oss.str());
		}
	}
	return false;
}

boost::filesystem::path FileSystemView::joinPath(const boost::filesystem::path& path) const
{
	assert(path.is_relative());
	return (currentPath / path).lexically_normal();
}

/*static*/
void FileSystemView::printListDirectory(const std::wstring& path, const std::vector<FileInfo>& files)
{
	std::wcout << "Directory scan result: " << path << std::endl;
	FileInfo fileInfo;
	BOOST_FOREACH(fileInfo, files)
	{
		// std::wcout << "\t" << boost::filesystem::basename(fileInfo.filePath) << std::endl;
		std::wcout << fileInfo.filePath;
		if ((int64_t)fileInfo.size >= 0)
			std::cout << "\t\t" << fileInfo.size << "KB";
		else
			std::cout << "\t\t<DIR>";
		std::cout << std::endl;
		// std::cout << "Last write time: " << fileInfo.lastUpdateTime << std::endl;
	}
}
