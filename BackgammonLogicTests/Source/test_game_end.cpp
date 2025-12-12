#include <gtest/gtest.h>
#include "Game.hpp"

// =============================
// END GAME TESTS
// =============================

TEST(GameEndTests, GameNotFinishedAtStart) {
    Game g;

    EXPECT_EQ(g.getPhase(), GamePhase::NOT_STARTED);

    g.start();
    EXPECT_EQ(g.getPhase(), GamePhase::IN_PROGRESS);
}

TEST(GameEndTests, GameNotFinishedAfterOneMove) {
    Game g;
    g.start();
    g.rollDice();

    g.makeMove(0, 0);

    EXPECT_NE(g.getPhase(), GamePhase::FINISHED);
}
