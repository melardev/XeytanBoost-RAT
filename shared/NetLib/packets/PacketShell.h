#pragma once
#include "Packet.h"

class PacketShell : public Packet
{
public:

	enum class PacketAction : int8_t
	{
		Invalid = -1,
		Start,
		Exec,
		Stop
	};

	PacketShell();
	PacketShell(std::string data);

	~PacketShell();
	std::string data;
	PacketAction action;

private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		boost::serialization::split_member(ar, *this, version);
	}

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		ar & boost::serialization::base_object<Packet>(*this);

		ar << (int8_t)action;
		ar << data;
	}

	template <class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar >> boost::serialization::base_object<Packet>(*this);

		int8_t actionInt = 0;
		ar >> actionInt;
		action = (PacketAction)actionInt;
		// same as ar >> data;
		ar & data;
	}
};
