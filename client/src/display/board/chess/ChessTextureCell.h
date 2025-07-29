// ChessBoardItem.h
#pragma once

// #include "base/EnumUtils.h"
#include "model/chess/ChessHelpers.h"

namespace bgg
{

  enum class ChessTextureCell : unsigned
  {
    WHITE_SQUARE,
    BLACK_SQUARE,
    WHITE_KING,
    WHITE_QUEEN,
    WHITE_ROOK,
    WHITE_BISHOP,
    WHITE_KNIGHT,
    WHITE_PAWN,
    BLACK_KING,
    BLACK_QUEEN,
    BLACK_ROOK,
    BLACK_BISHOP,
    BLACK_KNIGHT,
    BLACK_PAWN,
    CHOICE_HIGHLIGHTER,
    LAST_MOVE_HIGHLIGHTER,
    QUIET_MOVE_HIGHLIGHTER,
    CAPTURE_MOVE_HIGHLIGHTER,
    CHECK_HIGHLIGHTER,
    CHECKMATE_HIGHLIGHTER
  };
} // namespace bgg

namespace utils
{

  constexpr const char *STRINGS[]{
      "WhiteSquare",
      "BlackSquare",
      "WhiteKing",
      "WhiteQueen",
      "WhiteRook",
      "WhiteBishop",
      "WhiteKnight",
      "WhitePawn",
      "BlackKing",
      "BlackQueen",
      "BlackRook",
      "BlackBishop",
      "BlackKnight",
      "BlackPawn",
      "ChoiceHighlighter",
      "LastMoveHighlighter",
      "QuietMoveHighlighter",
      "CaptureMoveHighlighter",
      "CheckHighlighter",
      "CheckmateHighlighter"};

  inline auto toString(bgg::ChessTextureCell value) noexcept -> std::string
  {
    return STRINGS[int(value)];
  }

  inline auto getChessTextureCellIndex(
      bgg::EntityType const &pieceType, bgg::Side const &pieceColor) noexcept -> bgg::ChessTextureCell
  {
    if (bgg::chess::WHITE == pieceColor)
    {
      if (bgg::chess::KING == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_KING;
      }

      if (bgg::chess::QUEEN == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_QUEEN;
      }

      if (bgg::chess::ROOK == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_ROOK;
      }

      if (bgg::chess::BISHOP == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_BISHOP;
      }

      if (bgg::chess::KNIGHT == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_KNIGHT;
      }

      if (bgg::chess::PAWN == pieceType)
      {
        return bgg::ChessTextureCell::WHITE_PAWN;
      }
    }
    else if (bgg::chess::BLACK == pieceColor)
    {
      if (bgg::chess::KING == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_KING;
      }

      if (bgg::chess::QUEEN == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_QUEEN;
      }

      if (bgg::chess::ROOK == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_ROOK;
      }

      if (bgg::chess::BISHOP == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_BISHOP;
      }

      if (bgg::chess::KNIGHT == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_KNIGHT;
      }

      if (bgg::chess::PAWN == pieceType)
      {
        return bgg::ChessTextureCell::BLACK_PAWN;
      }
    }

    BOOST_ASSERT_MSG(false, "Invalid chess color or chess type");
    return bgg::ChessTextureCell::WHITE_SQUARE;
  };

} // namespace utils
