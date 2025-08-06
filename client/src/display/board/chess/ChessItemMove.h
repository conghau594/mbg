// ChessItemMove.h
#pragma once

#include <memory>
#include <optional>
#include <string>

#include <SFML/System/Vector2.hpp>
#include "base/Variant.h"
#include "model/EntityType.h"
#include "model/Side.h"
#include "model/chess/ChessMove.h"
#include "model/chess/ChessHelpers.h"

namespace bgg
{
  using TileCoords = sf::Vector2i;

  class ItemInfo
  {
  public:
    EntityType type;
    Side color;

    [[nodiscard]] auto toString() const noexcept -> std::string
    {
      return color.toString() + type.toString();
    }
  };

  class ChessItemMove
  {
  public:
    Side color;
    TileCoords fromTile;
    TileCoords toTile;
    std::optional<EntityType> promotedPiece;
  };

  class ChessMoveDetailAdapter
  {
    ChessMove::Detail nativeMoveDetail_;
    std::function<TileCoords(Position const &)> posToTileConverter_;

  public:
    ChessMoveDetailAdapter(
        ChessMove::Detail nativeMoveDetail,
        std::function<TileCoords(Position const &)> posToTileConverter) noexcept
        : nativeMoveDetail_(std::move(nativeMoveDetail)),
          posToTileConverter_(std::move(posToTileConverter))
    {
      BOOST_ASSERT_MSG(
          posToTileConverter_, "The posToTileConverter_ cannot be null");
      BOOST_ASSERT_MSG(
          !nativeMoveDetail_.isEmpty(), "The nativeMoveDetail_ cannot be empty");
    }

    auto getNativeMoveDetail() const noexcept -> ChessMove::Detail const &
    {
      return nativeMoveDetail_;
    }

    auto getMoveNumber() const noexcept -> int const &
    {
      return ChessMove::getMoveNumber(nativeMoveDetail_);
    }

    auto getSourceTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(ChessMove::getSourceSquare(nativeMoveDetail_));
    }

    auto getDestinationTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(ChessMove::getDestinationSquare(nativeMoveDetail_));
    }

    auto getOpponentKingTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(ChessMove::getOpponentKingSquare(nativeMoveDetail_));
    }
    auto getOpponentKingStatus() const noexcept -> Side::Status
    {
      return ChessMove::getOpponentKingStatus(nativeMoveDetail_);
    }

    auto getMovedItemInfo() const noexcept -> ItemInfo
    {
      return ItemInfo{
          ChessMove::getMovedPieceType(nativeMoveDetail_),
          ChessMove::getColor(nativeMoveDetail_)};
    }

    auto getCapturedItemInfo() const noexcept -> std::optional<ItemInfo>
    {
      if (auto capturedPieceType = ChessMove::getCapturedPieceType(nativeMoveDetail_))
      {
        Side allyColor = ChessMove::getColor(nativeMoveDetail_);
        return ItemInfo{*capturedPieceType, chess::getOpponentColor(allyColor)};
      }
      else
      {
        return std::nullopt;
      }
    }

    auto getPromotedItemInfo() const noexcept -> std::optional<ItemInfo>
    {
      if (auto promotedPieceType = ChessMove::getPromotedPieceType(nativeMoveDetail_))
      {
        return ItemInfo{*promotedPieceType, ChessMove::getColor(nativeMoveDetail_)};
      }
      else
      {
        return std::nullopt;
      }
    }

    auto getCastlingRookMove() const noexcept
        -> std::optional<std::pair<TileCoords, TileCoords>>
    {
      auto rookMove = ChessMove::getCastlingRookMove(nativeMoveDetail_);

      if (rookMove)
      {
        return std::make_pair(posToTileConverter_(rookMove->first),
                              posToTileConverter_(rookMove->second));
      }
      else
      {
        return std::nullopt;
      }
    }

    auto getEnPassantCaptureTile() const noexcept -> std::optional<TileCoords>
    {
      if (auto enPassantSquare = ChessMove::getEnPassantCaptureSquare(nativeMoveDetail_))
      {
        return posToTileConverter_(*enPassantSquare);
      }
      else
      {
        return std::nullopt;
      }
    }
  };
} // namespace bgg
