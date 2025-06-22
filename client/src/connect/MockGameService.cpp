// MockGameService.cpp
#include <iostream>

#include <boost/uuid.hpp>

#include "base/RandomUtils.h"
#include "MockGameService.h"

namespace iab
{
  MockGameService::MockGameService() noexcept
      : threadPool_(1),
        uuidGenerator_(new boost::uuids::random_generator),
        isConnected_(false)
  {
  }

  void MockGameService::connect(
      std::string const &userId,
      Callback const &callback) noexcept
  {
    threadPool_.push(
        [this, callback]()
        {
          std::string msg;
          int code = 0;
          if (!isConnected_)
          {
            try
            {
              simulateNetworkLatencyAndFailure(100, 2000, 5000);
              // connect successfully
              isConnected_ = true;
            }
            catch (std::runtime_error const &e)
            {
              msg = e.what();
              code = -1;
              isConnected_ = false;
            }
          }
          callback(code, msg);
        });
  }

  void MockGameService::disconnect() noexcept
  {
    isConnected_ = false;
  }

  auto MockGameService::isConnected() const noexcept -> bool
  {
    return isConnected_;
  }

  void MockGameService::cancelMatchmaking(
      std::string const & /*userId*/,
      Callback const &callback) noexcept
  {
    threadPool_.push(
        [this, &callback]()
        {
          if (!isConnected_)
          {
            std::string msg;
            int code = 0;
            try
            {
              simulateNetworkLatencyAndFailure(10);
            }
            catch (std::exception const &e)
            {
              msg = e.what();
            }

            // connect successfully
            isConnected_ = true;
            callback(code, msg);
          }
        });
  }

  auto MockGameService::findOpponent(
      std::string const & /*userId*/,
      int /*gameType*/,
      int /*playerType*/,
      Callback const &callback) noexcept -> std::string
  {
    threadPool_.push(
        [this, &callback]()
        {
          std::string msg;
          int code = 0;

          if (!isConnected_)
          {
            try
            {
              simulateNetworkLatencyAndFailure(50, 2000, 5000);
            }
            catch (std::exception const &e)
            {
              msg = e.what();
              code = -1;
            }

            // find opponent successfully, the create game UUID
            boost::uuids::uuid id = (*uuidGenerator_)();
            if (id.is_nil())
            {
              msg = "Failed to create UUID";
              code = -1;
            }

            isConnected_ = true;
          }
          callback(code, msg);
        });
    return "" /*boost::uuids::to_string(id)*/;
  }

  void MockGameService::resignGame(
      std::string const & /*userId*/,
      std::string const & /*gameId*/,
      Callback const &callback) noexcept
  {
    threadPool_.push(
        [this, &callback]()
        {
          if (!isConnected_)
          {
            std::string msg;
            int code = 0;
            try
            {
              simulateNetworkLatencyAndFailure(10);
            }
            catch (std::exception const &e)
            {
              msg = e.what();
            }

            // resign successfully
            isConnected_ = true;
            callback(code, msg);
          }
        });
  }

  void MockGameService::commitMove(
      std::string const & /*userId*/,
      std::string const & /*gameId*/,
      Move const *const /*move*/,
      Callback const &callback) noexcept
  {
    threadPool_.push(
        [this, &callback]()
        {
          if (!isConnected_)
          {
            std::string msg;
            int code = 0;
            try
            {
              simulateNetworkLatencyAndFailure(30);
            }
            catch (std::exception const &e)
            {
              msg = e.what();
            }

            // commit move successfully
            isConnected_ = true;
            callback(code, msg);
          }
        });
  }

  void MockGameService::simulateNetworkLatencyAndFailure(
      int failurePercent /*=0*/, int minDelay /*=100*/, int maxDelay /*=2000*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(minDelay, maxDelay)));

    if (util::randomInt(1, 100) < failurePercent)
    {
      isConnected_ = false;
      throw std::runtime_error("Failed to connect to server");
    }
  }

} // namespace iab