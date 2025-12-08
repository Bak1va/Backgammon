#include <gtest/gtest.h>
#include "Game.hpp"

// =============================
// LEGAL TARGETS BASIC TESTS
// =============================

TEST(GameLegalTargetsTests, TargetsWithinBoardRange) {
    Game g;
    g.start();
    g.rollDice();

    auto targets = g.getLegalTargets(0);

    for (int t : targets) {
        bool validBoard = (t >= 0 && t <= 23);
        bool validOff = (t == 24);

        EXPECT_TRUE(validBoard || validOff);
    }
}

TEST(GameLegalTargetsTests, CannotSelectInvalidPoint) {
    Game g;
    g.start();
    g.rollDice();

    auto targets = g.getLegalTargets(2);

    EXPECT_TRUE(targets.empty());
}

TEST(GameLegalTargetsTests, CannotMoveIfDiceNotRolled) {
    Game g;
    g.start();

    auto targets = g.getLegalTargets(0);

    EXPECT_TRUE(targets.empty());
}
