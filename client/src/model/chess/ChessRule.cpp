// ChessRule.cpp

#include <format>

#include "ChessRule.h"
#include "ChessHelpers.h"
#include "ChessPiece.h"

#include "model/Piece.h"
#include "model/Side.h"

namespace bgg
{

  ChessRule::ChessRule(
      std::list<std::tuple<EntityType, Side, Position>> const &initialPlacements) noexcept
      : pieceFactory_{},
        whitePieces_{},
        blackPieces_{},
        removedPieces_{},
        whiteKing_{nullptr},
        blackKing_{nullptr},
        moveHistory_{},
        moveCount_(0)
  {
    for (auto const &[type, color, square] : initialPlacements)
    {
      auto piece = pieceFactory_.createPiece(this, type, color, square);
      getPieceSet(color).addPiece(piece);
      if (type == chess::KING)
      {
        if (color == chess::WHITE)
        {
          whiteKing_ = piece;
        }
        else // if (color == chess::BLACK)
        {
          blackKing_ = piece;
        }
      }
    }
  }

  ChessRule::ChessRule(ChessRule const &other) noexcept
      : pieceFactory_(other.pieceFactory_),
        whitePieces_(other.whitePieces_, this),
        blackPieces_(other.blackPieces_, this),
        removedPieces_{},
        whiteKing_{nullptr},
        blackKing_{nullptr},
        moveHistory_(other.moveHistory_),
        moveCount_(other.moveCount_)
  {
    for (auto const &piece : whitePieces_)
    {
      // SPDLOG_DEBUG("{} {} at {}",
      //              piece->getSide().toString(),
      //              piece->getType().toString(),
      //              piece->getPosition().toString());
      if (piece->getType() == chess::KING)
      {
        whiteKing_ = piece;
        break;
      }
    }

    for (auto const &piece : blackPieces_)
    {
      // SPDLOG_DEBUG("{} {} at {}",
      //              piece->getSide().toString(),
      //              piece->getType().toString(),
      //              piece->getPosition().toString());
      if (piece->getType() == chess::KING)
      {
        blackKing_ = piece;
        break;
      }
    }

    for (auto const &[moveNumber, piece] : other.removedPieces_)
    {
      removedPieces_.emplace(moveNumber, piece->clone(this));
    }
  }

  auto ChessRule::getMoveCount() const noexcept -> int
  {
    return moveCount_;
  }

  auto ChessRule::getLastMove() const noexcept -> std::optional<ChessMove::Detail>
  {
    if (moveHistory_.empty())
    {
      return std::nullopt;
    }
    return moveHistory_.back();
  }

  auto ChessRule::getPositionStatus(
      Position const &square, Side const &color) const noexcept
      -> Position::Status
  {
    int const &file = square.getFile();
    int const &rank = square.getRank();

    if ((file < chess::FIRST_FILE) ||
        (file > chess::LAST_FILE) ||
        (rank < chess::FIRST_RANK) ||
        (rank > chess::LAST_RANK))
    {
      return Position::Status::OUT_OF_BOARD;
    }

    PieceSet const &allyPieceSet = getPieceSet(color);
    if (allyPieceSet.findPiece(square))
    {
      return Position::Status::ALLY;
    }

    auto opponentColor = chess::getOpponentColor(color);
    PieceSet const &enemyPieceSet = getPieceSet(opponentColor);
    if (enemyPieceSet.findPiece(square))
    {
      return Position::Status::OPPONENT;
    }

    return Position::Status::EMPTY;
  }

  auto ChessRule::collectPieces(Side const &color) const noexcept
      -> std::list<std::shared_ptr<Piece>>
  {
    PieceSet const &pieceSet = getPieceSet(color);

    std::list<std::shared_ptr<Piece>> result;
    for (auto const &piece : pieceSet)
    {
      result.emplace_back(piece);
    }

    return result;
  }

  auto ChessRule::collectSelectablePieces(Side const &color) const noexcept
      -> std::list<std::shared_ptr<Piece>>
  {
    PieceSet const &pieceSet = getPieceSet(color);

    std::list<std::shared_ptr<Piece>> result;
    for (auto const &piece : pieceSet)
    {
      if (piece->canMove())
      {
        result.emplace_back(piece);
      }
    }

    return result;
  }

  auto ChessRule::collectReachableSquares(
      Position const &square, Side const &color) const noexcept
      -> ReachablePosInfo
  {
    PieceSet const &pieceSet = getPieceSet(color);
    auto piece = pieceSet.findPiece(square);
    if (piece)
    {
      return piece->collectReachablePositions();
    }

    return ReachablePosInfo{};
  }

  auto ChessRule::findPiece(Position const &square) const noexcept
      -> std::shared_ptr<Piece>
  {
    auto piece = whitePieces_.findPiece(square);
    if (piece)
    {
      return piece;
    }

    piece = blackPieces_.findPiece(square);
    if (piece)
    {
      return piece;
    }

    return nullptr;
  }

  auto ChessRule::tryMove(ChessMove const &move) const
      -> ChessMove::Detail
  {
    ChessMove::Detail moveDetail = tryParseMove(move);

    Side opponentColor = chess::getOpponentColor(move.color);
    ChessRule cloneRule(*this);
    cloneRule.commitMove(moveDetail);
    Piece const &allyKing = cloneRule.getKing(move.color);
    Piece const &opponentKing = cloneRule.getKing(opponentColor);

    auto allyKingAttackers = cloneRule.findAttackers(allyKing, true);
    if (!allyKingAttackers.empty())
    {
      auto &attacker = allyKingAttackers.back();
      if (allyKing.getPosition() == move.toSquare)
      {
        throw std::logic_error(std::format(
            "{} King cannot do this move because the destination is under "
            "attack by the opponent's {} from '{}'",
            move.color.toString(),
            attacker->getType().toString(),
            attacker->getPosition().toString()));
      }
      else
      {
        throw std::logic_error(std::format(
            "{} cannot do this move because their king is in check "
            "by the opponent's {} from '{}'",
            move.color.toString(),
            attacker->getType().toString(),
            attacker->getPosition().toString()));
      }
    }

    moveDetail.setOpponentKingSquare(opponentKing.getPosition());
    auto opponentKingAttackers = cloneRule.findAttackers(opponentKing, true);
    bool isStalemated = cloneRule.isStalemated(opponentColor);
    if (opponentKingAttackers.empty())
    {
      if (isStalemated)
      {
        moveDetail.setOpponentKingStatus(Side::Status::STALEMATED);
      }
      else
      {
        moveDetail.setOpponentKingStatus(Side::Status::SAFE);
      }
    }
    else
    {
      if (isStalemated)
      {
        moveDetail.setOpponentKingStatus(Side::Status::CHECKMATED);
      }
      else
      {
        moveDetail.setOpponentKingStatus(Side::Status::IN_CHECK);
      }
    }

    return moveDetail;
  }

  auto ChessRule::commitMove(ChessMove::Detail moveDetail) noexcept -> int
  {
    BOOST_ASSERT_MSG(!moveDetail.isEmpty(), "Cannot commit an empty move");
    ++moveCount_;

    Side const &color = moveDetail.getColor();
    Position const &fromSquare = moveDetail.getSourceSquare();
    Position const &toSquare = moveDetail.getDestinationSquare();

    PieceSet &allyPieceSet = getPieceSet(color);
    PieceSet &opponentPieceSet = getPieceSet(chess::getOpponentColor(color));

    auto movedPiece = allyPieceSet.findPiece(fromSquare);
    BOOST_ASSERT_MSG(
        movedPiece, "There must be a moved piece at the 'from square'");

    allyPieceSet.removePiece(fromSquare);
    if (auto capturedPieceType = moveDetail.getCapturedPieceType())
    {
      std::shared_ptr<Piece> capturedPiece;
      Position captureSquare;

      if (auto enPassantCaptureSqr =
              moveDetail.getEnPassantCaptureSquare())
      {
        captureSquare = *enPassantCaptureSqr;
      }
      else
      {
        captureSquare = toSquare;
      }

      capturedPiece = opponentPieceSet.findPiece(captureSquare);
      BOOST_ASSERT_MSG(
          capturedPiece != nullptr,
          "There must be a captured piece in removedPieces_ at 'moveNumber'");

      auto [iter, inserted] = removedPieces_.try_emplace(moveCount_, capturedPiece);
      BOOST_ASSERT_MSG(
          inserted, "Need to ensure that moveCount_ is unique for each commit");

      opponentPieceSet.removePiece(captureSquare);
    }

    if (auto promotedPieceType = moveDetail.getPromotedPieceType())
    {
      auto newPiece = pieceFactory_.createPiece(
          this, *promotedPieceType, color, toSquare);
      auto [iter, inserted] = removedPieces_.try_emplace(~moveCount_, movedPiece);
      BOOST_ASSERT_MSG(
          inserted, "Need to ensure that ~moveCount_ is unique for each commit");

      allyPieceSet.addPiece(newPiece);
    }
    else
    {
      movedPiece->move(toSquare);
      allyPieceSet.addPiece(movedPiece);

      if (auto rookMove = moveDetail.getCastlingRookMove())
      {
        auto rook = findPiece(rookMove->first);
        allyPieceSet.removePiece(rookMove->first);
        rook->setMoveInfo(rookMove->second, 0);
        allyPieceSet.addPiece(rook);
      }
    }

    moveDetail.setMoveNumber(moveCount_);
    moveHistory_.emplace_back(std::move(moveDetail));
    return moveCount_;
  }

  auto ChessRule::revertLastMoveCommit() noexcept -> ChessMove::Detail
  {
    BOOST_ASSERT_MSG(
        !moveHistory_.empty(),
        "Cannot use this function when there is no committed move before");

    auto &moveDetail = moveHistory_.back();
    int const &moveNumber = moveDetail.getMoveNumber();
    BOOST_ASSERT_MSG(
        moveNumber == moveCount_,
        "Need to ensure the moveCount_ and the moveNumber stored in "
        "the moveDetail are the same");

    Side const &color = moveDetail.getColor();
    Position const &fromSquare = moveDetail.getSourceSquare();
    Position const &toSquare = moveDetail.getDestinationSquare();

    PieceSet &allyPieceSet = getPieceSet(color);
    PieceSet &opponentPieceSet = getPieceSet(chess::getOpponentColor(color));

    auto piece = allyPieceSet.findPiece(toSquare);
    allyPieceSet.removePiece(toSquare);

    if (auto capturedPieceType = moveDetail.getCapturedPieceType())
    {
      auto foundPiece = removedPieces_.find(moveNumber);
      BOOST_ASSERT_MSG(
          foundPiece != removedPieces_.end(),
          "There must be a captured piece in removedPieces_ at 'moveNumber'");
      opponentPieceSet.addPiece(foundPiece->second);
      removedPieces_.erase(foundPiece);

      if (auto enPassantCaptureSqr =
              moveDetail.getEnPassantCaptureSquare())
      {
        SPDLOG_DEBUG("Restore en passant captured piece");
      }
      else
      {
      }
    }

    if (auto promotedPieceInfo = moveDetail.getPromotedPieceType())
    {
      auto foundPiece = removedPieces_.find(~moveNumber);
      BOOST_ASSERT_MSG(
          foundPiece != removedPieces_.end(),
          "There must be a removed piece in removedPieces_ due to promotion "
          "at bitwise negation of the 'moveNumber'");
      removedPieces_.erase(foundPiece);
      allyPieceSet.addPiece(foundPiece->second);
    }
    else
    {
      piece->setMoveInfo(fromSquare, piece->getMoveCount() - 1); ///< revert move info to the previous state
      allyPieceSet.addPiece(piece);

      if (auto rookMove = moveDetail.getCastlingRookMove())
      {
        auto rook = findPiece(rookMove->second);
        allyPieceSet.removePiece(rookMove->second);
        rook->setMoveInfo(rookMove->first, 0);
        allyPieceSet.addPiece(rook);
      }
    }

    moveHistory_.pop_back();
    --moveCount_;

    return std::move(moveDetail);
  }

  auto ChessRule::getPieceSet(Side const &color) noexcept
      -> PieceSet &
  {
    if (color == chess::WHITE)
    {
      return whitePieces_;
    }
    return blackPieces_;
  }

  auto ChessRule::getPieceSet(Side const &color) const noexcept
      -> PieceSet const &
  {
    if (color == chess::WHITE)
    {
      return whitePieces_;
    }
    return blackPieces_;
  }

  auto ChessRule::getKing(Side const &color) const noexcept -> Piece &
  {
    if (color == chess::WHITE)
    {
      return *whiteKing_;
    }
    return *blackKing_;
  }

  auto ChessRule::tryParseMove(ChessMove const &move) const
      -> ChessMove::Detail
  {
    chess::validateStandardChessMove(move);
    SPDLOG_DEBUG(
        "Trying to parse move: {} from {} to {}",
        move.color.toString(),
        move.fromSquare.toString(),
        move.toSquare.toString());

    Side opponentColor = chess::getOpponentColor(move.color);
    auto movedPiece = findPiece(move.fromSquare);

    if (!movedPiece)
    {
      throw std::logic_error(std::format(
          "There is no ally piece at the square {}",
          move.fromSquare.toString()));
    }

    EntityType const &movedPieceType = movedPiece->getType();
    if (move.color == opponentColor)
    {
      throw std::logic_error(std::format(
          "Cannot move opponent's {} at square {}",
          movedPieceType.toString(),
          move.fromSquare.toString()));
    }

    auto pieceAtDestination = findPiece(move.toSquare);
    if (pieceAtDestination && (move.color == pieceAtDestination->getSide()))
    {
      throw std::logic_error(std::format(
          "Cannot move to square '{}' because the piece at that square is an ally",
          move.toSquare.toString()));
    }

    ReachablePosInfo reachable = movedPiece->collectReachablePositions();
    ChessMove::Detail moveDetail;
    if (move.promotedPiece.has_value())
    {
      moveDetail = tryParsePromotionMove(
          move, movedPieceType, pieceAtDestination, reachable);
    }
    else
    {
      moveDetail = tryParseNormalMove(
          move, movedPieceType, pieceAtDestination, reachable);

      if (moveDetail.isEmpty())
      {
        if (movedPieceType == chess::PAWN)
        {
          moveDetail = tryParseEnPassantMove(move, movedPieceType, reachable);
        }
        else if (movedPieceType == chess::KING)
        {
          moveDetail = tryParseCastlingMove(
              move, movedPieceType, pieceAtDestination, reachable);
        }
        else
        {
          throwDefaultMoveError(move, movedPieceType);
        }
      }
    }

    return moveDetail;
  }

  auto ChessRule::tryParseNormalMove(
      ChessMove const &move,
      EntityType const &movedPieceType,
      std::shared_ptr<Piece> const &capturedPiece,
      ReachablePosInfo const &reachable) const -> ChessMove::Detail
  {
    if (movedPieceType == chess::PAWN &&
        move.toSquare.getRank() == chess::getPromotionRank(move.color))
    {
      throw std::logic_error(std::format(
          "{} pawn at '{}' must promote when it moves to the square {}",
          move.color.toString(),
          move.fromSquare.toString(),
          move.toSquare.toString()));
    }

    for (auto &square : reachable.quietPositions)
    {
      if (square == move.toSquare)
      {
        return ChessMove::Normal{
            -1, ///< deferred
            move.color,
            move.fromSquare,
            move.toSquare,
            movedPieceType,
            std::nullopt,
            Position{},             ///< deferred
            Side::Status::UNDEFINED ///< deferred
        };
      }
    }

    for (auto &square : reachable.capturePositions)
    {
      if (square == move.toSquare)
      {
        return ChessMove::Normal{
            -1, ///< deferred
            move.color,
            move.fromSquare,
            move.toSquare,
            movedPieceType,
            capturedPiece->getType(),
            Position{},             ///< deferred
            Side::Status::UNDEFINED ///< deferred
        };
      }
    }

    return ChessMove::Detail{};
  }

  auto ChessRule::tryParsePromotionMove(
      ChessMove const &move,
      EntityType const &movedPieceType,
      std::shared_ptr<Piece> const &capturedPiece,
      ReachablePosInfo const &reachable) const -> ChessMove::Detail
  {
    if (movedPieceType != chess::PAWN)
    {
      throw std::logic_error(std::format(
          "The moved piece is {} and cannot promote",
          movedPieceType.toString()));
    }
    if (move.toSquare.getRank() != chess::getPromotionRank(move.color))
    {
      throw std::logic_error(std::format(
          "{} pawn cannot promote when it has only just reached rank '{}'",
          move.color.toString(),
          char(move.toSquare.getRank())));
    }

    for (auto &square : reachable.quietPositions)
    {
      if (square == move.toSquare)
      {
        return ChessMove::Promotion{
            -1, ///< deferred
            move.color,
            move.fromSquare,
            move.toSquare,
            movedPieceType,
            *move.promotedPiece,
            std::nullopt,
            Position{},             ///< deferred
            Side::Status::UNDEFINED ///< deferred
        };
      }
    }

    for (auto &square : reachable.capturePositions)
    {
      if (square == move.toSquare)
      {
        return ChessMove::Promotion{
            -1, ///< deferred
            move.color,
            move.fromSquare,
            move.toSquare,
            movedPieceType,
            *move.promotedPiece,
            capturedPiece->getType(),
            Position{},             ///< deferred
            Side::Status::UNDEFINED ///< deferred
        };
      }
    }

    throwDefaultMoveError(move, movedPieceType);
    return ChessMove::Detail{};
  }

  auto ChessRule::tryParseEnPassantMove(
      ChessMove const &move,
      EntityType const &movedPieceType,
      ReachablePosInfo const &reachable) const -> ChessMove::Detail
  {
    if (reachable.specialPositions.empty())
    {
      int dRank = move.toSquare.getRank() - move.fromSquare.getRank();
      int dFile = move.toSquare.getFile() - move.fromSquare.getFile();
      if (dRank == chess::getPawnStep(move.color) &&
          std::abs(dFile) == 1)
      {
        throw std::logic_error(std::format(
            "{} seems to capture en passant by the pawn at square '{}', "
            "but it's not a legal move",
            move.color.toString(),
            move.toSquare.toString()));
      }

      throwDefaultMoveError(move, movedPieceType);
    }

    Position const &enPassantDestination = reachable.specialPositions.back();
    if (enPassantDestination == move.toSquare)
    {
      Position enPassantCaptureSquare{
          enPassantDestination.getFile(), move.fromSquare.getRank()};
      return ChessMove::EnPassant{
          -1, ///< deferred
          move.color,
          move.fromSquare,
          move.toSquare,
          movedPieceType,
          enPassantCaptureSquare,
          chess::PAWN,
          Position{},             ///< deferred
          Side::Status::UNDEFINED ///< deferred
      };
    }

    throwDefaultMoveError(move, movedPieceType);
    return ChessMove::Detail{};
  }

  auto ChessRule::tryParseCastlingMove(
      ChessMove const &move,
      EntityType const &movedPieceType,
      std::shared_ptr<Piece> const &pieceAtDestination,
      ReachablePosInfo const &reachable) const -> ChessMove::Detail
  {
    if (reachable.specialPositions.empty())
    {
      int dFile = move.toSquare.getFile() - move.fromSquare.getFile();
      int step = dFile > 0 ? 1 : -1;

      if (std::abs(dFile) == 2)
      {
        if (pieceAtDestination != nullptr)
        {
          throw std::logic_error(std::format(
              "{} cannot castle because there is a opponent's {} "
              "obtaining their king's destination",
              move.color.toString(),
              pieceAtDestination->getType().toString()));
        }

        for (int file = move.fromSquare.getFile() + step;
             file != move.toSquare.getFile();
             file += step)
        {
          Position midwaySquare{file, move.fromSquare.getRank()};
          if (auto blockingPiece = findPiece(midwaySquare))
          {
            throw std::logic_error(std::format(
                "{} cannot castle because there is a {} {} blocking "
                "their king's path at square '{}'",
                move.color.toString(),
                blockingPiece->getSide().toString(),
                blockingPiece->getType().toString(),
                midwaySquare.toString()));
          }
        }
      }

      throwDefaultMoveError(move, movedPieceType);
    }

    for (auto &square : reachable.specialPositions)
    {
      if (square != move.toSquare)
      {
        continue;
      }

      ChessRule cloneRule(*this);
      Piece const &allyKing = cloneRule.getKing(move.color);

      //=============================================================
      //
      auto attackerList = cloneRule.findAttackers(allyKing, true);
      if (!attackerList.empty())
      {
        auto &attacker = attackerList.back();
        throw std::logic_error(std::format(
            "{} cannot castle because their king is in check "
            "by the opponent's {} from '{}'",
            move.color.toString(),
            attacker->getType().toString(),
            attacker->getPosition().toString()));
      }

      //=============================================================
      //
      Position midwaySquare{
          (move.fromSquare.getFile() + move.toSquare.getFile()) / 2,
          move.toSquare.getRank()};

      cloneRule.commitMove(ChessMove::generateMinimalNormalMove(
          ChessMove{move.color, move.fromSquare, midwaySquare, std::nullopt},
          false));
      attackerList = cloneRule.findAttackers(allyKing, true);
      if (!attackerList.empty())
      {
        auto &attacker = attackerList.back();
        throw std::logic_error(std::format(
            "{} cannot castle because the midway square '{}' is "
            "under attack by the opponent's {} from '{}'",
            move.color.toString(),
            midwaySquare.toString(),
            attacker->getType().toString(),
            attacker->getPosition().toString()));
      }

      cloneRule.revertLastMoveCommit();

      //=============================================================
      // TODO: consider remove this step because this check is implemented
      // at ChessRule::tryMove(ChessMove const &move) const
      cloneRule.commitMove(ChessMove::generateMinimalNormalMove(move, false));
      attackerList = cloneRule.findAttackers(allyKing, true);
      if (!attackerList.empty())
      {
        auto &attacker = attackerList.back();
        throw std::logic_error(std::format(
            "{} cannot castle because the king's destination '{}' "
            "is under attack by the opponent's {} from '{}'",
            move.color.toString(),
            move.toSquare.toString(),
            attacker->getType().toString(),
            attacker->getPosition().toString()));
      }

      //=============================================================
      //
      auto [rookSrc, rookDst] = chess::getCastlingRookMove(move.toSquare);
      return ChessMove::Castling{
          -1, ///< deferred
          move.color,
          move.fromSquare,
          move.toSquare,
          movedPieceType,
          rookSrc,
          rookDst,
          Position{},             ///< deferred
          Side::Status::UNDEFINED ///< deferred
      };
    }

    throwDefaultMoveError(move, movedPieceType);
    return ChessMove::Detail{};
  }

  auto ChessRule::findAttackers(
      Piece const &piece,
      bool onlyFirst) const noexcept -> std::list<std::shared_ptr<Piece>>
  {
    std::list<std::shared_ptr<Piece>> result;
    PieceSet const &opponentPieces = getPieceSet(
        chess::getOpponentColor(piece.getSide()));
    for (auto const &opponentPiece : opponentPieces)
    {
      if (opponentPiece->canCapture(piece))
      {
        result.emplace_back(opponentPiece);
        if (onlyFirst)
        {
          return result;
        }
      }
    }

    return result;
  }

  auto ChessRule::isStalemated(Side const &color) const noexcept -> bool
  {
    ChessRule cloneRule(*this);
    auto allyPieceList = collectPieces(color);
    auto const &allyKing = cloneRule.getKing(color);
    for (auto const &piece : allyPieceList)
    {
      ReachablePosInfo reachable = cloneRule.collectReachableSquares(
          piece->getPosition(), color);
      for (Position const &toSquare : reachable.quietPositions)
      {
        if (toSquare.getRank() == chess::getPromotionRank(color) &&
            piece->getType() == chess::PAWN)
        {
          cloneRule.commitMove(ChessMove::generateMinimalPromotionMove(
              ChessMove{color, piece->getPosition(), toSquare, chess::QUEEN},
              false));
        }
        else
        {
          cloneRule.commitMove(ChessMove::generateMinimalNormalMove(
              ChessMove{color, piece->getPosition(), toSquare, std::nullopt},
              false));
        }

        auto attackerList = cloneRule.findAttackers(allyKing, true);
        if (attackerList.empty())
        {
          return false;
        }

        cloneRule.revertLastMoveCommit();
      }

      for (Position const &toSquare : reachable.capturePositions)
      {
        if (toSquare.getRank() == chess::getPromotionRank(color) &&
            piece->getType() == chess::PAWN)
        {
          cloneRule.commitMove(ChessMove::generateMinimalPromotionMove(
              ChessMove{color, piece->getPosition(), toSquare, chess::QUEEN},
              true));
        }
        else
        {
          cloneRule.commitMove(ChessMove::generateMinimalNormalMove(
              ChessMove{color, piece->getPosition(), toSquare, std::nullopt},
              true));
        }

        auto attackerList = cloneRule.findAttackers(allyKing, true);
        if (attackerList.empty())
        {
          return false;
        }
        cloneRule.revertLastMoveCommit();
      }

      if (reachable.specialPositions.empty())
      {
        continue;
      }

      if (piece->getType() == chess::PAWN)
      {
        Position const &fromSquare = piece->getPosition();
        Position const &toSquare = reachable.specialPositions.back();
        Position enPassantCaptureSquare{
            toSquare.getFile(), fromSquare.getRank()};
        cloneRule.commitMove(ChessMove::generateMinimalEnPassantMove(
            ChessMove{color, fromSquare, toSquare, std::nullopt},
            enPassantCaptureSquare));

        auto attackerList = cloneRule.findAttackers(allyKing, true);
        if (attackerList.empty())
        {
          return false;
        }
        cloneRule.revertLastMoveCommit();
      }

      BOOST_ASSERT_MSG(
          piece->getType() == chess::KING,
          "Special position list is just for PAWNs and KING");

      for (Position const &toSquare : reachable.specialPositions)
      {
        auto [rookSource, rookDestination] = chess::getCastlingRookMove(toSquare);
        cloneRule.commitMove(ChessMove::generateMinimalCastlingMove(
            ChessMove{color, piece->getPosition(), toSquare, std::nullopt},
            rookSource, rookDestination));

        auto attackerList = cloneRule.findAttackers(allyKing, true);
        if (attackerList.empty())
        {
          return false;
        }
        cloneRule.revertLastMoveCommit();
      }
    }
    return true;
  }

  void ChessRule::throwDefaultMoveError(
      ChessMove const &move, EntityType const &movedPieceType)
  {
    throw std::logic_error(std::format(
        "{} {} from '{}' cannot reach to square '{}'",
        move.color.toString(),
        movedPieceType.toString(),
        move.fromSquare.toString(),
        move.toSquare.toString()));
  }
} // namespace bgg
