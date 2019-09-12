#pragma once
#include "Packet.h"

class PacketProcess : public Packet
{
public:
	PacketProcess();
	~PacketProcess();

	std::list<Process> processes;
	
private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Packet>(*this);

		ar & processes;
	}
};

// Non intrusive Process serialization
namespace boost
{
	namespace serialization
	{
		template <typename Archive>
		void serialize(Archive& ar, Process& process, const unsigned int version)
		{
			ar & process.pid & process.path & process.hasUi & process.title & process.icon;
		}
	}
}
