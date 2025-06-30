// MockGameService.cpp
#include <iostream>
#include <numeric>
#include <boost/uuid.hpp>

#include "base/RandomUtils.h"
#include "MockGameService.h"

namespace bgg
{
  MockGameService::MockGameService(std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(1),
        uuidGenerator_(std::make_shared<boost::uuids::random_generator>()),
        eventBus_(std::move(eventBus))
  {
    std::optional<std::size_t> subscriptionId;
    subscriptionId = eventBus_->subscribe<ClientEvent, LoginRequest>(
        [this](LoginRequest const &loginRqt)
        {
          sendRequest(loginRqt);
          std::cout << "\nListen from MockGameService."
                    << "\n    Username = " << loginRqt.username
                    << "\n    Password = " << loginRqt.password;
        });
    if (!subscriptionId)
    {
      throw(std::runtime_error("Cannot subscribe LoginRequest from MockGameService"));
    }
    subscriptionIdList_.emplace_back(subscriptionId.value());
#ifdef _DEBUG
    std::clog << "\nMockGameService has subscribed to ClientEvent of "
              << typeid(LoginRequest).name() << " successfully";
#endif
  }

  MockGameService::~MockGameService()
  {
    for (auto &id : subscriptionIdList_)
    {
      eventBus_->unsubscribe<ClientEvent>(id);
    }
  }

  void MockGameService::emit(ServerMessage const &msg) noexcept
  {
    eventBus_->emit<ServerMessage>(msg);
  }

  void MockGameService::sendRequest(LoginRequest const & /*loginRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          int errCodeValue = 0;
          std::string msg;
          std::string userId;
          try
          {
            simulateNetworkLatencyAndFailure(10, 500, 5000);

            // login successfully, the create user ID
            boost::uuids::uuid id = (*uuidGenerator_)();
            if (id.is_nil())
            {
              msg = "Failed to create UUID";
              errCodeValue = -1;
            }
            else
            {
              userId = boost::uuids::to_string(id);
            }
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          emit(LoginResponse{ErrorCode{errCodeValue, "Mock", msg}, userId});
        });
  }

  // std::future<ServerMessage> MockGameService::operator()(
  //     ClientEvent::FindGameRequest const &) noexcept
  // {
  //   auto futureResponse = threadPool_.push(
  //       [this]() -> ServerMessage
  //       {
  //         std::string msg;
  //         int errCodeValue = 0;
  //         std::string gameId;
  //         int side = -1;

  //         try
  //         {
  //           simulateNetworkLatencyAndFailure(10, 12000, 15000);

  //           // FindGameRequest successfully, then create user ID
  //           boost::uuids::uuid id = (*uuidGenerator_)();
  //           if (id.is_nil())
  //           {
  //             msg = "Failed to create UUID";
  //             errCodeValue = -1;
  //           }
  //           else
  //           {
  //             gameId = boost::uuids::to_string(id);
  //             side = std::abs(std::accumulate(gameId.begin(), gameId.end(), 0)) % 2;
  //           }
  //         }
  //         catch (std::runtime_error const &e)
  //         {
  //           msg = e.what();
  //           errCodeValue = -1;
  //         }

  //         return ServerMessage::FindGameRequest{
  //             ErrorCode{errCodeValue, "Mock", msg}, gameId, side};
  //       });

  //   return futureResponse;
  // }

  // std::future<ServerMessage> MockGameService::operator()(
  //     ClientEvent::CancelMatchmaking const &) noexcept
  // {
  //   auto futureResponse = threadPool_.push(
  //       [this]() -> ServerMessage
  //       {
  //         int errCodeValue = 0;
  //         std::string msg;
  //         try
  //         {
  //           simulateNetworkLatencyAndFailure(100, 200, 1000);
  //         }
  //         catch (std::runtime_error const &e)
  //         {
  //           msg = e.what();
  //           errCodeValue = -1;
  //         }

  //         return ServerMessage::CancelMatchmaking{ErrorCode{errCodeValue, "Mock", msg}};
  //       });

  //   return futureResponse;
  // }

  // std::future<ServerMessage> MockGameService::operator()(
  //     ClientEvent::CommitMove const &commitMoveRqt) noexcept
  // {
  //   return std::future<ServerMessage>();
  // }

  // std::future<ServerMessage> MockGameService::operator()(
  //     ClientEvent::ResignGame const &resignGameRqt) noexcept
  // {
  //   return std::future<ServerMessage>();
  // }

  // void MockGameService::cancelMatchmaking(
  //     std::string const & /*userId*/,
  //     Callback const &callback) noexcept
  // {
  //   threadPool_.push(
  //       [this, &callback]()
  //       {
  //         if (!isConnected_)
  //         {
  //           std::string msg;
  //           int code = 0;
  //           try
  //           {
  //             simulateNetworkLatencyAndFailure(10);
  //           }
  //           catch (std::exception const &e)
  //           {
  //             msg = e.what();
  //           }

  //           // connect successfully
  //           isConnected_ = true;
  //           callback(code, msg);
  //         }
  //       });
  // }

  // void MockGameService::resignGame(
  //     std::string const & /*userId*/,
  //     std::string const & /*gameId*/,
  //     Callback const &callback) noexcept
  // {
  //   threadPool_.push(
  //       [this, &callback]()
  //       {
  //         if (!isConnected_)
  //         {
  //           std::string msg;
  //           int code = 0;
  //           try
  //           {
  //             simulateNetworkLatencyAndFailure(10);
  //           }
  //           catch (std::exception const &e)
  //           {
  //             msg = e.what();
  //           }

  //           // resign successfully
  //           isConnected_ = true;
  //           callback(code, msg);
  //         }
  //       });
  // }

  // void MockGameService::commitMove(
  //     std::string const & /*userId*/,
  //     std::string const & /*gameId*/,
  //     Move const *const /*move*/,
  //     Callback const &callback) noexcept
  // {
  //   threadPool_.push(
  //       [this, &callback]()
  //       {
  //         if (!isConnected_)
  //         {
  //           std::string msg;
  //           int code = 0;
  //           try
  //           {
  //             simulateNetworkLatencyAndFailure(30);
  //           }
  //           catch (std::exception const &e)
  //           {
  //             msg = e.what();
  //           }

  //           // commit move successfully
  //           isConnected_ = true;
  //           callback(code, msg);
  //         }
  //       });
  // }

  void MockGameService::simulateNetworkLatencyAndFailure(
      int failurePercent /*=0*/, int minDelay /*=100*/, int maxDelay /*=2000*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(util::randomInt(minDelay, maxDelay)));

    if (util::randomInt(1, 100) < failurePercent)
    {
      throw std::runtime_error("Failed to connect to server");
    }
  }

} // namespace bgg