// SfChessRuleAdapter.cpp

#include <boost/assert.hpp>

#include "SfChessRuleAdapter.h"
#include "model/ChessPiece.h"

namespace bgg
{
  SfChessRuleAdapter::SfChessRuleAdapter(
      std::vector<SfItemStore::Entry> itemEntries, int side) noexcept
      : itemEntries_(std::move(itemEntries)),
        rule_(side),
        squareToTileConverter_(side == ChessColor::WHITE
                                   ? squareToTileAtWhite
                                   : squareToTileAtBlack),
        tileToSquareConverter_(side == ChessColor::WHITE
                                   ? tileToSquareAtWhite
                                   : tileToSquareAtBlack)
  {
  }

  auto SfChessRuleAdapter::getItemPlacements() const -> std::list<SfItemPlacement>
  {
    std::map<ChessRule::Piece, ChessRule::Square>
        piecePlacements = rule_.getPiecePlacements();

    std::list<SfItemPlacement> itemPlacements;
    for (auto &[piece, square] : piecePlacements)
    {
      itemPlacements.emplace_back(
          itemEntries_[std::size_t(piece)], squareToTileConverter_(square));
    }

    return itemPlacements;
  }

  auto SfChessRuleAdapter::getSelectableTiles() const noexcept
      -> std::list<SfItemPlacement>
  {
    std::map<ChessRule::Piece, ChessRule::Square>
        piecePlacements = rule_.getSelectablePieces();

    std::list<SfItemPlacement> itemPlacements;
    for (auto &[piece, square] : piecePlacements)
    {
      itemPlacements.emplace_back(
          itemEntries_[std::size_t(piece)], squareToTileConverter_(square));
    }

    return itemPlacements;
  }

  auto SfChessRuleAdapter::getReachableTiles(TileCoords const &tile) const noexcept
      -> std::optional<SfReachableTileInfo>
  {
    ChessRule::Square originSquare = tileToSquareConverter_(tile);

    std::optional<ChessRule::CandidateMoveInfo>
        candidateMovesOpt = rule_.getCandidateMoves(originSquare);
    if (!candidateMovesOpt.has_value())
    {
      return std::optional<SfReachableTileInfo>(std::nullopt);
    }

    ChessRule::CandidateMoveInfo const &candidateMoveInfo = candidateMovesOpt.value();

    SfItemPlacement itemPlacement{
      itemEntries_[std::size_t(candidateMoveInfo.piece)],
      squareToTileConverter_(originSquare)
    };
    
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
        std::move(itemPlacement),
        std::move(quietMoves),
        std::move(captureMoves),
        std::move(specialMoves)};
  }

  auto SfChessRuleAdapter::getItemIndex(TileCoords const &tile) const noexcept -> int
  {
    ChessRule::Square square = tileToSquareConverter_(tile);
    return rule_.getPiece(square);
  }

  auto SfChessRuleAdapter::getItemTile(int itemIndex) const noexcept -> TileCoords
  {
    ChessRule::Square square = rule_.getSquare(itemIndex);
    return squareToTileConverter_(square);
  }

  auto SfChessRuleAdapter::getItemEntry(int itemIndex) const noexcept -> SfItemStore::Entry
  {
    return itemEntries_[std::size_t(itemIndex)];
  }

  constexpr auto SfChessRuleAdapter::squareToTileAtWhite(
      ChessRule::Square const &square) noexcept -> TileCoords
  {
    BGG_VALIDATE_SQUARE(square);

    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        col - ChessRule::Board::FIRST_COL,
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_ROW - row};
  }

  constexpr auto SfChessRuleAdapter::squareToTileAtBlack(
      ChessRule::Square const &square) noexcept -> TileCoords
  {
    BGG_VALIDATE_SQUARE(square);

    int const row = int(square[1]);
    int const col = int(square[0]);

    return TileCoords{
        ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_COL - col,
        row - ChessRule::Board::FIRST_ROW};
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtWhite(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(tile.x + ChessRule::Board::FIRST_COL),
        char(ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_ROW - tile.y),
        '\0'};
  }

  constexpr auto SfChessRuleAdapter::tileToSquareAtBlack(
      TileCoords const &tile) noexcept -> ChessRule::Square
  {
    BGG_VALIDATE_TILE(tile);

    return ChessRule::Square{
        char(ChessRule::Board::SIDE_LENGTH - 1 + ChessRule::Board::FIRST_COL - tile.x),
        char(tile.y + ChessRule::Board::FIRST_ROW),
        '\0'};
  }

} // namespace bgg
