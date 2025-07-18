// ChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "ChessRuleAdapter.h"
#include "display/board/ZOrder.h"
#include "display/board/chess/ChessTextureCell.h"

namespace bgg
{
  ChessRuleAdapter::ChessRuleAdapter(
      std::shared_ptr<ItemStore> itemStore, ChessBoardState rule) noexcept
      : rule_(std::move(rule)),
        positionToTileConverter_(getPositionToTileConverter(rule_.getYourColor())),
        tileToPositionConverter_(getTileToPositionConverter(rule_.getYourColor()))
  {
    BOOST_ASSERT_MSG(itemStore, "The 'itemStore' cannot be null");

    // add piece items to itemStore, also assign the returned entry
    std::map<Position, Piece> const &initialBoard = rule_.getPiecePlacements();
    for (auto &[square, piece] : initialBoard)
    {
      ItemStore::Entry itemEntry = itemStore->addItem(
          ZOrder::SECOND_LAYER,
          ChessTextureCell::getIndex(piece),
          piece.toString());

      TileCoords tile = positionToTile(square);
      auto [iter, inserted] = itemPlacements_.try_emplace(tile, itemEntry);

      BOOST_ASSERT_MSG(inserted, "There must be one tile for each itemEntry");
    }
  }

  auto ChessRuleAdapter::positionToTile(
      Position const &position) const noexcept -> TileCoords
  {
    return positionToTileConverter_(position);
  }

  auto ChessRuleAdapter::tileToPosition(
      TileCoords const &tile) const noexcept -> Position
  {
    return tileToPositionConverter_(tile);
  }

  // TODO: Need refactor
  // auto ChessRuleAdapter::commitMove(
  //     ChessMove const &move, std::optional<BoardItem> promotedItem) noexcept
  //     -> ItemStore::Entry
  // {
  //   // change item entries
  //   auto selectedItemIter = itemPlacements_.find(move.fromTile);
  //   BOOST_ASSERT_MSG(
  //       selectedItemIter != itemPlacements_.end(),
  //       "There should be one item at the 'fromTile'");

  //   if (promotedItem)
  //   {
  //     selectedItemIter->second.getItem() = std::move(promotedItem.value());
  //   }

  //   ItemStore::Entry capturedItemEntry;
  //   auto capturedItemIter = itemPlacements_.find(move.toTile);
  //   if (capturedItemIter == itemPlacements_.end()) ///< if 'toSquare' is empty...
  //   {
  //     itemPlacements_.emplace(move.toTile, selectedItemIter->second);
  //   }
  //   else
  //   {
  //     capturedItemEntry = capturedItemIter->second;
  //     capturedItemIter->second = std::move(selectedItemIter->second);
  //   }

  //   itemPlacements_.erase(selectedItemIter);

  //   // change chess rule
  //   Position fromSquare = tileToPosition(move.fromTile);
  //   Position toSquare = tileToPosition(move.toTile);
  //   rule_.movePiece(fromSquare, toSquare, move.promote);

  //   return capturedItemEntry; // return captured item if any
  // }

  auto ChessRuleAdapter::getYourColor() const -> std::string const &
  {
    return rule_.getYourColor();
  }

  auto ChessRuleAdapter::getItemColor(TileCoords const &tile) const noexcept
      -> std::optional<std::string>
  {
    return rule_.getPieceColor(tileToPosition(tile));
  }

  auto ChessRuleAdapter::getItemPlacements() -> ItemPlacementMap const &
  {
    return itemPlacements_;
  }

  auto ChessRuleAdapter::getSelectableTiles() const noexcept -> ItemPlacementMap
  {
    std::map<Position, Piece>
        piecePlacements = rule_.getSelectablePieces(rule_.getYourColor());

    ItemPlacementMap itemPlacements;
    for (auto &[square, piece] : piecePlacements)
    {
      TileCoords tile = positionToTile(square);
      ItemStore::Entry entry = itemPlacements_.at(tile);
      auto [iter, inserted] = itemPlacements.try_emplace(
          std::move(tile), std::move(entry));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    return itemPlacements;
  }

  auto ChessRuleAdapter::getReachableTiles(TileCoords const &tile) const noexcept
      -> std::optional<ReachableTileInfo>
  {
    auto itemEntry = getItemEntry(tile);
    if (itemEntry.isNull())
    {
      return std::nullopt;
    }

    Position originSquare = tileToPosition(tile);

    CandidateChessMoveInfo candidateMoveInfo = rule_.getCandidateMoves(originSquare);

    std::list<TileCoords> quietSquares;
    for (auto &square : candidateMoveInfo.quietSquares)
    {
      quietSquares.emplace_back(positionToTile(square));
    }

    std::list<TileCoords> captureSquares;
    for (auto &square : candidateMoveInfo.captureSquares)
    {
      captureSquares.emplace_back(positionToTile(square));
    }

    std::list<TileCoords> specialSquares;
    for (auto &square : candidateMoveInfo.specialSquares)
    {
      specialSquares.emplace_back(positionToTile(square));
    }

    return ReachableTileInfo{
        itemEntry,
        std::move(quietSquares),
        std::move(captureSquares),
        std::move(specialSquares)};
  }

  // auto ChessRuleAdapter::getItemType(TileCoords const &tile) const noexcept
  //     -> std::optional<int>
  // {
  //   Position square = tileToPosition(tile);
  //   auto piece = rule_.getPiece(square);
  //   if(!piece)
  //   {
  //     return std::nullopt;
  //   }
  //   return int(piece->type);
  // }

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

  auto ChessRuleAdapter::getPositionToTileConverter(std::string color) noexcept
      -> std::function<TileCoords(Position const &)>
  {
    auto squareToTileAtWhite =
        [](Position const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const row = int(square[1]);
      int const col = int(square[0]);

      return TileCoords{
          col - ChessRule::FIRST_COL,
          ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - row};
    };

    auto squareToTileAtBlack =
        [](Position const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const row = int(square[1]);
      int const col = int(square[0]);

      return TileCoords{
          ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_COL - col,
          row - ChessRule::FIRST_ROW};
    };

    return Color::WHITE == color
               ? squareToTileAtWhite
               : squareToTileAtBlack;
  }

  auto ChessRuleAdapter::getTileToPositionConverter(std::string color) noexcept
      -> std::function<Position(TileCoords const &)>
  {
    auto tileToSquareAtWhite =
        [](TileCoords const &tile) noexcept -> Position
    {
      BGG_VALIDATE_TILE(tile);

      return Position{
          char(tile.x + ChessRule::FIRST_COL),
          char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - tile.y),
          '\0'};
    };

    auto tileToSquareAtBlack =
        [](TileCoords const &tile) noexcept -> Position
    {
      BGG_VALIDATE_TILE(tile);

      return Position{
          char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_COL - tile.x),
          char(tile.y + ChessRule::FIRST_ROW),
          '\0'};
    };

    return Color::WHITE == color
               ? tileToSquareAtWhite
               : tileToSquareAtBlack;
  }
} // namespace bgg
