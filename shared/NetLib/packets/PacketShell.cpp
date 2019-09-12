#include "PacketShell.h"
#include <utility>

PacketShell::PacketShell() : Packet(PacketType::Shell)
{
}

PacketShell::PacketShell(std::string data) : Packet(PacketType::Shell),
                                             data(std::move(data)),
                                             action(PacketAction::Exec)
{
}

PacketShell::~PacketShell()
{
}
