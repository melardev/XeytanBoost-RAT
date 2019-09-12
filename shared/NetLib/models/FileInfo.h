#pragma once
#include <inttypes.h>
#include <xstring>
#include <boost/serialization/serialization.hpp>

struct FileInfo
{
	FileInfo();
	~FileInfo();


	uintmax_t size;
	std::wstring filePath;
	time_t lastUpdateTime;
private:
	friend class boost::serialization::access;

	template <typename Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & filePath;
		ar & lastUpdateTime;
		ar & size;
	}
};
