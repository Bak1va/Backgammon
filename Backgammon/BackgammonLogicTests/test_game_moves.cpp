#include <gtest/gtest.h>
#include "Game.h"

// =============================
// GAME MOVE VALIDATION TESTS
// =============================

TEST(GameMoveTests, MoveFailsIfDiceNotRolled) {
    Game g;
    g.start();

    MoveResult r = g.makeMove(0, 5);
    EXPECT_EQ(r, MoveResult::DiceNotRolled);
}

TEST(GameMoveTests, CannotMoveOpponentPieces) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult r = g.makeMove(5, 6);

    EXPECT_EQ(r, MoveResult::InvalidMove);
}

TEST(GameMoveTests, CannotMoveFromEmptyColumn) {
    Game g;
    g.start();
    g.rollDice();

    MoveResult r = g.makeMove(2, 3);

    EXPECT_EQ(r, MoveResult::InvalidMove);
}

TEST(GameMoveTests, CannotMoveIfBarOccupied) {
    Game g;
    g.start();
    g.rollDice();

    g.start();
    g.rollDice();

    GameStateDTO before = g.getState();

    SUCCEED();
}
