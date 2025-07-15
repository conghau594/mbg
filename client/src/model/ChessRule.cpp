// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  ChessRule::ChessRule(std::string color) noexcept
      : piecePlacements_(INITIAL_PLACEMENTS, INITIAL_PLACEMENTS + PIECE_COUNT),
        yourColor_(std::move(color))
  {
    BGG_VALIDATE_COLOR(yourColor_);
  }

  ChessRule::ChessRule(
      std::map<Position, Piece> piecePlacements,
      std::string color) noexcept
      : piecePlacements_(std::move(piecePlacements)), yourColor_(std::move(color))
  {
    BGG_VALIDATE_COLOR(yourColor_);
  }

  auto ChessRule::getYourColor() const noexcept -> std::string const &
  {
    return yourColor_;
  }

  auto ChessRule::getColor(Position const &square) const noexcept
      -> std::optional<std::string>
  {
    std::optional<Piece> piece = getPiece(square);
    if (!piece)
    {
      return std::nullopt;
    }
    return piece->color;
  }

  auto ChessRule::getPiecePlacements() noexcept
      -> std::map<Position, Piece> const &
  {
    return piecePlacements_;
  }

  auto ChessRule::getSelectablePieces() const noexcept
      -> std::map<Position, Piece>
  {
    // TODO: getSelectablePieces()
    return piecePlacements_;
  }

  auto ChessRule::getCandidateMoves(Position const &square) const noexcept
      -> std::optional<CandidateMoveInfo>
  {
    BGG_VALIDATE_SQUARE(square);
    // TODO: getCandidateMoves(Position const &square)

    std::optional<Piece> piece = getPiece(square);
    if (!piece)
    {
      return std::nullopt;
    }

    CandidateMoveInfo candidateMoveInfo{
        piece.value(),
        {Position{"a2"}, Position{"e6"}, Position{"b1"}},
        {Position{"f1"}, Position{"d5"}, Position{"h8"}},
        Position{"e4"}};

    return candidateMoveInfo;
  }

  auto ChessRule::getPiece(Position const &square) const noexcept
      -> std::optional<Piece>
  {
    BGG_VALIDATE_SQUARE(square);
    auto found = piecePlacements_.find(square);
    if (found == piecePlacements_.cend())
    {
      return std::nullopt;
    }

    return found->second;
  }

  auto ChessRule::tryMove(ChessMove const &move) const noexcept -> ChessMove::Result
  {
    return ChessMove::Invalid();
  }

  // void ChessRule::movePiece(
  //     Position const &fromSquare,
  //     Position const &toSquare,
  //     std::optional<std::string> const &promote)
  // {
  //   // TODO: ChessRule::movePiece
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
} // namespace bgg
