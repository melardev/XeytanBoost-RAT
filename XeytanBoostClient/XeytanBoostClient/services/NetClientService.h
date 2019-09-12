#pragma once
#include "../../../shared/NetLib/services/BaseNetClientService.h"
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include <boost/asio/buffer.hpp>
#include <vector>
#include "../../../shared/NetLib/models/FileInfo.h"
#include "../../../shared/NetLib/packets/PacketProcess.h"
#include <boost/asio/buffer.hpp>

class Application;

class NetClientService : public BaseNetClientService
{
public:
	NetClientService();
	NetClientService(const boost::shared_ptr<Application>& app,
	                 const boost::shared_ptr<boost::asio::io_service>& ioService);
	~NetClientService();
	void runAsync();

	void onConnection(const boost::system::error_code& err);

	void sendDirEntries(const std::wstring& path, const std::vector<FileInfo>& files);
	void sendShellData(const std::string& data);
	void sendStopShell();
	void sendClientInformation(const std::string& pcName, const std::wstring& username, const std::string& osName, const std::map<std::string, std::string>& environment);
	void sendProcessList(const std::list<Process>& processes);


	static boost::shared_ptr<NetClientService> createInstance(const boost::shared_ptr<Application>& application);
	virtual void onPacketReceived(const boost::shared_ptr<Packet>& packet) override;
	boost::shared_ptr<Application> app;
};
