#include "IView.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

IView::IView()
{
}

IView::IView(const boost::shared_ptr<ConsoleUiMediator>& mediator): mediator(mediator)
{
}


IView::~IView()
{
}

std::vector<std::string> IView::loop()
{
	char instructionLine[500];
	while (true)
	{
		printBanner();
		std::cin.getline(instructionLine, 500);
		std::string instruction(instructionLine);

		std::vector<std::string> parts;
		boost::split(parts, instruction, boost::is_any_of(" "));
		if (!onInstruction(parts))
		{
			return parts;
		}
	}
}

void IView::print(const std::string& message) const
{
	std::cout << message;
}

void IView::wprint(const std::wstring& message) const
{
	std::wcout << message;
}
