#pragma once
#include <array>
#include "Column.h"
class Board
{
	private:
		std::array<Column,24> m_columns;
		int m_barCount[2];//nr de piese pe bara pentru fiecare jucator
		int m_borneOffCount[2];//nr de piese scoase pentru fiecare jucator
	public:
	Board();
	~Board();
	Column& getColumn(int index);
	int getBarCount(int playerIndex);

};

