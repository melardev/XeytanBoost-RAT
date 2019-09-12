#pragma once
#include "Packet.h"
#include "../models/FileInfo.h"

class PacketFileSystem : public Packet
{
public:
	enum class FileSystemFocus : int8_t
	{
		Invalid = -1,
		ListDirEntries,
		ListRoots,
		DirectoryExists
	};

	PacketFileSystem();
	PacketFileSystem(std::wstring path);
	PacketFileSystem(const FileSystemFocus& fileSystemFocus, std::wstring path);
	~PacketFileSystem();
	std::wstring path;

	// To be able to serialize std::vector we would need #include <boost/serialization/vector.hpp>
	// which is already included in packet.h
	std::vector<FileInfo> files;
	FileSystemFocus fileSystemFocus;

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

		ar << (int8_t)fileSystemFocus;
		ar << path;
		ar << files;
	}

	template <class Archive>
	void load(Archive& ar, const unsigned int version)
	{
		ar >> boost::serialization::base_object<Packet>(*this);
		int8_t fsFocus = 0;
		ar >> fsFocus;
		fileSystemFocus = (FileSystemFocus)fsFocus;
		ar & path;
		ar & files;
	}
};
