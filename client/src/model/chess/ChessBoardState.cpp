// ChessBoardState.cpp

#include "ChessBoardState.h"

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

  auto ChessBoardState::getPieceColor(Position const &square) const noexcept
      -> std::optional<std::string>
  {
    std::optional<Piece> piece = getPiece(square);
    if (!piece)
    {
      return std::nullopt;
    }
    return piece->color;
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
        if (ChessRule::isNormalPawnMovePossible(piecePlacements_, square, color) ||
            !getEnPassantSquare(square, color).empty())
        {
          selectablePieces.emplace(square, piece);
        }
      }
    }

    return selectablePieces;
  }

  auto ChessBoardState::getCandidateMoves(Position const &square) const noexcept
      -> CandidateChessMoveInfo
  {
    BGG_VALIDATE_SQUARE(square);

    std::optional<Piece> piece = getPiece(square);
    CandidateChessMoveInfo candidateMoves;
    if (!piece)
    {
      return candidateMoves; // empty
    }

    if (piece->type == ChessRule::QUEEN)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);

      ChessRule::collectDiagonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);
    }
    else if (piece->type == ChessRule::ROOK)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);
    }
    else if (piece->type == ChessRule::BISHOP)
    {
      ChessRule::collectDiagonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);
    }
    else if (piece->type == ChessRule::KNIGHT)
    {
      ChessRule::collectKnightReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);
    }
    else if (piece->type == ChessRule::PAWN)
    {
      ChessRule::collectPawnBasicReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);

      candidateMoves.specialSquares = getEnPassantSquare(square, piece->color);
    }
    else // if (piece->type == ChessRule::KING)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);

      ChessRule::collectDiagonalReachableSquares(
          piecePlacements_,
          square,
          piece->color,
          1,
          &candidateMoves.quietSquares,
          &candidateMoves.captureSquares);

      candidateMoves.specialSquares = getValidCastlingSquares(piece->color);
    }

    return candidateMoves;
  }

  auto ChessBoardState::getPiece(
      Position const &square) const noexcept -> std::optional<Piece>
  {
    BGG_VALIDATE_SQUARE(square);
    auto found = piecePlacements_.find(square);
    if (found == piecePlacements_.cend())
    {
      return std::nullopt;
    }

    return found->second;
  }

  auto ChessBoardState::tryMove(
      ChessMove const &move) const noexcept -> ChessMove::VariantAction
  {
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

    // ====================
    // check the move based on the CandidateChessMoveInfo
    CandidateChessMoveInfo candidateMoves = getCandidateMoves(move.fromSquare);

    ChessMove::VariantAction moveAction;
    // ----------
    // 1st, check if the move is a normal move
    for (auto &sqr : candidateMoves.quietSquares)
    {
      if (sqr == move.toSquare)
      {
        moveAction = ChessMove::Normal{
            move.fromSquare, move.toSquare, false, KingStatus::SAFE};
        break;
      }
    }

    // ----------
    // 2nd, check if the move is a normal capture move
    if (moveAction.isEmpty())
    {
      for (auto &sqr : candidateMoves.captureSquares)
      {
        if (sqr == move.toSquare)
        {
          moveAction = ChessMove::Normal{
              move.fromSquare, move.toSquare, true, KingStatus::SAFE};
          break;
        }
      }
    }

    // ----------
    // 3rd, check if the move is a special move
    if (moveAction.isEmpty())
    {
      // 3.1: if the move is of the king
      if (movedPiece->type == ChessRule::KING)
      {
        for (auto &sqr : candidateMoves.specialSquares)
        {
          if (sqr == move.toSquare)
          {
            if (ChessRule::isKingInCheck(piecePlacements_, movedPiece->color))
            {
              return ChessMove::Invalid{ChessMove::Error::INVALID_PROMOTION};
            }

            std::pair<Position, Position> castlingRookMove =
                ChessRule::getCastlingRookMove(move.toSquare, movedPiece->color);

            std::map<Position, Piece> copiedPiecePlacements = piecePlacements_;
            copiedPiecePlacements.erase(move.fromSquare);

            Position midwaySquare{
                char((move.fromSquare[0] + castlingRookMove.second[0]) / 2),
                move.fromSquare[1],
                '\0'};
            copiedPiecePlacements.emplace(midwaySquare, movedPiece.value());
            if (ChessRule::isKingInCheck(copiedPiecePlacements, movedPiece->color))
            {
              return ChessMove::Invalid{ChessMove::Error::INVALID_PROMOTION};
            }

            moveAction = ChessMove::Castling{
                move.fromSquare,
                move.toSquare,
                castlingRookMove.first,
                castlingRookMove.second,
                KingStatus::SAFE};
            break;
          }
        }
      }
      // 3.2: if the move is of the pawn
      else if (movedPiece->type == ChessRule::PAWN)
      {
        if (!candidateMoves.specialSquares.empty())
        {
          Position const &enPassantSqr = candidateMoves.specialSquares.front();
          if (enPassantSqr[0] == move.toSquare[0] &&
              enPassantSqr[1] == move.fromSquare[1])
          {
            moveAction = ChessMove::EnPassant{
                move.fromSquare,
                move.toSquare,
                enPassantSqr,
                KingStatus::SAFE};
          }
        }
      }
    }
    // ----------
    // 4th, check if the move is a promotion
    else if (ChessRule::isPromotionSquare(move.toSquare, movedPiece->color) &&
             (movedPiece->type == ChessRule::PAWN))
    {
      if (move.promote)
      {
        auto normalMove = moveAction.getIf<ChessMove::Normal>();

        moveAction = ChessMove::Promotion{
            normalMove->fromSquare,
            normalMove->toSquare,
            normalMove->canCapture,
            normalMove->enemyKingStatus,
            move.promote.value()};
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

    //TODO: continue from here
    std::map<Position, Piece> copiedPiecePlacements = piecePlacements_;
    ChessRule::commitMoveAction(copiedPiecePlacements, moveAction);

    if (ChessRule::isKingInCheck(copiedPiecePlacements, movedPiece->color))
    {
      return ChessMove::Invalid{ChessMove::Error::KING_EXPOSED};
    }

    std::string enemyColor = ChessRule::getEnemyColor(movedPiece->color);
    if (ChessRule::isKingInCheck(copiedPiecePlacements, enemyColor))
    {
      auto moveActionVisitor = []<typename T>(T &action)
      {
        if constexpr (requires { {action.enemyKingStatus} -> std::same_as<KingStatus>; })
        {
          action.enemyKingStatus = KingStatus::CHECK;
        }
      };
      moveAction.visit(moveActionVisitor);
    }

    // TODO: handle checkmate
    return moveAction;
  }

  auto ChessBoardState::commitMoveAction(
      ChessMove::VariantAction const &moveAction) noexcept -> bool
  {
    moveTracker_.increaseTotalMoveCount();

    // void ChessBoardState::movePiece(
    //     Position const &fromSquare,
    //     Position const &toSquare,
    //     std::optional<std::string> const &promote)
    // {
    //   // TODO: ChessBoardState::movePiece
    //   auto movedPiece = piecePlacements_.find(fromSquare);

    //   BOOST_ASSERT_MSG(
    //       movedPiece != piecePlacements_.end(),
    //       "There must be a piece at the 'fromSquare'");

    //   auto targetedPiece = piecePlacements_.find(toSquare);
    //   if (targetedPiece == piecePlacements_.end()) ///< if 'toSquare' is empty...
    //   {
    //     piecePlacements_.try_emplace(toSquare, movedPiece->second);
    //   }
    //   else
    //   {
    //     BOOST_ASSERT_MSG(
    //         targetedPiece->second.color != movedPiece->second.color,
    //         "The color of piece at the 'fromSquare' must be different from "
    //         "the color of piece at the 'toSquare'");

    //     targetedPiece->second = movedPiece->second;
    //   }

    //   piecePlacements_.erase(movedPiece);
    // }

    return true;
  }

  auto ChessBoardState::getEnPassantSquare(
      Position const &square,
      std::string const &color) const noexcept -> std::list<Position>
  {
    std::list<Position> enPassantSquare;
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return enPassantSquare;
    }

    auto const &specialMoveTracker = moveTracker_.getSpecialMoveTracker(color);
    int const col = int(square[0]);
    if (col > ChessRule::FIRST_COL)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col - 1) ==
          moveTracker_.getTotalMoveCount())
      {
        enPassantSquare.emplace_back(Position{char(col - 1), square[1], '\0'});
      }
    }
    else if (col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col + 1) ==
          moveTracker_.getTotalMoveCount())
      {
        enPassantSquare.emplace_back(Position{char(col + 1), square[1], '\0'});
      }
    }

    return enPassantSquare;
  }

  auto ChessBoardState::getValidCastlingSquares(
      std::string const &color) const noexcept -> std::list<Position>
  {
    BGG_VALIDATE_COLOR(color);

    Position kingSquare{"e1"}; // if (color == Color::WHITE)
    if (color == Color::BLACK)
    {
      kingSquare = Position("e8");
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

} // namespace bgg
