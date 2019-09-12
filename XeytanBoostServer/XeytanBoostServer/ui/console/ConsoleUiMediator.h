#pragma once
#include "../../concurrent/channels/double/AppEventDoubleQueueThreadChannel.h"
#include <boost/array.hpp>
#include <boost/enable_shared_from_this.hpp>
#include "../../concurrent/channels/double/AppEventDoubleQueueThreadChannel.h"
#include <boost/thread/win32/shared_mutex.hpp>

class Client;
class MainView;
class ProcessView;
class FileSystemView;
class UiAppEventDoubleQueueThreadChannel;
class ShellView;
class IView;

class ConsoleUiMediator : public boost::enable_shared_from_this<ConsoleUiMediator>
{
public:
	ConsoleUiMediator();
	~ConsoleUiMediator();
	bool handleInstructionInteracting(const std::vector<std::string>& parts) const;
	void setActiveClient(const boost::shared_ptr<Client>& client);
	bool handleInstructionNotInteracting(const std::vector<std::string>& parts) const;
	bool handleInstruction(const std::vector<std::string>& instruction) const;
	boost::shared_ptr<AppEvent> getEventForInstruction(const std::vector<std::string>& instruction);
	void mainLoop();
	void setUiChannel(const boost::shared_ptr<UiAppEventDoubleQueueThreadChannel>& uiChannel);
	boost::shared_ptr<UiAppEventDoubleQueueThreadChannel> uiChannel;
	bool running;

	mutable boost::shared_mutex clientMutex;

	void receiveAppEvent();

private:
	boost::shared_ptr<Client> activeClient;
	boost::shared_ptr<MainView> mainView;
	boost::shared_ptr<ProcessView> processView;
	boost::shared_ptr<FileSystemView> fileSystemView;
	boost::shared_ptr<ShellView> shellView;
	boost::shared_ptr<IView> currentView;
};
