// ChessItemMove.h
#pragma once

#include <optional>
#include <string>

#include <SFML/System/Vector2.hpp>

#include "base/Variant.h"

namespace bgg
{
  using TileCoords = sf::Vector2i;

  class ChessItemMove
  {
  public:
    TileCoords fromTile;
    TileCoords toTile;
    std::optional<std::string> promote;
  };

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
