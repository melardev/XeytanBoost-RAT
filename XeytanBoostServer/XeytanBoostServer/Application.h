#pragma once
#include <boost/shared_ptr.hpp>

class NetServerService;
class ConsoleUiMediator;
class NetAppEventDoubleQueueThreadChannel;
class UiAppEventDoubleQueueThreadChannel;

class Application
{
public:
	Application();
	~Application();
	void run();
	void listenNetEvents();
	void listenUiEvents();

private:
	bool running;

	boost::shared_ptr<NetServerService> netServerService;
	boost::shared_ptr<NetAppEventDoubleQueueThreadChannel> netChannel;
	boost::shared_ptr<UiAppEventDoubleQueueThreadChannel> uiChannel;
	boost::shared_ptr<ConsoleUiMediator> uiMediator;
};
