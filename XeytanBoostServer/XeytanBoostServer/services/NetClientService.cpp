#include "NetClientService.h"
#include "NetServerService.h"


NetClientService::NetClientService(
	const boost::shared_ptr<NetServerService>& server,
	const boost::shared_ptr<boost::asio::io_service>& ioService,
	const boost::shared_ptr<Client>& client)
	: BaseNetClientService(ioService), server(server), client(client)
{
}


NetClientService::~NetClientService()
{
}

void NetClientService::onPacketReceived(const boost::shared_ptr<Packet>& packet)
{
	// Success
	// 1. Notify the server instance
	// server->onDataReceived(id, packet);

	server->onPacketReceived(client, packet);
}

boost::shared_ptr<Client> NetClientService::getClient()
{
	return client;
}
