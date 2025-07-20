// ChessItemMove.h
#pragma once

#include "ItemMove.h"

namespace bgg
{
  /////////////////////////////////////////////////////////////////////////////
  class NormalChessItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;

    TileCoords enemyKingTile;
    KingState enemyKingState;
  };

  /** \brief just for Pawns
   *
   */
  class ChessPromotionItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;

    std::string promote; ///< the piece name, is one of: Queen, Rook, Bishop, Knight

    TileCoords enemyKingTile;
    KingState enemyKingState;
  };

  /** \brief just for Pawns
   *
   */
  class ChessEnPassantItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;

    TileCoords enPassantTile;

    TileCoords enemyKingTile;
    KingState enemyKingState;
  };

  /** \brief just for Kings
   *
   */
  class ChessCastlingItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;

    TileCoords rookSource;
    TileCoords rookDestination;

    TileCoords enemyKingTile;
    KingState enemyKingState;
  };

  class InvalidChessItemMove
  {
  public:
    std::string errorMsg;
  };

  using ChessItemMoveAction = Variant<
      InvalidChessItemMove,
      NormalChessItemMove,
      ChessPromotionItemMove,
      ChessEnPassantItemMove,
      ChessCastlingItemMove>;
} // namespace bgg
