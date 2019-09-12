#pragma once
#include "../../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"

class Packet;

class PacketThreadChannel : public QueueThreadChannel<Packet>
{
public:
	PacketThreadChannel();
	~PacketThreadChannel();
};
