#pragma once
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include "NetClientService.h"
#include <boost/array.hpp>

#include <boost/thread/shared_mutex.hpp>

#include "../concurrent/channels/single/PacketThreadChannel.h"

class NetAppEventDoubleQueueThreadChannel;

class NetServerService : public boost::enable_shared_from_this<NetServerService>
{
public:
	~NetServerService();
	void acceptOneClientAsync();
	void runAsync();
	void setChannel(const boost::shared_ptr<NetAppEventDoubleQueueThreadChannel>& netChannel);
	boost::shared_ptr<Client> getClient(unsigned long long clientId);

	static boost::shared_ptr<NetServerService> createInstance();
	void onClientAcceptResult(boost::shared_ptr<NetClientService> client, const boost::system::error_code& error);
	std::vector<boost::shared_ptr<Client>> getClients();
	void queuePacket(const boost::shared_ptr<Packet>& packet, unsigned long long clientId);
	boost::shared_ptr<NetClientService> getNetClientService(unsigned long long clientId);
	void receiveAppEvent();

	void onPacketReceived(const boost::shared_ptr<Client>& netClientService,
	                      const boost::shared_ptr<Packet>& packet) const;
	void sendPacketsProc();
	void onPacketSent();
private:
	NetServerService(const boost::shared_ptr<boost::asio::io_service>& ioService);

	boost::asio::ip::tcp::acceptor acceptor;
	boost::shared_ptr<boost::asio::io_service> ioService;
	boost::shared_ptr<NetAppEventDoubleQueueThreadChannel> netChannel;
	bool running;

	std::map<unsigned long long, boost::shared_ptr<NetClientService>> clients;
	boost::shared_mutex clientsMutex;
	boost::mutex packetToSendMutex;
	PacketThreadChannel packetToSendChannel;
	boost::condition_variable isSendingPacketCv;
	volatile bool isSendingPacket;
	boost::mutex isSendingPacketMutex;
};
