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
    class Detail;

    TileCoords fromTile;
    TileCoords toTile;
    std::optional<EntityType> promote;
  };

  class ChessItemMove::Detail
  {
    ChessMove::Detail nativeMoveDetail_;
    std::function<TileCoords(Position const &)> posToTileConverter_;

  public:
    Detail(
        ChessMove::Detail nativeMoveDetail,
        std::function<TileCoords(Position const &)> posToTileConverter) noexcept
        : nativeMoveDetail_(std::move(nativeMoveDetail)),
          posToTileConverter_(std::move(posToTileConverter))
    {
    }

    auto getNativeMoveDetail() const noexcept -> ChessMove::Detail const &
    {
      return nativeMoveDetail_;
    }

    auto isEmpty() const noexcept -> bool
    {
      return nativeMoveDetail_.isEmpty();
    }

    void setEmpty() noexcept
    {
      return nativeMoveDetail_.setEmpty();
    }

    auto isValid() const noexcept -> bool
    {
      return !nativeMoveDetail_.is<ChessMove::Invalid>();
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

    auto getCastlingRookMove() const noexcept -> std::optional<std::pair<TileCoords, TileCoords>>
    {
      auto rookSource = ChessMove::getRookSourceSquare(nativeMoveDetail_);
      auto rookDest = ChessMove::getRookDestinationSquare(nativeMoveDetail_);
      if (rookSource && rookDest)
      {
        return std::make_pair(
            posToTileConverter_(*rookSource), posToTileConverter_(*rookDest));
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

    auto getErrorMessage() const noexcept -> std::string
    {
      return ChessMove::getErrorMessage(nativeMoveDetail_);
    }
  };
} // namespace bgg
