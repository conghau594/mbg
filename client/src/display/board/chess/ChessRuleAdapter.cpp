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

  // auto ChessRuleAdapter::chessMoveActionToItemMoveAction(
  //     ChessMove::Action const &chessMoveAction) const noexcept
  //     -> ChessItemMoveAction
  // {
  //   if (auto normalChessMove = chessMoveAction.getIf<ChessMove::Normal>())
  //   {
  //     NormalChessItemMove normalMove{
  //         positionToTile(normalChessMove->fromSquare),
  //         positionToTile(normalChessMove->toSquare),
  //         positionToTile(normalChessMove->opponentKingSquare),
  //         normalChessMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from ({}, {}) to ({}, {}) and is a 'NormalChessItemMove': "
  //     //     "opponentkingTile = ({}, {}), opponentKingStatus = {}",
  //     //     normalMove.fromTile.x, normalMove.fromTile.y,
  //     //     normalMove.toTile.x, normalMove.toTile.y,
  //     //     normalMove.opponentKingTile.x, normalMove.opponentKingTile.y,
  //     //     utils::toString(normalMove.opponentKingStatus));

  //     return normalMove;
  //   }
  //   else if (auto chessPromotionMove = chessMoveAction.getIf<ChessMove::Promotion>())
  //   {
  //     ChessPromotionItemMove promotionMove{
  //         positionToTile(chessPromotionMove->fromSquare),
  //         positionToTile(chessPromotionMove->toSquare),
  //         chessPromotionMove->promote,
  //         positionToTile(chessPromotionMove->opponentKingSquare),
  //         chessPromotionMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessPromotionItemMove': "
  //     //     "promote = {}, opponentkingTile = ({}, {}), opponentKingStatus = {}",
  //     //     promotionMove.fromTile.x, promotionMove.fromTile.y,
  //     //     promotionMove.toTile.x, promotionMove.toTile.y,
  //     //     promotionMove.promote,
  //     //     promotionMove.opponentKingTile.x, promotionMove.opponentKingTile.y,
  //     //     utils::toString(promotionMove.opponentKingStatus));

  //     return promotionMove;
  //   }
  //   else if (auto chessEnPassantMove = chessMoveAction.getIf<ChessMove::EnPassant>())
  //   {
  //     ChessEnPassantItemMove enPassantMove{
  //         positionToTile(chessEnPassantMove->fromSquare),
  //         positionToTile(chessEnPassantMove->toSquare),
  //         positionToTile(chessEnPassantMove->enPassantSquare),
  //         positionToTile(chessEnPassantMove->opponentKingSquare),
  //         chessEnPassantMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessEnPassantItemMove': "
  //     //     "enPassantTile = ({}, {}), opponentkingTile = ({}, {}), opponentKingStatus = {}",
  //     //     enPassantMove.fromTile.x, enPassantMove.fromTile.y,
  //     //     enPassantMove.toTile.x, enPassantMove.toTile.y,
  //     //     enPassantMove.enPassantTile.x, enPassantMove.enPassantTile.y,
  //     //     enPassantMove.opponentKingTile.x, enPassantMove.opponentKingTile.y,
  //     //     utils::toString(enPassantMove.opponentKingStatus));

  //     return enPassantMove;
  //   }
  //   else if (auto chessCastlingMove = chessMoveAction.getIf<ChessMove::Castling>())
  //   {
  //     ChessCastlingItemMove castlingMove{
  //         positionToTile(chessCastlingMove->fromSquare),
  //         positionToTile(chessCastlingMove->toSquare),
  //         positionToTile(chessCastlingMove->rookSource),
  //         positionToTile(chessCastlingMove->rookDestination),
  //         positionToTile(chessCastlingMove->opponentKingSquare),
  //         chessCastlingMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessCastlingItemMove': "
  //     //     "the rook move from ({}, {}) to ({}, {}), "
  //     //     "opponentkingTile = ({}, {}), opponentKingStatus = {}",
  //     //     castlingMove.fromTile.x, castlingMove.fromTile.y,
  //     //     castlingMove.toTile.x, castlingMove.toTile.y,
  //     //     castlingMove.rookSource.x, castlingMove.rookSource.y,
  //     //     castlingMove.rookDestination.x, castlingMove.rookDestination.y,
  //     //     castlingMove.opponentKingTile.x, castlingMove.opponentKingTile.y,
  //     //     utils::toString(castlingMove.opponentKingStatus));

  //     return castlingMove;
  //   }
  //   else if (chessMoveAction.getIf<std::monostate>())
  //   {
  //     // SPDLOG_DEBUG("The move is empty");
  //     return std::monostate{};
  //   }
  //   else
  //   {
  //     auto invalidChessMove = chessMoveAction.getIf<ChessMove::Invalid>();
  //     BOOST_ASSERT_MSG(
  //         invalidChessMove,
  //         "The last case of this 'chessMoveAction' should be of type ChessMove::Invalid");

  //     std::string errorMsg = utils::toString(invalidChessMove->error);
  //     // SPDLOG_DEBUG("The move is invalid: {}", errorMsg);
  //     return InvalidChessItemMove{errorMsg};
  //   }
  // }

  // auto ChessRuleAdapter::itemMoveActionToChessMoveAction(
  //     ChessItemMoveAction const &itemMoveAction) const noexcept
  //     -> ChessMove::Action
  // {
  //   auto itemMoveVisitor = [this]<typename T>(T const &action) {

  //   };
  //   if (auto normalItemMove = itemMoveAction.getIf<NormalChessItemMove>())
  //   {
  //     ChessMove::Normal normalMove{
  //         tileToPosition(normalItemMove->fromTile),
  //         tileToPosition(normalItemMove->toTile),
  //         tileToPosition(normalItemMove->opponentKingTile),
  //         normalItemMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Normal': "
  //     //     "opponentkingSquare = {}{}, opponentKingStatus = {}",
  //     //     normalMove.fromSquare[0], normalMove.fromSquare[1],
  //     //     normalMove.toSquare[0], normalMove.toSquare[1],
  //     //     normalMove.opponentKingSquare[0], normalMove.opponentKingSquare[1],
  //     //     utils::toString(normalMove.opponentKingStatus));

  //     return normalMove;
  //   }
  //   else if (auto promotionItemMove = itemMoveAction.getIf<ChessPromotionItemMove>())
  //   {
  //     ChessMove::Promotion promotionMove{
  //         tileToPosition(promotionItemMove->fromTile),
  //         tileToPosition(promotionItemMove->toTile),
  //         promotionItemMove->promote,
  //         tileToPosition(promotionItemMove->opponentKingTile),
  //         promotionItemMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Promotion': "
  //     //     "promote = {}, opponentkingSquare = {}{}, opponentKingStatus = {}",
  //     //     promotionMove.fromSquare[0], promotionMove.fromSquare[1],
  //     //     promotionMove.toSquare[0], promotionMove.toSquare[1],
  //     //     promotionMove.promote,
  //     //     promotionMove.opponentKingSquare[0], promotionMove.opponentKingSquare[1],
  //     //     utils::toString(promotionMove.opponentKingStatus));

  //     return promotionMove;
  //   }
  //   else if (auto chessEnPassantMove = itemMoveAction.getIf<ChessEnPassantItemMove>())
  //   {
  //     ChessMove::EnPassant enPassantMove{
  //         tileToPosition(chessEnPassantMove->fromTile),
  //         tileToPosition(chessEnPassantMove->toTile),
  //         tileToPosition(chessEnPassantMove->enPassantTile),
  //         tileToPosition(chessEnPassantMove->opponentKingTile),
  //         chessEnPassantMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::EnPassant': "
  //     //     "enPassantSquare = {}{}, opponentkingSquare = {}{}, opponentKingStatus = {}",
  //     //     enPassantMove.fromSquare[0], enPassantMove.fromSquare[1],
  //     //     enPassantMove.toSquare[0], enPassantMove.toSquare[1],
  //     //     enPassantMove.enPassantSquare[0], enPassantMove.enPassantSquare[1],
  //     //     enPassantMove.opponentKingSquare[0], enPassantMove.opponentKingSquare[1],
  //     //     utils::toString(enPassantMove.opponentKingStatus));

  //     return enPassantMove;
  //   }
  //   else if (auto chessCastlingMove = itemMoveAction.getIf<ChessCastlingItemMove>())
  //   {
  //     ChessMove::Castling castlingMove{
  //         tileToPosition(chessCastlingMove->fromTile),
  //         tileToPosition(chessCastlingMove->toTile),
  //         tileToPosition(chessCastlingMove->rookSource),
  //         tileToPosition(chessCastlingMove->rookDestination),
  //         tileToPosition(chessCastlingMove->opponentKingTile),
  //         chessCastlingMove->opponentKingStatus};

  //     // SPDLOG_DEBUG(
  //     //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Castling': "
  //     //     "the rook move from '{}{}' to '{}{}', "
  //     //     "opponentkingSquare = {}{}, opponentKingStatus = {}",
  //     //     castlingMove.fromSquare[0], castlingMove.fromSquare[1],
  //     //     castlingMove.toSquare[0], castlingMove.toSquare[1],
  //     //     castlingMove.rookSource[0], castlingMove.rookSource[1],
  //     //     castlingMove.rookDestination[0], castlingMove.rookDestination[1],
  //     //     castlingMove.opponentKingSquare[0], castlingMove.opponentKingSquare[1],
  //     //     utils::toString(castlingMove.opponentKingStatus));

  //     return castlingMove;
  //   }
  //   else if (itemMoveAction.getIf<std::monostate>())
  //   {
  //     SPDLOG_DEBUG("The move is empty");
  //     return std::monostate{};
  //   }
  //   else
  //   {
  //     auto invalidItemMove = itemMoveAction.getIf<InvalidChessItemMove>();
  //     BOOST_ASSERT_MSG(
  //         invalidItemMove,
  //         "The last case of this 'itemMoveAction' should be of type ChessMove::Invalid");

  //     SPDLOG_DEBUG("The move is invalid: {}", invalidItemMove->errorMsg);
  //     return ChessMove::Invalid{"", "", ChessMove::Error::UNDEFINED};
  //   }
  // }

  auto ChessRuleAdapter::tryMove(
      ChessItemMove const &itemMove, Side const &color) const noexcept
      -> ChessItemMove::Detail
  {
    ChessMove chessMove{
        color,
        tileToPosition(itemMove.fromTile),
        tileToPosition(itemMove.toTile),
        itemMove.promote};

    // SPDLOG_DEBUG(
    //     "Try move from ({}, {}) to ({}, {}) (promote = {})",
    //     itemMove.fromTile.x, itemMove.fromTile.y,
    //     itemMove.toTile.x, itemMove.toTile.y,
    //     itemMove.promote ? itemMove.promote.value() : std::string("None"));

    return ChessItemMove::Detail{chessRule_->tryMove(chessMove), posToTileConverter_};
  }

  void ChessRuleAdapter::commitMove(
      ChessMove::Detail const &nativeMoveDetail) noexcept
  {
    chessRule_->commitMove(nativeMoveDetail);
  }

  void ChessRuleAdapter::commitMove(
      ChessItemMove::Detail const &itemMoveDetail) noexcept
  {
    chessRule_->commitMove(itemMoveDetail.getNativeMoveDetail());
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
