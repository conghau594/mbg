// ChessRule.cpp
#include <format>
#include <boost/assert.hpp>

#include "ChessRule.h"

#define BGG_VALIDATE_SIDE(side) BOOST_ASSERT_MSG(           \
    side == PieceColor::WHITE || side == PieceColor::BLACK, \
    "Side must be PieceColor::WHITE or PieceColor::BLACK")

#define BGG_VALIDATE_SQUARE(square) BOOST_ASSERT_MSG(                             \
    square[0] >= 'a' && square[0] <= 'h' && square[1] >= '1' && square[1] <= '8', \
    "Square index must be from 'a1' to 'h8'")

#define BGG_VALIDATE_PIECE(piece) BOOST_ASSERT_MSG(                       \
    piece >= ChessPiece::WHITE_KING && piece <= ChessPiece::BLACK_PAWN_H, \
    "Piece must be from ChessPiece::WHITE_KING to ChessPiece::BLACK_PAWN_H")

namespace bgg
{
  ChessRule::ChessRule(int side) noexcept
      : piecePlacements_(INITIAL_PLACEMENTS, INITIAL_PLACEMENTS + PIECE_COUNT),
        side_(side)
  {
    // place initial pieces based on the value of side:
    // 0 is white side, 1 is black side.
  }
  auto ChessRule::getPiecePlacements() const noexcept -> std::map<Piece, Square> const &
  {
    return piecePlacements_;
  }

  auto ChessRule::getSelectablePieces() const noexcept -> std::list<PiecePlacement>
  {
    return std::list<PiecePlacement>();
  }

  auto ChessRule::getCandidateMoves(Square const &square) const noexcept
      -> std::optional<CandidateMoveInfo>
  {
    BGG_VALIDATE_SQUARE(square);
    return std::optional<CandidateMoveInfo>(std::nullopt);
  }

  auto ChessRule::getPiece(Square const &square) const noexcept -> int
  {
    BGG_VALIDATE_SQUARE(square);
    // TODO:
    return 0;
  }
  auto ChessRule::getSquare(int piece) const noexcept -> Square
  {
    BGG_VALIDATE_PIECE(piece);
    return Square{"a1"};
  }

  ChessRule::Board::Board() noexcept
  {
    for (std::size_t i = 0; i < AREA; ++i)
    {
      pieceMap_[i] = EMPTY_SQUARE;
    }

    for (std::size_t i = 0; i < PIECE_COUNT; ++i)
    {
      int const &piece = INITIAL_PLACEMENTS[i].first;
      Square const &square = INITIAL_PLACEMENTS[i].second;
      operator[](square) = piece;
    }
  }

  auto ChessRule::Board::operator[](Square const &square) noexcept -> int &
  {
    BGG_VALIDATE_SQUARE(square);
    int const row = int(square[1]);
    int const col = int(square[0]);
    int const index = (row - FIRST_ROW) * SIDE_LENGTH + col - FIRST_COL;
    return pieceMap_[std::size_t(index)];
  }
} // namespace bgg
