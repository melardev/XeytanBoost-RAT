#include "ProcessView.h"


ProcessView::ProcessView(const boost::shared_ptr<ConsoleUiMediator>& mediator)
{
}

ProcessView::~ProcessView()
{
}

void ProcessView::printBanner() const
{
}

bool ProcessView::onInstruction(std::vector<std::string>& instructionTokens)
{
	return false;
}

void ProcessView::printProcesses(const std::list<Process>& processes)
{
	std::cout << std::endl << std::endl;
	if (processes.empty())
	{
		std::cout << "Process list is empty ;(" << std::endl << std::endl;
		return;
	}

	for (auto&& process : processes)
	{
		std::cout << "Pid: " << process.pid << std::endl;
		std::wcout << "Path: " << process.path << std::endl;
		if (process.hasUi)
			std::wcout << "Window Title: " << process.title << std::endl;
		std::cout << std::endl;
	}
}
