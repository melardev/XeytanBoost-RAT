#pragma once
#include <boost/serialization/export.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/list.hpp> // Needed for serialization of std::list<T>
#include <boost/serialization/vector.hpp> // Needed for serialization of std::vector<T>
#include <boost/serialization/map.hpp> // Needed for serialization of std::map<T, I>

#include "packetdefs.h"
#include <utility>
#include <cstdint>

class Packet
{
public:
	Packet();
	Packet(const PacketType& packetType);
	virtual ~Packet();
	std::pair<int64_t, const char*> getHeaderBuffer() const;
	PacketType packetType;
	unsigned long long clientId;

private:
	friend class boost::serialization::access;

	// BOOST_SERIALIZATION_SPLIT_MEMBER()

	// This method is the same as teh above Macro
	// https://www.boost.org/doc/libs/1_70_0/libs/serialization/doc/serialization.html#splitting
	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		boost::serialization::split_member(ar, *this, version);
	}

	template <class Archive>
	void save(Archive& ar, const unsigned int version) const
	{
		int16_t packetTypeInt16 = (int16_t)packetType;
		ar & packetTypeInt16;
	}

	template <class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		int16_t packetTypeInt16 = (int16_t)packetType;
		ar >> packetTypeInt16;
	}
};
