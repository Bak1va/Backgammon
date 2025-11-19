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

    // --- SETUP STANDARD BACKGAMMON ---
    // Cadran Stânga-Sus (0-5), Dreapta-Sus (6-11)
    // Cadran Dreapta-Jos (18-23), Stânga-Jos (12-17)

    // WHITE pieces (Player 0)
    m_columns[0] = Column(5, WHITE); // 5 albe, sus-stanga colț (13-point)
    m_columns[22] = Column(3, WHITE); // 3 albe, jos-stanga langa bară (8-point)
    m_columns[12] = Column(5, WHITE); // 5 albe, jos-dreapta langa bară (6-point)
    m_columns[11] = Column(2, WHITE); // 2 albe, sus-dreapta colț (24-point)

    // BLACK pieces (Player 1)
    m_columns[18] = Column(5, BLACK); // 5 negre, jos-stanga colț (13-point)
    m_columns[4] = Column(3, BLACK); // 3 negre, sus-stanga langa bară (8-point)
    m_columns[6] = Column(5, BLACK); // 5 negre, sus-dreapta langa bară (6-point)
    m_columns[17] = Column(2, BLACK); // 2 negre, jos-dreapta colț (24-point)
}

Board::~Board() {
}

Column& Board::getColumn(int index) {
    if (index >= 0 && index < 24) {
        return m_columns[index];
    }
    static Column emptyColumn(0, NONE);
    return emptyColumn;
}

const Column& Board::getColumn(int index) const {
    if (index >= 0 && index < 24) {
        return m_columns[index];
    }
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