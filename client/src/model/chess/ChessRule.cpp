// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  ChessRule::ChessRule(std::string color) noexcept
      : piecePlacements_(
            ChessUtils::INITIAL_PLACEMENTS,
            ChessUtils::INITIAL_PLACEMENTS + ChessUtils::PIECE_COUNT),
        yourColor_(std::move(color))
  {
    BGG_VALIDATE_COLOR(yourColor_);
  }

  ChessRule::ChessRule(
      std::map<Position, Piece> piecePlacements,
      std::string color) noexcept
      : piecePlacements_(std::move(piecePlacements)),
        yourColor_(std::move(color))
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

  auto ChessRule::getPiecePlacements() const noexcept
      -> std::map<Position, Piece> const &
  {
    return piecePlacements_;
  }

  auto ChessRule::getSelectablePieces() const noexcept
      -> std::map<Position, Piece>
  {
    std::map<Position, Piece> selectablePieces;
    for (auto &[square, piece] : piecePlacements_)
    {
      if (piece.color != yourColor_)
      {
        continue;
      }

      if (piece.type == ChessUtils::KING ||
          piece.type == ChessUtils::QUEEN)
      {
        if (isStraightMovePossible(square, yourColor_) ||
            isDiagonalMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessUtils::ROOK)
      {
        if (isStraightMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessUtils::BISHOP)
      {
        if (isDiagonalMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessUtils::KNIGHT)
      {
        if (isKnightMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessUtils::PAWN)
      {
        if (isPawnMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
    }

    return selectablePieces;
  }

  auto ChessRule::getCandidateMoves(Position const &square) const noexcept
      -> std::optional<CandidateMoveInfo>
  {
    BGG_VALIDATE_SQUARE(square);

    std::optional<Piece> piece = getPiece(square);
    if (!piece)
    {
      return std::nullopt;
    }

    if (piece->type == ChessUtils::QUEEN)
    {
      CandidateMoveInfo candidateMoves = getStraightMovableSquares(
          square, piece->color, ChessUtils::BOARD_SIDE - 1);
      candidateMoves.merge(getDiagonalMovableSquares(
          square, piece->color, ChessUtils::BOARD_SIDE - 1));

      return candidateMoves;
    }
    else if (piece->type == ChessUtils::KING)
    {
      CandidateMoveInfo candidateMoves = getStraightMovableSquares(
          square, piece->color, 1);
      candidateMoves.merge(getDiagonalMovableSquares(
          square, piece->color, 1));
      candidateMoves.quietMoves.splice(
          candidateMoves.quietMoves.end(), getCastlableSquares(square, piece->color));

      return candidateMoves;
    }
    else if (piece->type == ChessUtils::ROOK)
    {
      return getStraightMovableSquares(square, piece->color, ChessUtils::BOARD_SIDE - 1);
    }
    else if (piece->type == ChessUtils::BISHOP)
    {
      return getDiagonalMovableSquares(square, piece->color, ChessUtils::BOARD_SIDE - 1);
    }
    else if (piece->type == ChessUtils::KNIGHT)
    {
      return getKnightMovableSquares(square, piece->color);
    }
    else if (piece->type == ChessUtils::PAWN)
    {
      return getPawnMovableSquares(square, piece->color);
    }
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

  auto ChessRule::attemptMove(ChessMove const &move) const noexcept -> bool
  {
    std::optional<Piece> movedPiece = getPiece(move.fromPos);
    if (!movedPiece)
    {
      return false; // ChessMove::Invalid{ChessMove::Error::INVALID_SOURCE_SQUARE};
    }

    std::optional<Piece> capturedPiece = getPiece(move.toPos);
    if (capturedPiece && (movedPiece->color == capturedPiece->color))
    {
      return false; // ChessMove::Invalid{ChessMove::Error::INVALID_TARGET_SQUARE};
    }

    if (movedPiece->type == ChessUtils::PAWN)
    {
    }
    else if (movedPiece->type == ChessUtils::KING)
    {
    }
    else
    {
    }
  }

  void ChessRule::commitLastAttemptedMove() noexcept
  {
    moveTracker.increaseTotalMoveCount();

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
  }

  auto ChessRule::getSquareStatus(
      Position const &square, std::string const &color) const noexcept
      -> SquareStatus
  {
    int const col = int(square[0]);
    int const row = int(square[1]);

    if ((col < ChessUtils::FIRST_COL) || (col > ChessUtils::BOARD_SIDE + ChessUtils::FIRST_COL - 1) ||
        (row < ChessUtils::FIRST_ROW) || (row > ChessUtils::BOARD_SIDE + ChessUtils::FIRST_ROW - 1))
    {
      return SquareStatus::OUT_OF_BOARD;
    }

    auto found = piecePlacements_.find(square);
    if (found == piecePlacements_.cend())
    {
      return SquareStatus::EMPTY;
    }

    if (found->second.color == color)
    {
      return SquareStatus::ALLY;
    }

    return SquareStatus::ENEMY;
  }

  auto ChessRule::evaluateAndCollectCandidateMove(
      Position const &square,
      std::string const &color,
      CandidateMoveInfo &candidateMoves) const noexcept -> SquareStatus
  {
    auto squareStatus = getSquareStatus(square, color);

    if (squareStatus == SquareStatus::EMPTY)
    {
      candidateMoves.quietMoves.emplace_back(square);
    }
    else if (squareStatus == SquareStatus::ENEMY)
    {
      candidateMoves.captureMoves.emplace_back(square);
    }

    return squareStatus;
  }

  auto ChessRule::isStraightMovePossible(
      Position const &square, std::string const &color) const noexcept -> bool
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
      if (getSquareStatus(nearSquare, color) == SquareStatus::ENEMY ||
          getSquareStatus(nearSquare, color) == SquareStatus::EMPTY)
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
    Position const nearSquares[]{
        Position{char(col + 1), char(row + 1), '\0'},
        Position{char(col + 1), char(row - 1), '\0'},
        Position{char(col - 1), char(row + 1), '\0'},
        Position{char(col - 1), char(row - 1), '\0'}};

    for (Position const &nearSquare : nearSquares)
    {
      if (getSquareStatus(nearSquare, color) == SquareStatus::ENEMY ||
          getSquareStatus(nearSquare, color) == SquareStatus::EMPTY)
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
      if (getSquareStatus(sqr, color) == SquareStatus::ENEMY ||
          getSquareStatus(sqr, color) == SquareStatus::EMPTY)
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
    if (piecePlacements_.find(front) == piecePlacements_.cend())
    {
      return true;
    }

    Position left{char(col - 1), char(row + step), '\0'};
    auto found = piecePlacements_.find(left);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      return true;
    }

    Position right{char(col + 1), char(row + step), '\0'};
    found = piecePlacements_.find(right);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      return true;
    }

    return isEnPassantCapturePossible(square, color);
  }

  auto ChessRule::isEnPassantCapturePossible(
      Position const &square,
      std::string const &color) const noexcept -> bool
  {
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return false;
    }

    auto const &specialMoveTracker = moveTracker.getSpecialPieceMoveTracker(color);
    int const col = int(square[0]);
    if (col > ChessUtils::FIRST_COL)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col - 1) ==
          moveTracker.getTotalMoveCount())
      {
        return true;
      }
    }

    if (col < ChessUtils::FIRST_COL + ChessUtils::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col + 1) ==
          moveTracker.getTotalMoveCount())
      {
        return true;
      }
    }

    return false;
  }

  auto ChessRule::getStraightMovableSquares(
      Position const &square,
      std::string const &color,
      int radius) const noexcept -> CandidateMoveInfo
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        radius > 0 && radius < ChessUtils::BOARD_SIDE,
        "The radius of neighborhood must be greater than 0 and less than ChessUtils::BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    CandidateMoveInfo candidateMoves;

    for (int i = 1; i <= radius; ++i)
    {
      if (row + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_ROW - 1)
      {
        Position sqr{char(col), char(row + i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (row - i >= ChessUtils::FIRST_ROW)
      {
        Position sqr{char(col), char(row - i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_COL - 1)
      {
        Position sqr{char(col + i), char(row), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col - i >= ChessUtils::FIRST_COL)
      {
        Position sqr{char(col - i), char(row), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    return candidateMoves;
  }

  auto ChessRule::getDiagonalMovableSquares(
      Position const &square,
      std::string const &color,
      int radius) const noexcept -> CandidateMoveInfo
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        radius > 0 && radius < ChessUtils::BOARD_SIDE,
        "The radius of neighborhood must be greater than 0 and less than ChessUtils::BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    CandidateMoveInfo candidateMoves;

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_COL - 1) &&
          (row + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_ROW - 1))
      {
        Position sqr{char(col + i), char(row + i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_COL - 1) && (row - i >= ChessUtils::FIRST_ROW))
      {
        Position sqr{char(col + i), char(row - i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i >= ChessUtils::FIRST_COL) && (row + i <= ChessUtils::BOARD_SIDE + ChessUtils::FIRST_ROW - 1))
      {
        Position sqr{char(col - i), char(row + i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i >= ChessUtils::FIRST_COL) && (row - i >= ChessUtils::FIRST_ROW))
      {
        Position sqr{char(col - i), char(row - i), '\0'};
        auto squareStatus = evaluateAndCollectCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    return candidateMoves;
  }

  auto ChessRule::getKnightMovableSquares(
      Position const &square,
      std::string const &color) const noexcept -> CandidateMoveInfo
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

    CandidateMoveInfo result;
    for (Position const &sqr : possibleSquares)
    {
      if (getSquareStatus(sqr, color) == SquareStatus::EMPTY)
      {
        result.quietMoves.emplace_back(sqr);
      }

      if (getSquareStatus(sqr, color) == SquareStatus::ENEMY)
      {
        result.captureMoves.emplace_back(sqr);
      }
    }

    return result;
  }

  auto ChessRule::getPawnMovableSquares(
      Position const &square,
      std::string const &color) const noexcept -> CandidateMoveInfo
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

    CandidateMoveInfo result;

    Position front1{char(col), char(row + step), '\0'};
    if (piecePlacements_.find(front1) == piecePlacements_.cend())
    {
      result.quietMoves.emplace_back(front1);

      Position front2{char(col), char(row + 2 * step), '\0'};
      if (piecePlacements_.find(front2) == piecePlacements_.cend())
      {
        result.quietMoves.emplace_back(front2);
      }
    }

    Position left{char(col - 1), char(row + step), '\0'};
    auto found = piecePlacements_.find(left);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      result.captureMoves.emplace_back(left);
    }

    Position right{char(col + 1), char(row + step), '\0'};
    found = piecePlacements_.find(right);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      result.captureMoves.emplace_back(right);
    }

    result.enPassantPos = getEnPassantCapturableSquare(square, color);

    return result;
  }

  auto ChessRule::getEnPassantCapturableSquare(
      Position const &square,
      std::string const &color) const noexcept -> std::optional<Position>
  {
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return std::nullopt;
    }

    auto const &specialMoveTracker = moveTracker.getSpecialPieceMoveTracker(color);
    int const col = int(square[0]);
    if (col > ChessUtils::FIRST_COL)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col - 1) ==
          moveTracker.getTotalMoveCount())
      {
        return Position{char(col - 1), square[1], '\0'};
      }
    }
    else if (col < ChessUtils::FIRST_COL + ChessUtils::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col + 1) ==
          moveTracker.getTotalMoveCount())
      {
        return Position{char(col + 1), square[1], '\0'};
      }
    }

    return std::nullopt;
  }

  auto ChessRule::getCastlableSquares(
      Position const &square,
      std::string const &color) const noexcept -> std::list<Position>
  {
    std::list<Position> result;
    auto const &specialMoveTracker = moveTracker.getSpecialPieceMoveTracker(color);
    if (specialMoveTracker.isKingMoved())
    {
      return result;
    }

    if (!specialMoveTracker.isRookAMoved())
    {
      result.emplace_back(Position{square[0] - 2, square[1], '\0'});
    }

    if (!specialMoveTracker.isRookHMoved())
    {
      result.emplace_back(Position{square[0] + 2, square[1], '\0'});
    }

    return result;
  }

} // namespace bgg
