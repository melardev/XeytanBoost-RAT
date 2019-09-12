#pragma once
#include "../../../../../shared/ConcurrentLib/channels/single/QueueThreadChannel.h"

class AppEvent;

class AppEventQueueThreadChannel : public QueueThreadChannel<AppEvent>
{
public:
	AppEventQueueThreadChannel();
	~AppEventQueueThreadChannel();
};
