#include "Application.h"
#include "services/NetServerService.h"
#include <boost/thread.hpp>
#include "concurrent/channels/double/UiAppEventDoubleQueueThreadChannel.h"
#include "concurrent/channels/double/NetAppEventDoubleQueueThreadChannel.h"
#include "ui/console/ConsoleUiMediator.h"
#include "concurrent/events/AppEvent.h"
#include "concurrent/events/ClientAppEvent.h"

Application::Application()
{
}

Application::~Application()
{
}

void Application::run()
{
	running = true;
	netChannel = boost::make_shared<NetAppEventDoubleQueueThreadChannel>();
	uiChannel = boost::make_shared<UiAppEventDoubleQueueThreadChannel>();

	uiMediator.reset(new ConsoleUiMediator);

	boost::thread netEventsListen(&Application::listenNetEvents, this);
	boost::thread uiEventsListen(&Application::listenUiEvents, this);

	netServerService = NetServerService::createInstance();
	netServerService->setChannel(netChannel);
	netServerService->runAsync();

	uiMediator->setUiChannel(uiChannel);
	uiMediator->mainLoop();
}

void Application::listenNetEvents()
{
	while (running)
	{
		boost::shared_ptr<AppEvent> netEvent = netChannel->takeFromNet();
		Target target = netEvent->target;
		Subject subject = netEvent->subject;
		boost::shared_ptr<void> object = netEvent->object;
		Action action = netEvent->action;

		// Forward event to ui
		uiChannel->submitToUi(netEvent);
	}
}

void Application::listenUiEvents()
{
	while (running)
	{
		boost::shared_ptr<AppEvent> uiEvent = uiChannel->takeFromUi();
		assert(uiEvent != nullptr);

		if (uiEvent->target == Target::Client &&
			uiEvent->subject == Subject::Connection &&
			uiEvent->action == Action::Start)
		{
			auto clientEvent = boost::dynamic_pointer_cast<ClientAppEvent>(uiEvent);
			const boost::shared_ptr<Client> client = netServerService->getClient(clientEvent->clientId);
			clientEvent->client = client;
			uiChannel->submitToUi(clientEvent);
		}

		// Forward event to net
		netChannel->submitToNet(uiEvent);
	}
}
