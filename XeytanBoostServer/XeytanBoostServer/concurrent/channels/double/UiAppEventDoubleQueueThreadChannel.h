#pragma once
#include <boost/shared_ptr.hpp>
#include "AppEventDoubleQueueThreadChannel.h"

class AppEvent;

class UiAppEventDoubleQueueThreadChannel : public AppEventDoubleQueueThreadChannel
{
public:
	UiAppEventDoubleQueueThreadChannel();
	~UiAppEventDoubleQueueThreadChannel();

	boost::shared_ptr<AppEvent> takeFromApp();
	boost::shared_ptr<AppEvent> takeFromUi();
	void submitToApp(const boost::shared_ptr<AppEvent>& event);
	void submitToUi(const boost::shared_ptr<AppEvent>& event);
};

