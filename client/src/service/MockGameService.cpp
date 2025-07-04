// MockGameService.cpp
#include <iostream>
#include <numeric>
#include <boost/uuid.hpp>

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
         using RequestType = ClientEvent::Pack::At<I>;
         auto subscriptionId = eventBus_->subscribe<ClientEvent, RequestType>(
             [this](RequestType const &request)
             {
               sendRequest(request);
               std::cout << "\nMockGameService has received ClientEvent of "
                         << typeid(RequestType).name();
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
       }(),
       ...);
    }(std::make_index_sequence<ClientEvent::Pack::Count>{});
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

  void MockGameService::sendRequest(FindGameRequest const & /*findGameRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          std::string msg;
          int errCodeValue = 0;

          try
          {
            simulateNetworkLatencyAndFailure(10, 200, 1000);
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
            simulateNetworkLatencyAndFailure(10, 1000, 2000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }
          
          std::string gameId = "@Test123";
          int side = 0; //0: WHITE, 1: BLACK
          emit(FindGameResponse{ErrorCode{errCodeValue, "Mock", msg}, std::move(gameId), side});
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
            simulateNetworkLatencyAndFailure(100, 200, 1000);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
            errCodeValue = -1;
          }

          emit(CancelMatchmakingResponse{ErrorCode{errCodeValue, "Mock", msg}});
        });
  }

  void MockGameService::sendRequest(CommitMoveRequest const & /*commitMoveRqt*/) noexcept
  {
    threadPool_.push(
        [this]()
        {
          std::string msg;
          int errCodeValue = 0;
          try
          {
            simulateNetworkLatencyAndFailure(30);
          }
          catch (std::runtime_error const &e)
          {
            msg = e.what();
          }

          emit(CommitMoveResponse{ErrorCode{errCodeValue, "Mock", msg}});
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
            simulateNetworkLatencyAndFailure(10);
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
        std::chrono::milliseconds(util::randomInt(minDelay, maxDelay)));

    if (util::randomInt(1, 100) < failurePercent)
    {
      throw std::runtime_error("Failed to connect to server");
    }
  }

} // namespace bgg