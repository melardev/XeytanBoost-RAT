#pragma once
#include <boost/shared_ptr.hpp>

template <class T>
class IThreadChannel
{
public:
	IThreadChannel();
	virtual ~IThreadChannel();
	virtual boost::shared_ptr<T> takeSync() = 0;
	virtual void submitAsync(const boost::shared_ptr<T>&) = 0;
};

template <class T>
IThreadChannel<T>::IThreadChannel()
{
}

template <class T>
IThreadChannel<T>::~IThreadChannel()
{
}
