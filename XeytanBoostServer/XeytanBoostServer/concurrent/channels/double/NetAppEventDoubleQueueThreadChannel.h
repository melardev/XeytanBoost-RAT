#pragma once
#include "AppEventDoubleQueueThreadChannel.h"

class NetAppEventDoubleQueueThreadChannel : public AppEventDoubleQueueThreadChannel
{
public:
	NetAppEventDoubleQueueThreadChannel();
	~NetAppEventDoubleQueueThreadChannel();

	boost::shared_ptr<AppEvent> takeFromApp();
	boost::shared_ptr<AppEvent> takeFromNet();
	void submitToApp(const boost::shared_ptr<AppEvent>& event);
	void submitToNet(const boost::shared_ptr<AppEvent>& event);
};

