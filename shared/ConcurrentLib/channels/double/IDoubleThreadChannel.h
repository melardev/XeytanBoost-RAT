#pragma once
#include <boost/shared_ptr.hpp>

template <class T>
class IDoubleThreadChannel
{
public:
	IDoubleThreadChannel();
	virtual ~IDoubleThreadChannel();

protected:
	virtual void submitToLeft(const boost::shared_ptr<T>& event) = 0;
	virtual void submitToRight(const boost::shared_ptr<T>& event) = 0;
	virtual boost::shared_ptr<T> takeFromLeft() = 0;
	virtual boost::shared_ptr<T> takeFromRight() = 0;
};

template <class T>
IDoubleThreadChannel<T>::IDoubleThreadChannel()
{
}

template <class T>
IDoubleThreadChannel<T>::~IDoubleThreadChannel()
{
}
