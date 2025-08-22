#include <gtest/gtest.h>

#include "model/chess/ChessHelpers.h"
#include "model/chess/ChessRule.h"

namespace bgg
{
  // class ChessRuleTest : public testing::Test
  // {
  // protected:
  //   ChessRule chessRule_;
  //   ChessRuleTest()
  //       : chessRule_(chess::STANDARD_PIECE_PLACEMENTS)
  //   {
  //     // Initialize chessRule_ with some initial placements
  //     // This can be customized based on the test requirements
  //   }
  // };

  TEST(ChessRuleTest, initialChessRule)
  {
    ChessRule chessRule(chess::STANDARD_PIECE_PLACEMENTS);
    EXPECT_EQ(chessRule.getMoveCount(), 0);
    EXPECT_EQ(chessRule.getLastMove(), std::nullopt);
    // TODO: Add more assertions to check the initial state of the chess rule
  }

  TEST(ChessRuleTest, collectPieces)
  // Scenario: chess rule with standard piece placements
  // Expected: 16 pieces for each side
  {
    ChessRule chessRule(chess::STANDARD_PIECE_PLACEMENTS);
    EXPECT_EQ(chessRule.collectPieces(chess::WHITE).size(), 16);
    EXPECT_EQ(chessRule.collectPieces(chess::BLACK).size(), 16);

    // TODO: Add assertions to check the actual pieces collected
  }
} // namespace bgg
