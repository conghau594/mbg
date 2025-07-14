// SfChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "SfChessRuleAdapter.h"
#include "model/ChessPiece.h"

namespace bgg
{
  SfChessRuleAdapter::SfChessRuleAdapter(ChessRule rule) noexcept
      : rule_(std::move(rule)),
        squareToTileConverter_(getSquareToTileConverter(rule_.getColor())),
        tileToSquareConverter_(getTileToSquareConverter(rule_.getColor()))
  {
  }

  auto SfChessRuleAdapter::positionToTile(
      Position const &position) const noexcept -> TileCoords
  {
    return squareToTileConverter_(position);
  }

  auto SfChessRuleAdapter::tileToPosition(
      TileCoords const &tile) const noexcept -> Position
  {
    return tileToSquareConverter_(tile);
  }

  auto SfChessRuleAdapter::getSide() const -> std::string const &
  {
    return rule_.getColor();
  }

  auto SfChessRuleAdapter::getItemPlacements() -> SfItemPlacementMap &
  {
    return itemPlacements_;
  }

  auto SfChessRuleAdapter::getSelectableTiles() const noexcept -> SfItemPlacementMap
  {
    std::map<ChessRule::Square, Piece>
        piecePlacements = rule_.getSelectablePieces();

    SfItemPlacementMap itemPlacements;
    for (auto &[square, piece] : piecePlacements)
    {
      TileCoords tile = squareToTileConverter_(square);
      SfItemStore::Entry entry = itemPlacements_.at(tile);
      auto [iter, inserted] = itemPlacements.try_emplace(
          std::move(tile), std::move(entry));

      BOOST_ASSERT_MSG(inserted, "There should be one item per tile");
    }

    return itemPlacements;
  }

  auto SfChessRuleAdapter::getReachableTiles(TileCoords const &tile) const noexcept
      -> std::optional<SfReachableTileInfo>
  {
    auto itemEntry = getItemEntry(tile);
    if (!itemEntry)
    {
      return std::nullopt;
    }

    ChessRule::Square originSquare = tileToSquareConverter_(tile);

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
      quietMoves.emplace_back(squareToTileConverter_(square));
    }

    std::list<TileCoords> captureMoves;
    for (auto &square : candidateMoveInfo.captureMoves)
    {
      captureMoves.emplace_back(squareToTileConverter_(square));
    }

    std::vector<TileCoords> specialMoves;
    if (candidateMoveInfo.specialMove.has_value())
    {
      ChessRule::Square square = candidateMoveInfo.specialMove.value();
      specialMoves.emplace_back(squareToTileConverter_(square));
    }

    return SfReachableTileInfo{
        itemEntry.value(),
        std::move(quietMoves),
        std::move(captureMoves),
        std::move(specialMoves)};
  }

  // auto SfChessRuleAdapter::getItemType(TileCoords const &tile) const noexcept
  //     -> std::optional<int>
  // {
  //   ChessRule::Square square = tileToSquareConverter_(tile);
  //   auto piece = rule_.getPiece(square);
  //   if(!piece)
  //   {
  //     return std::nullopt;
  //   }
  //   return int(piece.value().type);
  // }

  auto SfChessRuleAdapter::getItemEntry(TileCoords const &tile) const noexcept
      -> std::optional<SfItemStore::Entry>
  {
    auto found = itemPlacements_.find(tile);
    if (found == itemPlacements_.cend())
    {
      return std::nullopt;
    }

    return found->second;
  }

  auto SfChessRuleAdapter::getSquareToTileConverter(
      std::string color) noexcept
      -> std::function<TileCoords(ChessRule::Square const &)>
  {
    auto squareToTileAtWhite =
        [](ChessRule::Square const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const row = int(square[1]);
      int const col = int(square[0]);

      return TileCoords{
          col - ChessRule::FIRST_COL,
          ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - row};
    };

    auto squareToTileAtBlack =
        [](ChessRule::Square const &square) noexcept -> TileCoords
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

  auto SfChessRuleAdapter::getTileToSquareConverter(
      std::string color) noexcept
      -> std::function<ChessRule::Square(TileCoords const &)>
  {
    auto tileToSquareAtWhite =
        [](TileCoords const &tile) noexcept -> ChessRule::Square
    {
      BGG_VALIDATE_TILE(tile);

      return ChessRule::Square{
          char(tile.x + ChessRule::FIRST_COL),
          char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - tile.y),
          '\0'};
    };

    auto tileToSquareAtBlack =
        [](TileCoords const &tile) noexcept -> ChessRule::Square
    {
      BGG_VALIDATE_TILE(tile);

      return ChessRule::Square{
          char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_COL - tile.x),
          char(tile.y + ChessRule::FIRST_ROW),
          '\0'};
    };

    return Color::WHITE == color
               ? tileToSquareAtWhite
               : tileToSquareAtBlack;
  }
} // namespace bgg
