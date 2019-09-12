#pragma once
#include "../../shared/NetLib/packets/PacketProcess.h"

class ProcessService
{
public:
	static void getProcesses(std::list<Process>& processes);

#ifdef XEYTAN_LINUX
private:
	static Display* display;
	static Atom atomPID;
#endif
};
