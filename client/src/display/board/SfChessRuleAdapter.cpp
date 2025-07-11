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

  constexpr auto SfChessRuleAdapter::squareToTile(
      ChessRule::Square const &square,
      ChessPiece::Color color) noexcept -> TileCoords
  {
    return TileCoords();
  }

  auto SfChessRuleAdapter::getSide() const -> int
  {
    return int(rule_.getColor());
  }

  auto SfChessRuleAdapter::getItemPlacements() -> SfItemPlacementMap &
  {
    return itemPlacements_;
  }

  auto SfChessRuleAdapter::getSelectableTiles() const noexcept -> SfItemPlacementMap
  {
    std::map<ChessRule::Square, ChessPiece>
        piecePlacements = rule_.getSelectablePieces();

    SfItemPlacementMap itemPlacements;
    for (auto &[square, piece] : piecePlacements)
    {
      TileCoords tile = squareToTileConverter_(square);
      auto [iter, inserted] = itemPlacements.try_emplace(
          tile, itemPlacements_.at(tile));

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

  static constexpr auto SfChessRuleAdapter::getSquareToTileConverter(ChessPiece::Color color) noexcept
      -> std::functional<TileCoords(ChessRule::Square const &)>
  {
    std::functional<TileCoords(ChessRule::Square const &)> squareToTileAtWhite =
        [](ChessRule::Square const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const row = int(square[1]);
      int const col = int(square[0]);

      return TileCoords{
          col - ChessRule::FIRST_COL,
          ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - row};
    };

    std::functional<TileCoords(ChessRule::Square const &)> squareToTileAtBlack =
        [](ChessRule::Square const &square) noexcept -> TileCoords
    {
      BGG_VALIDATE_SQUARE(square);

      int const row = int(square[1]);
      int const col = int(square[0]);

      return TileCoords{
          ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_COL - col,
          row - ChessRule::FIRST_ROW};
    };

    return color == ChessPiece::Color::WHITE
               ? squareToTileAtWhite
               : squareToTileAtBlack;
  }

  static constexpr auto SfChessRuleAdapter::getTileToSquareConverter(ChessPiece::Color color) noexcept
      -> std::functional<ChessRule::Square(TileCoords const &)>
  {
    return color == ChessPiece::Color::WHITE
               ? tileToSquareAtWhite
               : tileToSquareAtBlack;
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtWhite(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(tile.x + ChessRule::FIRST_COL),
        char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_ROW - tile.y),
        '\0'};
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtBlack(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(ChessRule::BOARD_SIDE - 1 + ChessRule::FIRST_COL - tile.x),
        char(tile.y + ChessRule::FIRST_ROW),
        '\0'};
  }

} // namespace bgg
