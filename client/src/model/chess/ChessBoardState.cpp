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

  auto ChessBoardState::getColor(Position const &square) const noexcept
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

  auto ChessBoardState::getSelectablePieces() const noexcept
      -> std::map<Position, Piece>
  {
    std::map<Position, Piece> selectablePieces;
    for (auto &[square, piece] : piecePlacements_)
    {
      if (piece.color != yourColor_)
      {
        continue;
      }

      if (piece.type == ChessRule::KING ||
          piece.type == ChessRule::QUEEN)
      {
        if (isStraightMovePossible(square, yourColor_) ||
            isDiagonalMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::ROOK)
      {
        if (isStraightMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::BISHOP)
      {
        if (isDiagonalMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::KNIGHT)
      {
        if (isKnightMovePossible(square, yourColor_))
        {
          selectablePieces.emplace(square, piece);
        }
      }
      else if (piece.type == ChessRule::PAWN)
      {
        if (isPawnMovePossible(square, yourColor_))
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
    if (!piece)
    {
      return CandidateChessMoveInfo{};
    }

    if (piece->type == ChessRule::QUEEN)
    {
      CandidateChessMoveInfo candidateMoves = getStraightMovableSquares(
          square, piece->color, ChessRule::BOARD_SIDE - 1);
      candidateMoves.merge(getDiagonalMovableSquares(
          square, piece->color, ChessRule::BOARD_SIDE - 1));

      return candidateMoves;
    }
    else if (piece->type == ChessRule::KING)
    {
      CandidateChessMoveInfo candidateMoves = getStraightMovableSquares(
          square, piece->color, 1);
      candidateMoves.merge(getDiagonalMovableSquares(
          square, piece->color, 1));

      candidateMoves.quietSquares.splice(
          candidateMoves.quietSquares.end(),
          getValidCastlingSquares(square, piece->color));

      return candidateMoves;
    }
    else if (piece->type == ChessRule::ROOK)
    {
      return getStraightMovableSquares(square, piece->color, ChessRule::BOARD_SIDE - 1);
    }
    else if (piece->type == ChessRule::BISHOP)
    {
      return getDiagonalMovableSquares(square, piece->color, ChessRule::BOARD_SIDE - 1);
    }
    else if (piece->type == ChessRule::KNIGHT)
    {
      return getKnightMovableSquares(square, piece->color);
    }
    else // if (piece->type == ChessRule::PAWN)
    {
      return getPawnMovableSquares(square, piece->color);
    }
  }

  auto ChessBoardState::getPiece(Position const &square) const noexcept
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

  auto ChessBoardState::tryMove(ChessMove const &move) const noexcept
      -> ChessMove::VariantAction
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

    ChessMove::VariantAction moveAction;
    if (movedPiece->type == ChessRule::KING)
    {
      std::list<Position> castlingSquares = getValidCastlingSquares(
          move.fromSquare, movedPiece->color);

      for (auto &sqr : castlingSquares)
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
              (move.fromSquare[0] + castlingRookMove.second[0]) / 2,
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

      CandidateChessMoveInfo candidateMoves = getStraightMovableSquares(
          move.fromSquare, movedPiece->color, 1);
      candidateMoves.merge(getDiagonalMovableSquares(
          move.fromSquare, movedPiece->color, 1));

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.quietSquares)
        {
          if (sqr == move.toSquare)
          {
            moveAction = ChessMove::Normal{
                move.fromSquare,
                move.toSquare,
                false,
                KingStatus::SAFE};
            break;
          }
        }
      }

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.captureSquares)
        {
          if (sqr == move.toSquare)
          {
            moveAction = ChessMove::Normal{
                move.fromSquare,
                move.toSquare,
                true,
                KingStatus::SAFE};
            break;
          }
        }
      }

      if (moveAction.isEmpty())
      {
        return ChessMove::Invalid{ChessMove::Error::INVALID_DESTINATION_SQUARE};
      }
    }
    else if (movedPiece->type == ChessRule::PAWN)
    {
      CandidateChessMoveInfo candidateMoves = getPawnMovableSquares(
          move.fromSquare, movedPiece->color);
      if (candidateMoves.enPassantSquare)
      {
        Position const &enPassantSqr = candidateMoves.enPassantSquare.value();
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

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.quietSquares)
        {
          if (sqr == move.toSquare)
          {
            if (!ChessRule::isPromotionSquare(move.toSquare, movedPiece->color))
            {
              moveAction = ChessMove::Normal{
                  move.fromSquare,
                  move.toSquare,
                  false,
                  KingStatus::SAFE};
              break;
            }
            else if (move.promote)
            {
              moveAction = ChessMove::Promotion{
                  move.fromSquare,
                  move.toSquare,
                  false,
                  KingStatus::SAFE,
                  move.promote.value()};
              break;
            }
            else
            {
              return ChessMove::Invalid{ChessMove::Error::INVALID_PROMOTION};
            }
          }
        }
      }

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.captureSquares)
        {
          if (sqr == move.toSquare)
          {
            if (!ChessRule::isPromotionSquare(move.toSquare, movedPiece->color))
            {
              moveAction = ChessMove::Normal{
                  move.fromSquare,
                  move.toSquare,
                  true,
                  KingStatus::SAFE};
              break;
            }
            else if (move.promote)
            {
              moveAction = ChessMove::Promotion{
                  move.fromSquare,
                  move.toSquare,
                  true,
                  KingStatus::SAFE,
                  move.promote.value()};
              break;
            }
            else
            {
              return ChessMove::Invalid{ChessMove::Error::INVALID_PROMOTION};
            }
          }
        }
      }

      if (moveAction.isEmpty())
      {
        return ChessMove::Invalid{ChessMove::Error::INVALID_DESTINATION_SQUARE};
      }
    }
    else // other pieces
    {
      CandidateChessMoveInfo candidateMoves = getCandidateMoves(move.fromSquare);

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.quietSquares)
        {
          if (sqr == move.toSquare)
          {
            moveAction = ChessMove::Normal{
                move.fromSquare,
                move.toSquare,
                false,
                KingStatus::SAFE};
            break;
          }
        }
      }

      if (moveAction.isEmpty())
      {
        for (auto &sqr : candidateMoves.captureSquares)
        {
          if (sqr == move.toSquare)
          {
            moveAction = ChessMove::Normal{
                move.fromSquare,
                move.toSquare,
                true,
                KingStatus::SAFE};
            break;
          }
        }
      }

      if (moveAction.isEmpty())
      {
        return ChessMove::Invalid{ChessMove::Error::INVALID_DESTINATION_SQUARE};
      }
    }

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

  auto ChessBoardState::getSquareStatus(
      Position const &square, std::string const &color) const noexcept
      -> SquareStatus
  {
    int const col = int(square[0]);
    int const row = int(square[1]);

    if ((col < ChessRule::FIRST_COL) || (col > ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) ||
        (row < ChessRule::FIRST_ROW) || (row > ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
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

  auto ChessBoardState::tryAddCandidateMove(
      Position const &square,
      std::string const &color,
      CandidateChessMoveInfo &candidateMoves) const noexcept -> SquareStatus
  {
    auto squareStatus = getSquareStatus(square, color);

    if (squareStatus == SquareStatus::EMPTY)
    {
      candidateMoves.quietSquares.emplace_back(square);
    }
    else if (squareStatus == SquareStatus::ENEMY)
    {
      candidateMoves.captureSquares.emplace_back(square);
    }

    return squareStatus;
  }

  auto ChessBoardState::isStraightMovePossible(
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

  auto ChessBoardState::isDiagonalMovePossible(
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

  auto ChessBoardState::isKnightMovePossible(
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

  auto ChessBoardState::isPawnMovePossible(
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

  auto ChessBoardState::isEnPassantCapturePossible(
      Position const &square,
      std::string const &color) const noexcept -> bool
  {
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return false;
    }

    auto const &specialMoveTracker = moveTracker_.getSpecialPieceMoveTracker(color);
    int const col = int(square[0]);
    int const moveCount = moveTracker_.getTotalMoveCount();
    if (col > ChessRule::FIRST_COL)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col - 1) == moveCount)
      {
        return true;
      }
    }

    if (col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col + 1) == moveCount)
      {
        return true;
      }
    }

    return false;
  }

  auto ChessBoardState::getStraightMovableSquares(
      Position const &square,
      std::string const &color,
      int radius) const noexcept -> CandidateChessMoveInfo
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        (radius > 0) && (radius < ChessRule::BOARD_SIDE),
        "The radius of neighborhood must be greater than 0 and less than BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    CandidateChessMoveInfo candidateMoves;

    for (int i = 1; i <= radius; ++i)
    {
      if (row + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1)
      {
        Position sqr{char(col), char(row + i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (row - i >= ChessRule::FIRST_ROW)
      {
        Position sqr{char(col), char(row - i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1)
      {
        Position sqr{char(col + i), char(row), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if (col - i >= ChessRule::FIRST_COL)
      {
        Position sqr{char(col - i), char(row), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    return candidateMoves;
  }

  auto ChessBoardState::getDiagonalMovableSquares(
      Position const &square,
      std::string const &color,
      int radius) const noexcept -> CandidateChessMoveInfo
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BOOST_ASSERT_MSG(
        radius > 0 && radius < ChessRule::BOARD_SIDE,
        "The radius of neighborhood must be greater than 0 and less than BOARD_SIDE");

    int const col = int(square[0]);
    int const row = int(square[1]);

    CandidateChessMoveInfo candidateMoves;

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) &&
          (row + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
      {
        Position sqr{char(col + i), char(row + i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_COL - 1) &&
          (row - i >= ChessRule::FIRST_ROW))
      {
        Position sqr{char(col + i), char(row - i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i >= ChessRule::FIRST_COL) &&
          (row + i <= ChessRule::BOARD_SIDE + ChessRule::FIRST_ROW - 1))
      {
        Position sqr{char(col - i), char(row + i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    for (int i = 1; i <= radius; ++i)
    {
      if ((col - i >= ChessRule::FIRST_COL) &&
          (row - i >= ChessRule::FIRST_ROW))
      {
        Position sqr{char(col - i), char(row - i), '\0'};
        auto squareStatus = tryAddCandidateMove(sqr, color, candidateMoves);
        if (squareStatus != SquareStatus::EMPTY)
        {
          break;
        }
      }
    }

    return candidateMoves;
  }

  auto ChessBoardState::getKnightMovableSquares(
      Position const &square,
      std::string const &color) const noexcept -> CandidateChessMoveInfo
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

    CandidateChessMoveInfo candidateMoves;
    for (Position const &sqr : possibleSquares)
    {
      tryAddCandidateMove(sqr, color, candidateMoves);
    }

    return candidateMoves;
  }

  auto ChessBoardState::getPawnMovableSquares(
      Position const &square,
      std::string const &color) const noexcept -> CandidateChessMoveInfo
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

    CandidateChessMoveInfo result;

    Position front1{char(col), char(row + step), '\0'};
    if (piecePlacements_.find(front1) == piecePlacements_.cend())
    {
      result.quietSquares.emplace_back(front1);

      Position front2{char(col), char(row + 2 * step), '\0'};
      if (piecePlacements_.find(front2) == piecePlacements_.cend())
      {
        result.quietSquares.emplace_back(front2);
      }
    }

    Position left{char(col - 1), char(row + step), '\0'};
    auto found = piecePlacements_.find(left);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      result.captureSquares.emplace_back(left);
    }

    Position right{char(col + 1), char(row + step), '\0'};
    found = piecePlacements_.find(right);
    if ((found != piecePlacements_.cend()) && (found->second.color != color))
    {
      result.captureSquares.emplace_back(right);
    }

    auto enPassantPos = getEnPassantSquare(square, color);
    if (enPassantPos)
    {
      result.enPassantSquare = std::move(enPassantPos);
    }

    return result;
  }

  auto ChessBoardState::getEnPassantSquare(
      Position const &square,
      std::string const &color) const noexcept -> std::optional<Position>
  {
    if (!(square[1] == '5' && color == Color::WHITE) &&
        !(square[1] == '4' && color == Color::BLACK))
    {
      return std::nullopt;
    }

    auto const &specialMoveTracker = moveTracker_.getSpecialPieceMoveTracker(color);
    int const col = int(square[0]);
    if (col > ChessRule::FIRST_COL)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col - 1) ==
          moveTracker_.getTotalMoveCount())
      {
        return Position{char(col - 1), square[1], '\0'};
      }
    }
    else if (col < ChessRule::FIRST_COL + ChessRule::BOARD_SIDE - 1)
    {
      if (specialMoveTracker.getFirstDoubleStepMoveOfPawn(col + 1) ==
          moveTracker_.getTotalMoveCount())
      {
        return Position{char(col + 1), square[1], '\0'};
      }
    }

    return std::nullopt;
  }

  auto ChessBoardState::getValidCastlingSquares(
      Position const &square,
      std::string const &color) const noexcept -> std::list<Position>
  {
    std::list<Position> result;
    if (!(square == Position("e1") && color == Color::WHITE) &&
        !(square == Position("e8") && color == Color::BLACK))
    {
      return result;
    }

    auto const &specialMoveTracker = moveTracker_.getSpecialPieceMoveTracker(color);
    if (specialMoveTracker.isKingMoved())
    {
      return result;
    }

    if (!specialMoveTracker.isRookAMoved())
    {
      result.emplace_back(Position{char(square[0] - 2), square[1], '\0'});
    }

    if (!specialMoveTracker.isRookHMoved())
    {
      result.emplace_back(Position{char(square[0] + 2), square[1], '\0'});
    }

    return result;
  }

} // namespace bgg
