#include <gtest/gtest.h>
#include "Column.hpp"
#include "Color.hpp"

// =========================
// COLUMN TESTS
// =========================

TEST(ColumnTests, StartsEmpty) {
    Column c;
    EXPECT_EQ(c.getPieceCount(), 0);
    EXPECT_EQ(c.getColor(), Color::NONE);
}

TEST(ColumnTests, AddPieceToEmptyColumn) {
    Column c;
    c.addPiece(Color::WHITE);

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), Color::WHITE);
}

TEST(ColumnTests, AddPieceSameColor) {
    Column c;
    c.addPiece(Color::WHITE);
    c.addPiece(Color::WHITE);

    EXPECT_EQ(c.getPieceCount(), 2);
    EXPECT_EQ(c.getColor(), Color::WHITE);
}

TEST(ColumnTests, AddPieceDifferentColorDoesNotCapture) {
    Column c;
    c.addPiece(Color::WHITE);

    c.addPiece(Color::BLACK);

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), Color::WHITE);
}

TEST(ColumnTests, RemovePiece) {
    Column c;
    c.addPiece(Color::BLACK);
    c.addPiece(Color::BLACK);

    c.removePiece();

    EXPECT_EQ(c.getPieceCount(), 1);
    EXPECT_EQ(c.getColor(), Color::BLACK);
}

TEST(ColumnTests, RemoveLastPieceResetsColor) {
    Column c;
    c.addPiece(Color::WHITE);

    c.removePiece();

    EXPECT_EQ(c.getPieceCount(), 0);
    EXPECT_EQ(c.getColor(), NONE);
}
