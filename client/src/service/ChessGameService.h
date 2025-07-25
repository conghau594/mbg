// ChessGameService.h
#pragma once

#include "GameService.h"

#include "ClientEventBus.h"
#include "GeminiAgent.h"
#include "ServerMessage.h"

#include "base/ThreadPool.h"
#include "model/Piece.h"
#include "model/chess/ChessMove.h"

namespace bgg
{
  class ChessBoardState;
  class ChessGameService : public GameService
  {
    utils::ThreadPool threadPool_;
    std::shared_ptr<ClientEventBus> eventBus_;
    std::vector<std::size_t> subscriptionIdList_;

    GeminiAgent agent_;

    std::shared_ptr<ChessBoardState> chessRule_;
    std::string agentColor_;
    // std::string moveHistoryStr_;

  public:
    ChessGameService(
        std::string apiKey,
        std::shared_ptr<ChessBoardState> chessRule,
        std::shared_ptr<ClientEventBus> eventBus);

    ~ChessGameService();

  private:
    void emit(ServerMessage const &msg) noexcept override;

    void handleMoveRequest(MoveRequest const &moveRqt) noexcept;

    void sendChessGamePromptToAgent();

    static auto chessMoveActionToJsonString(
        ChessMove::Action const &moveAction) noexcept -> std::string;
  };

} // namespace bgg
