#include <gtest/gtest.h>

#include "model/chess/ChessHelpers.h"
#include "model/chess/ChessRule.h"
#include "model/chess/ChessPiece.h"

namespace bgg
{
  TEST(ChessPieceTest, King_canMoveTo)
  {
    Position initialPosition{"a1"};
    ChessRule chessRule({{chess::KING, chess::WHITE, initialPosition},
                         {chess::KNIGHT, chess::WHITE, Position{"a2"}},
                         {chess::KNIGHT, chess::BLACK, Position{"b2"}}});

    auto king = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(king != nullptr);

    // TC01: INPUT: right adjacent empty square
    EXPECT_TRUE(king->canMoveTo(Position{"b1"}));

    // TC02: INPUT: top right adjacent occupied square
    EXPECT_TRUE(king->canMoveTo(Position{"b2"}));

    EXPECT_FALSE(king->canMoveTo(Position{"a0"}));
    EXPECT_FALSE(king->canMoveTo(Position{"a1"}))
        << "King can not move to its own position";

    EXPECT_FALSE(king->canMoveTo(Position{"a2"}));
    EXPECT_FALSE(king->canMoveTo(Position{"a3"}));
  }

  TEST(ChessPieceTest, Queen_canMoveTo)
  {
    Position initialPosition{"a1"};
    ChessRule chessRule({{chess::QUEEN, chess::WHITE, initialPosition},
                         {chess::KNIGHT, chess::WHITE, Position{"a4"}},
                         {chess::KNIGHT, chess::BLACK, Position{"c3"}}});

    auto queen = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(queen != nullptr);

    EXPECT_TRUE(queen->canMoveTo(Position{"e1"}));
    EXPECT_TRUE(queen->canMoveTo(Position{"c3"}));

    EXPECT_FALSE(queen->canMoveTo(initialPosition))
        << "Queen can not move to its own position";
    EXPECT_FALSE(queen->canMoveTo(Position{"a0"}));
    EXPECT_FALSE(queen->canMoveTo(Position{"a4"}));
    EXPECT_FALSE(queen->canMoveTo(Position{"c2"}));
  }

  TEST(ChessPieceTest, Rook_canMoveTo)
  {
    Position initialPosition{"a1"};
    ChessRule chessRule({{chess::ROOK, chess::WHITE, initialPosition},
                         {chess::KNIGHT, chess::WHITE, Position{"a4"}},
                         {chess::KNIGHT, chess::BLACK, Position{"g1"}}});

    auto rook = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(rook != nullptr);

    EXPECT_TRUE(rook->canMoveTo(Position{"a3"}));
    EXPECT_TRUE(rook->canMoveTo(Position{"b1"}));
    EXPECT_TRUE(rook->canMoveTo(Position{"g1"}));
    EXPECT_FALSE(rook->canMoveTo(Position{"h1"}))
        << "Rook can not move to h1 due to the knight on g1";

    EXPECT_FALSE(rook->canMoveTo(initialPosition))
        << "Rook can not move to its own position";
    EXPECT_FALSE(rook->canMoveTo(Position{"a0"}));
    EXPECT_FALSE(rook->canMoveTo(Position{"a4"}));
    EXPECT_FALSE(rook->canMoveTo(Position{"c2"}));
  }

  TEST(ChessPieceTest, Bishop_canMoveTo)
  {
    Position initialPosition{"b2"};
    ChessRule chessRule({{chess::BISHOP, chess::WHITE, initialPosition},
                         {chess::KNIGHT, chess::WHITE, Position{"d4"}},
                         {chess::KNIGHT, chess::BLACK, Position{"a3"}}});

    auto bishop = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(bishop != nullptr);

    EXPECT_TRUE(bishop->canMoveTo(Position{"a1"}));
    EXPECT_TRUE(bishop->canMoveTo(Position{"c3"}));
    EXPECT_TRUE(bishop->canMoveTo(Position{"a3"}));
    EXPECT_TRUE(bishop->canMoveTo(Position{"c1"}));
    EXPECT_FALSE(bishop->canMoveTo(Position{"f6"}))
        << "Bishop can not move to f6 due to the knight on e5";

    EXPECT_FALSE(bishop->canMoveTo(initialPosition))
        << "Bishop can not move to its own position";
    EXPECT_FALSE(bishop->canMoveTo(Position{"d0"}));
    EXPECT_FALSE(bishop->canMoveTo(Position{"b1"}));
    EXPECT_FALSE(bishop->canMoveTo(Position{"d2"}));
  }

  TEST(ChessPieceTest, Knight_canMoveTo)
  {
    Position initialPosition{"b2"};
    ChessRule chessRule({{chess::KNIGHT, chess::WHITE, initialPosition},
                         {chess::BISHOP, chess::WHITE, Position{"d1"}},
                         {chess::BISHOP, chess::BLACK, Position{"d3"}}});

    auto knight = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(knight != nullptr);

    EXPECT_TRUE(knight->canMoveTo(Position{"c4"}));
    EXPECT_TRUE(knight->canMoveTo(Position{"d3"}));
    EXPECT_TRUE(knight->canMoveTo(Position{"a4"}));

    EXPECT_FALSE(knight->canMoveTo(initialPosition))
        << "Knight can not move to its own position";
    EXPECT_FALSE(knight->canMoveTo(Position{"c0"}));
    EXPECT_FALSE(knight->canMoveTo(Position{"d1"}))
        << "Knight can not move to d1 due to the ally bishop on d1";

    EXPECT_FALSE(knight->canMoveTo(Position{"f4"}));
  }

  TEST(ChessPieceTest, WhitePawn_canMoveTo)
  {
    Position initialPosition{"a2"};
    ChessRule chessRule({{chess::PAWN, chess::WHITE, initialPosition},
                         {chess::BISHOP, chess::BLACK, Position{"b3"}}});

    auto pawn = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(pawn != nullptr);

    EXPECT_TRUE(pawn->canMoveTo(Position{"b3"}));
    EXPECT_TRUE(pawn->canMoveTo(Position{"a3"}));
    EXPECT_TRUE(pawn->canMoveTo(Position{"a4"}));

    EXPECT_FALSE(pawn->canMoveTo(initialPosition))
        << "Pawn can not move to its own position";
    EXPECT_FALSE(pawn->canMoveTo(Position{"a1"}));
    EXPECT_FALSE(pawn->canMoveTo(Position{"a5"}));
  }

  TEST(ChessPieceTest, BlackPawn_canMoveTo)
  {
    Position initialPosition{"b7"};
    ChessRule chessRule({{chess::PAWN, chess::BLACK, initialPosition},
                         {chess::BISHOP, chess::WHITE, Position{"a6"}},
                         {chess::BISHOP, chess::WHITE, Position{"b6"}}});

    auto pawn = chessRule.findPiece(initialPosition);
    ASSERT_TRUE(pawn != nullptr);

    EXPECT_TRUE(pawn->canMoveTo(Position{"a6"}));

    EXPECT_FALSE(pawn->canMoveTo(initialPosition))
        << "Pawn can not move to its own position";
    EXPECT_FALSE(pawn->canMoveTo(Position{"b8"}));
    EXPECT_FALSE(pawn->canMoveTo(Position{"b6"}));
    EXPECT_FALSE(pawn->canMoveTo(Position{"b5"}))
        << "Black pawn can not move to b5 due to the bishop on b6";
    EXPECT_FALSE(pawn->canMoveTo(Position{"c6"}));
    EXPECT_FALSE(pawn->canMoveTo(Position{"b4"}));
  }

  TEST(ChessPieceTest, King_canCastle)
  {
    ChessRule chessRule({{chess::KING, chess::WHITE, Position{"e1"}},
                         {chess::ROOK, chess::WHITE, Position{"h1"}},
                         {chess::ROOK, chess::WHITE, Position{"a1"}},
                         {chess::KING, chess::BLACK, Position{"e8"}},
                         {chess::ROOK, chess::BLACK, Position{"h8"}},
                         {chess::ROOK, chess::BLACK, Position{"a8"}}});

    auto whiteKing = chessRule.findPiece(Position{"e1"});
    auto blackKing = chessRule.findPiece(Position{"e8"});

    ASSERT_TRUE(whiteKing != nullptr);
    ASSERT_TRUE(blackKing != nullptr);

    EXPECT_TRUE(whiteKing->canMoveTo(Position{"g1"}));
    EXPECT_TRUE(whiteKing->canMoveTo(Position{"c1"}));

    EXPECT_TRUE(blackKing->canMoveTo(Position{"g8"}));
    EXPECT_TRUE(blackKing->canMoveTo(Position{"c8"}));
  }

  TEST(ChessPieceTest, King_canNotCastle)
  {
    ChessRule chessRule({{chess::KING, chess::WHITE, Position{"e1"}},
                         {chess::ROOK, chess::WHITE, Position{"h1"}},
                         {chess::ROOK, chess::WHITE, Position{"a1"}},
                         {chess::KING, chess::BLACK, Position{"e8"}},
                         {chess::KNIGHT, chess::BLACK, Position{"g8"}},
                         {chess::ROOK, chess::BLACK, Position{"h8"}},
                         {chess::ROOK, chess::BLACK, Position{"a8"}}});

    auto whiteKing = chessRule.findPiece(Position{"e1"});
    auto blackKing = chessRule.findPiece(Position{"e8"});
    auto blackRookA = chessRule.findPiece(Position{"a8"});

    ASSERT_TRUE(whiteKing != nullptr);
    ASSERT_TRUE(blackKing != nullptr);
    ASSERT_TRUE(blackRookA != nullptr);

    whiteKing->setMoveInfo(Position{"e1"}, 2);  // Simulate a move
    blackRookA->setMoveInfo(Position{"a8"}, 2); // Simulate a move

    EXPECT_FALSE(whiteKing->canMoveTo(Position{"g1"}))
        << "White king can not castle because it was moved";
    EXPECT_FALSE(whiteKing->canMoveTo(Position{"c1"}))
        << "White king can not castle because it was moved";

    EXPECT_FALSE(blackKing->canMoveTo(Position{"c8"}))
        << "Black king can not castle to c8 because the rook on a8 was moved";
    EXPECT_FALSE(blackKing->canMoveTo(Position{"g8"}))
        << "Black king can not castle to g8 due to the knight on g8";
  }

  TEST(ChessPieceTest, Pawn_canCaptureEnPassant)
  {
    ChessRule chessRule({{chess::PAWN, chess::WHITE, Position{"e5"}},
                         {chess::PAWN, chess::BLACK, Position{"f7"}},
                         {chess::PAWN, chess::BLACK, Position{"d7"}},
                         {chess::KING, chess::WHITE, Position{"e1"}}});

    auto whitePawn = chessRule.findPiece(Position{"e5"});
    whitePawn->setMoveInfo(Position{"e5"}, 2); // Simulate a move
    ASSERT_TRUE(whitePawn != nullptr);

    // Simulate the black pawn on f7 moving two squares forward
    ChessMove::Normal blackPawnFMove{
        -1,
        chess::BLACK,
        Position{"f7"},
        Position{"f5"},
        chess::PAWN,
        std::nullopt,
        Position{},
        Side::Status::UNDEFINED};

    chessRule.commitMove(blackPawnFMove);

    EXPECT_TRUE(whitePawn->canMoveTo(Position{"f6"}))
        << "White pawn can capture black pawn en passant";

    // Simulate a certain move of the white
    chessRule.commitMove(ChessMove::generateMinimalNormalMove(
        ChessMove{chess::WHITE, Position{"e1"}, Position{"f1"}}, false));

    // Simulate the black pawn on d7 moving two squares forward
    ChessMove::Normal blackPawnDMove{
        -1,
        chess::BLACK,
        Position{"d7"},
        Position{"d5"},
        chess::PAWN,
        std::nullopt,
        Position{},
        Side::Status::UNDEFINED};
    chessRule.commitMove(blackPawnDMove);

    EXPECT_TRUE(whitePawn->canMoveTo(Position{"d6"}))
        << "White pawn can capture black pawn en passant";
    EXPECT_FALSE(whitePawn->canMoveTo(Position{"f6"}))
        << "White pawn can not capture black pawn on f6 anymore";
  }

  TEST(ChessPieceTest, Pawn_promote)
  {
    ChessRule chessRule({{chess::PAWN, chess::WHITE, Position{"e7"}},
                         {chess::PAWN, chess::WHITE, Position{"d6"}}});

    auto promotablePawn = chessRule.findPiece(Position{"e7"});
    auto nonPromotablePawn = chessRule.findPiece(Position{"d6"});
    ASSERT_TRUE(promotablePawn != nullptr);
    ASSERT_TRUE(nonPromotablePawn != nullptr);

    EXPECT_TRUE(promotablePawn->canMoveTo(Position{"e8"}))
        << "White pawn can move to e8 for promotion";

    EXPECT_FALSE(nonPromotablePawn->canMoveTo(Position{"e8"}))
        << "White pawn can not move to e8 for promotion";
  }
} // namespace bgg
