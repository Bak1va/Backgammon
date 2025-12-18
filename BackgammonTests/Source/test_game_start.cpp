#include <gtest/gtest.h>
#include "Game.hpp"

// =============================
// GAME START & STATE TESTS
// =============================

TEST(GameStartTests, PhaseIsNotStartedByDefault) {
    Game g;
    EXPECT_EQ(g.getPhase(), GamePhase::NOT_STARTED);
}

TEST(GameStartTests, StartInitializesBoardCorrectly) {
    Game g;
    g.start();

    GameStateDTO s = g.getState();

    EXPECT_EQ(g.getPhase(), GamePhase::IN_PROGRESS);
    EXPECT_EQ(s.currentPlayer, WHITE);

    EXPECT_EQ(s.pieceCounts[0], 2);
    EXPECT_EQ(s.colors[0], WHITE);

    EXPECT_EQ(s.pieceCounts[11], 5);
    EXPECT_EQ(s.colors[11], WHITE);

    EXPECT_EQ(s.pieceCounts[5], 5);
    EXPECT_EQ(s.colors[5], BLACK);
}

TEST(GameStartTests, DiceAreZeroAfterStart) {
    Game g;
    g.start();

    GameStateDTO s = g.getState();

    EXPECT_EQ(s.dice1, 0);
    EXPECT_EQ(s.dice2, 0);
}

TEST(GameStartTests, BarAndBorneOffAreZeroAfterStart) {
    Game g;
    g.start();

    GameStateDTO s = g.getState();

    EXPECT_EQ(s.barWhite, 0);
    EXPECT_EQ(s.barBlack, 0);

    EXPECT_EQ(s.borneOffWhite, 0);
    EXPECT_EQ(s.borneOffBlack, 0);
}
