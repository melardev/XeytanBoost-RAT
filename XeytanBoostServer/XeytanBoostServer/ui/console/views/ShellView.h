#pragma once
#include "IClientView.h"

class ShellView : public IClientView
{
public:
	ShellView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	~ShellView();
	virtual bool onInstruction(std::vector<std::string>& instructionTokens) override;
	virtual void printBanner() const override;
	
};
