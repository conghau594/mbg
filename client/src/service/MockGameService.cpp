// MockGameService.cpp
#include <numeric>
#include <format>
#include <boost/uuid.hpp>

#include "base/Logger.h"
#include "base/RandomUtils.h"
#include "MockGameService.h"

namespace bgg
{
  MockGameService::MockGameService(std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(2),
        uuidGenerator_(std::make_shared<boost::uuids::random_generator>()),
        eventBus_(std::move(eventBus))
  {
    [this]<std::size_t... I>(std::index_sequence<I...>)
    {
      ([this]
       {
          using RequestType = ClientRequest::Pack::At<I>;
          auto subscriptionId = eventBus_->subscribe<ClientRequest, RequestType>(
             [this](RequestType const &request)
             {
               sendRequest(request);
               SPDLOG_INFO("A client request of type '{}' has come to '{}'",
                           typeid(RequestType).name(), typeid(*this).name());
             });

          if (!subscriptionId)
          {
            std::string msg = std::format(
              "Cannot subscribe the client request of '{}' from '{}'", 
                typeid(RequestType).name(), 
                typeid(*this).name());
            throw(std::runtime_error(msg));
          }
          subscriptionIdList_.emplace_back(subscriptionId.value());

          SPDLOG_INFO("'{}' has subscribed to client requests of type '{}'",
                     typeid(*this).name(), 
                     typeid(RequestType).name()); }(),
       ...);
    }(std::make_index_sequence<ClientRequest::Pack::Count>{});
  }

  MockGameService::~MockGameService()
  {
    for (auto &id : subscriptionIdList_)
    {
      eventBus_->unsubscribe<ClientRequest>(id);
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
            int failurePercent = 10;
            simulateNetworkLatencyAndFailure(failurePercent, 500, 5000);

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

  void MockGameService::sendRequest(FindGameRequest const & /*findGameRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          std::string msg;
          int errCodeValue = 0;

          try
          {
            int failurePercent = 10;
            simulateNetworkLatencyAndFailure(failurePercent, 200, 1000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          emit(FindGameAcceptedNotification{ErrorCode{errCodeValue, "Mock", msg}});

          msg = "";
          errCodeValue = 0;

          try
          {
            int failurePercent = 10;
            simulateNetworkLatencyAndFailure(failurePercent, 1000, 2000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          std::string gameId = "@Test123";
          int side = 0; // 0: WHITE, 1: BLACK
          emit(FindGameResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }
  // void MockGameService::sendRequest(FindGameRequest const & /*findGameRqt*/) noexcept
  // {
  //   threadPool_.push(
  //       [this]()
  //       {
  //         std::string msg;
  //         int errCodeValue = 0;
  //         std::string gameId;
  //         int side = -1;

  //         try
  //         {
  //           simulateNetworkLatencyAndFailure(10, 200, 1000);

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

  //         emit(FindGameResponse{ErrorCode{errCodeValue, "Mock", msg}, gameId, side});
  //       });
  // }

  void MockGameService::sendRequest(CancelMatchmakingRequest const & /*cancelMatchmakingRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          int errCodeValue = 0;
          std::string msg;
          try
          {
            int failurePercent = 100;
            simulateNetworkLatencyAndFailure(failurePercent, 200, 1000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          emit(CancelMatchmakingResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }

  void MockGameService::sendRequest(MoveRequest const & /*commitMoveRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          std::string msg;
          int errCodeValue = 0;
          try
          {
            int failurePercent = 0;
            simulateNetworkLatencyAndFailure(failurePercent);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
            emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
            return;
          }

          emit(MoveResponse{ErrorCode{errCodeValue, "Mock", msg}});

          msg = "";
          errCodeValue = 0;

          try
          {
            int failurePercent = 0;
            simulateNetworkLatencyAndFailure(failurePercent, 1000, 2000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          std::string gameId = "@Test123 ";
          int side = 0; // 0: WHITE, 1: BLACK
          emit(GameUpdatedNotification{
              Position("a7"),
              Position("a5"),
              "Bishop",
              0,
              0});
        });
  }

  void MockGameService::sendRequest(ResignGameRequest const & /*resignGameRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          std::string msg;
          int errCodeValue = 0;
          try
          {
            int failurePercent = 10;
            simulateNetworkLatencyAndFailure(failurePercent);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
          }

          emit(ResignGameResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }

  void MockGameService::simulateNetworkLatencyAndFailure(
      int failurePercent /*=0*/, int minDelay /*=100*/, int maxDelay /*=2000*/)
  {
    std::this_thread::sleep_for(
        std::chrono::milliseconds(utils::randomInt(minDelay, maxDelay)));

    if (utils::randomInt(0, 99) < failurePercent)
    {
      throw std::runtime_error("Failed to connect to server");
    }
  }

} // namespace bgg