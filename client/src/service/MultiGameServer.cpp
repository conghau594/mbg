// MultiGameServer.cpp
#include "MultiGameServer.h"
#include "ChessGameService.h"

#include "model/GameType.h"
#include "model/chess/ChessHelpers.h"
#include "model/chess/ChessRule.h"
#include "base/EnvUtils.h"
#include "base/Logger.h"

namespace bgg
{
  MultiGameServer::MultiGameServer(std::shared_ptr<ClientEventBus> eventBus)
      : threadPool_(4),
        eventBus_(std::move(eventBus)),
        currentService_(nullptr)
  {
    //=============================
    auto subscriptionId = eventBus_->subscribe<ClientRequest, LoginRequest>(
        [this](LoginRequest const & /*request*/)
        {
          emit(LoginResponse{ErrorCode{0, "", ""}, ""});
          SPDLOG_INFO(
              "A client request of type 'LoginRequest' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);

    //=============================
    subscriptionId = eventBus_->subscribe<ClientRequest, CancelMatchmakingRequest>(
        [this](CancelMatchmakingRequest const & /*request*/)
        {
          emit(CancelMatchmakingResponse{
              ErrorCode{-1, "", "Cannot cancel matchmaking in MultiGameServer"}});
          SPDLOG_INFO(
              "A client request of type 'CancelMatchmakingRequest' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);

    //=============================
    subscriptionId = eventBus_->subscribe<ClientRequest, FindGameRequest>(
        [this](FindGameRequest const &request)
        {
          handleFindGameRequest(request);
          SPDLOG_INFO(
              "A client request of type 'FindGameRequest' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);

    //=============================
    subscriptionId = eventBus_->subscribe<ClientRequest, MoveRequest>(
        [this](MoveRequest const &request)
        {
          threadPool_.push(
              [this, request]()
              {
                if (currentService_)
                {
                  currentService_->handleRequest(request);
                }
              });

          SPDLOG_INFO(
              "A client request of type 'MoveRequest' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);

    //=============================
    subscriptionId = eventBus_->subscribe<ClientRequest, ResignGameRequest>(
        [this](ResignGameRequest const &request)
        {
          if (currentService_)
          {
            currentService_->handleRequest(request);
          }

          SPDLOG_INFO(
              "A client request of type 'ResignGameRequest' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);

    //=============================
    subscriptionId = eventBus_->subscribe<ServerMessage, GameFinishedNotification>(
        [this](GameFinishedNotification const & /*notif*/)
        {
          threadPool_.push([this]()
                           { currentService_ = nullptr; });

          SPDLOG_INFO(
              "A client request of type 'GameFinishedNotification' has come to '{}'",
              std::string(typeid(*this).name()));
        });
    subscriptionIdList_.emplace_back(*subscriptionId);
  }

  MultiGameServer::~MultiGameServer() noexcept
  {
    for (auto &id : subscriptionIdList_)
    {
      eventBus_->unsubscribe<ClientRequest>(id);
    }
    SPDLOG_DEBUG("MultiGameServer has been destroyed.");
  }

  void MultiGameServer::emit(ServerMessage const &msg) noexcept
  {
    eventBus_->emit<ServerMessage>(msg);
  }

  void MultiGameServer::handleFindGameRequest(FindGameRequest const &findGameRqt) noexcept
  {
    eventBus_->emit(FindGameAcceptedNotification{ErrorCode{0, "", ""}});

    // parse API key from an .env file
    std::map<std::string, std::string> envMap = utils::parseEnvFile(".env");
    envMap.merge(utils::parseEnvFile("./.env"));

    auto envIter = envMap.find("GEMINI_API_KEY");
    std::string geminiApiKey;
    if (envIter == envMap.end())
    {
      std::string msg = "GEMINI_API_KEY is not found!";
      SPDLOG_ERROR(msg);
      eventBus_->emit(FindGameResponse{
          ErrorCode{-1, "", msg},
          findGameRqt.gameType,
          {},           // initialPlacements
          Side{"\0\0"}, // yourSide
          Side{"\0\0"}  // currentTurn
      });
      return;
    }
    else
    {
      geminiApiKey = envIter->second;
    }

    if (findGameRqt.gameType == GameType::CHESS)
    {
      Side playerSide = chess::WHITE;
      Side agentSide = chess::BLACK;
      if (findGameRqt.side == 1)
      {
        playerSide = chess::BLACK;
        agentSide = chess::WHITE;
      }

      std::list<std::tuple<EntityType, Side, Position>>
          standardPiecePlacements(chess::STANDARD_PIECE_PLACEMENTS);

      std::shared_ptr<IChessRule>
          chessRule = std::make_shared<ChessRule>(standardPiecePlacements);

      auto messageSender = [this](ServerMessage const &msg)
      {
        eventBus_->emit(msg);
      };
      currentService_ = std::make_shared<ChessGameService>(
          std::move(geminiApiKey), chessRule, agentSide, messageSender);

      eventBus_->emit(FindGameResponse{
          {0, "", "Mock"}, // error code
          findGameRqt.gameType,
          standardPiecePlacements, // initialBoard
          playerSide,              // yourSide
          chess::WHITE             // currentTurn
      });

      if (playerSide == chess::BLACK)
      {
        eventBus_->emit(MoveRequest{
            "",          // userId
            "",          // gameId
            std::nullopt // empty move
        });
      }
    }
    else
    {
      eventBus_->emit(FindGameResponse{
          ErrorCode{-1, "", "Unsupported game type"},
          findGameRqt.gameType,
          {},           // initialPlacements
          Side{"\0\0"}, // yourSide
          Side{"\0\0"}  // currentTurn
      });
      return;
    }
  }

} // namespace bgg
