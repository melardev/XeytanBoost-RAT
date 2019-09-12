#pragma once
#include <string>
#include <vector>
#include "../../../Application.h"
#include <boost/array.hpp>

class IView
{
public:
	IView();
	IView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	virtual ~IView();

	virtual std::vector<std::string> loop();
	virtual bool onInstruction(std::vector<std::string>& instructionTokens) =0;
	virtual void printBanner() const = 0;
	void print(const std::string& message) const;
	void wprint(const std::wstring& message) const;
	boost::shared_ptr<ConsoleUiMediator> mediator;
};
