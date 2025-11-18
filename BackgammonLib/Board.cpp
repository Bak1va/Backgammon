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
    // Presupunem că White mută de la 0 -> 23 (sens trigonometric invers sau direct, depinde de interpretare)
    // Cadran Stânga-Sus (0-5), Dreapta-Sus (6-11)
    // Cadran Dreapta-Jos (12-17), Stânga-Jos (18-23)

    // WHITE pieces (Player 0)
    m_columns[0] = Column(2, WHITE);   // 2 piese la start (depărtare maximă)
    m_columns[11] = Column(5, WHITE);  // 5 piese la mijloc-dreapta sus
    m_columns[16] = Column(3, WHITE);  // 3 piese atac (dreapta jos)
    m_columns[23] = Column(5, WHITE);  // 5 piese "acasă" lângă bară (stânga jos) -> Corectat față de v1

    // BLACK pieces (Player 1)
    m_columns[5] = Column(5, BLACK);   // 5 piese adversar (stânga sus lângă bară)
    m_columns[7] = Column(3, BLACK);   // 3 piese adversar (dreapta sus)
    m_columns[12] = Column(5, BLACK);  // 5 piese adversar (dreapta jos lângă bară)
    m_columns[18] = Column(2, BLACK);  // 2 piese adversar în colțul casei tale (stânga jos) -> Corectat față de v1
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