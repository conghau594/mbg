// ChessBoardItem.h
#pragma once

// #include "base/EnumUtils.h"
#include "model/chess/ChessBoardState.h"

namespace bgg
{

  enum class ChessTextureCell : int
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
      bgg::Piece const &piece) noexcept -> bgg::ChessTextureCell
  {
    if (bgg::Color::WHITE == piece.color)
    {
      if (bgg::ChessRule::KING == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_KING;
      }

      if (bgg::ChessRule::QUEEN == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_QUEEN;
      }

      if (bgg::ChessRule::ROOK == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_ROOK;
      }

      if (bgg::ChessRule::BISHOP == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_BISHOP;
      }

      if (bgg::ChessRule::KNIGHT == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_KNIGHT;
      }

      if (bgg::ChessRule::PAWN == piece.type)
      {
        return bgg::ChessTextureCell::WHITE_PAWN;
      }
    }
    else if (bgg::Color::BLACK == piece.color)
    {
      if (bgg::ChessRule::KING == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_KING;
      }

      if (bgg::ChessRule::QUEEN == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_QUEEN;
      }

      if (bgg::ChessRule::ROOK == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_ROOK;
      }

      if (bgg::ChessRule::BISHOP == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_BISHOP;
      }

      if (bgg::ChessRule::KNIGHT == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_KNIGHT;
      }

      if (bgg::ChessRule::PAWN == piece.type)
      {
        return bgg::ChessTextureCell::BLACK_PAWN;
      }
    }

    BOOST_ASSERT_MSG(false, "Invalid chess piece");
    return bgg::ChessTextureCell(-1);
  };

} // namespace utils
