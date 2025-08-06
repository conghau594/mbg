// ChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "base/Logger.h"

#include "ChessRuleAdapter.h"
#include "display/board/ZOrder.h"
#include "display/board/chess/ChessTextureCell.h"
#include "IChessRuleAdapter.h"

namespace bgg
{
  ChessRuleAdapter::ChessRuleAdapter(
      std::shared_ptr<ItemStore> itemStore,
      std::shared_ptr<IChessRule> chessRule,
      Side const &allyColor) noexcept
      : chessRule_(std::move(chessRule)),
        allyColor_(allyColor),
        opponentColor_(chess::getOpponentColor(allyColor)),
        posToTileConverter_(getPositionToTileConverter(allyColor_)),
        tileToPositionConverter_(getTileToPositionConverter(allyColor_))
  {
    BOOST_ASSERT_MSG(itemStore, "The 'itemStore' cannot be null");

    // add piece items to itemStore, also assign the returned entry
    std::list<std::shared_ptr<Piece>> initialBoard = chessRule_->collectPieces(chess::WHITE);
    initialBoard.splice(initialBoard.end(), chessRule_->collectPieces(chess::BLACK));
    for (auto &piece : initialBoard)
    {
      std::string itemName = piece->getSide().toString() + piece->getType().toString();
      ItemStore::Entry itemEntry = itemStore->addItem(
          ZOrder::SECOND_LAYER,
          int(utils::getChessTextureCellIndex(piece->getType(), piece->getSide())),
          itemName);

      TileCoords tile = positionToTile(piece->getPosition());
      auto [iter, inserted] = itemPlacements_.try_emplace(tile, itemEntry);

      BOOST_ASSERT_MSG(inserted, "There must be one tile for each itemEntry");
    }
  }

  auto ChessRuleAdapter::positionToTile(
      Position const &position) const noexcept -> TileCoords
  {
    return posToTileConverter_(position);
  }

  auto ChessRuleAdapter::tileToPosition(
      TileCoords const &tile) const noexcept -> Position
  {
    return tileToPositionConverter_(tile);
  }

  auto ChessRuleAdapter::tryMove(ChessItemMove const &itemMove) const
      -> ChessMoveDetailAdapter
  {
    ChessMove chessMove{
        itemMove.color,
        tileToPosition(itemMove.fromTile),
        tileToPosition(itemMove.toTile),
        itemMove.promotedPiece};

    // SPDLOG_DEBUG(
    //     "Try move from ({}, {}) to ({}, {}) (promote = {})",
    //     itemMove.fromTile.x, itemMove.fromTile.y,
    //     itemMove.toTile.x, itemMove.toTile.y,
    //     itemMove.promote ? itemMove.promote.value() : std::string("None"));

    return ChessMoveDetailAdapter{chessRule_->tryMove(chessMove), posToTileConverter_};
  }

  auto ChessRuleAdapter::commitMove(
      ChessMove::Detail const &nativeMoveDetail) noexcept -> int
  {
    ChessMoveDetailAdapter itemMoveDetail(nativeMoveDetail, posToTileConverter_);
    return commitMove(itemMoveDetail);
  }

  auto ChessRuleAdapter::commitMove(
      ChessMoveDetailAdapter const &itemMoveDetail) noexcept -> int
  {
    TileCoords fromTile = itemMoveDetail.getSourceTile();
    TileCoords toTile = itemMoveDetail.getDestinationTile();
    auto movedItemEntry = getItemEntry(fromTile);
    BOOST_ASSERT_MSG(
        !movedItemEntry.isNull(),
        "There must be an item at the 'fromTile'");

    ///< commit basic move action
    itemPlacements_.erase(toTile); ///< erase even if there is no entry at 'toTile'
    itemPlacements_.emplace(toTile, movedItemEntry);
    itemPlacements_.erase(fromTile);

    if (itemMoveDetail.getPromotedItemInfo())
    {
      ///< do nothing
      ///< because the appearance change of 'movedItemEntry' must be done
      ///< outside this class.
    }
    else if (auto captureTile = itemMoveDetail.getEnPassantCaptureTile())
    {
      ///< remove en passant captured item
      itemPlacements_.erase(*captureTile);
    }
    else if (auto castlingRookMove = itemMoveDetail.getCastlingRookMove())
    {
      ///< move the related rook to the destination
      auto rookItemEntry = getItemEntry(castlingRookMove->first);
      BOOST_ASSERT_MSG(
          !rookItemEntry.isNull(),
          "There must be an item at the source tile of the castling rook");

      itemPlacements_.emplace(castlingRookMove->second, rookItemEntry);
      itemPlacements_.erase(castlingRookMove->first);
    }

    return chessRule_->commitMove(itemMoveDetail.getNativeMoveDetail());
  }

  auto ChessRuleAdapter::getAllyColor() const -> Side
  {
    return allyColor_;
  }

  auto ChessRuleAdapter::getOpponentColor() const -> Side
  {
    return opponentColor_;
  }

  auto ChessRuleAdapter::getItemInfo(TileCoords const &tile) const noexcept
      -> std::optional<ItemInfo>
  {
    auto piece = chessRule_->findPiece(tileToPosition(tile));
    if (piece)
    {
      return ItemInfo{piece->getType(), piece->getSide()};
    }
    return std::nullopt;
  }

  auto ChessRuleAdapter::getItemPlacements() -> ItemPlacementMap const &
  {
    return itemPlacements_;
  }

  auto ChessRuleAdapter::collectSelectableTiles() const noexcept -> ItemPlacementMap
  {
    std::list<std::shared_ptr<Piece>>
        selectablePieces = chessRule_->collectSelectablePieces(allyColor_);

    ItemPlacementMap itemPlacements;
    for (auto &piece : selectablePieces)
    {
      TileCoords tile = positionToTile(piece->getPosition());
      ItemStore::Entry entry = itemPlacements_.at(tile);
      auto [iter, inserted] = itemPlacements.try_emplace(
          std::move(tile), std::move(entry));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    return itemPlacements;
  }

  auto ChessRuleAdapter::collectReachableTiles(TileCoords const &tile) const noexcept
      -> std::optional<ReachableTileInfo>
  {
    auto itemEntry = getItemEntry(tile);
    if (itemEntry.isNull())
    {
      return std::nullopt;
    }

    Position originSquare = tileToPosition(tile);

    ReachablePosInfo reachable = chessRule_->collectReachableSquares(
        originSquare, allyColor_);

    std::list<TileCoords> quietSquares;
    for (auto &square : reachable.quietPositions)
    {
      quietSquares.emplace_back(positionToTile(square));
    }

    std::list<TileCoords> captureSquares;
    for (auto &square : reachable.capturePositions)
    {
      captureSquares.emplace_back(positionToTile(square));
    }

    std::list<TileCoords> specialMoveTiles;
    for (auto &square : reachable.specialPositions)
    {
      specialMoveTiles.emplace_back(positionToTile(square));
    }

    return ReachableTileInfo{
        itemEntry,
        std::move(quietSquares),
        std::move(captureSquares),
        std::move(specialMoveTiles)};
  }

  auto ChessRuleAdapter::getItemEntry(TileCoords const &tile) const noexcept
      -> ItemStore::Entry
  {
    auto found = itemPlacements_.find(tile);
    if (found == itemPlacements_.cend())
    {
      return ItemStore::Entry();
    }

    return found->second;
  }

  // auto ChessRuleAdapter::addItemEntry(
  //     TileCoords tile, ItemStore::Entry entry) noexcept -> bool
  // {
  //   auto [iter, inserted] = itemPlacements_.try_emplace(
  //       std::move(tile), std::move(entry));

  //   return inserted;
  // }

  auto ChessRuleAdapter::getPositionToTileConverter(Side const &color) noexcept
      -> std::function<TileCoords(Position const &)>
  {
    constexpr auto squareToTileAtWhite =
        [](Position const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const &rank = square.getRank();
      int const &file = square.getFile();

      return TileCoords{file - chess::FIRST_FILE, chess::LAST_RANK - rank};
    };

    constexpr auto squareToTileAtBlack =
        [](Position const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const &rank = square.getRank();
      int const &file = square.getFile();

      return TileCoords{chess::LAST_FILE - file, rank - chess::FIRST_RANK};
    };

    return chess::WHITE == color
               ? squareToTileAtWhite
               : squareToTileAtBlack;
  }

  auto ChessRuleAdapter::getTileToPositionConverter(Side const &color) noexcept
      -> std::function<Position(TileCoords const &)>
  {
    constexpr auto tileToSquareAtWhite =
        [](TileCoords const &tile) noexcept -> Position
    {
      BGG_VALIDATE_TILE(tile);

      return Position{tile.x + chess::FIRST_FILE, chess::LAST_RANK - tile.y};
    };

    constexpr auto tileToSquareAtBlack =
        [](TileCoords const &tile) noexcept -> Position
    {
      BGG_VALIDATE_TILE(tile);

      return Position{chess::LAST_FILE - tile.x, tile.y + chess::FIRST_RANK};
    };

    return chess::WHITE == color
               ? tileToSquareAtWhite
               : tileToSquareAtBlack;
  }
} // namespace bgg
