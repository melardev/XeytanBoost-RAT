#include "ShellView.h"

ShellView::ShellView(const boost::shared_ptr<ConsoleUiMediator>& mediator)
{
}

ShellView::~ShellView() = default;

bool ShellView::onInstruction(std::vector<std::string>& instructionTokens)
{
	// TODO implement help menu
	return false;
}

void ShellView::printBanner() const
{
}


