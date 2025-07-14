// ChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "ChessRuleAdapter.h"
#include "model/ChessPiece.h"
#include "display/board/ZOrder.h"
#include "display/board/chess/ChessTextureCell.h"

namespace bgg
{
  ChessRuleAdapter::ChessRuleAdapter(
      std::shared_ptr<ItemStore> itemStore, ChessRule rule) noexcept
      : itemStore_(std::move(itemStore)),
        rule_(std::move(rule)),
        positionToTileConverter_(getPositionToTileConverter(rule_.getYourColor())),
        tileToPositionConverter_(getTileToPositionConverter(rule_.getYourColor()))
  {
    // add piece items to itemStore, also assign the returned entry
    std::map<Position, Piece> const &initialBoard = rule_.getPiecePlacements();
    for (auto &[square, piece] : initialBoard)
    {
      ItemStore::Entry itemEntry = itemStore_->addItem(
          ChessTextureCell::getIndex(piece),
          ZOrder::SECOND_LAYER,
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

  void ChessRuleAdapter::commitMove(ChessPieceMove const &move) noexcept
  {
    auto selectedItemIter = itemPlacements_.find(move.fromTile);
    if (selectedItemIter == itemPlacements_.end())
    {
      BOOST_ASSERT_MSG(false, "There must be one item at the 'fromTile'");
    }

    auto capturedItemEntry = itemPlacements_.find(move.toTile);
    if (capturedItemEntry == itemPlacements_.end()) ///< if 'toSquare' is empty...
    {
      itemPlacements_.emplace(move.toTile, selectedItemIter->second);
    }
    else
    {
      capturedItemEntry->second = selectedItemIter->second;
    }

    itemPlacements_.erase(selectedItemIter);

    // change chess rule
    Position fromSquare = tileToPosition(move.fromTile);
    Position toSquare = tileToPosition(move.toTile);
    rule_.movePiece(fromSquare, toSquare, move.promote);
  }

  auto ChessRuleAdapter::getYourColor() const -> std::string const &
  {
    return rule_.getYourColor();
  }

  auto ChessRuleAdapter::getColor(TileCoords const &tile) const noexcept
      -> std::optional<std::string>
  {
    return rule_.getColor(tileToPosition(tile));
  }

  auto ChessRuleAdapter::getItemPlacements() -> ItemPlacementMap const &
  {
    return itemPlacements_;
  }

  auto ChessRuleAdapter::getSelectableTiles() const noexcept -> ItemPlacementMap
  {
    std::map<Position, Piece>
        piecePlacements = rule_.getSelectablePieces();

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

    std::optional<ChessRule::CandidateMoveInfo>
        candidateMovesOpt = rule_.getCandidateMoves(originSquare);
    if (!candidateMovesOpt)
    {
      return std::nullopt;
    }

    ChessRule::CandidateMoveInfo const &candidateMoveInfo = candidateMovesOpt.value();

    std::list<TileCoords> quietMoves;
    for (auto &square : candidateMoveInfo.quietMoves)
    {
      quietMoves.emplace_back(positionToTile(square));
    }

    std::list<TileCoords> captureMoves;
    for (auto &square : candidateMoveInfo.captureMoves)
    {
      captureMoves.emplace_back(positionToTile(square));
    }

    std::vector<TileCoords> specialMoves;
    if (candidateMoveInfo.specialMove.has_value())
    {
      Position square = candidateMoveInfo.specialMove.value();
      specialMoves.emplace_back(positionToTile(square));
    }

    return ReachableTileInfo{
        itemEntry,
        std::move(quietMoves),
        std::move(captureMoves),
        std::move(specialMoves)};
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
