#pragma once
#include "Color.h"

class Column {
private:
	int m_pieceCount;
	Color m_color;

public:
	Column();
	Column(int pieceCount, Color color);
	~Column();

	void addPiece(Color color);
	void removePiece();
    int getPieceCount() const;
    Color getColor() const;
	void setColor(Color color);

};
