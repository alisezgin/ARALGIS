#pragma once

#ifndef CONCURRENT_QUEUE_ 
#define CONCURRENT_QUEUE_

#include "..\\..\\ARALGIS.h"

#include <queue> 
#include <thread>
#include <mutex>
#include <condition_variable>



// Singleton Class Implementation
// Singleton = There is only 1 (one) controller in the program


template <typename T> class CThreadSafeQueue
{

private:

	static CThreadSafeQueue *singleton; // pointer to the singleton

	std::queue<T> queue_;
	std::mutex mutex_;
	std::condition_variable cond_;


public:
	CThreadSafeQueue()
	{ // private constructor
		//setting up the application
	}

	// Deletes all processor objects created by the controller.
	~CThreadSafeQueue()
	{
		destroy(); //// new for leak
	}

	// Singleton static members
	static CThreadSafeQueue *getInstance()
	{
		if (singleton == 0)
		{
			singleton = new CThreadSafeQueue;

		}

		return singleton;
	}

	// Releases the singleton instance of this controller.
	static void destroy() 
	{
		if (singleton != 0) 
		{
			delete singleton;
			singleton = 0;
		}
	}

public:

	T pop()
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		auto val = queue_.front();
		queue_.pop();
		return val;
	}

	void pop(T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		while (queue_.empty())
		{
			cond_.wait(mlock);
		}
		item = queue_.front();
		queue_.pop();
	}

	void push(const T& item)
	{
		std::unique_lock<std::mutex> mlock(mutex_);
		queue_.push(item);
		mlock.unlock();
		cond_.notify_one();
		SetEvent(g_ErrorOccurredEvent);
	}

	//CThreadSafeQueue() = default;
	CThreadSafeQueue(const CThreadSafeQueue&) = delete;            // disable copying 
	CThreadSafeQueue& operator=(const CThreadSafeQueue&) = delete; // disable assignment 

};

#endif
