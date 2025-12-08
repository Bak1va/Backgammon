#include <gtest/gtest.h>
#include "Column.hpp"
#include "Color.hpp"

// =========================
// COLUMN TESTS
// =========================

TEST(ColumnTests, StartsEmpty) {
    Column c;
    EXPECT_EQ(c.getPieceCount(), 0);
    EXPECT_EQ(c.getColor(), NONE);
}

TEST(ColumnTests, AddPieceToEmptyColumn) {
    Column c;
    c.addPiece(WHITE);

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), WHITE);
}

TEST(ColumnTests, AddPieceSameColor) {
    Column c;
    c.addPiece(WHITE);
    c.addPiece(WHITE);

    EXPECT_EQ(c.getPieceCount(), 2);
    EXPECT_EQ(c.getColor(), WHITE);
}

TEST(ColumnTests, AddPieceDifferentColorDoesNotCapture) {
    Column c;
    c.addPiece(WHITE);

    c.addPiece(BLACK);

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), WHITE);
}

TEST(ColumnTests, RemovePiece) {
    Column c;
    c.addPiece(BLACK);
    c.addPiece(BLACK);

    c.removePiece();

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), BLACK);
}

TEST(ColumnTests, RemoveLastPieceResetsColor) {
    Column c;
    c.addPiece(WHITE);

    c.removePiece();

    EXPECT_EQ(c.getPieceCount(), 0);
    EXPECT_EQ(c.getColor(), NONE);
}
