#pragma once
#include <boost/smart_ptr/shared_ptr.hpp>
#include "IView.h"
#include "IClientView.h"

class ConsoleUiMediator;

class MainView : public IClientView
{
public:
	MainView(const boost::shared_ptr<ConsoleUiMediator>& mediator);
	~MainView();

	virtual void printBanner() const override;

	virtual bool onInstruction(std::vector<std::string>& instructionTokens) override;
	static void printClientList(const std::vector<Client>& vector);
	static void printNewClientConnection(int64_t clientId, const std::string& uuidStr, const std::string& pcName,
	                                     const std::string& os, const std::wstring& username);
	const char* banner;
	const char* bannerFormatWithClientId;
	boost::shared_ptr<ConsoleUiMediator> mediator;
};
