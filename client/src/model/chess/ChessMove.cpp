// ChessMove.cpp
#include "ChessMove.h"
#include "ChessHelpers.h"

namespace bgg
{
  auto ChessMove::Detail::getMoveNumber() const noexcept -> int const &
  {
    auto moveNumberVisitor = []<typename T>(T const &concreteDetail)
        -> int const &
    {
      if constexpr (requires { 
          { concreteDetail.moveNumber } -> std::same_as<int const&>; })
      {
        return concreteDetail.moveNumber;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'moveNumber' member");
      std::abort();
    };

    return visit(moveNumberVisitor);
  }

  auto ChessMove::Detail::getColor() const noexcept
      -> Side const &
  {
    auto colorVisitor = []<typename T>(T const &concreteDetail)
        -> Side const &
    {
      if constexpr (requires { 
          { concreteDetail.color } -> std::same_as<Side const&>; })
      {
        return concreteDetail.color;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'side' member");
      std::abort();
    };

    return visit(colorVisitor);
  }

  auto ChessMove::Detail::getMovedPieceType() const noexcept
      -> EntityType const &
  {
    auto movedPieceVisitor = []<typename T>(T const &concreteDetail)
        -> EntityType const &
    {
      if constexpr (requires { 
          { concreteDetail.movedPiece } -> std::same_as<EntityType const&>; })
      {
        return concreteDetail.movedPiece;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'movedPiece' member");
      std::abort();
    };

    return visit(movedPieceVisitor);
  }

  auto ChessMove::Detail::getSourceSquare() const noexcept
      -> Position const &
  {
    auto sourceSquareVisitor = []<typename T>(T const &concreteDetail)
        -> Position const &
    {
      if constexpr (requires { 
          { concreteDetail.fromSquare } -> std::same_as<Position const&>; })
      {
        return concreteDetail.fromSquare;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'fromSquare' member");
      std::abort();
    };

    return visit(sourceSquareVisitor);
  }

  auto ChessMove::Detail::getDestinationSquare() const noexcept
      -> Position const &
  {
    auto destinationSquareVisitor = []<typename T>(T const &concreteDetail)
        -> Position const &
    {
      if constexpr (requires { 
          { concreteDetail.toSquare } -> std::same_as<Position const&>; })
      {
        return concreteDetail.toSquare;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'toSquare' member");
      std::abort();
    };

    return visit(destinationSquareVisitor);
  }

  auto ChessMove::Detail::getOpponentKingSquare() const noexcept
      -> Position const &
  {
    auto opponentKingSquareVisitor = []<typename T>(T const &concreteDetail)
        -> Position const &
    {
      if constexpr (requires { 
          { concreteDetail.opponentKingSquare } -> std::same_as<Position const&>; })
      {
        return concreteDetail.opponentKingSquare;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'opponentKingSquare' member");
      std::abort();
    };

    return visit(opponentKingSquareVisitor);
  }

  auto ChessMove::Detail::getOpponentKingStatus() const noexcept
      -> Side::Status const &
  {
    auto opponentKingStateVisitor = []<typename T>(T const &concreteDetail)
        -> Side::Status const &
    {
      if constexpr (requires { 
          { concreteDetail.opponentKingStatus } -> std::same_as<Side::Status const&>; })
      {
        return concreteDetail.opponentKingStatus;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have the 'opponentKingStatus' member");
      std::abort();
    };

    return visit(opponentKingStateVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessMove::Detail::setMoveNumber(int moveNumber) noexcept
  {
    auto moveNumberVisitor = [&moveNumber]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.moveNumber } -> std::same_as<int &>; })
      {
        concreteDetail.moveNumber = moveNumber;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'moveNumber' member");
      std::abort();
    };

    visit(moveNumberVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessMove::Detail::setColor(Side const &newColor) noexcept
  {
    auto colorVisitor = [&newColor]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.color } -> std::same_as<Side &>; })
      {
        concreteDetail.color = newColor;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'side' member");
      std::abort();
    };

    visit(colorVisitor);
  }

  void ChessMove::Detail::setMovedPieceType(EntityType const &newType) noexcept
  {
    auto movedPieceVisitor = [&newType]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.movedPiece } -> std::same_as<EntityType &>; })
      {
        concreteDetail.movedPiece = newType;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'movedPiece' member");
      std::abort();
    };

    visit(movedPieceVisitor);
  }

  void ChessMove::Detail::setSourceSquare(Position const &newSquare) noexcept
  {
    auto sourceSquareVisitor = [&newSquare]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.fromSquare } -> std::same_as<Position &>; })
      {
        concreteDetail.fromSquare = newSquare;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'fromSquare' member");
      std::abort();
    };

    visit(sourceSquareVisitor);
  }

  void ChessMove::Detail::setDestinationSquare(Position const &newSquare) noexcept
  {
    auto destinationSquareVisitor = [&newSquare]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.toSquare } -> std::same_as<Position &>; })
      {
        concreteDetail.toSquare = newSquare;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'toSquare' member");
      std::abort();
    };

    visit(destinationSquareVisitor);
  }

  void ChessMove::Detail::setOpponentKingSquare(Position const &newSquare) noexcept
  {
    auto opponentKingSquareVisitor = [&newSquare]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.opponentKingSquare } -> std::same_as<Position &>; })
      {
        concreteDetail.opponentKingSquare = newSquare;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have an the 'opponentKingSquare' member");
      std::abort();
    };

    visit(opponentKingSquareVisitor);
  }

  void ChessMove::Detail::setOpponentKingStatus(Side::Status const &newStatus) noexcept
  {
    auto opponentKingStateVisitor = [&newStatus]<typename T>(T &concreteDetail)
    {
      if constexpr (requires { 
          { concreteDetail.opponentKingStatus } -> std::same_as<Side::Status &>; })
      {
        concreteDetail.opponentKingStatus = newStatus;
        return;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have the 'opponentKingStatus' member");
      std::abort();
    };

    visit(opponentKingStateVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////

  auto ChessMove::Detail::getCapturedPieceType() const noexcept
      -> std::optional<EntityType>
  {
    auto capturedPieceVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<EntityType>
    {
      if constexpr (requires { 
          { concreteDetail.capturedPiece } -> std::convertible_to<std::optional<EntityType>>; })
      {
        return concreteDetail.capturedPiece;
      }

      return std::nullopt;
    };

    return visit(capturedPieceVisitor);
  }

  auto ChessMove::Detail::getPromotedPieceType() const noexcept
      -> std::optional<EntityType>
  {
    auto promotedPieceVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<EntityType>
    {
      if constexpr (requires { 
          { concreteDetail.promotedPiece } -> std::same_as<EntityType const &>; })
      {
        return concreteDetail.promotedPiece;
      }

      return std::nullopt;
    };

    return visit(promotedPieceVisitor);
  }

  auto ChessMove::Detail::getCastlingRookMove() const noexcept
      -> std::optional<std::pair<Position, Position>>
  {
    auto rookSourceVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<std::pair<Position, Position>>
    {
      if constexpr (requires { 
          { concreteDetail.rookSource } -> std::same_as<Position const &>;
          { concreteDetail.rookDestination } -> std::same_as<Position const &>; })
      {
        return std::make_pair(
            concreteDetail.rookSource, concreteDetail.rookDestination);
      }

      return std::nullopt;
    };

    return visit(rookSourceVisitor);
  }

  auto ChessMove::Detail::getEnPassantCaptureSquare() const noexcept
      -> std::optional<Position>
  {
    auto enPassantCaptureSquareVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<Position>
    {
      if constexpr (requires { 
          { concreteDetail.enPassantCaptureSquare } -> std::same_as<Position const &>; })
      {
        return concreteDetail.enPassantCaptureSquare;
      }

      return std::nullopt;
    };

    return visit(enPassantCaptureSquareVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////
  auto ChessMove::generateMinimalNormalMove(
      ChessMove const &move,
      bool shouldCapture) noexcept -> ChessMove::Normal
  {
    return ChessMove::Normal{
        -1,
        move.color,
        move.fromSquare,
        move.toSquare,
        EntityType{},
        shouldCapture ? std::optional<EntityType>(EntityType{}) : std::nullopt,
        Position{},
        Side::Status::UNDEFINED};
  }

  auto ChessMove::generateMinimalPromotionMove(
      ChessMove const &move,
      bool shouldCapture) noexcept -> ChessMove::Promotion
  {
    return ChessMove::Promotion{
        -1,
        move.color,
        move.fromSquare,
        move.toSquare,
        EntityType{},
        *move.promotedPiece,
        shouldCapture ? std::optional<EntityType>(EntityType{}) : std::nullopt,
        Position{},
        Side::Status::UNDEFINED};
  }
  auto ChessMove::generateMinimalEnPassantMove(
      ChessMove const &move,
      Position const &enPassantCaptureSquare) noexcept -> ChessMove::EnPassant
  {
    return ChessMove::EnPassant{
        -1,
        move.color,
        move.fromSquare,
        move.toSquare,
        EntityType{},
        enPassantCaptureSquare,
        chess::PAWN,
        Position{},
        Side::Status::UNDEFINED};
  }

  auto ChessMove::generateMinimalCastlingMove(
      ChessMove const &move,
      Position const &rookSource,
      Position const &rookDestination) noexcept -> ChessMove::Castling
  {
    return ChessMove::Castling{
        -1,
        move.color,
        move.fromSquare,
        move.toSquare,
        EntityType{},
        rookSource,
        rookDestination,
        Position{},
        Side::Status::UNDEFINED};
  }
} // namespace bgg
