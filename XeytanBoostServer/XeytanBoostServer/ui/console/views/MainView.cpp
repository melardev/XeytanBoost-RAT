#include "MainView.h"
#include <iostream>
#include <vector>
#include <boost/algorithm/string.hpp>
#include "../ConsoleUiMediator.h"
#include <boost/foreach.hpp>

MainView::MainView(const boost::shared_ptr<ConsoleUiMediator>& mediator)
	: IClientView(mediator)
{
	banner = "XeytanBoost>$ ";
	bannerFormatWithClientId = "XeytanBoost//> %llu>$ ";
}

MainView::~MainView()
{
}


void MainView::printBanner() const
{
	std::cout << "" << std::endl;

	if (client != nullptr)
		printf_s(bannerFormatWithClientId, client->clientId);
	else
		printf_s(banner);
}

bool MainView::onInstruction(std::vector<std::string>& instructionTokens)
{
	return false;
}



void MainView::printClientList(const std::vector<Client>& clients)
{
	Client client;
	const int counter = 0;
	BOOST_FOREACH(client, clients)
	{
		std::cout << counter << client.pcName << std::endl;
		std::cout << "\t" << "Client Id: " << client.clientId << std::endl;
		std::cout << "\t" << "Pc Name: " << client.pcName << std::endl;
		std::cout << "\t" << "OS Name: " << client.osName << std::endl;
		std::wcout << "\t" << "Username: " << client.username << std::endl;
	}
}

void MainView::printNewClientConnection(int64_t clientId, const std::string& uuidStr, const std::string& pcName,
                                        const std::string& os, const std::wstring& username)
{
	std::cout << std::endl;
	std::cout << "Net client connection" << std::endl;
	std::cout << "\t" << "Client Id: " << clientId << std::endl;
	std::cout << "\t" << "Client UUID: " << uuidStr << std::endl;
	std::cout << "\t" << "PcName: " << pcName << std::endl;
	std::cout << "\t" << "OSName: " << os << std::endl;
	std::wcout << "\t" << "Username: " << username << std::endl;
}
