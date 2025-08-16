// ChessGameService.h
#pragma once

#include <mutex>

#include "GameService.h"

#include "ClientEventBus.h"
#include "ServerMessage.h"

#include "base/ThreadPool.h"
#include "model/Side.h"
#include "model/chess/ChessMove.h"

namespace bgg
{
  class IChessRule;
  class GeminiAgent;
  class Piece;
  class ChessGameService : public GameService
  {
    utils::ThreadPool threadPool_;
    std::shared_ptr<ClientEventBus> eventBus_;
    std::vector<std::size_t> subscriptionIdList_;

    std::shared_ptr<GeminiAgent> agent_;
    Side agentColor_;
    std::atomic_int maxPromptRetries_;

    std::shared_ptr<IChessRule> chessRule_;
    std::mutex mutexForThis_;
    // std::string moveHistoryStr_;

  public:
    ChessGameService(
        std::string apiKey,
        std::shared_ptr<IChessRule> chessRule,
        Side const &agentColor,
        std::shared_ptr<ClientEventBus> eventBus);

    ~ChessGameService();

  private:
    void emit(ServerMessage const &msg) noexcept override;

    void requestMoveFromAgent();

    static auto chessMoveDetailToJsonStr(
        ChessMove::Detail const &moveAction) noexcept -> std::string;
    static auto piecePlacementsToJsonStr(
        std::list<std::shared_ptr<Piece>> const &piecePlacements) noexcept -> std::string;
  };

} // namespace bgg
