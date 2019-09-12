#include "PacketInformation.h"

PacketInformation::PacketInformation() : Packet(PacketType::ClientInformation)
{
}

PacketInformation::~PacketInformation() = default;
