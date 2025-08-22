// ChessGameService.h
#pragma once

#include <mutex>
#include <functional>

#include "IGameService.h"

#include "ClientEventBus.h"
#include "ServerMessage.h"

#include "model/Side.h"
#include "model/chess/ChessMove.h"

namespace bgg
{
  class IChessRule;
  class GeminiAgent;
  class Piece;
  class ChessGameService : public IGameService
  {
    std::shared_ptr<GeminiAgent> agent_;
    Side agentColor_;
    Side opponentColor_;
    int const maxPromptRetries_;

    std::shared_ptr<IChessRule> chessRule_;
    std::function<void(ServerMessage const &)> messageSender_;

    std::mutex mutexForThis_;
    std::atomic_bool gameFinished_;

  public:
    ChessGameService(
        std::string apiKey,
        std::shared_ptr<IChessRule> chessRule,
        Side const &agentColor,
        std::function<void(ServerMessage const &)> messageSender) noexcept;

    ~ChessGameService();

  private:
    void handleRequest(ResignGameRequest const &request) override;
    void handleRequest(MoveRequest const &moveRqt) override;

    auto sendMoveRequestToAgent() -> ChessMove::Detail;

    static auto chessMoveDetailToJsonStr(
        ChessMove::Detail const &moveAction) noexcept -> std::string;
    static auto piecePlacementsToJsonStr(
        std::list<std::shared_ptr<Piece>> const &piecePlacements) noexcept -> std::string;
  };

} // namespace bgg
