#include "PacketFileSystem.h"
#include <utility>


PacketFileSystem::PacketFileSystem(): Packet(PacketType::FileSystem)
{
}

PacketFileSystem::PacketFileSystem(std::wstring path)
	: Packet(PacketType::FileSystem), path(std::move(path))
{
}

PacketFileSystem::PacketFileSystem(const FileSystemFocus& fileSystemFocus,
                                   std::wstring path) : Packet(PacketType::FileSystem), path(std::move(path)),
                                                        fileSystemFocus(fileSystemFocus)
{
}


PacketFileSystem::~PacketFileSystem()
{
}
