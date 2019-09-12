#include "NetServerService.h"
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include "NetClientService.h"
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include "../concurrent/events/AppEvent.h"
#include "../concurrent/channels/double/NetAppEventDoubleQueueThreadChannel.h"
#include "../models/Client.h"
#include <boost/uuid/uuid_generators.hpp>
#include <boost/thread.hpp>
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "../concurrent/events/ClientAppEvent.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include "../../../shared/NetLib/packets/PacketShell.h"
#include "../../../shared/NetLib/packets/PacketInformation.h"


NetServerService::NetServerService(const boost::shared_ptr<boost::asio::io_service>& ios)
	: acceptor(*ios),
	  ioService(ios)
{
}


NetServerService::~NetServerService()
{
}

boost::shared_ptr<NetServerService> NetServerService::createInstance()
{
	const boost::shared_ptr<boost::asio::io_service> ios = boost::make_shared<boost::asio::io_service>();
	return boost::shared_ptr<NetServerService>(
		new NetServerService(ios));
}

void NetServerService::runAsync()
{
	running = true;
	boost::asio::ip::tcp::resolver resolver(*ioService);
	const boost::asio::ip::tcp::resolver::query query(
		"127.0.0.1",
		boost::lexical_cast<std::string>(3002)
		// or std::to_string(3002) using standard cpp
	);

	const boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor.bind(endpoint);
	acceptor.listen();

	boost::thread packetSenderThread(boost::bind(&NetServerService::sendPacketsProc, shared_from_this()));
	boost::thread listenAppEventsThread(boost::bind(&NetServerService::receiveAppEvent, shared_from_this()));

	acceptOneClientAsync();

	boost::thread ioThread(boost::bind(&boost::asio::io_service::run, ioService.get()));
}

void NetServerService::setChannel(const boost::shared_ptr<NetAppEventDoubleQueueThreadChannel>& netChannel)
{
	this->netChannel = netChannel;
}

boost::shared_ptr<Client> NetServerService::getClient(unsigned long long clientId)
{
	boost::shared_lock<boost::shared_mutex> lock(clientsMutex);
	const auto clientIterator = clients.find(clientId);
	if (clientIterator != clients.end())
	{
		const boost::shared_ptr<NetClientService> clientService = clientIterator->second;
		return clientService->client;
	}

	return nullptr;
}

void NetServerService::acceptOneClientAsync()
{
	boost::uuids::uuid clientUuid = boost::uuids::random_generator()();
	const boost::shared_ptr<Client> client = boost::make_shared<Client>(clientUuid);

	const boost::shared_ptr<NetClientService> netClientService(
		new NetClientService(shared_from_this(), ioService, client));

	acceptor.async_accept(netClientService->getSocket(),
	                      boost::bind(&NetServerService::onClientAcceptResult, shared_from_this(), netClientService,
	                                  boost::asio::placeholders::error));
}

void NetServerService::onClientAcceptResult(boost::shared_ptr<NetClientService> netClientService,
                                            const boost::system::error_code& error)
{
	if (error)
	{
	}
	else
	{
		const unsigned long long clientId = netClientService
		                                    ->clientSocket.native_handle().operator unsigned long long();
		netClientService->client->clientId = clientId;
		boost::lock_guard<boost::shared_mutex> lk(clientsMutex);
		clients.insert({clientId, netClientService});
	}
	netClientService->interactAsync();
	acceptOneClientAsync();
}


boost::shared_ptr<NetClientService> NetServerService::getNetClientService(unsigned long long clientId)
{
	boost::shared_lock_guard<boost::shared_mutex> lock(clientsMutex);
	const auto clientIterator = clients.find(clientId);

	if (clientIterator != clients.end())
	{
		return (*clientIterator).second;
	}

	return nullptr;
}

void NetServerService::onPacketReceived(const boost::shared_ptr<Client>& client,
                                        const boost::shared_ptr<Packet>& packet) const
{
	if (packet->packetType == PacketType::Presentation)
	{
		const boost::shared_ptr<PacketPresentation> packetPresentation
			= boost::dynamic_pointer_cast<PacketPresentation>(packet);

		client->pcName = packetPresentation->sysInfo.pcName;
		client->username = packetPresentation->sysInfo.username;
		client->osName = packetPresentation->sysInfo.osName;
	}

	const boost::shared_ptr<AppEvent> appEvent
		= boost::make_shared<ClientAppEvent>(Subject::PacketReceived, client, packet);
	netChannel->submitToApp(appEvent);
}

void NetServerService::sendPacketsProc()
{
	while (running)
	{
		/* I actually don't need this synchronization code.
		 * Even if I try to send a packet before the previous one has finished from being sent
		 * Asio will send first the packets queued first
		{
			boost::unique_lock<boost::mutex> lock(isSendingPacketMutex);
			if (isSendingPacket)
				isSendingPacketCv.wait(lock, [&]() { return isSendingPacket; });

			isSendingPacket = true;
		}
		*/
		const boost::shared_ptr<Packet> packet = packetToSendChannel.takeSync();
		boost::shared_ptr<NetClientService> netClient = getNetClientService(packet->clientId);
		if (netClient != nullptr)
			netClient->sendPacket(packet);
	}
}

void NetServerService::onPacketSent()
{
	// TODO can I let the code as such and skip mutex here?
	isSendingPacket = false;
	isSendingPacketCv.notify_one();
}

std::vector<boost::shared_ptr<Client>> NetServerService::getClients()
{
	std::vector<boost::shared_ptr<Client>> onlineClients;
	{
		// For reading we use shared_lock
		boost::shared_lock_guard<boost::shared_mutex> lk(clientsMutex);
		for (auto&& clientPair : clients)
		{
			onlineClients.push_back(clientPair.second->client);
		}
	}

	return onlineClients;
}

void NetServerService::receiveAppEvent()
{
	running = true;
	while (running)
	{
		const boost::shared_ptr<AppEvent> appEvent = netChannel->takeFromApp();

		const Target target = appEvent->target;
		const Subject subject = appEvent->subject;
		boost::shared_ptr<void> object = appEvent->object;
		const Action action = appEvent->action;

		if (target == Target::Server)
		{
			std::vector<boost::shared_ptr<Client>> onlineClients = getClients();
			if (subject == Subject::Connection)
			{
				if (action == Action::ListAvailable)
				{
					const boost::shared_ptr<void> clientsLive =
						boost::make_shared<std::vector<boost::shared_ptr<Client>>>(getClients());

					appEvent->object = clientsLive;
					netChannel->submitToApp(appEvent);
				}
			}
		}
		else if (target == Target::Client)
		{
			const boost::shared_ptr<ClientAppEvent> clientEvent = boost::dynamic_pointer_cast<ClientAppEvent>(appEvent);
			const boost::shared_ptr<NetClientService> netClientService = getNetClientService(clientEvent->clientId);
			const unsigned long long clientId = clientEvent->clientId;
			if (subject == Subject::Process)
			{
				if (action == Action::Get || action == Action::Start)
				{
					const boost::shared_ptr<Packet> packet = boost::make_shared<PacketProcess>();
					queuePacket(packet, clientId);
				}
			}
			else if (subject == Subject::Information)
			{
				if (action == Action::Fetch)
				{
					const boost::shared_ptr<Packet> packet = boost::make_shared<PacketInformation>();
					queuePacket(packet, clientId);
				}
			}
			else if (subject == Subject::FileSystem)
			{
				if (action == Action::Get)
				{
					const std::wstring* path = (std::wstring*)object.get();
					const boost::shared_ptr<PacketFileSystem> packet =
						boost::make_shared<PacketFileSystem>(path ? *path : std::wstring());
					queuePacket(packet, clientId);
				}
			}
			else if (subject == Subject::Shell)
			{
				if (action == Action::Start)
				{
					boost::shared_ptr<PacketShell> packet = boost::make_shared<PacketShell>();
					packet->action = PacketShell::PacketAction::Start;
					queuePacket(packet, clientId);
				}
				else if (action == Action::Push)
				{
					const std::string* command = (std::string*)object.get();
					const boost::shared_ptr<PacketShell> packet =
						boost::make_shared<PacketShell>(command ? *command : std::string());
					queuePacket(packet, clientId);
				}
			}
			else if (subject == Subject::Process)
			{
				if (action == Action::Fetch)
				{
					const boost::shared_ptr<PacketProcess> packet = boost::make_shared<PacketProcess>();
					queuePacket(packet, clientId);
				}
			}
			else
			{
				std::cout << "Unhandled application event @NetServerService::receiveAppEvent()" << std::endl;
			}
		}
	}
}

void NetServerService::queuePacket(const boost::shared_ptr<Packet>& packet, unsigned long long clientId)
{
	packet->clientId = clientId;
	this->packetToSendChannel.submitAsync(packet);
}
