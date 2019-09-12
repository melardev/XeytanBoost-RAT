#include "Packet.h"
#include <boost/archive/binary_oarchive.hpp>
#include <sstream>

Packet::Packet()
{
	packetType = PacketType::Invalid;
}

Packet::Packet(const PacketType& packetType): packetType(packetType)
{
}

Packet::~Packet()
{
}

std::pair<int64_t, const char*> Packet::getHeaderBuffer() const
{
	std::ostringstream outputStringStream;
	outputStringStream << static_cast<int64_t>(0);
	{
		boost::archive::binary_oarchive binaryOutputStream(outputStringStream);
		binaryOutputStream << static_cast<int16_t>(packetType);
	}


	outputStringStream.seekp(0, std::ios::end);
	int64_t size = outputStringStream.tellp();
	unsigned long long sizeWithoutPacketSize = size - sizeof int64_t;

	// Write packet size at the beginning
	outputStringStream.seekp(0, std::ios::beg);
	outputStringStream << size;

	// return std::pair<int64_t, const char*>(size, outputStringStream.str().c_str());
	return {size, outputStringStream.str().c_str()};
}
