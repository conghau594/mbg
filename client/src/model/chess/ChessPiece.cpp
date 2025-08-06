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
      IChessRule const *rule,
      EntityType const &type,
      Side const &color,
      Position const &square,
      std::vector<std::pair<int, int>> moveVectorList,
      int maxMoveVectorFactor) noexcept
      : Piece(rule, type, color, square),
        moveVectorList_(std::move(moveVectorList)),
        maxMoveVectorFactor_(maxMoveVectorFactor)
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_CHESS_PIECE(type);
    BGG_VALIDATE_SQUARE(square);
  }

  NormalChessPiece::NormalChessPiece(
      NormalChessPiece const &other, IChessRule const *rule) noexcept
      : Piece(rule, other.getType(), other.getSide(), other.getPosition()),
        moveVectorList_(other.moveVectorList_),
        maxMoveVectorFactor_(other.maxMoveVectorFactor_)
  {
  }

  auto NormalChessPiece::clone(IChessRule const *rule) const noexcept
      -> std::shared_ptr<Piece>
  {
    return std::make_shared<NormalChessPiece>(*this, rule);
  }

  auto NormalChessPiece::canMove() const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    for (auto const &[dFile, dRank] : moveVectorList_)
    {
      Position nextSquare{file + dFile, rank + dRank};

      IChessRule const *rule = getRule();
      auto positionStatus = rule->getPositionStatus(nextSquare, getSide());
      if (positionStatus == Position::Status::OPPONENT ||
          positionStatus == Position::Status::EMPTY)
      {
        return true;
      }
    }

    return false;
  }

  auto NormalChessPiece::canMoveTo(Position const &square) const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();
    for (auto const &[dFile, dRank] : moveVectorList_)
    {
      if (dRank * (file - square.getFile()) != dFile * (rank - square.getRank()))
      {
        continue;
      }

      Position nextSquare{file, rank};
      for (int i = 0; i < maxMoveVectorFactor_; ++i)
      {
        nextSquare.getFile() += dFile;
        nextSquare.getRank() += dRank;
        auto positionStatus = rule->getPositionStatus(nextSquare, getSide());
        if (square == nextSquare)
        {
          if (positionStatus == Position::Status::OPPONENT ||
              positionStatus == Position::Status::EMPTY)
          {
            return true;
          }
          return false;
        }
        else if (positionStatus != Position::Status::EMPTY)
        {
          break;
        }
      }
    }

    return false;
  }

  auto NormalChessPiece::canCapture(Piece const &piece) const noexcept -> bool
  {
    if (getSide() == piece.getSide())
    {
      return false;
    }

    return canMoveTo(piece.getPosition());
  }

  auto NormalChessPiece::collectReachablePositions() const noexcept
      -> ReachablePosInfo
  {
    ReachablePosInfo reachable;

    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();
    for (auto const &[dFile, dRank] : moveVectorList_)
    {
      Position nextSquare{file, rank};
      for (int i = 0; i < maxMoveVectorFactor_; ++i)
      {
        nextSquare.getFile() += dFile;
        nextSquare.getRank() += dRank;

        auto positionStatus = rule->getPositionStatus(nextSquare, getSide());
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

  Knight::Knight(IChessRule const *rule,
                 Side const &color,
                 Position const &square) noexcept
      : NormalChessPiece(
            rule,
            chess::KNIGHT,
            color,
            square,
            KNIGHT_MOVE_VECTORS,
            1)
  {
  }

  Bishop::Bishop(IChessRule const *rule,
                 Side const &color,
                 Position const &square) noexcept
      : NormalChessPiece(
            rule,
            chess::BISHOP,
            color,
            square,
            BISHOP_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  Rook::Rook(IChessRule const *rule,
             Side const &color,
             Position const &square) noexcept
      : NormalChessPiece(
            rule,
            chess::ROOK,
            color,
            square,
            ROOK_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  Queen::Queen(IChessRule const *rule,
               Side const &color,
               Position const &square) noexcept
      : NormalChessPiece(
            rule,
            chess::QUEEN,
            color,
            square,
            KING_QUEEN_MOVE_VECTORS,
            chess::BOARD_SIDE_LENGTH - 1)
  {
  }

  King::King(IChessRule const *rule,
             Side const &color,
             Position const &square) noexcept
      : NormalChessPiece(
            rule,
            chess::KING,
            color,
            square,
            KING_QUEEN_MOVE_VECTORS,
            1),
        initialRookSquares_{{color == chess::WHITE ? "a1" : "a8"},
                            {color == chess::WHITE ? "h1" : "h8"}}
  {
  }

  King::King(King const &other, IChessRule const *rule) noexcept
      : NormalChessPiece(other, rule),
        initialRookSquares_(other.initialRookSquares_)
  {
  }

  auto King::clone(IChessRule const *rule) const noexcept -> std::shared_ptr<Piece>
  {
    return std::make_shared<King>(*this, rule);
  }

  auto King::canMoveTo(Position const &square) const noexcept -> bool
  {
    if (isMoved())
    {
      return NormalChessPiece::canMoveTo(square);
    }

    // IChessRule *rule = getRule();
    for (Position const &rookSource : initialRookSquares_)
    {
      auto rookDestination = findRookCastlingDestination(rookSource);
      if (!rookDestination)
      {
        continue;
      }

      if (square == *rookDestination)
      {
        return true;
      }
    }

    return false;
  }

  auto King::collectReachablePositions() const noexcept
      -> ReachablePosInfo
  {
    auto reachable = NormalChessPiece::collectReachablePositions();
    if (isMoved())
    {
      return reachable;
    }

    for (Position const &rookSource : initialRookSquares_)
    {
      auto rookDestination = findRookCastlingDestination(rookSource);
      if (!rookDestination)
      {
        continue;
      }

      Position kingDestination = chess::getCastlingKingDestination(rookSource);
      reachable.specialPositions.emplace_back(kingDestination);
    }

    return reachable;
  }

  auto King::findRookCastlingDestination(Position const &rookSource) const noexcept -> std::optional<Position>
  {
    IChessRule const *rule = getRule();
    auto rook = rule->findPiece(rookSource);
    if (isMoved() ||
        rook == nullptr ||
        rook->isMoved() ||
        rook->getSide() != getSide() ||
        rook->getType() != chess::ROOK)
    {
      return std::nullopt;
    }

    int const &kingFile = getPosition().getFile();
    int const &kingRank = getPosition().getRank();
    int const &rookFile = rook->getPosition().getFile();

    BOOST_ASSERT_MSG(
        kingFile != rookFile,
        "The files of king and rook cannot be the same");

    if (kingFile < rookFile)
    {
      for (int file = 1 + kingFile; file < rookFile; ++file)
      {
        if (rule->findPiece(Position(file, kingRank)))
        {
          return std::nullopt;
        }
      }
    }
    else // if (kingFile > rookFile)
    {
      for (int file = 1 + rookFile; file < kingFile; ++file)
      {
        if (rule->findPiece(Position(file, kingRank)))
        {
          return std::nullopt;
        }
      }
    }

    return Position{(1 + kingFile + rookFile) / 2, kingRank};
  }

  Pawn::Pawn(IChessRule const *rule,
             Side const &color,
             Position const &square) noexcept
      : Piece(rule, chess::PAWN, color, square),
        step_(chess::getPawnStep(color)),
        enPassantRank_(chess::getEnPassantRank(color))
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
  }

  Pawn::Pawn(Pawn const &other, IChessRule const *rule) noexcept
      : Piece(rule, chess::PAWN, other.getSide(), other.getPosition()),
        step_(other.step_),
        enPassantRank_(other.enPassantRank_)
  {
  }

  auto Pawn::clone(IChessRule const *rule) const noexcept -> std::shared_ptr<Piece>
  {
    return std::make_shared<Pawn>(*this, rule);
  }

  auto Pawn::canMove() const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();

    Position normalCaptureSquares[2]{
        {file - 1, rank + step_},
        {file + 1, rank + step_}};
    for (auto const &square : normalCaptureSquares)
    {
      auto squareStatus = rule->getPositionStatus(square, getSide());
      if (squareStatus == Position::Status::OPPONENT)
      {
        return true;
      }
    }

    Position forwardSquare(file, rank + step_);
    if (!rule->findPiece(forwardSquare))
    {
      return true;
    }

    if (findEnPassantDestination())
    {
      return true;
    }

    return false;
  }

  auto Pawn::canMoveTo(Position const &square) const noexcept -> bool
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();
    Position forwardSquare(file, rank + step_);
    if (square == forwardSquare)
    {
      if (!rule->findPiece(forwardSquare))
      {
        return true;
      }
      return false;
    }

    Position normalCaptureSquares[2]{
        {file - 1, rank + step_},
        {file + 1, rank + step_}};
    for (auto const &captureSquare : normalCaptureSquares)
    {
      if (square == captureSquare)
      {
        auto captureSquareStatus = rule->getPositionStatus(
            captureSquare, getSide());
        if (captureSquareStatus == Position::Status::OPPONENT)
        {
          return true;
        }
        return false;
      }
    }

    if (!isMoved())
    {
      Position twoSquaresForward(file, rank + 2 * step_);
      if (square == twoSquaresForward)
      {
        if (!rule->findPiece(twoSquaresForward))
        {
          return true;
        }
        return false;
      }
    }
    else if (auto enPassantDestination = findEnPassantDestination())
    {
      if (square == *enPassantDestination)
      {
        return true;
      }
    }

    return false;
  }

  auto Pawn::canCapture(Piece const &piece) const noexcept -> bool
  {
    if (getSide() == piece.getSide())
    {
      return false;
    }

    Position normalCaptureSquares[2]{
        {getPosition().getFile() - 1, getPosition().getRank() + step_},
        {getPosition().getFile() + 1, getPosition().getRank() + step_}};
    for (auto const &captureSquare : normalCaptureSquares)
    {
      if (piece.getPosition() == captureSquare)
      {
        return true;
      }
    }

    if (piece.getType() != chess::PAWN)
    {
      return false;
    }

    if (auto enPassantDestination = findEnPassantDestination())
    {
      Position enPassantCaptureSquare{
          enPassantDestination->getFile(), enPassantRank_};
      if (piece.getPosition() == enPassantCaptureSquare)
      {
        return true;
      }
    }

    return false;
  }

  auto Pawn::collectReachablePositions() const noexcept -> ReachablePosInfo
  {
    ReachablePosInfo reachable;
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();
    Position normalCaptureSquares[2]{
        {file - 1, rank + step_},
        {file + 1, rank + step_}};
    for (auto const &square : normalCaptureSquares)
    {
      auto squareStatus = rule->getPositionStatus(square, getSide());
      if (squareStatus == Position::Status::OPPONENT)
      {
        reachable.capturePositions.emplace_back(square);
      }
    }

    Position forwardSquare(file, rank + step_);
    if (!rule->findPiece(forwardSquare))
    {
      reachable.quietPositions.emplace_back(forwardSquare);
      if (!isMoved())
      {
        Position twoSquaresForward(file, rank + 2 * step_);
        if (!rule->findPiece(twoSquaresForward))
        {
          reachable.quietPositions.emplace_back(twoSquaresForward);
        }
      }
    }

    if (auto enPassantDestination = findEnPassantDestination())
    {
      reachable.specialPositions.emplace_back(*enPassantDestination);
    }

    return reachable;
  }

  auto Pawn::findEnPassantDestination() const noexcept -> std::optional<Position>
  {
    int const &file = getPosition().getFile();
    int const &rank = getPosition().getRank();
    IChessRule const *rule = getRule();
    if (enPassantRank_ != rank)
    {
      return std::nullopt;
    }

    Position enPassantDestinations[2]{
        {file - 1, enPassantRank_ + step_},
        {file + 1, enPassantRank_ + step_}};

    for (auto const &destination : enPassantDestinations)
    {
      if (destination.getFile() < chess::FIRST_FILE ||
          destination.getFile() > chess::LAST_FILE)
      {
        continue;
      }

      auto initialPosOfCapturedPawn = Position{
          destination.getFile(), destination.getRank() + step_};
      auto lastMove = rule->getLastMove().getIf<ChessMove::Normal>();
      if (lastMove == nullptr ||
          lastMove->fromSquare != initialPosOfCapturedPawn ||
          lastMove->movedPiece != chess::PAWN ||
          lastMove->toSquare != Position{destination.getFile(), enPassantRank_})
      {
        continue;
      }

      return destination;
    }

    return std::nullopt;
  }

  auto ChessPieceFactory::createPiece(
      IChessRule *rule,
      EntityType type,
      Side color,
      Position square) const noexcept -> std::shared_ptr<Piece>
  {
    BGG_VALIDATE_COLOR(color);
    BGG_VALIDATE_SQUARE(square);
    BGG_VALIDATE_CHESS_PIECE(type);

    if (type == chess::PAWN)
    {
      return std::make_shared<Pawn>(rule, color, square);
    }
    else if (type == chess::KNIGHT)
    {
      return std::make_shared<Knight>(rule, color, square);
    }
    else if (type == chess::BISHOP)
    {
      return std::make_shared<Bishop>(rule, color, square);
    }
    else if (type == chess::ROOK)
    {
      return std::make_shared<Rook>(rule, color, square);
    }
    else if (type == chess::QUEEN)
    {
      return std::make_shared<Queen>(rule, color, square);
    }
    else if (type == chess::KING)
    {
      return std::make_shared<King>(rule, color, square);
    }
    else
    {
      BOOST_ASSERT_MSG(false, "This factory is only for creating chess piece");
      return nullptr;
    }
  }
} // namespace bgg
