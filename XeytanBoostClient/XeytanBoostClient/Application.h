#pragma once
#include <boost/enable_shared_from_this.hpp>
#include "../../shared/NetLib/packets/Packet.h"
#include <boost/asio/buffer.hpp>
#include "services/NetClientService.h"

class ShellService;

class Application : public boost::enable_shared_from_this<Application>
{
public:
	Application();
	~Application();
	void run();
	void sendListDir(const std::wstring& path);
	
	void sendFsRoots();
	void onPacketReceived(const boost::shared_ptr<Packet>& packet);
	void onShellData(const std::string& data);
	void onShellStopped();
	static boost::shared_ptr<Application> getInstance();
	bool running;
	boost::shared_ptr<NetClientService> netClientService;
private:
	boost::shared_ptr<ShellService> shellService;
};

extern boost::shared_ptr<Application> appInstance;
