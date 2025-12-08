#include <gtest/gtest.h>
#include "Game.hpp"

// =============================
// BEAR OFF TESTS
// =============================

TEST(GameBearOffTests, CannotBearOffAtStart) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult rWhite = g.makeMove(18, 24);
    EXPECT_EQ(rWhite, MoveResult::CANNOT_BEAR_OFF);
}