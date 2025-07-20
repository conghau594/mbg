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
          int(utils::getChessTextureCellIndex(piece)),
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

  auto ChessRuleAdapter::tryMove(
      ItemMove const &itemMove) const noexcept -> ChessItemMoveAction
  {
    ChessMove chessMove{
        tileToPosition(itemMove.fromTile),
        tileToPosition(itemMove.toTile),
        itemMove.promote};

    SPDLOG_DEBUG(
        "Try move from ({}, {}) to ({}, {}) (promote = {})",
        itemMove.fromTile.x, itemMove.fromTile.y,
        itemMove.toTile.x, itemMove.toTile.y,
        itemMove.promote ? itemMove.promote.value() : std::string("None"));

    ChessMove::VariantAction chessMoveAction = rule_.tryMove(chessMove);
    if (auto normalChessMove = chessMoveAction.getIf<ChessMove::Normal>())
    {
      NormalChessItemMove normalMove{
          positionToTile(normalChessMove->fromSquare),
          positionToTile(normalChessMove->toSquare),
          positionToTile(normalChessMove->enemyKingSquare),
          normalChessMove->enemyKingState};

      SPDLOG_DEBUG(
          "Move try from ({}, {}) to ({}, {}) is a 'NormalChessItemMove': "
          "enemykingTile = ({}, {}), enemyKingState = {}",
          normalMove.fromTile.x, normalMove.fromTile.y,
          normalMove.toTile.x, normalMove.toTile.y,
          normalMove.enemyKingTile.x, normalMove.enemyKingTile.y,
          utils::toString(normalMove.enemyKingState));

      return normalMove;
    }
    else if (auto chessPromotionMove = chessMoveAction.getIf<ChessMove::Promotion>())
    {
      ChessPromotionItemMove promotionMove{
          positionToTile(chessPromotionMove->fromSquare),
          positionToTile(chessPromotionMove->toSquare),
          chessPromotionMove->promote,
          positionToTile(chessPromotionMove->enemyKingSquare),
          chessPromotionMove->enemyKingState};

      SPDLOG_DEBUG(
          "Move try from ({}, {}) to ({}, {}) is a 'ChessPromotionItemMove': "
          "enemykingTile = ({}, {}), enemyKingState = {}, promote = {}",
          promotionMove.fromTile.x, promotionMove.fromTile.y,
          promotionMove.toTile.x, promotionMove.toTile.y,
          promotionMove.enemyKingTile.x, promotionMove.enemyKingTile.y,
          utils::toString(promotionMove.enemyKingState),
          promotionMove.promote);

      return promotionMove;
    }
    else if (auto chessEnPassantMove = chessMoveAction.getIf<ChessMove::EnPassant>())
    {
      ChessEnPassantItemMove enPassantMove{
          positionToTile(chessEnPassantMove->fromSquare),
          positionToTile(chessEnPassantMove->toSquare),
          positionToTile(chessEnPassantMove->enPassantSquare),
          positionToTile(chessEnPassantMove->enemyKingSquare),
          chessEnPassantMove->enemyKingState};

      SPDLOG_DEBUG(
          "Move try from ({}, {}) to ({}, {}) is a 'ChessEnPassantItemMove': "
          "enPassantTile = ({}, {}), enemykingTile = ({}, {}), enemyKingState = {}",
          enPassantMove.fromTile.x, enPassantMove.fromTile.y,
          enPassantMove.toTile.x, enPassantMove.toTile.y,
          enPassantMove.enPassantTile.x, enPassantMove.enPassantTile.y,
          enPassantMove.enemyKingTile.x, enPassantMove.enemyKingTile.y,
          utils::toString(enPassantMove.enemyKingState));

      return enPassantMove;
    }
    else if (auto chessCastlingMove = chessMoveAction.getIf<ChessMove::Castling>())
    {
      ChessCastlingItemMove castlingMove{
          positionToTile(chessCastlingMove->fromSquare),
          positionToTile(chessCastlingMove->toSquare),
          positionToTile(chessCastlingMove->rookSource),
          positionToTile(chessCastlingMove->rookDestination),
          positionToTile(chessCastlingMove->enemyKingSquare),
          chessCastlingMove->enemyKingState};

      SPDLOG_DEBUG(
          "Move try from ({}, {}) to ({}, {}) is a 'ChessCastlingItemMove': "
          "the rook move from ({}, {}) to ({}, {}), "
          "enemykingTile = ({}, {}), enemyKingState = {}",
          castlingMove.fromTile.x, castlingMove.fromTile.y,
          castlingMove.toTile.x, castlingMove.toTile.y,
          castlingMove.rookSource.x, castlingMove.rookSource.y,
          castlingMove.rookDestination.x, castlingMove.rookDestination.y,
          castlingMove.enemyKingTile.x, castlingMove.enemyKingTile.y,
          utils::toString(castlingMove.enemyKingState));

      return castlingMove;
    }
    else if (chessMoveAction.getIf<std::monostate>())
    {
      SPDLOG_ERROR("The move try is empty");
    }
    else
    {
      auto invalidChessMove = chessMoveAction.getIf<ChessMove::Invalid>();
      BOOST_ASSERT_MSG(
          invalidChessMove,
          "The last case must be of type ChessMove::Invalid");

      std::string errorMsg = utils::toString(invalidChessMove->error);
      SPDLOG_ERROR("The move try is an error: {}", errorMsg);
      return InvalidChessItemMove{errorMsg};
    }
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
    auto piece = rule_.getPiece(tileToPosition(tile));
    if (piece)
    {
      return piece->color;
    }
    return std::nullopt;
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

    CandidateChessMoveInfo candidateMoveInfo = rule_.collectCandidateMoves(originSquare);

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

    std::list<TileCoords> specialMoveTiles;
    for (auto &square : candidateMoveInfo.specialMoveSquares)
    {
      specialMoveTiles.emplace_back(positionToTile(square));
    }

    return ReachableTileInfo{
        itemEntry,
        std::move(quietSquares),
        std::move(captureSquares),
        std::move(specialMoveTiles)};
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
