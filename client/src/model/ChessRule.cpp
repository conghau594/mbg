// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  ChessRule::ChessRule(int side) noexcept
      : piecePlacements_(INITIAL_PLACEMENTS, INITIAL_PLACEMENTS + PIECE_COUNT),
        side_(side)
  {
    BGG_VALIDATE_SIDE(side);

    // place initial pieces based on the value of side:
    // 0 is white side, 1 is black side.
  }

  auto ChessRule::getColor() const noexcept -> int
  {
    return side_;
  }

  auto ChessRule::getPiecePlacements() const noexcept
      -> std::map<Piece, Square> const &
  {
    return piecePlacements_;
  }

  auto ChessRule::getSelectablePieces() const noexcept -> std::map<Piece, Square>
  {
    // TODO: getSelectablePieces()
    return piecePlacements_;
  }

  auto ChessRule::getCandidateMoves(Square const &square) const noexcept
      -> std::optional<CandidateMoveInfo>
  {
    BGG_VALIDATE_SQUARE(square);
    // TODO: getCandidateMoves(Square const &square)

    std::optional<Piece> piece = getPiece(square);
    if (!piece.has_value())
    {
      return std::optional<CandidateMoveInfo>(std::nullopt);
    }

    CandidateMoveInfo candidateMoveInfo{
        piece.value(),
        square,
        {Square{"a2"}, Square{"e6"}, Square{"b1"}},
        {Square{"f1"}, Square{"d5"}},
        Square{"e4"}};

    return candidateMoveInfo; //
  }

  auto ChessRule::getPiece(Square const &square) const noexcept
      -> std::optional<Piece>
  {
    BGG_VALIDATE_SQUARE(square);
    if (board_[square] == Board::EMPTY_SQUARE)
    {
      return std::nullopt;
    }

    return board_[square];
  }

  auto ChessRule::getSquare(int piece) const noexcept -> std::optional<Square>
  {
    BGG_VALIDATE_PIECE(piece);

    std::map<Piece, Square>::const_iterator found = piecePlacements_.find(piece);
    if (found == piecePlacements_.end())
    {
      return std::nullopt;
    }

    return found->second;
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

  auto ChessRule::Board::operator[](Square const &square) noexcept -> Piece &
  {
    BGG_VALIDATE_SQUARE(square);
    return pieceMap_[squareToIndex(square)];
  }

  auto ChessRule::Board::operator[](Square const &square) const noexcept -> Piece
  {
    BGG_VALIDATE_SQUARE(square);
    return pieceMap_[squareToIndex(square)];
  }

  constexpr auto ChessRule::Board::indexToSquare(int index) -> Square
  {
    int const row = index / SIDE_LENGTH + FIRST_ROW;
    int const col = index % SIDE_LENGTH + FIRST_COL;

    return Square{char(col), char(row), '\0'};
  }

  constexpr auto ChessRule::Board::squareToIndex(Square const &square) -> std::size_t
  {
    int const col = int(square[0]);
    int const row = int(square[1]);
    int const index = (row - FIRST_ROW) * SIDE_LENGTH + col - FIRST_COL;

    return std::size_t(index);
  }
} // namespace bgg
