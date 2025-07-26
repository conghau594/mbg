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
      std::shared_ptr<ChessBoardState> chessRule,
      std::string allyColor) noexcept
      : chessRule_(std::move(chessRule)),
        allyColor_(std::move(allyColor)),
        positionToTileConverter_(getPositionToTileConverter(allyColor_)),
        tileToPositionConverter_(getTileToPositionConverter(allyColor_))
  {
    BOOST_ASSERT_MSG(itemStore, "The 'itemStore' cannot be null");

    // add piece items to itemStore, also assign the returned entry
    std::map<Position, Piece> const &initialBoard = chessRule_->getPiecePlacements();
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

  auto ChessRuleAdapter::chessMoveActionToItemMoveAction(
      ChessMove::Action const &chessMoveAction) const noexcept
      -> ChessItemMoveAction
  {
    if (auto normalChessMove = chessMoveAction.getIf<ChessMove::Normal>())
    {
      NormalChessItemMove normalMove{
          positionToTile(normalChessMove->fromSquare),
          positionToTile(normalChessMove->toSquare),
          positionToTile(normalChessMove->enemyKingSquare),
          normalChessMove->enemyKingState};

      // SPDLOG_DEBUG(
      //     "The move is from ({}, {}) to ({}, {}) and is a 'NormalChessItemMove': "
      //     "enemykingTile = ({}, {}), enemyKingState = {}",
      //     normalMove.fromTile.x, normalMove.fromTile.y,
      //     normalMove.toTile.x, normalMove.toTile.y,
      //     normalMove.enemyKingTile.x, normalMove.enemyKingTile.y,
      //     utils::toString(normalMove.enemyKingState));

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

      // SPDLOG_DEBUG(
      //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessPromotionItemMove': "
      //     "promote = {}, enemykingTile = ({}, {}), enemyKingState = {}",
      //     promotionMove.fromTile.x, promotionMove.fromTile.y,
      //     promotionMove.toTile.x, promotionMove.toTile.y,
      //     promotionMove.promote,
      //     promotionMove.enemyKingTile.x, promotionMove.enemyKingTile.y,
      //     utils::toString(promotionMove.enemyKingState));

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

      // SPDLOG_DEBUG(
      //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessEnPassantItemMove': "
      //     "enPassantTile = ({}, {}), enemykingTile = ({}, {}), enemyKingState = {}",
      //     enPassantMove.fromTile.x, enPassantMove.fromTile.y,
      //     enPassantMove.toTile.x, enPassantMove.toTile.y,
      //     enPassantMove.enPassantTile.x, enPassantMove.enPassantTile.y,
      //     enPassantMove.enemyKingTile.x, enPassantMove.enemyKingTile.y,
      //     utils::toString(enPassantMove.enemyKingState));

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

      // SPDLOG_DEBUG(
      //     "The move is from ({}, {}) to ({}, {}) and is a 'ChessCastlingItemMove': "
      //     "the rook move from ({}, {}) to ({}, {}), "
      //     "enemykingTile = ({}, {}), enemyKingState = {}",
      //     castlingMove.fromTile.x, castlingMove.fromTile.y,
      //     castlingMove.toTile.x, castlingMove.toTile.y,
      //     castlingMove.rookSource.x, castlingMove.rookSource.y,
      //     castlingMove.rookDestination.x, castlingMove.rookDestination.y,
      //     castlingMove.enemyKingTile.x, castlingMove.enemyKingTile.y,
      //     utils::toString(castlingMove.enemyKingState));

      return castlingMove;
    }
    else if (chessMoveAction.getIf<std::monostate>())
    {
      // SPDLOG_DEBUG("The move is empty");
      return std::monostate{};
    }
    else
    {
      auto invalidChessMove = chessMoveAction.getIf<ChessMove::Invalid>();
      BOOST_ASSERT_MSG(
          invalidChessMove,
          "The last case of this 'chessMoveAction' should be of type ChessMove::Invalid");

      std::string errorMsg = utils::toString(invalidChessMove->error);
      // SPDLOG_DEBUG("The move is invalid: {}", errorMsg);
      return InvalidChessItemMove{errorMsg};
    }
  }

  auto ChessRuleAdapter::itemMoveActionToChessMoveAction(
      ChessItemMoveAction const &itemMoveAction) const noexcept
      -> ChessMove::Action
  {
    auto itemMoveVisitor = [this]<typename T>(T const &action) {

    };
    if (auto normalItemMove = itemMoveAction.getIf<NormalChessItemMove>())
    {
      ChessMove::Normal normalMove{
          tileToPosition(normalItemMove->fromTile),
          tileToPosition(normalItemMove->toTile),
          tileToPosition(normalItemMove->enemyKingTile),
          normalItemMove->enemyKingState};

      // SPDLOG_DEBUG(
      //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Normal': "
      //     "enemykingSquare = {}{}, enemyKingState = {}",
      //     normalMove.fromSquare[0], normalMove.fromSquare[1],
      //     normalMove.toSquare[0], normalMove.toSquare[1],
      //     normalMove.enemyKingSquare[0], normalMove.enemyKingSquare[1],
      //     utils::toString(normalMove.enemyKingState));

      return normalMove;
    }
    else if (auto promotionItemMove = itemMoveAction.getIf<ChessPromotionItemMove>())
    {
      ChessMove::Promotion promotionMove{
          tileToPosition(promotionItemMove->fromTile),
          tileToPosition(promotionItemMove->toTile),
          promotionItemMove->promote,
          tileToPosition(promotionItemMove->enemyKingTile),
          promotionItemMove->enemyKingState};

      // SPDLOG_DEBUG(
      //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Promotion': "
      //     "promote = {}, enemykingSquare = {}{}, enemyKingState = {}",
      //     promotionMove.fromSquare[0], promotionMove.fromSquare[1],
      //     promotionMove.toSquare[0], promotionMove.toSquare[1],
      //     promotionMove.promote,
      //     promotionMove.enemyKingSquare[0], promotionMove.enemyKingSquare[1],
      //     utils::toString(promotionMove.enemyKingState));

      return promotionMove;
    }
    else if (auto chessEnPassantMove = itemMoveAction.getIf<ChessEnPassantItemMove>())
    {
      ChessMove::EnPassant enPassantMove{
          tileToPosition(chessEnPassantMove->fromTile),
          tileToPosition(chessEnPassantMove->toTile),
          tileToPosition(chessEnPassantMove->enPassantTile),
          tileToPosition(chessEnPassantMove->enemyKingTile),
          chessEnPassantMove->enemyKingState};

      // SPDLOG_DEBUG(
      //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::EnPassant': "
      //     "enPassantSquare = {}{}, enemykingSquare = {}{}, enemyKingState = {}",
      //     enPassantMove.fromSquare[0], enPassantMove.fromSquare[1],
      //     enPassantMove.toSquare[0], enPassantMove.toSquare[1],
      //     enPassantMove.enPassantSquare[0], enPassantMove.enPassantSquare[1],
      //     enPassantMove.enemyKingSquare[0], enPassantMove.enemyKingSquare[1],
      //     utils::toString(enPassantMove.enemyKingState));

      return enPassantMove;
    }
    else if (auto chessCastlingMove = itemMoveAction.getIf<ChessCastlingItemMove>())
    {
      ChessMove::Castling castlingMove{
          tileToPosition(chessCastlingMove->fromTile),
          tileToPosition(chessCastlingMove->toTile),
          tileToPosition(chessCastlingMove->rookSource),
          tileToPosition(chessCastlingMove->rookDestination),
          tileToPosition(chessCastlingMove->enemyKingTile),
          chessCastlingMove->enemyKingState};

      // SPDLOG_DEBUG(
      //     "The move is from '{}{}' to '{}{}' and is a 'ChessMove::Castling': "
      //     "the rook move from '{}{}' to '{}{}', "
      //     "enemykingSquare = {}{}, enemyKingState = {}",
      //     castlingMove.fromSquare[0], castlingMove.fromSquare[1],
      //     castlingMove.toSquare[0], castlingMove.toSquare[1],
      //     castlingMove.rookSource[0], castlingMove.rookSource[1],
      //     castlingMove.rookDestination[0], castlingMove.rookDestination[1],
      //     castlingMove.enemyKingSquare[0], castlingMove.enemyKingSquare[1],
      //     utils::toString(castlingMove.enemyKingState));

      return castlingMove;
    }
    else if (itemMoveAction.getIf<std::monostate>())
    {
      SPDLOG_DEBUG("The move is empty");
      return std::monostate{};
    }
    else
    {
      auto invalidItemMove = itemMoveAction.getIf<InvalidChessItemMove>();
      BOOST_ASSERT_MSG(
          invalidItemMove,
          "The last case of this 'itemMoveAction' should be of type ChessMove::Invalid");

      SPDLOG_DEBUG("The move is invalid: {}", invalidItemMove->errorMsg);
      return ChessMove::Invalid{"", "", ChessMove::Error::UNDEFINED};
    }
  }

  auto ChessRuleAdapter::tryMove(
      ChessItemMove const &itemMove, std::string const &color) const noexcept
      -> ChessItemMoveAction
  {
    ChessMove chessMove{
        tileToPosition(itemMove.fromTile),
        tileToPosition(itemMove.toTile),
        itemMove.promote};

    // SPDLOG_DEBUG(
    //     "Try move from ({}, {}) to ({}, {}) (promote = {})",
    //     itemMove.fromTile.x, itemMove.fromTile.y,
    //     itemMove.toTile.x, itemMove.toTile.y,
    //     itemMove.promote ? itemMove.promote.value() : std::string("None"));

    ChessMove::Action chessMoveAction = chessRule_->tryMove(chessMove, color);

    return chessMoveActionToItemMoveAction(chessMoveAction);
  }

  auto ChessRuleAdapter::getAllyColor() const -> std::string
  {
    return allyColor_;
  }

  auto ChessRuleAdapter::getEnemyColor() const -> std::string
  {
    return ChessRule::getEnemyColor(allyColor_);
  }

  auto ChessRuleAdapter::getItemColor(TileCoords const &tile) const noexcept
      -> std::optional<std::string>
  {
    auto piece = chessRule_->getPiece(tileToPosition(tile));
    if (piece)
    {
      return piece->color;
    }
    return std::nullopt;
  }

  auto ChessRuleAdapter::getItemType(TileCoords const &tile) const noexcept
      -> std::optional<std::string>
  {
    auto piece = chessRule_->getPiece(tileToPosition(tile));
    if (piece)
    {
      return piece->type;
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
        piecePlacements = chessRule_->collectSelectablePieces(allyColor_);

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

    CandidateChessMoveInfo candidateMoveInfo = chessRule_->collectCandidateMoves(originSquare);

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

  void ChessRuleAdapter::commitMove(
      ChessItemMoveAction const &itemMoveAction) noexcept
  {
    auto moveActionVisitor = [this]<typename T>(T const &action)
    {
      if constexpr (requires {
          { action.fromTile } -> std::same_as<TileCoords const &>;
          { action.toTile } -> std::same_as<TileCoords const &>; })
      {
        // SPDLOG_INFO("You have committed a '{}'", typeid(T).name());

        auto movedItemEntry = getItemEntry(action.fromTile);
        BOOST_ASSERT_MSG(
            !movedItemEntry.isNull(),
            "There must be an item at the 'fromTile'");

        ///< commit basic move action
        itemPlacements_.erase(action.toTile);
        itemPlacements_.emplace(action.toTile, movedItemEntry);
        itemPlacements_.erase(action.fromTile);

        if constexpr (requires { 
            { action.promote } -> std::same_as<std::string const &>; }) ///< if constexpr (std::is_same_v<T, ChessPromotionItemMove>)
        {
          ///< do nothing
          ///< because the appearance change of 'movedItemEntry' must be done
          ///< outside this class.
        }
        else if constexpr (requires { 
            { action.enPassantTile } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessEnPassantItemMove>)
        {
          ///< remove en passant captured item
          itemPlacements_.erase(action.enPassantTile);
        }
        else if constexpr (requires { 
                { action.rookSource } -> std::same_as<TileCoords const &>;
                { action.rookDestination } -> std::same_as<TileCoords const &>; }) ///< if constexpr (std::is_same_v<T, ChessCastlingItemMove>)
        {
          ///< move the related rook to the destination
          auto rookItemEntry = getItemEntry(action.rookSource);
          BOOST_ASSERT_MSG(
              !rookItemEntry.isNull(),
              "There must be an item at the tile of action.rookSource");

          itemPlacements_.emplace(action.rookDestination, rookItemEntry);
          itemPlacements_.erase(action.rookSource);
        }
      }
      else if constexpr (std::is_same_v<T, std::monostate>)
      {
        SPDLOG_WARN("You have committed an empty move");
      }
      else if constexpr (std::is_same_v<T, InvalidChessItemMove>)
      {
        SPDLOG_WARN("You have committed an 'InvalidChessItemMove'");
      }
    };

    itemMoveAction.visit(moveActionVisitor);

    ///< commit move to chess rule
    chessRule_->commitMove(itemMoveActionToChessMoveAction(itemMoveAction));
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
