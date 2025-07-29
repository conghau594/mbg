// ChessRule.cpp

#include "ChessRule.h"
#include "model/Piece.h"
#include "model/Side.h"
#include "ChessHelpers.h"

namespace bgg
{

  ChessRule::ChessRule(
      std::list<std::tuple<EntityType, Side, Position>> const &initialPlacements) noexcept
  {
  }

  auto ChessRule::getMoveCount() const noexcept -> int
  {
    return moveCount_;
  }

  auto ChessRule::getLastMove() const noexcept -> ChessMove::Detail const &
  {
    return moveHistory_.back();
  }

  auto ChessRule::getPositionStatus(
      Position const &square, Side const &color) const noexcept
      -> Position::Status
  {
    PieceSet const *pieceSet = getPieceSet(color);

    int const &file = square.getFile();
    int const &rank = square.getRank();

    if ((file < chess::FIRST_FILE) ||
        (file > chess::LAST_FILE) ||
        (rank < chess::FIRST_RANK) ||
        (rank > chess::LAST_RANK))
    {
      return Position::Status::OUT_OF_BOARD;
    }

    // TODO: uncomment this line
    //  auto piece = pieceSet->findPiece(square);
    //  if (!piece)
    //  {
    //    return Position::Status::EMPTY;
    //  }

    // if (piece->getSide() == color)
    // {
    //   return Position::Status::ALLY;
    // }

    return Position::Status::OPPONENT;
  }

  auto ChessRule::collectPieces(Side const &color) const noexcept
      -> std::list<std::shared_ptr<Piece>>
  {
    PieceSet const *pieceSet = getPieceSet(color);

    std::list<std::shared_ptr<Piece>> result;
    for (auto const &piece : *pieceSet)
    {
      result.emplace_back(piece);
    }

    return result;
  }

  auto ChessRule::collectSelectablePieces(Side const &color) const noexcept
      -> std::list<std::shared_ptr<Piece>>
  {
    return std::list<std::shared_ptr<Piece>>();
  }

  auto ChessRule::collectReachableSquares(
      Position const &square, Side const &color) const noexcept
      -> ReachablePosInfo
  {
    return ReachablePosInfo();
  }

  auto ChessRule::findPiece(Position const &square) noexcept
      -> std::shared_ptr<Piece>
  {
    return std::shared_ptr<Piece>();
  }

  auto ChessRule::tryMove(ChessMove const &move) const noexcept
      -> ChessMove::Detail
  {
    return ChessMove::Detail();
  }

  void ChessRule::commitMove(ChessMove::Detail moveAction) noexcept
  {
  }

  auto ChessRule::getPieceSet(Side const &color) noexcept -> PieceSet *
  {
    if (color == chess::WHITE)
    {
      return &whitePieces_;
    }
    else
    {
      return &blackPieces_;
    }
  }
  auto ChessRule::getPieceSet(Side const &color) const noexcept -> PieceSet const *
  {
    if (color == chess::WHITE)
    {
      return &whitePieces_;
    }
    else
    {
      return &blackPieces_;
    }
  }

} // namespace bgg
