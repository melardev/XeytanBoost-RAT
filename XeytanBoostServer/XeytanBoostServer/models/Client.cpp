#include "Client.h"


Client::Client()
{
}

Client::Client(boost::uuids::uuid uuid): clientId(CLIENT_COUNTER++), uuid(uuid)
{
}

Client::Client(boost::uuids::uuid uuid, const std::wstring& username, const std::string& pcName)
	: pcName(pcName), username(username), clientId(CLIENT_COUNTER++), uuid(uuid)
{
}

Client::~Client() = default;

std::wstring Client::getUsername() const
{
	return username;
}

std::string Client::getPcName() const
{
	return pcName;
}

boost::uuids::uuid Client::getUuid() const
{
	return uuid;
}

void Client::setUsername(const std::wstring& username)
{
	this->username = username;
}

void Client::setPcName(const std::string& pcName)
{
	this->pcName = pcName;
}

int64_t Client::getId() const
{
	return clientId;
}
