#pragma once
#include <array>
#include "Column.hpp"

class Board {
public:
    Board();
    ~Board();
    Column &getColumn(int index);
    const Column &getColumn(int index) const;
    int getBarCount(int playerIndex) const;
    int getBorneOffCount(int playerIndex) const;
    void incrementBarCount(int playerIndex);
    void decrementBarCount(int playerIndex);
    void incrementBorneOffCount(int playerIndex);

private:
    std::array<Column, 24> m_columns;
    int m_barCount[2];
    int m_borneOffCount[2];
};
