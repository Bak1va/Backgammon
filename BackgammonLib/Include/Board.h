#pragma once
#include <array>
#include "Column.h"

class Board
{
private:
	std::array<Column, 24> m_columns;
	int m_barCount[2]; // nr de piese pe bara pentru fiecare jucator
	int m_borneOffCount[2]; // nr de piese scoase pentru fiecare jucator

public:
	Board();
	~Board();
	Column& getColumn(int index);
	const Column& getColumn(int index) const;
    int getBarCount(int playerIndex) const;
    int getBorneOffCount(int playerIndex) const;
	void incrementBarCount(int playerIndex);
	void decrementBarCount(int playerIndex);
	void incrementBorneOffCount(int playerIndex);
};

