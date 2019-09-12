#pragma once
#include "Packet.h"

class PacketInformation : public Packet
{
public:
	PacketInformation();
	~PacketInformation();

	std::map<std::string, std::string> environment;
	std::string osName;
	std::string pcName;
	std::wstring username;
private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Packet>(*this);

		ar & environment;
		ar & osName;
		ar & pcName;
		ar & username;
	}
};
