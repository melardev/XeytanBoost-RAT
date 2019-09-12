#pragma once
#include "../../../../../shared/ConcurrentLib/channels/double/DoubleQueueThreadChannel.h"

class AppEvent;

class AppEventDoubleQueueThreadChannel : public DoubleQueueThreadChannel<AppEvent>
{
public:
	AppEventDoubleQueueThreadChannel();
	~AppEventDoubleQueueThreadChannel();

};
