#include <gtest/gtest.h>
#include "Game.h"

// =============================
// BEAR OFF TESTS
// =============================

TEST(GameBearOffTests, CannotBearOffAtStart) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult rWhite = g.makeMove(18, 24);
    EXPECT_EQ(rWhite, MoveResult::CannotBearOff);
}