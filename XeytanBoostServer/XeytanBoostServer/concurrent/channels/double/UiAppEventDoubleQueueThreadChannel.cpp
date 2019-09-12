#include "UiAppEventDoubleQueueThreadChannel.h"


UiAppEventDoubleQueueThreadChannel::UiAppEventDoubleQueueThreadChannel()
{
}

UiAppEventDoubleQueueThreadChannel::~UiAppEventDoubleQueueThreadChannel()
{
}

boost::shared_ptr<AppEvent> UiAppEventDoubleQueueThreadChannel::takeFromApp()
{
	// The app posts on the left queue, so take from left
	return takeFromLeft();
}

boost::shared_ptr<AppEvent> UiAppEventDoubleQueueThreadChannel::takeFromUi()
{
	// The ui posts on the left queue, so take from right
	return takeFromRight();
}

void UiAppEventDoubleQueueThreadChannel::submitToApp(const boost::shared_ptr<AppEvent>& event)
{
	submitToRight(event);
}

void UiAppEventDoubleQueueThreadChannel::submitToUi(const boost::shared_ptr<AppEvent>& event)
{
	submitToLeft(event);
}
