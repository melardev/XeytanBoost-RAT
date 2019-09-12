#pragma once
#include <boost/smart_ptr/shared_ptr.hpp>
#include "IClientView.h"
#include <boost/filesystem.hpp>
#include "../../../../../shared/NetLib/models/FileInfo.h"
#include <vector>

class ConsoleUiMediator;

class FileSystemView : public IClientView
{
public:
	FileSystemView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	~FileSystemView();

	virtual void printBanner() const override;
	void changeDirectory(const std::wstring& path);
	virtual bool onInstruction(std::vector<std::string>& instructionTokens) override;
	boost::filesystem::path joinPath(const boost::filesystem::path& path) const;
	static void printListDirectory(const std::wstring& path, const std::vector<FileInfo>& files);

	boost::filesystem::path currentPath;
};
