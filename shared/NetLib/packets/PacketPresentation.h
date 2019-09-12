#pragma once
#include "Packet.h"
#include <string>

#include <string>

struct BasicSysInfo
{
	std::string pcName;
	std::wstring username;
	std::string osName;

private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & pcName;
		ar & osName;
		ar & username;
	}
};

class PacketPresentation : public Packet
{
public:
	PacketPresentation();
	~PacketPresentation();

	BasicSysInfo sysInfo;
private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & boost::serialization::base_object<Packet>(*this);
		// ar & sysInfo.pcName;
		// ar & sysInfo.osName;
		// ar & sysInfo.username;

		ar & sysInfo;
	}

};
