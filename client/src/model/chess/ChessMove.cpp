// ChessMove.cpp
#include "ChessMove.h"
#include "ChessHelpers.h"

namespace bgg
{
  auto ChessMove::getColor(ChessMove::Detail const &moveDetail) noexcept -> Side const &
  {
    auto colorVisitor = []<typename T>(T const &concreteDetail)
        -> Side const &
    {
      if constexpr (requires { 
          { concreteDetail.color } -> std::same_as<Side const &>; })
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
          { concreteDetail.movedPiece } -> std::same_as<EntityType const &>; })
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
          { concreteDetail.fromSquare } -> std::same_as<Position const &>; })
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
          { concreteDetail.toSquare } -> std::same_as<Position const &>; })
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
          { concreteDetail.opponentKingSquare } -> std::same_as<Position const &>; })
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
          { concreteDetail.opponentKingStatus } -> std::same_as<Side::Status const &>; })
      {
        return concreteDetail.opponentKingStatus;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have the 'opponentKingStatus' member");
      std::terminate();
    };

    return moveDetail.visit(opponentKingStateVisitor);
  }

  auto ChessMove::getCapturedPieceType(ChessMove::Detail const &moveDetail) noexcept
      -> std::optional<EntityType>
  {
    auto capturedPieceVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<EntityType>
    {
      if constexpr (requires { 
          { concreteDetail.capturedPiece } -> std::same_as<std::optional<EntityType> const &>; })
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

  auto ChessMove::getRookSourceSquare(ChessMove::Detail const &moveDetail) noexcept
      -> std::optional<Position>
  {
    auto rookSourceVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<Position>
    {
      if constexpr (requires { 
          { concreteDetail.rookSource } -> std::same_as<Position const &>; })
      {
        return concreteDetail.rookSource;
      }

      return std::nullopt;
    };

    return moveDetail.visit(rookSourceVisitor);
  }

  auto ChessMove::getRookDestinationSquare(ChessMove::Detail const &moveDetail) noexcept
      -> std::optional<Position>
  {
    auto rookDestinationVisitor = []<typename T>(T const &concreteDetail)
        -> std::optional<Position>
    {
      if constexpr (requires { 
          { concreteDetail.rookDestination } -> std::same_as<Position const &>; })
      {
        return concreteDetail.rookDestination;
      }

      return std::nullopt;
    };

    return moveDetail.visit(rookDestinationVisitor);
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

  auto ChessMove::getErrorMessage(ChessMove::ChessMove::Detail const &moveDetail) noexcept
      -> std::string
  {
    auto errorMessageVisitor = []<typename T>(T const &concreteDetail)
        -> std::string const &
    {
      if constexpr (requires { 
          { concreteDetail.errorMessage } -> std::same_as<std::string const &>; })
      {
        return concreteDetail.errorMessage;
      }

      BOOST_ASSERT_MSG(
          false, "The move detail must have the 'errorMessage' member");
      std::terminate();
    };

    return moveDetail.visit(errorMessageVisitor);
  }
} // namespace bgg
