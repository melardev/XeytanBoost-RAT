#pragma once
#include <boost/uuid/uuid.hpp>
#include <xstring>

static int CLIENT_COUNTER = 1;

class Client
{
public:

	Client();

	Client(boost::uuids::uuid uuid);
	Client(boost::uuids::uuid uuid, const std::wstring& username, const std::string& pcName);
	~Client();
	std::wstring getUsername() const;
	std::string getPcName() const;
	boost::uuids::uuid getUuid() const;

	void setUsername(const std::wstring& username);
	void setPcName(const std::string& pcName);
	int64_t getId() const;


	std::string pcName;
	std::wstring username;
	std::string osName;
	int64_t clientId;
	boost::uuids::uuid uuid;
};
