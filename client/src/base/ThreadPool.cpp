// ThreadPool.cpp
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

#include <boost/assert.hpp>

#include "ThreadPool.h"

namespace util
{

  ThreadPool::ThreadPool(int threadCount)
      : shouldStop_(false),
        shouldPause_(false)
  {
    BOOST_ASSERT_MSG(threadCount > 0, "Number of theads must be greater 0");

    for (int i = 0; i < threadCount; ++i)
    {
      threadList_.emplace_back(
          [this]
          {
            this->processTaskQueue();
          });

      for (auto &thread : threadList_)
      {
        auto threadId = thread.get_id();
      }
    }
  }

  ThreadPool::~ThreadPool()
  {
    stop();
  }

  void ThreadPool::stop()
  {
    {
      std::lock_guard lock(queueMutex_);
      if (!shouldStop_)
      {

        shouldStop_ = true;
        queueCondition_.notify_all(); // Notify all worker threads to exit
      }
    }

    for (std::thread &thread : threadList_)
    {
      if (thread.joinable())
      {
        thread.join();
      }
    }
  }

  void ThreadPool::pause()
  {
    std::lock_guard lock(queueMutex_);
    if (!shouldPause_)
    {
      shouldPause_ = true;
      queueCondition_.notify_all(); // Notify all worker threads to exit
    }
  }

  void ThreadPool::resume()
  {
    std::lock_guard lock(queueMutex_);
    if (shouldPause_)
    {
      shouldPause_ = false;
      queueCondition_.notify_all(); // Notify all worker threads to pause
    }
  }

  void ThreadPool::clearTasks()
  {
    std::lock_guard<std::mutex> lock(queueMutex_);
    taskQueue_.clear();
  }

  void ThreadPool::processTaskQueue()
  {
    while (true)
    {
      std::function<void()> task;
      { // lock mutex
        std::unique_lock<std::mutex> lock(queueMutex_);

        // Wait until there's a task or the pool is stopping
        queueCondition_.wait(
            lock,
            [this]
            {
              return shouldStop_ || (!taskQueue_.empty() && !shouldPause_);
            });

        if (shouldStop_)
          return;

        // Fetch the next task
        task = std::move(taskQueue_.front());
        taskQueue_.pop_front();
      }

      // Execute the task
      task();
    }
  }
}