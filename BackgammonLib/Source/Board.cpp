/**
 * @file Board.cpp
 * @brief Implementation of the Board class.
 *
 * The board is initialized with the standard Backgammon starting position:
 * - White pieces: 2 on point 0, 5 on point 11, 3 on point 16, 5 on point 18
 * - Black pieces: 5 on point 5, 3 on point 7, 5 on point 12, 2 on point 23
 */

#include "Board.hpp"

Board::Board() {for (int i = 0; i < 24; ++i) {
        m_columns[i] = Column(0, Color::NONE);
    }

    m_barCount[0] = 0;
    m_barCount[1] = 0;
    m_borneOffCount[0] = 0;
    m_borneOffCount[1] = 0;


    m_columns[5] = Column(5, Color::BLACK);
    m_columns[7] = Column(3, Color::BLACK);
    m_columns[12] = Column(5, Color::BLACK);
    m_columns[23] = Column(2, Color::BLACK);

    m_columns[0] = Column(2, Color::WHITE);
    m_columns[11] = Column(5, Color::WHITE);
    m_columns[16] = Column(3, Color::WHITE);
    m_columns[18] = Column(5, Color::WHITE);
}

Board::~Board() {
}

Column& Board::getColumn(int index) {
    if (index >= 0 && index < 24) {
        return m_columns[index];
    }
    static Column emptyColumn(0, Color::NONE);
    return emptyColumn;
}

const Column& Board::getColumn(int index) const {
    if (index >= 0 && index < 24) {
        return m_columns[index];
    }
    static Column emptyColumn(0, Color::NONE);
    return emptyColumn;
}

int Board::getBarCount(int playerIndex) const {
    if (playerIndex >= 0 && playerIndex < 2) {
        return m_barCount[playerIndex];
    }
    return 0;
}

int Board::getBorneOffCount(int playerIndex) const {
    if (playerIndex >= 0 && playerIndex < 2) {
        return m_borneOffCount[playerIndex];
    }
    return 0;
}

void Board::incrementBarCount(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < 2) {
        m_barCount[playerIndex]++;
    }
}

void Board::decrementBarCount(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < 2 && m_barCount[playerIndex] > 0) {
        m_barCount[playerIndex]--;
    }
}

void Board::incrementBorneOffCount(int playerIndex) {
    if (playerIndex >= 0 && playerIndex < 2) {
        m_borneOffCount[playerIndex]++;
    }
}