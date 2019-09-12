#pragma once
#include "IDoubleThreadChannel.h"
#include "../single/QueueThreadChannel.h"


template <class T>
class DoubleQueueThreadChannel : public IDoubleThreadChannel<T>
{
public:
	DoubleQueueThreadChannel(
		const boost::shared_ptr<QueueThreadChannel<T>>& left,
		const boost::shared_ptr<QueueThreadChannel<T>>& right);
	virtual ~DoubleQueueThreadChannel();
protected:
	void submitToLeft(const boost::shared_ptr<T>& event) override;
	void submitToRight(const boost::shared_ptr<T>& event) override;
	boost::shared_ptr<T> takeFromLeft() override;
	boost::shared_ptr<T> takeFromRight() override;

	boost::shared_ptr<QueueThreadChannel<T>> getLeftThreadChannel();
	void setLeftThreadChannel(IThreadChannel<T>*);

	boost::shared_ptr<QueueThreadChannel<T>> getRightThreadChannel();
	void setRightThreadChannel(IThreadChannel<T>*);

protected:
	boost::shared_ptr<QueueThreadChannel<T>> leftThreadChannel;
	boost::shared_ptr<QueueThreadChannel<T>> rightThreadChannel;
};


template <class T>
DoubleQueueThreadChannel<T>::DoubleQueueThreadChannel(
	const boost::shared_ptr<QueueThreadChannel<T>>& left,
	const boost::shared_ptr<QueueThreadChannel<T>>& right)
	:
	leftThreadChannel(left),
	rightThreadChannel(right)
{
}

template <class T>
DoubleQueueThreadChannel<T>::~DoubleQueueThreadChannel()
{
}


template <class T>
void DoubleQueueThreadChannel<T>::submitToLeft(const boost::shared_ptr<T>& event)
{
	leftThreadChannel->submitAsync(event);
}

template <class T>
void DoubleQueueThreadChannel<T>::submitToRight(const boost::shared_ptr<T>& event)
{
	rightThreadChannel->submitAsync(event);
}

template <class T>
boost::shared_ptr<T> DoubleQueueThreadChannel<T>::takeFromLeft()
{
	return leftThreadChannel->takeSync();
}

template <class T>
boost::shared_ptr<T> DoubleQueueThreadChannel<T>::takeFromRight()
{
	return rightThreadChannel->takeSync();
}

template <class T>
boost::shared_ptr<QueueThreadChannel<T>> DoubleQueueThreadChannel<T>::getLeftThreadChannel()
{
	return leftThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setLeftThreadChannel(IThreadChannel<T>* channel)
{
	leftThreadChannel = channel;
}

template <class T>
boost::shared_ptr<QueueThreadChannel<T>> DoubleQueueThreadChannel<T>::getRightThreadChannel()
{
	return rightThreadChannel;
}

template <class T>
void DoubleQueueThreadChannel<T>::setRightThreadChannel(IThreadChannel<T>* channel)
{
	rightThreadChannel = channel;
}
