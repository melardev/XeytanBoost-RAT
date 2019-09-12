#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"

class NetServerService;
class Client;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService() = default;
	NetClientService(const boost::shared_ptr<NetServerService>& server,
	                 const boost::shared_ptr<boost::asio::io_service>& iosService,
	                 const boost::shared_ptr<Client>& client);
	~NetClientService();

	virtual void onPacketReceived(const boost::shared_ptr<Packet>& packet) override;
	boost::shared_ptr<Client> getClient();
	boost::shared_ptr<NetServerService> server;

	boost::shared_ptr<Client> client;
};
