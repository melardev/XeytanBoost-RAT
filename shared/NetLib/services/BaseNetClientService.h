#pragma once
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include "../packets/Packet.h"
#include <iomanip>
#include <boost/bind.hpp>

class PacketPresentation;

class BaseNetClientService : public boost::enable_shared_from_this<BaseNetClientService>
{
public:
	BaseNetClientService( // const boost::shared_ptr<NetServerService>& server,
		const boost::shared_ptr<boost::asio::io_context>& ioService);
	virtual ~BaseNetClientService();
	boost::asio::ip::tcp::socket& getSocket();
	void interactAsync();
	void readHeader();
	bool isValidPacketSize();
	void handleReadHeader(const boost::system::error_code& error);
	void readPayload();
	virtual void onPacketReceived(const boost::shared_ptr<Packet>& packet) = 0;
	void handleReadBody(const boost::system::error_code& error);

	void sendPacket(const boost::shared_ptr<Packet>& packet);

	void onWrite(const boost::system::error_code& err, size_t bytesTransferred);


	boost::shared_ptr<boost::asio::io_context> ioService;
	boost::asio::ip::tcp::socket clientSocket;
	Packet* receivedPacket;
	char headerBuffer[PACKET_SIZE_LENGTH];
	std::vector<char> inputBuffer;
	std::string headerStr;
	std::string payloadStr;
};
