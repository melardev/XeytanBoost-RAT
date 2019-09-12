#include "BaseNetClientService.h"
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../packets/packetdefs.h"
#include <boost/archive/binary_iarchive.hpp>
#include "../packets/PacketPresentation.h"
#include <boost/archive/binary_oarchive.hpp>
#include <iomanip>
#include "../packets/PacketFileSystem.h"
#include "../packets/PacketShell.h"
#include "../packets/PacketInformation.h"
#include "../packets/PacketProcess.h"

BaseNetClientService::BaseNetClientService(const boost::shared_ptr<boost::asio::io_context>& ioService)
	: ioService(ioService), clientSocket(*ioService)
{
}

BaseNetClientService::~BaseNetClientService()
{
}

void BaseNetClientService::interactAsync()
{
	readHeader();
}

void BaseNetClientService::readHeader()
{
	clientSocket.async_receive(
		boost::asio::buffer(headerBuffer,
		                    sizeof int64_t),
		boost::bind(&BaseNetClientService::handleReadHeader, shared_from_this(),
		            boost::asio::placeholders::error));
}

bool BaseNetClientService::isValidPacketSize()
{
	std::istringstream is(headerBuffer);

	uint64_t payloadLen = 0;
	is >> std::hex >> payloadLen;

	return !(payloadLen == 0 || payloadLen > MAX_PACKET_LENGTH);
}

void BaseNetClientService::handleReadHeader(const boost::system::error_code& error)
{
	if (error)
	{
		// Error
		boost::system::error_code copyError = error;
		clientSocket.shutdown(boost::asio::socket_base::shutdown_both, copyError);
		clientSocket.close();
	}
	else
	{
		// getting the packet length as int in Cpp basically..
		std::istringstream is(std::string(headerBuffer, sizeof int64_t));
		std::size_t packetLength = 0;
		is >> std::hex >> packetLength;
		inputBuffer.resize(packetLength);
		// Success
		if (isValidPacketSize())
		{
			// Now read the body
			readPayload();
		}
		else
		{
			// Retry
			std::cerr << "Invalid PacketSize" << std::endl;
			readHeader();
		}
	}
}

void BaseNetClientService::readPayload()
{
	boost::asio::async_read(clientSocket,
	                        boost::asio::buffer(inputBuffer,
	                                            inputBuffer.size()),
	                        boost::bind(&BaseNetClientService::handleReadBody, shared_from_this(),
	                                    boost::asio::placeholders::error));
}


void BaseNetClientService::handleReadBody(const boost::system::error_code& error)
{
	if (error)
	{
#if _DEBUG && _WIN64 || _WIN32
		DebugBreak();
#endif
	}
	else
	{
		const std::string serializedStr(&inputBuffer[0], inputBuffer.size());
		std::istringstream serializedStream(serializedStr);
		boost::shared_ptr<Packet> packet;
		{
			{
				boost::archive::binary_iarchive binaryInputStream(serializedStream);

				binaryInputStream.register_type<PacketPresentation>();
				binaryInputStream.register_type<PacketInformation>();
				binaryInputStream.register_type<PacketFileSystem>();
				binaryInputStream.register_type<PacketShell>();
				binaryInputStream.register_type<PacketProcess>();

				binaryInputStream >> packet;
			}

			if (packet->packetType < PacketType::MinPacketTypeValue
				|| packet->packetType > PacketType::MaxPacketTypeValue)
			{
				// TODO: Exception handling
			}
		}
		onPacketReceived(packet);

		// We turn back reading another packet(read length then body)
		readHeader();
	}
}

void BaseNetClientService::sendPacket(const boost::shared_ptr<Packet>& packet)
{
	// Serialize the data first so we know how large it is.
	std::ostringstream outputStringStream;
	{
		boost::archive::binary_oarchive binaryOutputStream(outputStringStream);

		// Subclasses must be either registered or exported, in this case
		// let's register them using register_type
		binaryOutputStream.register_type<PacketPresentation>();
		binaryOutputStream.register_type<PacketInformation>();
		binaryOutputStream.register_type<PacketFileSystem>();
		binaryOutputStream.register_type<PacketShell>();
		binaryOutputStream.register_type<PacketProcess>();

		binaryOutputStream << packet;
	}

	payloadStr = outputStringStream.str();

	// Format the header.
	std::ostringstream headerStream;
	headerStream << std::setw(sizeof int64_t) << std::hex << payloadStr.size();

	if (!headerStream)
	{
		// Something went wrong, inform the caller.
		boost::system::error_code error(boost::asio::error::invalid_argument);
		// TODO: error handling
		// clientSocket.get_io_service().post(boost::bind(handler, error));
		return;
	}

	headerStr = headerStream.str();

	// Write the serialized data to the socket. We use "gather-write" to send
	// both the header and the data in a single write operation.
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back(boost::asio::buffer(headerStr));
	buffers.push_back(boost::asio::buffer(payloadStr));
	boost::asio::async_write(clientSocket, buffers,
	                         boost::bind(&BaseNetClientService::onWrite, this,
	                                     boost::asio::placeholders::error,
	                                     boost::asio::placeholders::bytes_transferred));
}

void BaseNetClientService::onWrite(const boost::system::error_code& err, size_t bytesTransferred)
{
	if (err)
	{
		// Error handling
	}
}

boost::asio::ip::tcp::socket& BaseNetClientService::getSocket()
{
	return clientSocket;
}
