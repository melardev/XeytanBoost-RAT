#include "AppEventDoubleQueueThreadChannel.h"
#include <boost/make_shared.hpp>
#include "../single/AppEventQueueThreadChannel.h"
class AppEvent;

AppEventDoubleQueueThreadChannel::AppEventDoubleQueueThreadChannel()
	: DoubleQueueThreadChannel<AppEvent>(boost::make_shared<AppEventQueueThreadChannel>(),
	                                     boost::make_shared<AppEventQueueThreadChannel>())
{
}


AppEventDoubleQueueThreadChannel::~AppEventDoubleQueueThreadChannel()
{
}
