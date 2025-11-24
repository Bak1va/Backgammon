#include <gtest/gtest.h>
#include "Game.h"

// =============================
// HIT LOGIC TESTS
// =============================

TEST(GameHitTests, CannotHitIfSameColor) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult r = g.makeMove(11, 11);

    EXPECT_NE(r, MoveResult::Success);
}
