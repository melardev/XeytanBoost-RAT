#pragma once
#include <boost/smart_ptr/shared_ptr.hpp>
#include "IClientView.h"
#include <list>
#include "../../../../../shared/NetLib/packets/PacketProcess.h"

class ConsoleUiMediator;

class ProcessView : public IClientView
{
public:
	ProcessView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	~ProcessView();

	virtual void printBanner() const override;
	virtual bool onInstruction(std::vector<std::string>& instructionTokens) override;
	static void printProcesses(const std::list<Process>& processes);
};
