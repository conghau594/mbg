// ChessBoardItem.h
#pragma once

// #include "base/EnumUtils.h"
#include "model/ChessRule.h"

namespace bgg
{

  class ChessTextureCell
  {
    static constexpr const char *STRINGS[]{
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

  public:
    enum
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

    static auto toString(int value) noexcept -> std::string
    {
      return STRINGS[value];
    }

    static auto getIndex(Piece const &piece) noexcept -> int
    {
      // constexpr const char *PIECE_STRINGS[]{
      //     ChessRule::PieceType::KING,
      //     ChessRule::PieceType::QUEEN,
      //     ChessRule::PieceType::ROOK,
      //     ChessRule::PieceType::BISHOP,
      //     ChessRule::PieceType::KNIGHT,
      //     ChessRule::PieceType::PAWN,
      // };

      if (Color::WHITE == piece.color)
      {
        if (ChessRule::PieceType::KING == piece.type)
        {
          return ChessTextureCell::WHITE_KING;
        }

        if (ChessRule::PieceType::QUEEN == piece.type)
        {
          return ChessTextureCell::WHITE_QUEEN;
        }

        if (ChessRule::PieceType::ROOK == piece.type)
        {
          return ChessTextureCell::WHITE_ROOK;
        }

        if (ChessRule::PieceType::BISHOP == piece.type)
        {
          return ChessTextureCell::WHITE_BISHOP;
        }

        if (ChessRule::PieceType::KNIGHT == piece.type)
        {
          return ChessTextureCell::WHITE_KNIGHT;
        }

        if (ChessRule::PieceType::PAWN == piece.type)
        {
          return ChessTextureCell::WHITE_PAWN;
        }
      }
      else if (Color::BLACK == piece.color)
      {
        if (ChessRule::PieceType::KING == piece.type)
        {
          return ChessTextureCell::BLACK_KING;
        }

        if (ChessRule::PieceType::QUEEN == piece.type)
        {
          return ChessTextureCell::BLACK_QUEEN;
        }

        if (ChessRule::PieceType::ROOK == piece.type)
        {
          return ChessTextureCell::BLACK_ROOK;
        }

        if (ChessRule::PieceType::BISHOP == piece.type)
        {
          return ChessTextureCell::BLACK_BISHOP;
        }

        if (ChessRule::PieceType::KNIGHT == piece.type)
        {
          return ChessTextureCell::BLACK_KNIGHT;
        }

        if (ChessRule::PieceType::PAWN == piece.type)
        {
          return ChessTextureCell::BLACK_PAWN;
        }
      }

      BOOST_ASSERT_MSG(false, "Invalid chess piece");
      return -1;
    };
  };
} // namespace bgg
