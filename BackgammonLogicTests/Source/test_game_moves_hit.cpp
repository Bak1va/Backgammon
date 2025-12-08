#include <gtest/gtest.h>
#include "Game.hpp"

// =============================
// HIT LOGIC TESTS
// =============================

TEST(GameHitTests, CannotHitIfSameColor) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult r = g.makeMove(11, 11);

    EXPECT_NE(r, MoveResult::SUCCESS);
}
