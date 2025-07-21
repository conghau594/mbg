// ChessBoardState.cpp

#include "ChessBoardState.h"
#include "base/Logger.h"

namespace bgg
{
  ChessBoardState::ChessBoardState(std::string color) noexcept
      : piecePlacements_(ChessRule::INITIAL_PLACEMENTS.begin(),
                         ChessRule::INITIAL_PLACEMENTS.end()),
        yourColor_(std::move(color))
  {
    BGG_VALIDATE_COLOR(yourColor_);
  }

  ChessBoardState::ChessBoardState(
      std::map<Position, Piece> piecePlacements,
      std::string color) noexcept
      : piecePlacements_(std::move(piecePlacements)),
        yourColor_(std::move(color))
  {
    BGG_VALIDATE_COLOR(yourColor_);
  }

  auto ChessBoardState::getYourColor() const noexcept -> std::string const &
  {
    return yourColor_;
  }

  auto ChessBoardState::getPiecePlacements() const noexcept
      -> std::map<Position, Piece> const &
  {
    return piecePlacements_;
  }

  auto ChessBoardState::getSelectablePieces(std::string const &color) const noexcept
      -> std::map<Position, Piece>
  {
    std::map<Position, Piece> selectablePieces;
    for (auto &[square, piece] : piecePlacements_)
    {
      if (piece.color != color)
      {
        continue;
      }

      if (piece.type == ChessRule::QUEEN || piece.type == ChessRule::KING)
      {
        if (ChessRule::isOrthogonalMovePossible(piecePlacements_, square, color) ||
            ChessRule::isDiagonalMovePossible(piecePlacements_, square, color))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::ROOK)
      {
        if (ChessRule::isOrthogonalMovePossible(piecePlacements_, square, color))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::BISHOP)
      {
        if (ChessRule::isDiagonalMovePossible(piecePlacements_, square, color))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::KNIGHT)
      {
        if (ChessRule::isKnightMovePossible(piecePlacements_, square, color))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::PAWN)
      {
        if (ChessRule::isPawnBasicMovePossible(piecePlacements_, square, color) ||
            getEnPassantSquare(square, color))
        {
          selectablePieces.emplace(square, piece);
        }
      }
    }

    return selectablePieces;
  }

  auto ChessBoardState::collectCandidateMoves(
      Position const &square) const noexcept -> CandidateChessMoveInfo
  {
    CandidateChessMoveInfo candidateMoves;

    ChessRule::collectBasicCandidateMoves(
        piecePlacements_,
        square,
        &candidateMoves.quietSquares,
        &candidateMoves.captureSquares);

    std::optional<Piece> piece = ChessRule::getPiece(piecePlacements_, square);
    if (!piece)
    {
      return candidateMoves; // empty
    }

    if (piece->type == ChessRule::PAWN)
    {
      auto &specialMoveTracker = moveTracker_.getSpecialMoveTracker(piece->color);
      int firstDoubleStep = specialMoveTracker.getMoveInfoOfPawn(int(square[0]));
      if (firstDoubleStep == 0) // the pawn has never moved
      {
        if (candidateMoves.quietSquares.empty())
        {
          return candidateMoves;
        }

        int step = candidateMoves.quietSquares.front()[1] - square[1];
        Position front2{square[0], char(square[1] + 2 * step), '\0'};
        if (piecePlacements_.find(front2) == piecePlacements_.end())
        {
          candidateMoves.quietSquares.emplace_back(front2);
        }
      }
      else if (auto enPassantSqr = getEnPassantSquare(square, piece->color))
      {
        candidateMoves.specialMoveSquares.emplace_back(
            std::move(enPassantSqr.value()));
      }
    }
    else if (piece->type == ChessRule::KING)
    {
      candidateMoves.specialMoveSquares = getKingCastlingSquares(piece->color);
    }

    return candidateMoves;
  }

  auto ChessBoardState::getPiece(
      Position const &square) const noexcept -> std::optional<Piece>
  {
    return ChessRule::getPiece(piecePlacements_, square);
  }

  auto ChessBoardState::tryMove(
      ChessMove const &move) const noexcept -> ChessMove::Action
  {
    // ========================================
    // check simple conditions
    std::optional<Piece> movedPiece = getPiece(move.fromSquare);
    if (!movedPiece)
    {
      return ChessMove::Invalid{ChessMove::Error::INVALID_SOURCE_SQUARE};
    }

    std::optional<Piece> capturedPiece = getPiece(move.toSquare);
    if (capturedPiece && (movedPiece->color == capturedPiece->color))
    {
      return ChessMove::Invalid{ChessMove::Error::INVALID_DESTINATION_SQUARE};
    }

    // ========================================
    // check the move based on the CandidateChessMoveInfo
    CandidateChessMoveInfo candidateMoves = collectCandidateMoves(move.fromSquare);

    ChessMove::Action moveAction;
    auto enemyKingSqr = ChessRule::findKingSquare(
        piecePlacements_,
        ChessRule::getEnemyColor(movedPiece->color));

    BOOST_ASSERT_MSG(
        enemyKingSqr,
        "There must be a king of enemy of 'color' on the chess board");

    // --------------------
    // 1st, check if the move is a normal move
    for (auto &sqr : candidateMoves.quietSquares)
    {
      if (sqr == move.toSquare)
      {
        moveAction = ChessMove::Normal{
            move.fromSquare, move.toSquare, *enemyKingSqr, KingState::SAFE};
        break;
      }
    }

    // --------------------
    // 2nd, check if the move is a normal capture move
    if (moveAction.isEmpty())
    {
      for (auto &sqr : candidateMoves.captureSquares)
      {
        if (sqr == move.toSquare)
        {
          moveAction = ChessMove::Normal{
              move.fromSquare, move.toSquare, *enemyKingSqr, KingState::SAFE};
          break;
        }
      }
    }

    // --------------------
    // 3rd, check if the move is a special move
    if (moveAction.isEmpty())
    {
      // 3.1: if the special move is of the king...
      if (movedPiece->type == ChessRule::KING)
      {
        for (auto &sqr : candidateMoves.specialMoveSquares)
        {
          if (sqr != move.toSquare)
          {
            continue;
          }

          KingState allyKingState = ChessRule::evaluateKingState(
              piecePlacements_, movedPiece->color, false);
          if (allyKingState == KingState::IN_CHECK)
          {
            return ChessMove::Invalid{ChessMove::Error::INVALID_CASTLING};
          }

          std::pair<Position, Position> castlingRookMove =
              ChessRule::getCastlingRookMove(move.toSquare, movedPiece->color);

          // TODO: consider using ChessRule::commitMoveAction(ChessMove::Normal
          //       "King to the next left or right square") instead
          std::map<Position, Piece> copiedPiecePlacements = piecePlacements_;
          copiedPiecePlacements.erase(move.fromSquare);

          Position midwaySquare{
              char((move.fromSquare[0] + castlingRookMove.second[0]) / 2),
              move.fromSquare[1],
              '\0'};
          copiedPiecePlacements.emplace(midwaySquare, *movedPiece);

          allyKingState = ChessRule::evaluateKingState(
              copiedPiecePlacements, movedPiece->color, false);
          if (allyKingState == KingState::IN_CHECK)
          {
            return ChessMove::Invalid{ChessMove::Error::INVALID_CASTLING};
          }

          moveAction = ChessMove::Castling{
              move.fromSquare,
              move.toSquare,
              castlingRookMove.first,
              castlingRookMove.second,
              *enemyKingSqr,
              KingState::SAFE};
          break;
        }
      }
      // 3.2: if the special move is of the pawn...
      else if (movedPiece->type == ChessRule::PAWN &&
               !candidateMoves.specialMoveSquares.empty())
      {
        Position const &enPassantSqr = candidateMoves.specialMoveSquares.front();
        if (enPassantSqr[0] == move.toSquare[0] &&
            enPassantSqr[1] == move.fromSquare[1])
        {
          moveAction = ChessMove::EnPassant{
              move.fromSquare,
              move.toSquare,
              enPassantSqr,
              *enemyKingSqr,
              KingState::SAFE};
        }
      }
    }
    else if (ChessRule::isPromotionSquare(move.toSquare, movedPiece->color) &&
             (movedPiece->type == ChessRule::PAWN))
    {
      // --------------------
      // 4th, check if the move is a promotion
      if (move.promote)
      {
        auto normalMove = moveAction.getIf<ChessMove::Normal>();

        moveAction = ChessMove::Promotion{
            normalMove->fromSquare,
            normalMove->toSquare,
            *(move.promote),
            *enemyKingSqr,
            normalMove->enemyKingState};
      }
      else
      {
        return ChessMove::Invalid{ChessMove::Error::INVALID_PROMOTION};
      }
    }

    if (moveAction.isEmpty())
    {
      return ChessMove::Invalid{ChessMove::Error::INVALID_DESTINATION_SQUARE};
    }

    // ========================================
    // With a valid moveAction, check if the 2 kings are in check
    std::map<Position, Piece> copiedPiecePlacements = piecePlacements_;
    ChessRule::commitMoveAction(copiedPiecePlacements, moveAction);

    KingState allyKingState = ChessRule::evaluateKingState(
        copiedPiecePlacements, movedPiece->color, false);
    if (allyKingState == KingState::IN_CHECK)
    {
      return ChessMove::Invalid{ChessMove::Error::KING_EXPOSED};
    }

    std::string enemyColor = ChessRule::getEnemyColor(movedPiece->color);
    KingState enemyKingState = ChessRule::evaluateKingState(
        copiedPiecePlacements, enemyColor, true);

    auto moveActionVisitor = [&enemyKingState]<typename T>(T &action)
    {
      if constexpr (requires { {action.enemyKingState}->std::same_as<KingState&>; })
      {
        action.enemyKingState = enemyKingState;
      }
    };
    moveAction.visit(moveActionVisitor);

    return moveAction;
  }

  void ChessBoardState::commitMove(ChessMove::Action const &moveAction) noexcept
  {
    auto moveActionVisitor = [this]<typename T>(T const &action)
    {
      if constexpr (requires {
        { action.fromSquare } -> std::same_as<Position const &>;
        { action.toSquare } -> std::same_as<Position const &>; })
      {
        SPDLOG_INFO("You have committed a '{}'", typeid(T).name());

        auto piece = getPiece(action.fromSquare);
        BOOST_ASSERT_MSG(piece, "There must be an item at the 'fromTile'");

        updateMoveTracker(*piece, action.fromSquare, action.toSquare);

        piecePlacements_.erase(action.toSquare);
        piecePlacements_.erase(action.fromSquare);

        if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
        {
          BGG_VALIDATE_CHESS_PIECE(action.promote);
          piece->type = action.promote;
          piecePlacements_.emplace(action.toSquare, std::move(*piece));
        }
        else
        {
          piecePlacements_.emplace(action.toSquare, std::move(*piece));

          if constexpr (requires {
            { action.enPassantSquare } -> std::same_as<Position const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
          {
            // remove en passant captured item
            piecePlacements_.erase(
                Position{action.toSquare[0], action.fromSquare[1]});
          }
          else if constexpr (requires {
            { action.rookSource } -> std::same_as<Position const &>;
            { action.rookDestination } -> std::same_as<Position const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
          {
            // move the related rook to the destination
            auto rook = getPiece(action.rookSource);
            BOOST_ASSERT_MSG(
                rook, "There must be an item at the tile of action.rookSource");

            piecePlacements_.emplace(action.rookDestination, std::move(*rook));
            piecePlacements_.erase(action.rookSource);
          }
        }
      }
      else if constexpr (std::is_same_v<T, std::monostate>)
      {
        SPDLOG_WARN("You have committed an empty move");
      }
      else if constexpr (std::is_same_v<T, ChessMove::Invalid>)
      {
        SPDLOG_WARN("You have committed an 'ChessMove::Invalid'");
      }
    };

    moveAction.visit(moveActionVisitor);
  }

  auto ChessBoardState::getEnPassantSquare(
      Position const &square,
      std::string const &color) const noexcept -> std::optional<Position>
  {
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return std::nullopt;
    }

    std::optional<Position> enPassantSquare;
    auto const &specialMoveTracker = moveTracker_.getSpecialMoveTracker(color);
    int const col = int(square[0]);
    if (col > ChessRule::FIRST_COL)
    {
      if (specialMoveTracker.getMoveInfoOfPawn(col - 1) ==
          moveTracker_.getTotalMoveCount())
      {
        return Position{char(col - 1), square[1], '\0'};
      }
    }
    else if (col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getMoveInfoOfPawn(col + 1) ==
          moveTracker_.getTotalMoveCount())
      {
        return Position{char(col + 1), square[1], '\0'};
      }
    }

    return std::nullopt;
  }

  auto ChessBoardState::getKingCastlingSquares(
      std::string const &color) const noexcept -> std::list<Position>
  {
    BGG_VALIDATE_COLOR(color);

    Position kingSquare{"e1"}; // if (color == Color::WHITE)
    if (color == Color::BLACK)
    {
      kingSquare = Position{"e8"};
    }

    std::list<Position> result;
    auto const &specialMoveTracker = moveTracker_.getSpecialMoveTracker(color);
    if (specialMoveTracker.isKingMoved())
    {
      return result;
    }

    if (!specialMoveTracker.isRookAMoved())
    {
      result.emplace_back(Position{char(kingSquare[0] - 2), kingSquare[1], '\0'});
    }

    if (!specialMoveTracker.isRookHMoved())
    {
      result.emplace_back(Position{char(kingSquare[0] + 2), kingSquare[1], '\0'});
    }

    return result;
  }

  void ChessBoardState::updateMoveTracker(Piece const &piece, Position const &fromSquare, Position const &toSquare) noexcept
  {
    moveTracker_.increaseTotalMoveCount();

    auto &specialMoveTracker = moveTracker_.getSpecialMoveTracker(piece.color);
    if (piece.type == ChessRule::KING && !specialMoveTracker.isKingMoved())
    {
      specialMoveTracker.markKingMoved();
    }
    else if (piece.type == ChessRule::ROOK)
    {
      if (fromSquare[0] == 'a' && !specialMoveTracker.isRookAMoved())
      {
        specialMoveTracker.markRookAMoved();
      }
      if (fromSquare[0] == 'h' && !specialMoveTracker.isRookHMoved())
      {
        specialMoveTracker.markRookHMoved();
      }
    }
    else if (piece.type == ChessRule::PAWN &&
             specialMoveTracker.getMoveInfoOfPawn(int(fromSquare[0])) == 0) ///< if this pawn has never moved
    {
      int moveStep = toSquare[1] - fromSquare[1];
      if (moveStep == 1)
      {
        specialMoveTracker.markPawnMoved(int(toSquare[0]), -1);
      }
      else // if (moveStep == +- 2)
      {
        specialMoveTracker.markPawnMoved(
            int(toSquare[0]), moveTracker_.getTotalMoveCount());
      }
    }
  }

} // namespace bgg
