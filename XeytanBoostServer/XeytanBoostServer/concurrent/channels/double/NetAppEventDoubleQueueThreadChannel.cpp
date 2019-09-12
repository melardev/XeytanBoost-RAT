#include "NetAppEventDoubleQueueThreadChannel.h"


NetAppEventDoubleQueueThreadChannel::NetAppEventDoubleQueueThreadChannel()
{
}

NetAppEventDoubleQueueThreadChannel::~NetAppEventDoubleQueueThreadChannel()
{
}

boost::shared_ptr<AppEvent> NetAppEventDoubleQueueThreadChannel::takeFromApp()
{
	return takeFromRight();
}

boost::shared_ptr<AppEvent> NetAppEventDoubleQueueThreadChannel::takeFromNet()
{
	return takeFromLeft();
}

void NetAppEventDoubleQueueThreadChannel::submitToApp(const boost::shared_ptr<AppEvent>& event)
{
	submitToLeft(event);
}

void NetAppEventDoubleQueueThreadChannel::submitToNet(const boost::shared_ptr<AppEvent>& event)
{
	submitToRight(event);
}
