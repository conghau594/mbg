// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  ChessRule::ChessRule(ChessPiece::Color color) noexcept
      : piecePlacements_(INITIAL_PLACEMENTS, INITIAL_PLACEMENTS + PIECE_COUNT),
        color_(color)
  {
    BGG_VALIDATE_SIDE(color);
  }

  ChessRule::ChessRule(
      std::map<Square, ChessPiece> piecePlacements,
      ChessPiece::Color color) noexcept
      : piecePlacements_(piecePlacements), color_(color)
  {
    BGG_VALIDATE_SIDE(color);
  }

  auto ChessRule::getColor() const noexcept -> ChessPiece::Color
  {
    return color_;
  }

  auto ChessRule::getPiecePlacements() noexcept
      -> std::map<Square, ChessPiece> &
  {
    return piecePlacements_;
  }

  auto ChessRule::getSelectablePieces() const noexcept
      -> std::map<Square, ChessPiece>
  {
    // TODO: getSelectablePieces()
    return piecePlacements_;
  }

  auto ChessRule::getCandidateMoves(Square const &square) const noexcept
      -> std::optional<CandidateMoveInfo>
  {
    BGG_VALIDATE_SQUARE(square);
    // TODO: getCandidateMoves(Square const &square)

    std::optional<ChessPiece> piece = getPiece(square);
    if (!piece)
    {
      return std::nullopt;
    }

    CandidateMoveInfo candidateMoveInfo{
        piece.value(),
        {Square{"a2"}, Square{"e6"}, Square{"b1"}},
        {Square{"f1"}, Square{"d5"}},
        Square{"e4"}};

    return candidateMoveInfo;
  }

  auto ChessRule::getPiece(Square const &square) const noexcept
      -> std::optional<ChessPiece>
  {
    BGG_VALIDATE_SQUARE(square);
    auto found = piecePlacements_.find(square);
    if (found == piecePlacements_.cend())
    {
      return std::nullopt;
    }

    return found->second;
  }

  // constexpr auto ChessRule::Board::indexToSquare(int index) -> Square
  // {
  //   int const row = index / SIDE_LENGTH + FIRST_ROW;
  //   int const col = index % SIDE_LENGTH + FIRST_COL;

  //   return Square{char(col), char(row), '\0'};
  // }

  // constexpr auto ChessRule::Board::squareToIndex(Square const &square) -> std::size_t
  // {
  //   int const col = int(square[0]);
  //   int const row = int(square[1]);
  //   int const index = (row - FIRST_ROW) * SIDE_LENGTH + col - FIRST_COL;

  //   return std::size_t(index);
  // }
} // namespace bgg
