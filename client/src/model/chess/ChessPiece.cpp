// ChessPiece.cpp
#include "ChessPiece.h"
#include "ChessHelpers.h"
#include "IChessRule.h"

namespace bgg
{
  static constexpr std::initializer_list<std::pair<int, int>> KNIGHT_MOVE_VECTORS{
      std::make_pair(1, 2),
      std::make_pair(1, -2),
      std::make_pair(-1, 2),
      std::make_pair(-1, -2),
      std::make_pair(2, 1),
      std::make_pair(2, -1),
      std::make_pair(-2, 1),
      std::make_pair(-2, -1)};

  static constexpr std::initializer_list<std::pair<int, int>> BISHOP_MOVE_VECTORS{
      std::make_pair(1, 1),
      std::make_pair(1, -1),
      std::make_pair(-1, 1),
      std::make_pair(-1, -1)};

  static constexpr std::initializer_list<std::pair<int, int>> ROOK_MOVE_VECTORS{
      std::make_pair(0, 1),
      std::make_pair(0, -1),
      std::make_pair(1, 0),
      std::make_pair(-1, 0)};

  static constexpr std::initializer_list<std::pair<int, int>> KING_QUEEN_MOVE_VECTORS{
      std::make_pair(0, 1),
      std::make_pair(0, -1),
      std::make_pair(1, 0),
      std::make_pair(-1, 0),
      std::make_pair(1, 1),
      std::make_pair(1, -1),
      std::make_pair(-1, 1),
      std::make_pair(-1, -1)};

  NormalChessPiece::NormalChessPiece(
      IChessRule *board,
      EntityType const &type,
      Side const &color,
      Position const &square,
      std::vector<std::pair<int, int>> moveVectorList,
      int maxMoveVectorFactor) noexcept
      : Piece(type, color, square),
        board_(board),
        moveVectorList_(std::move(moveVectorList)),
        maxMoveVectorFactor_(maxMoveVectorFactor)
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_CHESS_PIECE(type);
    BGG_VALIDATE_SQUARE(square);
  }

  auto NormalChessPiece::canMove() const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    for (auto const &[dFile, dRank] : moveVectorList_)
    {
      Position nextSquare{file + dFile, rank + dRank};

      auto positionStatus = board_->getPositionStatus(nextSquare, getSide());
      if (positionStatus == Position::Status::OPPONENT ||
          positionStatus == Position::Status::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto NormalChessPiece::collectReachablePositions() const noexcept
      -> ReachablePosInfo
  {
    ReachablePosInfo reachable;

    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    for (auto const &[dFile, dRank] : moveVectorList_)
    {
      Position nextSquare{file, rank};
      for (int i = 0; i < maxMoveVectorFactor_; ++i)
      {
        nextSquare.getFile() += dFile;
        nextSquare.getRank() += dRank;

        auto positionStatus = board_->getPositionStatus(nextSquare, getSide());
        if (positionStatus == Position::Status::EMPTY)
        {
          reachable.quietPositions.emplace_back(nextSquare);
        }
        else if (positionStatus == Position::Status::OPPONENT)
        {
          reachable.capturePositions.emplace_back(nextSquare);
          break;
        }
        else // if (positionStatus == ALLY or OUT_OF_BOARD)
        {
          break;
        }
      }
    }
    return reachable;
  }

  auto NormalChessPiece::getBoard() const noexcept -> IChessRule *
  {
    return board_;
  }

  Knight::Knight(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : NormalChessPiece(
            board,
            chess::KNIGHT,
            color,
            square,
            KNIGHT_MOVE_VECTORS,
            1)
  {
  }

  Bishop::Bishop(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : NormalChessPiece(
            board,
            chess::BISHOP,
            color,
            square,
            BISHOP_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  Rook::Rook(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : NormalChessPiece(
            board,
            chess::ROOK,
            color,
            square,
            ROOK_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  Queen::Queen(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : NormalChessPiece(
            board,
            chess::QUEEN,
            color,
            square,
            KING_QUEEN_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  King::King(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : NormalChessPiece(
            board,
            chess::KING,
            color,
            square,
            KING_QUEEN_MOVE_VECTORS,
            1),
        initialRookSquares_{
            Position{color == chess::WHITE ? "a1" : "a8"},
            Position{color == chess::WHITE ? "h1" : "h8"}}
  {
  }

  auto King::collectReachablePositions() const noexcept
      -> ReachablePosInfo
  {
    auto reachable = NormalChessPiece::collectReachablePositions();
    if (isMoved())
    {
      return reachable;
    }

    IChessRule *board = getBoard();
    for (Position const &square : initialRookSquares_)
    {
      auto rook = board->findPiece(square);
      if (rook == nullptr ||
          rook->isMoved() ||
          rook->getType() != chess::ROOK ||
          rook->getSide() != getSide())
      {
        continue;
      }
      auto const &rookPos = rook->getPosition();
      reachable.specialPositions.emplace_back(
          (1 + square.getFile() + rookPos.getFile()) / 2,
          (1 + square.getRank() + rookPos.getRank()) / 2);
    }

    return reachable;
  }

  Pawn::Pawn(
      IChessRule *board, Side const &color, Position const &square) noexcept
      : Piece(chess::PAWN, color, square),
        board_(board),
        step_(color == chess::WHITE ? 1 : -1),
        enPassantRank_(color == chess::WHITE ? int('5') : int('4'))
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
  }

  auto Pawn::canMove() const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    Position forwardSquare(file, rank + step_);
    if (!board_->findPiece(forwardSquare))
    {
      return true;
    }

    Position normalCaptureSquares[2]{
        {file - 1, rank + step_},
        {file + 1, rank + step_}};
    for (auto const &square : normalCaptureSquares)
    {
      if (board_->getPositionStatus(square, getSide()) == Position::Status::OPPONENT)
      {
        return true;
      }
    }

    if (!isMoved())
    {
      Position twoSquaresForward(file, rank + 2 * step_);
      if (!board_->findPiece(twoSquaresForward))
      {
        return true;
      }
    }
    else if (findEnPassantSquare())
    {
      return true;
    }

    return false;
  }

  auto Pawn::collectReachablePositions() const noexcept -> ReachablePosInfo
  {
    ReachablePosInfo reachable;
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    Position forwardSquare(file, rank + step_);
    if (!board_->findPiece(forwardSquare))
    {
      reachable.quietPositions.emplace_back(forwardSquare);
    }

    Position normalCaptureSquares[2]{
        {file - 1, rank + step_},
        {file + 1, rank + step_}};
    for (auto const &square : normalCaptureSquares)
    {
      if (board_->getPositionStatus(square, getSide()) == Position::Status::OPPONENT)
      {
        reachable.capturePositions.emplace_back(square);
      }
    }

    if (!isMoved())
    {
      Position twoSquaresForward(file, rank + 2 * step_);
      if (!board_->findPiece(twoSquaresForward))
      {
        reachable.quietPositions.emplace_back(twoSquaresForward);
      }
    }
    else if (auto enPassantSquare = findEnPassantSquare())
    {
      reachable.specialPositions.emplace_back(*enPassantSquare);
    }

    return reachable;
  }

  auto Pawn::findEnPassantSquare() const noexcept -> std::optional<Position>
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    if (enPassantRank_ != rank)
    {
      return std::nullopt;
    }

    Position enPassantSquares[2]{
        {file - 1, rank},
        {file + 1, rank}};

    for (auto const &square : enPassantSquares)
    {
      if (square.getFile() < chess::FIRST_FILE ||
          square.getFile() > chess::LAST_FILE)
      {
        continue;
      }

      auto capturedPawnFromSquare = Position{
          square.getFile(), square.getRank() + 2 * step_};
      auto lastMove = board_->getLastMove().getIf<ChessMove::Normal>();
      if (lastMove == nullptr ||
          lastMove->fromSquare != capturedPawnFromSquare ||
          lastMove->toSquare != square ||
          lastMove->movedPiece != chess::PAWN)
      {
        continue;
      }

      // Piece *piece = board_->findPiece(square);
      // BOOST_ASSERT_MSG(
      //     true ||
      //         piece == nullptr ||                ///< is empty?
      //         piece->getMoveCount() != 1 ||      ///< is not first move?
      //         piece->getType() != chess::PAWN || ///< is not a pawn?
      //         piece->getSide() == getSide(),     ///< is not of the opponent?
      //     "There is conflict with last move information");

      return lastMove->toSquare;
    }

    return std::nullopt;
  }

  auto ChessPieceFactory::createPiece(
      IChessRule *board,
      EntityType type,
      Side color,
      Position square) const noexcept -> std::shared_ptr<Piece>
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BGG_VALIDATE_CHESS_PIECE(type);

    if (type == chess::PAWN)
    {
      return std::make_shared<Pawn>(board, color, square);
    }
    else if (type == chess::KNIGHT)
    {
      return std::make_shared<Knight>(board, color, square);
    }
    else if (type == chess::BISHOP)
    {
      return std::make_shared<Bishop>(board, color, square);
    }
    else if (type == chess::ROOK)
    {
      return std::make_shared<Rook>(board, color, square);
    }
    else if (type == chess::QUEEN)
    {
      return std::make_shared<Queen>(board, color, square);
    }
    else if (type == chess::KING)
    {
      return std::make_shared<King>(board, color, square);
    }
    else
    {
      BOOST_ASSERT_MSG(false, "This factory is only for creating chess piece");
      return nullptr;
    }
  }
} // namespace bgg
