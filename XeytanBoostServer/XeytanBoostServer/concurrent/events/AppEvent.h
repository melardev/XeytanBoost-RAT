#pragma once
#include <boost/shared_ptr.hpp>

enum class Target
{
	InvalidTarget = -1,
	Application,
	Server,
	Client,
	Ui
};

enum class Subject
{
	InvalidSubject = -1,
	Interaction,
	// Used for console
	Connection,
	Desktop,
	Camera,
	Process,
	FileSystem,
	Shell,
	Download,
	Information,
	PacketReceived,
	Error,
};

enum class Action
{
	InvalidAction = -1,
	ListAvailable,
	Get,
	Post,
	Start,
	Update,
	Pause,
	Stop,
	Fetch = Get,
	Push = Post,
};


class AppEvent
{
public:
	AppEvent();

	AppEvent(Target target, Subject subject);
	AppEvent(Target target, Subject subject, const boost::shared_ptr<void> object);
	AppEvent(Target target, Subject subject, Action action);
	AppEvent(Target target, Subject subject, Action action, const boost::shared_ptr<void>& object);
	AppEvent(Target target, Subject subject, const std::string& description);
	AppEvent(Target target);

	virtual ~AppEvent();
public:
	Target target;
	Subject subject;
	Action action;

	boost::shared_ptr<void> object;
	std::string description;
};
