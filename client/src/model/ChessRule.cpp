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
    std::map<Position, Piece> selectablePieces;

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
    std::optional<Piece> movedPiece = getPiece(move.fromPos);
    if (!movedPiece)
    {
      return ChessMove::Invalid{ChessMove::Error::INVALID_FROM_POS};
    }

    std::optional<Piece> capturedPiece = getPiece(move.toPos);
    if (capturedPiece && (movedPiece->color == capturedPiece->color))
    {
      return ChessMove::Invalid{ChessMove::Error::INVALID_TO_POS};
    }

    if (movedPiece->type == PieceType::PAWN)
    {
    }
    else if (movedPiece->type == PieceType::KING)
    {
    }
    else
    {
    }
  }

  auto ChessRule::isSquareMovable(Position const &square, std::string const &color) const noexcept -> bool
  {
    int const col = int(square[0]);
    int const row = int(square[1]);

    if ((col < FIRST_COL) || (col > BOARD_SIDE + FIRST_COL - 1) ||
        (row < FIRST_ROW) || (row > BOARD_SIDE + FIRST_ROW - 1))
    {
      return false;
    }

    auto found = piecePlacements_.find(square);
    if ((found == piecePlacements_.cend()) || (found->second.color != color))
    {
      return true;
    }

    return false;
  }

  auto ChessRule::isStraightMovePossible(
      Position const &square, std::string const &color) const noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const nearSquares[4]{
        Position{char(col + 1), char(row), '\0'},
        Position{char(col - 1), char(row), '\0'},
        Position{char(col), char(row + 1), '\0'},
        Position{char(col), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      if (isSquareMovable(nearSquare, color))
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isDiagonalMovePossible(
      Position const &square, std::string const &color) const noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const nearSquares[4]{
        Position{char(col + 1), char(row + 1), '\0'},
        Position{char(col + 1), char(row - 1), '\0'},
        Position{char(col - 1), char(row + 1), '\0'},
        Position{char(col - 1), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      if (isSquareMovable(nearSquare, color))
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isKnightMovePossible(
      Position const &square, std::string const &color) const noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const nearSquares[8]{
        Position{char(col + 1), char(row + 2), '\0'},
        Position{char(col + 1), char(row - 2), '\0'},
        Position{char(col - 1), char(row + 2), '\0'},
        Position{char(col - 1), char(row - 2), '\0'},
        Position{char(col + 2), char(row + 1), '\0'},
        Position{char(col - 2), char(row + 1), '\0'},
        Position{char(col + 2), char(row - 1), '\0'},
        Position{char(col - 2), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      if (isSquareMovable(nearSquare, color))
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isPawnMovePossible(
      Position const &square, std::string const &color) const noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    BOOST_ASSERT_MSG(
        square[1] != '1' && square[1] != '8',
        "Pawns never stand on row 1 and row 8");

    int const col = int(square[0]);
    int const row = int(square[1]);

    int step = 1;
    if (color == Color::BLACK)
    {
      step = -1;
    }

    Position front{char(col), char(row + step), '\0'};
    auto found = piecePlacements_.find(front);
    if (found == piecePlacements_.cend())
    {
      return true;
    }

    Position left{char(col - 1), char(row + step), '\0'};
    found = piecePlacements_.find(left);
    if ((found != piecePlacements_.cend()) && (found->second.color == color))
    {
      return true;
    }

    Position right{char(col + 1), char(row + step), '\0'};
    found = piecePlacements_.find(right);
    if ((found != piecePlacements_.cend()) && (found->second.color == color))
    {
      return true;
    }

    return false;
  }

  auto ChessRule::getStraightMovableSquares(
      Position const &square, unsigned radius) const noexcept -> std::list<Position>
  {
    return std::list<Position>();
  }

  auto ChessRule::getDiagonalMovableSquares(
      Position const &square, unsigned radius) const noexcept -> std::list<Position>
  {
    return std::list<Position>();
  }

  auto ChessRule::getKnightMovableSquares(
      Position const &square) const noexcept -> std::list<Position>
  {
    return std::list<Position>();
  }

  auto ChessRule::getPawnMovableSquares(
      Position const &square) const noexcept -> std::list<Position>
  {
    return std::list<Position>();
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
