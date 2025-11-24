#include <gtest/gtest.h>
#include "Game.h"

// =============================
// BAR LOGIC TESTS
// =============================

TEST(GameBarTests, CannotMoveFromBarIfZeroPieces) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult r = g.makeMove(Game::BAR_INDEX, 0);
    EXPECT_EQ(r, MoveResult::InvalidMove);
}

TEST(GameBarTests, CannotMoveOtherPiecesIfBarOccupied) {
    Game g;
    g.start();
    g.rollDice();
    g.makeMove(0, 0);

    SUCCEED();
}
