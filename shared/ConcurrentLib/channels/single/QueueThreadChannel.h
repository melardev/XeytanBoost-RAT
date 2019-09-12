#pragma once
#include "IThreadChannel.h"
#include <queue>
#include <boost/thread/condition_variable.hpp>

template <class T>
class QueueThreadChannel : public IThreadChannel<T>
{
public:
	QueueThreadChannel();
	virtual ~QueueThreadChannel() = 0;

	boost::shared_ptr<T> takeSync() override;
	void submitAsync(const boost::shared_ptr<T>&) override;
protected:
	std::queue<boost::shared_ptr<T>> objectsQueue;
	boost::condition_variable cv;
	boost::mutex mu;
};

template <class T>
QueueThreadChannel<T>::QueueThreadChannel()
{
}

template <class T>
QueueThreadChannel<T>::~QueueThreadChannel()
{
}

template <class T>
boost::shared_ptr<T> QueueThreadChannel<T>::takeSync()
{
	boost::unique_lock<boost::mutex> lock(mu);
	// same as below:
	// while (queue.size() == 0) {
	//   empty.wait(lock);
	// }
	// cv.wait(lock, [&]() { return queue_.size() != 0; });
	if (objectsQueue.size() > 0)
	{
		auto event = objectsQueue.front();
		objectsQueue.pop();
		return event;
	}
	else
	{
		cv.wait(lock, [&]() { return objectsQueue.size() != 0; });
		auto event = objectsQueue.front();
		objectsQueue.pop();
		return event;
	}
}

template <class T>
void QueueThreadChannel<T>::submitAsync(const boost::shared_ptr<T>& object)
{
	boost::unique_lock<boost::mutex> lk(mu);
	objectsQueue.push(object);
	cv.notify_one(); // Notify one worker
}
