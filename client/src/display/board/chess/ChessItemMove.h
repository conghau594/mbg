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
      return nativeMoveDetail_.getMoveNumber();
    }

    auto getSourceTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(nativeMoveDetail_.getSourceSquare());
    }

    auto getDestinationTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(nativeMoveDetail_.getDestinationSquare());
    }

    auto getOpponentKingTile() const noexcept -> TileCoords
    {
      return posToTileConverter_(nativeMoveDetail_.getOpponentKingSquare());
    }
    auto getOpponentKingStatus() const noexcept -> Side::Status
    {
      return nativeMoveDetail_.getOpponentKingStatus();
    }

    auto getMovedItemInfo() const noexcept -> ItemInfo
    {
      return ItemInfo{
          nativeMoveDetail_.getMovedPieceType(),
          nativeMoveDetail_.getColor()};
    }

    auto getCapturedItemInfo() const noexcept -> std::optional<ItemInfo>
    {
      if (auto capturedPieceType = nativeMoveDetail_.getCapturedPieceType())
      {
        Side allyColor = nativeMoveDetail_.getColor();
        return ItemInfo{*capturedPieceType, chess::getOpponentColor(allyColor)};
      }
      else
      {
        return std::nullopt;
      }
    }

    auto getPromotedItemInfo() const noexcept -> std::optional<ItemInfo>
    {
      if (auto promotedPieceType = nativeMoveDetail_.getPromotedPieceType())
      {
        return ItemInfo{*promotedPieceType, nativeMoveDetail_.getColor()};
      }
      else
      {
        return std::nullopt;
      }
    }

    auto getCastlingRookMove() const noexcept
        -> std::optional<std::pair<TileCoords, TileCoords>>
    {
      auto rookMove = nativeMoveDetail_.getCastlingRookMove();

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
      if (auto enPassantSquare = nativeMoveDetail_.getEnPassantCaptureSquare())
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
