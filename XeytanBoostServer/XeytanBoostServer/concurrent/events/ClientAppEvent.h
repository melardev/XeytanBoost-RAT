#pragma once
#include "AppEvent.h"
#include <boost/shared_ptr.hpp>
#include "../../models/Client.h"

class ClientAppEvent : public AppEvent
{
public:
	ClientAppEvent();
	~ClientAppEvent();
	ClientAppEvent(Subject subject, const boost::shared_ptr<Client>& client, const boost::shared_ptr<void>& object);

	ClientAppEvent(Subject subject, Action action, const boost::shared_ptr<Client>& client);
	ClientAppEvent(Subject subject, Action action, const boost::shared_ptr<Client>& client, const boost::shared_ptr<void>& object);
	ClientAppEvent(Subject subject, Action action, unsigned long long clientId);
	ClientAppEvent(Subject subject, const boost::shared_ptr<Client>& client);

	boost::shared_ptr<Client> getClient();
	boost::shared_ptr<Client> client;
	unsigned long long clientId;
};
