// ChessRule.cpp

#include <set>

#include "ChessRule.h"
#include "base/Logger.h"

namespace bgg
{
  const std::initializer_list<std::pair<const Position, Piece>>
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

  auto ChessRule::getPiece(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square) noexcept -> std::optional<Piece>
  {
    BGG_VALIDATE_SQUARE(square);
    auto found = piecePlacements.find(square);
    if (found == piecePlacements.cend())
    {
      return std::nullopt;
    }

    return found->second;
  }

  auto ChessRule::getEnemyColor(std::string const &color) noexcept -> std::string
  {
    return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
  }

  void ChessRule::commitMoveAction(
      std::map<Position, Piece> &piecePlacements,
      ChessMove::Action const &moveAction) noexcept
  {
    auto moveActionVisitor = [&piecePlacements]<typename T>(T const &action)
    {
      if constexpr (requires {
        { action.fromSquare } -> std::same_as<Position const &>;
        { action.toSquare } -> std::same_as<Position const &>; })
      {
        // SPDLOG_INFO("You have committed a '{}'", typeid(T).name());

        auto piece = getPiece(piecePlacements, action.fromSquare);
        BOOST_ASSERT_MSG(piece, "There must be an item at the 'fromTile'");

        piecePlacements.erase(action.fromSquare);
        piecePlacements.erase(action.toSquare);

        if constexpr (requires {
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessMove::Promotion>)
        {
          BGG_VALIDATE_CHESS_PIECE(action.promote);
          piece->type = action.promote;
          piecePlacements.emplace(action.toSquare, std::move(*piece));
        }
        else
        {
          piecePlacements.emplace(action.toSquare, *piece);

          if constexpr (requires { 
              { action.enPassantSquare } -> std::same_as<Position const &>; }) ///< if constexpr (std::is_same_v<T, ChessMove::EnPassant>)
          {
            // remove en passant captured item
            piecePlacements.erase(
                Position{action.toSquare[0], action.fromSquare[1]});
          }
          else if constexpr (requires {
              { action.rookSource } -> std::same_as<Position const &>;
              { action.rookDestination } -> std::same_as<Position const &>; }) ///< if constexpr (std::is_same_v<T, ChessMove::Castling>)
          {
            // move the related rook to the destination
            auto relatedRook = getPiece(piecePlacements, action.rookSource);
            BOOST_ASSERT_MSG(relatedRook, "There must be a rook at action.rookSource");

            piecePlacements.emplace(action.rookDestination, std::move(*relatedRook));
            piecePlacements.erase(action.rookSource);
          }
        }
      }
      else if constexpr (std::is_same_v<T, std::monostate>)
      {
        SPDLOG_INFO("You have tried to commit an empty move");
      }
      else if constexpr (std::is_same_v<T, ChessMove::Invalid>)
      {
        SPDLOG_WARN("You have tryied to commit an 'ChessMove::Invalid'");
      }
    };

    moveAction.visit(moveActionVisitor);
  }

  auto ChessRule::evaluateKingState(
      std::map<Position, Piece> const &piecePlacements,
      std::string const &color,
      bool checkForCheckmate) noexcept -> KingState
  {
    if (!isKingInCheck(piecePlacements, color))
    {
      return KingState::SAFE;
    }

    if (!checkForCheckmate)
    {
      return KingState::IN_CHECK;
    }

    std::map<Position, Piece> allySquares = getPieceSquares(piecePlacements, color);
    std::map<Position, Piece> copiedPiecePlacements = piecePlacements;
    for (auto &[allySqr, piece] : allySquares)
    {
      std::list<Position> quietReachableSquares;
      std::list<Position> captureReachableSquares;

      collectBasicCandidateMoves(
          copiedPiecePlacements,
          allySqr,
          &quietReachableSquares,
          &captureReachableSquares);

      for (auto &quietSqr : quietReachableSquares)
      {
        copiedPiecePlacements.erase(allySqr);
        copiedPiecePlacements.emplace(quietSqr, piece);
        if (!isKingInCheck(copiedPiecePlacements, color))
        {
          return KingState::IN_CHECK;
        }
        else // -> revert to the previous placements
        {
          copiedPiecePlacements.erase(quietSqr);
          copiedPiecePlacements.emplace(allySqr, piece);
        }
      }

      for (auto &captureSqr : captureReachableSquares)
      {
        copiedPiecePlacements.erase(allySqr);

        auto enemyPiece = getPiece(copiedPiecePlacements, captureSqr);
        copiedPiecePlacements.erase(captureSqr);

        copiedPiecePlacements.emplace(captureSqr, piece);
        if (!isKingInCheck(copiedPiecePlacements, color))
        {
          return KingState::IN_CHECK;
        }
        else // -> revert to the previous placements
        {
          copiedPiecePlacements.erase(captureSqr);
          copiedPiecePlacements.emplace(allySqr, piece);
          copiedPiecePlacements.emplace(captureSqr, enemyPiece.value());
        }
      }
    }

    return KingState::CHECKMATED;
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
    return {};
  }
  auto ChessRule::getPositionStatus(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color) noexcept -> PositionStatus
  {
    int const col = int(square[0]);
    int const row = int(square[1]);

    if ((col < ChessRule::FIRST_COL) ||
        (row < ChessRule::FIRST_ROW) ||
        (col > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) ||
        (row > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
    {
      return PositionStatus::OUT_OF_BOARD;
    }

    auto found = piecePlacements.find(square);
    if (found == piecePlacements.cend())
    {
      return PositionStatus::EMPTY;
    }

    if (found->second.color == color)
    {
      return PositionStatus::ALLY;
    }

    return PositionStatus::ENEMY;
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
      PositionStatus sqrStatus = ChessRule::getPositionStatus(
          piecePlacements, nearSquare, color);
      if (sqrStatus == PositionStatus::ENEMY || sqrStatus == PositionStatus::EMPTY)
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
      PositionStatus sqrStatus = ChessRule::getPositionStatus(
          piecePlacements, nearSquare, color);
      if (sqrStatus == PositionStatus::ENEMY || sqrStatus == PositionStatus::EMPTY)
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
      PositionStatus sqrStatus = getPositionStatus(
          piecePlacements, sqr, color);
      if (sqrStatus == PositionStatus::ENEMY || sqrStatus == PositionStatus::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::isPawnBasicMovePossible(
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

      if (squareStatus != PositionStatus::EMPTY)
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

  void ChessRule::collectBasicCandidateMoves(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept
  {
    BGG_VALIDATE_SQUARE(square);

    std::optional<Piece> piece = ChessRule::getPiece(piecePlacements, square);
    if (!piece)
    {
      return;
    }

    if (piece->type == ChessRule::QUEEN)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          quietReachableSquares,
          captureReachableSquares);

      ChessRule::collectDiagonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          quietReachableSquares,
          captureReachableSquares);
    }
    else if (piece->type == ChessRule::ROOK)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          quietReachableSquares,
          captureReachableSquares);
    }
    else if (piece->type == ChessRule::BISHOP)
    {
      ChessRule::collectDiagonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          ChessRule::BOARD_SIDE - 1,
          quietReachableSquares,
          captureReachableSquares);
    }
    else if (piece->type == ChessRule::KNIGHT)
    {
      ChessRule::collectKnightReachableSquares(
          piecePlacements,
          square,
          piece->color,
          quietReachableSquares,
          captureReachableSquares);
    }
    else if (piece->type == ChessRule::PAWN)
    {
      ChessRule::collectPawnBasicReachableSquares(
          piecePlacements,
          square,
          piece->color,
          quietReachableSquares,
          captureReachableSquares);
    }
    else // if (piece->type == ChessRule::KING)
    {
      ChessRule::collectOrthogonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          1,
          quietReachableSquares,
          captureReachableSquares);

      ChessRule::collectDiagonalReachableSquares(
          piecePlacements,
          square,
          piece->color,
          1,
          quietReachableSquares,
          captureReachableSquares);
    }
  }

  auto ChessRule::isKingInCheck(
      std::map<Position, Piece> const &piecePlacements,
      std::string const &color) noexcept -> bool
  {
    // collect the square of the king of color and squares the enemy pieces
    auto kingSqr = findKingSquare(piecePlacements, color);

    BOOST_ASSERT_MSG(
        kingSqr, "There must be a king with 'color' on the chess board");

    for (auto &[enemySqr, piece] : piecePlacements)
    {
      if (piece.color == color)
      {
        continue;
      }

      std::list<Position> captureSquares;
      ChessRule::collectBasicCandidateMoves(
          piecePlacements, enemySqr, nullptr, &captureSquares);

      for (auto &captureSqr : captureSquares)
      {
        if (captureSqr == kingSqr.value())
        {
          return true;
        }
      }
    }

    return false;
  }

  auto ChessRule::findKingSquare(
      std::map<Position, Piece> const &piecePlacements,
      std::string const &color) noexcept -> std::optional<Position>
  {
    for (auto &[pos, piece] : piecePlacements)
    {
      if (piece.color == color)
      {
        if (piece.type == KING)
        {
          return pos;
        }
      }
    }

    return std::nullopt;
  }

  auto ChessRule::classifyAndCollectSquare(
      std::map<Position, Piece> const &piecePlacements,
      Position const &square,
      std::string const &color,
      std::list<Position> *quietReachableSquares,
      std::list<Position> *captureReachableSquares) noexcept -> PositionStatus
  {
    auto sqrStatus = ChessRule::getPositionStatus(piecePlacements, square, color);

    if ((sqrStatus == PositionStatus::EMPTY) && quietReachableSquares)
    {
      quietReachableSquares->emplace_back(square);
    }
    else if ((sqrStatus == PositionStatus::ENEMY) && captureReachableSquares)
    {
      captureReachableSquares->emplace_back(square);
    }

    return sqrStatus;
  }

  auto ChessRule::getPieceSquares(
      std::map<Position, Piece> const &piecePlacements,
      std::string const &color) noexcept -> std::map<Position, Piece>
  {
    std::map<Position, Piece> squares;
    for (auto &[pos, piece] : piecePlacements)
    {
      if (piece.color != color)
      {
        continue;
      }

      squares.emplace(pos, piece);
    }
    return squares;
  }

} // namespace bgg
