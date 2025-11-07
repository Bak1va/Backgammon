#include "Board.h"
#include <algorithm>

Board::Board() {
    // Initialize all columns to empty
    for (int i = 0; i < 24; ++i) {
        m_columns[i] = Column(0, NONE);
    }

    // Initialize bar and borne off counts
    m_barCount[0] = 0;
    m_barCount[1] = 0;
    m_borneOffCount[0] = 0;
    m_borneOffCount[1] = 0;

    // Set up initial board position
    // WHITE pieces (player 0)
    m_columns[0] = Column(2, WHITE);
    m_columns[11] = Column(5, WHITE);
    m_columns[16] = Column(3, WHITE);
    m_columns[18] = Column(5, WHITE);

    // BLACK pieces (player 1)
    m_columns[5] = Column(5, BLACK);
    m_columns[7] = Column(3, BLACK);
    m_columns[12] = Column(5, BLACK);
    m_columns[23] = Column(2, BLACK);
}

Board::~Board() {
}

Column& Board::getColumn(int index) {
    if (index >= 0 && index < 24) {
        return m_columns[index];
    }
    // Return a reference to a static empty column for invalid indices
    static Column emptyColumn(0, NONE);
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
