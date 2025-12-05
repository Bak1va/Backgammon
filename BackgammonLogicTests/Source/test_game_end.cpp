#include <gtest/gtest.h>
#include "Game.h"

// =============================
// END GAME TESTS
// =============================

TEST(GameEndTests, GameNotFinishedAtStart) {
    Game g;

    EXPECT_EQ(g.getPhase(), GamePhase::NotStarted);

    g.start();
    EXPECT_EQ(g.getPhase(), GamePhase::InProgress);
}

TEST(GameEndTests, GameNotFinishedAfterOneMove) {
    Game g;
    g.start();
    g.rollDice();

    g.makeMove(0, 0);

    EXPECT_NE(g.getPhase(), GamePhase::Finished);
}
