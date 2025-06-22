// ThreadPool.h
#pragma once

#include <functional>
#include <list>
#include <condition_variable>
#include <future>

namespace util
{

  /////////////////////////////////////////////////////////////////////////////
  /**
   * \brief
   *
   */
  class ThreadPool final
  {
    std::list<std::thread> threadList_;          ///< List holding all worker threads
    std::list<std::function<void()>> taskQueue_; ///< Task queue: holds tasks to be executed

    std::mutex queueMutex_;                  ///< for protecting the task queue
    std::condition_variable queueCondition_; ///< used along with the mutex

    bool shouldStop_;
    bool shouldPause_;

  public:
    explicit ThreadPool(int threadCount);
    ~ThreadPool();

    template <class F, class... ARGS>
    auto push(F &&f, ARGS &&...args)
        -> std::future<std::invoke_result_t<F, ARGS...>>;

    void stop();
    void pause();
    void resume();
    void clearTasks();

  private:
    void processTaskQueue();
  };

  //-------------------------------------------------------------------------
  /**
   * \brief
   *
   */
  template <class F, class... ARGS>
  auto ThreadPool::push(F &&f, ARGS &&...args)
      -> std::future<std::invoke_result_t<F, ARGS...>>
  {
    using ReturnType = std::invoke_result_t<F, ARGS...>;

    std::lock_guard lock(queueMutex_);
    if (shouldStop_)
    {
      return std::future<ReturnType>();
    }

    // Create a packaged_task wrapping the callable
    auto packagedTask = std::make_shared<std::packaged_task<ReturnType()>>(
        std::bind(std::forward<F>(f), std::forward<ARGS>(args)...));

    std::future<ReturnType> result = packagedTask->get_future();

    taskQueue_.emplace_back([packagedTask]()
                            { (*packagedTask)(); });

    queueCondition_.notify_one(); // Notify one worker thread
    return result;
  }

}
