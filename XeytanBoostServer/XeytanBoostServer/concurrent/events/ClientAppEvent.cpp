#include "ClientAppEvent.h"


ClientAppEvent::ClientAppEvent()
{
}


ClientAppEvent::~ClientAppEvent()
{
}

ClientAppEvent::ClientAppEvent(Subject subject,
                               const boost::shared_ptr<Client>& client,
                               const boost::shared_ptr<void>& object)
	: AppEvent(Target::Client, subject, object),
	  client(client), clientId(client->clientId)

{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, const boost::shared_ptr<Client>& client)
	: AppEvent(Target::Client, subject, action),
	  client(client), clientId(client->clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, const boost::shared_ptr<Client>& client,
                               const boost::shared_ptr<void>& object):
	AppEvent(Target::Client, subject, action, object),
	client(client), clientId(client->clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, Action action, unsigned long long clientId)
	: AppEvent(Target::Client, subject, action), clientId(clientId)
{
}

ClientAppEvent::ClientAppEvent(Subject subject, const boost::shared_ptr<Client>& client)
	: AppEvent(Target::Client, subject), client(client), clientId(client->clientId)
{
}

boost::shared_ptr<Client> ClientAppEvent::getClient()
{
	return client;
}
