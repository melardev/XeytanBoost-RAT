#include "AppEvent.h"


AppEvent::AppEvent()
{
}

AppEvent::AppEvent(Target target, Subject subject)
	:target(target), subject(subject)
{
}

AppEvent::AppEvent(Target target, Subject subject, const boost::shared_ptr<void> object)
	: target(target), subject(subject), object(object)
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action)
	: target(target), subject(subject), action(action)
{
}

AppEvent::AppEvent(Target target, Subject subject, Action action, const boost::shared_ptr<void>& object)
	: target(target), subject(subject), action(action), object(object)
{
}

AppEvent::AppEvent(Target target, Subject subject, const std::string& description)
	: target(target), subject(subject), description(description)
{
}

AppEvent::AppEvent(Target target)
	: target(target)
{
}


AppEvent::~AppEvent()
{
}
