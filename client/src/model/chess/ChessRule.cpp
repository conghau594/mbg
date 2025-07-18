// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  const std::initializer_list<std::pair<Position, Piece>>
      ChessRule::INITIAL_PLACEMENTS{
          {Position{"e1"}, Piece{KING, Color::WHITE}},
          {Position{"d1"}, Piece{QUEEN, Color::WHITE}},
          {Position{"a1"}, Piece{ROOK, Color::WHITE}},
          {Position{"h1"}, Piece{ROOK, Color::WHITE}},
          {Position{"b1"}, Piece{KNIGHT, Color::WHITE}},
          {Position{"g1"}, Piece{KNIGHT, Color::WHITE}},
          {Position{"c1"}, Piece{BISHOP, Color::WHITE}},
          {Position{"f1"}, Piece{BISHOP, Color::WHITE}},
          {Position{"a2"}, Piece{PAWN, Color::WHITE}},
          {Position{"b2"}, Piece{PAWN, Color::WHITE}},
          {Position{"c2"}, Piece{PAWN, Color::WHITE}},
          {Position{"d2"}, Piece{PAWN, Color::WHITE}},
          {Position{"e2"}, Piece{PAWN, Color::WHITE}},
          {Position{"f2"}, Piece{PAWN, Color::WHITE}},
          {Position{"g2"}, Piece{PAWN, Color::WHITE}},
          {Position{"h2"}, Piece{PAWN, Color::WHITE}},

          {Position{"e8"}, Piece{KING, Color::BLACK}},
          {Position{"d8"}, Piece{QUEEN, Color::BLACK}},
          {Position{"a8"}, Piece{ROOK, Color::BLACK}},
          {Position{"h8"}, Piece{ROOK, Color::BLACK}},
          {Position{"b8"}, Piece{KNIGHT, Color::BLACK}},
          {Position{"g8"}, Piece{KNIGHT, Color::BLACK}},
          {Position{"c8"}, Piece{BISHOP, Color::BLACK}},
          {Position{"f8"}, Piece{BISHOP, Color::BLACK}},
          {Position{"a7"}, Piece{PAWN, Color::BLACK}},
          {Position{"b7"}, Piece{PAWN, Color::BLACK}},
          {Position{"c7"}, Piece{PAWN, Color::BLACK}},
          {Position{"d7"}, Piece{PAWN, Color::BLACK}},
          {Position{"e7"}, Piece{PAWN, Color::BLACK}},
          {Position{"f7"}, Piece{PAWN, Color::BLACK}},
          {Position{"g7"}, Piece{PAWN, Color::BLACK}},
          {Position{"h7"}, Piece{PAWN, Color::BLACK}}};

  auto ChessRule::getEnemyColor(std::string const &color) noexcept -> std::string
  {
    return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
  }

  void ChessRule::commitMoveAction(
      std::map<Position, Piece> &piecePlacements,
      ChessMove::VariantAction const &moveAction) noexcept
  {
    // TODO: ChessRule::commitMoveAction
    return;
  }

  auto ChessRule::isKingInCheck(
      std::map<Position, Piece> const &piecePlacements,
      std::string const &color) noexcept -> bool
  {
    // TODO: ChessRule::isKingInCheck
    return false;
  }

  auto ChessRule::isPromotionSquare(
      Position const &square, std::string const &color) noexcept -> bool
  {
    char promotionRank = (color == Color::WHITE) ? '8' : '1';
    return square[1] == promotionRank;
  }

  auto ChessRule::getCastlingRookMove(
      Position const &kingTargetSquare,
      std::string const &color) noexcept -> std::pair<Position, Position>
  {
    if (color == Color::WHITE)
    {
      if (kingTargetSquare == Position{"g1"})
      {
        return {Position{"h1"}, Position{"f1"}}; // King-side castling
      }

      if (kingTargetSquare == Position{"c1"})
      {
        return {Position{"a1"}, Position{"d1"}}; // Queen-side castling
      }
    }
    else if (color == Color::BLACK)
    {
      if (kingTargetSquare == Position{"g8"})
      {
        return {Position{"h8"}, Position{"f8"}}; // King-side castling
      }

      if (kingTargetSquare == Position{"c8"})
      {
        return {Position{"a8"}, Position{"d8"}}; // Queen-side castling
      }
    }

    BOOST_ASSERT_MSG(false, "Invalid castling king target square");
  }
  auto ChessRule::getSquareStatus(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> SquareStatus
  {
    int const col = int(square[0]);
    int const row = int(square[1]);

    if ((col < ChessRule::FIRST_COL) ||
        (row < ChessRule::FIRST_ROW) ||
        (col > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) ||
        (row > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
    {
      return SquareStatus::OUT_OF_BOARD;
    }

    auto found = piecePlacements.find(square);
    if (found == piecePlacements.cend())
    {
      return SquareStatus::EMPTY;
    }

    if (found->second.color == color)
    {
      return SquareStatus::ALLY;
    }

    return SquareStatus::ENEMY;
  }

  auto ChessRule::isOrthogonalMovePossible(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const nearSquares[]{
        Position{char(col + 1), char(row), '\0'},
        Position{char(col - 1), char(row), '\0'},
        Position{char(col), char(row + 1), '\0'},
        Position{char(col), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      SquareStatus sqrStatus = ChessRule::getSquareStatus(
          piecePlacements, nearSquare, color);
      if (sqrStatus == SquareStatus::ENEMY || sqrStatus == SquareStatus::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isDiagonalMovePossible(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const nearSquares[]{
        Position{char(col + 1), char(row + 1), '\0'},
        Position{char(col + 1), char(row - 1), '\0'},
        Position{char(col - 1), char(row + 1), '\0'},
        Position{char(col - 1), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      SquareStatus sqrStatus = ChessRule::getSquareStatus(
          piecePlacements, nearSquare, color);
      if (sqrStatus == SquareStatus::ENEMY || sqrStatus == SquareStatus::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isKnightMovePossible(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> bool
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const possibleSquares[]{
        Position{char(col + 1), char(row + 2), '\0'},
        Position{char(col + 1), char(row - 2), '\0'},
        Position{char(col - 1), char(row + 2), '\0'},
        Position{char(col - 1), char(row - 2), '\0'},
        Position{char(col + 2), char(row + 1), '\0'},
        Position{char(col - 2), char(row + 1), '\0'},
        Position{char(col + 2), char(row - 1), '\0'},
        Position{char(col - 2), char(row - 1), '\0'}};

    for (Position const &sqr : possibleSquares)
    {
      SquareStatus sqrStatus = ChessRule::getSquareStatus(
          piecePlacements, sqr, color);
      if (sqrStatus == SquareStatus::ENEMY || sqrStatus == SquareStatus::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isNormalPawnMovePossible(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> bool
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

    Position frontSqr{char(col), char(row + step), '\0'};
    if (piecePlacements.find(frontSqr) == piecePlacements.cend())
    {
      return true;
    }

    Position leftSqr{char(col - 1), char(row + step), '\0'};
    auto found = piecePlacements.find(leftSqr);
    if ((found != piecePlacements.cend()) && (found->second.color != color))
    {
      return true;
    }

    Position rightSqr{char(col + 1), char(row + step), '\0'};
    found = piecePlacements.find(rightSqr);
    if ((found != piecePlacements.cend()) && (found->second.color != color))
    {
      return true;
    }

    return false;
  }

  auto ChessRule::classifyAndCollectSquare(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept -> SquareStatus
  {
    auto sqrStatus = ChessRule::getSquareStatus(piecePlacements, square, color);

    if ((sqrStatus == SquareStatus::EMPTY) && quietReachableSquares)
    {
      quietReachableSquares->emplace_back(square);
    }
    else if ((sqrStatus == SquareStatus::ENEMY) && captureReachableSquares)
    {
      captureReachableSquares->emplace_back(square);
    }

    return sqrStatus;
  }

  void ChessRule::collectOrthogonalReachableSquares(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      int radius,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        (radius > 0) && (radius < ChessRule::BOARD_SIDE),
        "The radius of neighborhood must be greater than 0 and less than BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    for (int i = 1; i <= radius; ++i)
    {
      if (row + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1)
      {
        break;
      }

      Position sqr{char(col), char(row + i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (row - i < ChessRule::FIRST_ROW)
      {
        break;
      }

      Position sqr{char(col), char(row - i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1)
      {
        break;
      }

      Position sqr{char(col + i), char(row), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col - i < ChessRule::FIRST_COL)
      {
        break;
      }

      Position sqr{char(col - i), char(row), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }
  }

  void ChessRule::collectDiagonalReachableSquares(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      int radius,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        radius > 0 && radius < ChessRule::BOARD_SIDE,
        "The radius of neighborhood must be greater than 0 and less than BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) ||
          (row + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
      {
        break;
      }

      Position sqr{char(col + i), char(row + i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) ||
          (row - i < ChessRule::FIRST_ROW))
      {
        break;
      }

      Position sqr{char(col + i), char(row - i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i < ChessRule::FIRST_COL) ||
          (row + i > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
      {
        break;
      }

      Position sqr{char(col - i), char(row + i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i < ChessRule::FIRST_COL) ||
          (row - i < ChessRule::FIRST_ROW))
      {
        break;
      }

      Position sqr{char(col - i), char(row - i), '\0'};
      auto squareStatus = classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);

      if (squareStatus != SquareStatus::EMPTY)
      {
        break;
      }
    }
  }

  void ChessRule::collectKnightReachableSquares(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    int const col = int(square[0]);
    int const row = int(square[1]);
    Position const possibleSquares[]{
        Position{char(col + 1), char(row + 2), '\0'},
        Position{char(col + 1), char(row - 2), '\0'},
        Position{char(col - 1), char(row + 2), '\0'},
        Position{char(col - 1), char(row - 2), '\0'},
        Position{char(col + 2), char(row + 1), '\0'},
        Position{char(col - 2), char(row + 1), '\0'},
        Position{char(col + 2), char(row - 1), '\0'},
        Position{char(col - 2), char(row - 1), '\0'}};

    for (Position const &sqr : possibleSquares)
    {
      classifyAndCollectSquare(
          piecePlacements,
          sqr,
          color,
          quietReachableSquares,
          captureReachableSquares);
    }
  }

  void ChessRule::collectPawnBasicReachableSquares(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);

    BOOST_ASSERT_MSG(square[1] != '1' && square[1] != '8',
                     "Pawns never stand on row 1 and row 8");

    int const col = int(square[0]);
    int const row = int(square[1]);

    int step = 1;
    if (color == Color::BLACK)
    {
      step = -1;
    }

    if (quietReachableSquares)
    {
      Position front1{char(col), char(row + step), '\0'};
      if (piecePlacements.find(front1) == piecePlacements.cend())
      {
        quietReachableSquares->emplace_back(front1);

        Position front2{char(col), char(row + 2 * step), '\0'};
        if (piecePlacements.find(front2) == piecePlacements.end())
        {
          quietReachableSquares->emplace_back(front2);
        }
      }
    }

    if (captureReachableSquares)
    {
      Position left{char(col - 1), char(row + step), '\0'};
      auto found = piecePlacements.find(left);
      if ((found != piecePlacements.end()) && (found->second.color != color))
      {
        captureReachableSquares->emplace_back(left);
      }

      Position right{char(col + 1), char(row + step), '\0'};
      found = piecePlacements.find(right);
      if ((found != piecePlacements.end()) && (found->second.color != color))
      {
        captureReachableSquares->emplace_back(right);
      }
    }
  }

} // namespace bgg
