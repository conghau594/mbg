// ChessRule.cpp

#include "ChessRule.h"

namespace bgg
{
  const std::initializer_list<std::pair<Position, Piece>>
      ChessRule::INITIAL_PLACEMENTS{
          {Position{"e1"}, Piece{KING, Color::WHITE}},
          {Position{"d1"}, Piece{QUEEN, Color::WHITE}},
          {Position{"a1"}, Piece{ROOK, Color::WHITE}},
          {Position{"h1"}, Piece{ROOK, Color::WHITE}},
          {Position{"b1"}, Piece{KNIGHT, Color::WHITE}},
          {Position{"g1"}, Piece{KNIGHT, Color::WHITE}},
          {Position{"c1"}, Piece{BISHOP, Color::WHITE}},
          {Position{"f1"}, Piece{BISHOP, Color::WHITE}},
          {Position{"a2"}, Piece{PAWN, Color::WHITE}},
          {Position{"b2"}, Piece{PAWN, Color::WHITE}},
          {Position{"c2"}, Piece{PAWN, Color::WHITE}},
          {Position{"d2"}, Piece{PAWN, Color::WHITE}},
          {Position{"e2"}, Piece{PAWN, Color::WHITE}},
          {Position{"f2"}, Piece{PAWN, Color::WHITE}},
          {Position{"g2"}, Piece{PAWN, Color::WHITE}},
          {Position{"h2"}, Piece{PAWN, Color::WHITE}},

          {Position{"e8"}, Piece{KING, Color::BLACK}},
          {Position{"d8"}, Piece{QUEEN, Color::BLACK}},
          {Position{"a8"}, Piece{ROOK, Color::BLACK}},
          {Position{"h8"}, Piece{ROOK, Color::BLACK}},
          {Position{"b8"}, Piece{KNIGHT, Color::BLACK}},
          {Position{"g8"}, Piece{KNIGHT, Color::BLACK}},
          {Position{"c8"}, Piece{BISHOP, Color::BLACK}},
          {Position{"f8"}, Piece{BISHOP, Color::BLACK}},
          {Position{"a7"}, Piece{PAWN, Color::BLACK}},
          {Position{"b7"}, Piece{PAWN, Color::BLACK}},
          {Position{"c7"}, Piece{PAWN, Color::BLACK}},
          {Position{"d7"}, Piece{PAWN, Color::BLACK}},
          {Position{"e7"}, Piece{PAWN, Color::BLACK}},
          {Position{"f7"}, Piece{PAWN, Color::BLACK}},
          {Position{"g7"}, Piece{PAWN, Color::BLACK}},
          {Position{"h7"}, Piece{PAWN, Color::BLACK}},
      };

  auto ChessRule::getEnemyColor(std::string const &color) noexcept -> std::string
  {
    return (color == Color::WHITE) ? Color::BLACK : Color::WHITE;
  }

  void ChessRule::commitMoveAction(
      std::map<Position, Piece> &piecePlacements,
      ChessMove::VariantAction const &moveAction) noexcept
  {
    return;
  }

  auto ChessRule::isKingInCheck(
      std::map<Position, Piece> &piecePlacements,
      std::string const &color) noexcept -> bool
  {
    return false;
  }

  auto ChessRule::isPromotionSquare(
      Position const &square, std::string const &color) noexcept -> bool
  {
    char promotionRank = (color == Color::WHITE) ? '8' : '1';
    return square[1] == promotionRank;
  }

  auto ChessRule::getCastlingRookMove(
      Position const &kingTargetSquare,
      std::string const &color) noexcept -> std::pair<Position, Position>
  {
    if (color == Color::WHITE)
    {
      if (kingTargetSquare == Position{"g1"})
      {
        return {Position{"h1"}, Position{"f1"}}; // King-side castling
      }

      if (kingTargetSquare == Position{"c1"})
      {
        return {Position{"a1"}, Position{"d1"}}; // Queen-side castling
      }
    }
    else if (color == Color::BLACK)
    {
      if (kingTargetSquare == Position{"g8"})
      {
        return {Position{"h8"}, Position{"f8"}}; // King-side castling
      }

      if (kingTargetSquare == Position{"c8"})
      {
        return {Position{"a8"}, Position{"d8"}}; // Queen-side castling
      }
    }

    BOOST_ASSERT_MSG(false, "Invalid castling king target square");
  }
} // namespace bgg
