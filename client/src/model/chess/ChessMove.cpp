// ChessMove.cpp
#include "ChessMove.h"
#include "ChessHelpers.h"

namespace bgg
{
  auto ChessMove::getMoveNumber(Detail const &moveDetail) noexcept -> int const &
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
      std::terminate();
      // return chess::WHITE;
    };

    return moveDetail.visit(moveNumberVisitor);
  }

  auto ChessMove::getColor(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
      // return chess::WHITE;
    };

    return moveDetail.visit(colorVisitor);
  }

  auto ChessMove::getMovedPieceType(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
    };

    return moveDetail.visit(movedPieceVisitor);
  }

  auto ChessMove::getSourceSquare(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
    };

    return moveDetail.visit(sourceSquareVisitor);
  }

  auto ChessMove::getDestinationSquare(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
    };

    return moveDetail.visit(destinationSquareVisitor);
  }

  auto ChessMove::getOpponentKingSquare(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
    };

    return moveDetail.visit(opponentKingSquareVisitor);
  }

  auto ChessMove::getOpponentKingStatus(ChessMove::Detail const &moveDetail) noexcept
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
      std::terminate();
    };

    return moveDetail.visit(opponentKingStateVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessMove::setMoveNumber(Detail &moveDetail, int moveNumber) noexcept
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
      std::terminate();
      // return chess::WHITE;
    };

    moveDetail.visit(moveNumberVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////
  void ChessMove::setColor(
      Detail &moveDetail, Side const &newColor) noexcept
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
      std::terminate();
      // return chess::WHITE;
    };

    moveDetail.visit(colorVisitor);
  }

  void ChessMove::setMovedPieceType(
      Detail &moveDetail, EntityType const &newType) noexcept
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
      std::terminate();
    };

    moveDetail.visit(movedPieceVisitor);
  }

  void ChessMove::setSourceSquare(
      Detail &moveDetail, Position const &newSquare) noexcept
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
      std::terminate();
    };

    moveDetail.visit(sourceSquareVisitor);
  }

  void ChessMove::setDestinationSquare(
      Detail &moveDetail, Position const &newSquare) noexcept
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
      std::terminate();
    };

    moveDetail.visit(destinationSquareVisitor);
  }

  void ChessMove::setOpponentKingSquare(
      Detail &moveDetail, Position const &newSquare) noexcept
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
      std::terminate();
    };

    moveDetail.visit(opponentKingSquareVisitor);
  }

  void ChessMove::setOpponentKingStatus(
      Detail &moveDetail, Side::Status const &newStatus) noexcept
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
      std::terminate();
    };

    moveDetail.visit(opponentKingStateVisitor);
  }

  /////////////////////////////////////////////////////////////////////////////

  auto ChessMove::getCapturedPieceType(ChessMove::Detail const &moveDetail) noexcept
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

    return moveDetail.visit(capturedPieceVisitor);
  }

  auto ChessMove::getPromotedPieceType(ChessMove::Detail const &moveDetail) noexcept
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

    return moveDetail.visit(promotedPieceVisitor);
  }

  auto ChessMove::getCastlingRookMove(Detail const &moveDetail) noexcept
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

    return moveDetail.visit(rookSourceVisitor);
  }

  auto ChessMove::getEnPassantCaptureSquare(ChessMove::Detail const &moveDetail) noexcept
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

    return moveDetail.visit(enPassantCaptureSquareVisitor);
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

  // auto ChessMove::getErrorMessage(ChessMove::ChessMove::Detail const &moveDetail) noexcept
  //     -> std::string
  // {
  //   auto errorMessageVisitor = []<typename T>(T const &concreteDetail)
  //       -> std::string const &
  //   {
  //     if constexpr (requires {
  //         { concreteDetail.errorMessage } -> std::same_as<std::string const &>; })
  //     {
  //       return concreteDetail.errorMessage;
  //     }

  //     BOOST_ASSERT_MSG(
  //         false, "The move detail must have the 'errorMessage' member");
  //     std::terminate();
  //   };

  //   return moveDetail.visit(errorMessageVisitor);
  // }
} // namespace bgg
