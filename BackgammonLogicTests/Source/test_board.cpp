#include <gtest/gtest.h>
#include "Board.hpp"

// =========================
// BOARD TESTS
// =========================

TEST(BoardTests, InitialSetupIsCorrect) {
    Board b;

    EXPECT_EQ(b.getColumn(5).getPieceCount(), 5);
    EXPECT_EQ(b.getColumn(5).getColor(), BLACK);

    EXPECT_EQ(b.getColumn(7).getPieceCount(), 3);
    EXPECT_EQ(b.getColumn(12).getPieceCount(), 5);
    EXPECT_EQ(b.getColumn(23).getPieceCount(), 2);

    EXPECT_EQ(b.getColumn(0).getPieceCount(), 2);
    EXPECT_EQ(b.getColumn(11).getPieceCount(), 5);
    EXPECT_EQ(b.getColumn(16).getPieceCount(), 3);
    EXPECT_EQ(b.getColumn(18).getPieceCount(), 5);
}

TEST(BoardTests, BarAndBorneOffStartAtZero) {
    Board b;

    EXPECT_EQ(b.getBarCount(0), 0);
    EXPECT_EQ(b.getBarCount(1), 0);

    EXPECT_EQ(b.getBorneOffCount(0), 0);
    EXPECT_EQ(b.getBorneOffCount(1), 0);
}

TEST(BoardTests, IncrementBarCount) {
    Board b;

    b.incrementBarCount(0);
    EXPECT_EQ(b.getBarCount(0), 1);

    b.incrementBarCount(1);
    EXPECT_EQ(b.getBarCount(1), 1);
}

TEST(BoardTests, DecrementBarCount) {
    Board b;

    b.incrementBarCount(0);
    b.decrementBarCount(0);

    EXPECT_EQ(b.getBarCount(0), 0);
}

TEST(BoardTests, IncrementBorneOffCount) {
    Board b;

    b.incrementBorneOffCount(0);
    EXPECT_EQ(b.getBorneOffCount(0), 1);
}
