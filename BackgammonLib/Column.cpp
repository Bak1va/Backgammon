#include "Column.h"


Column::Column(int pieceCount, Color color)
	: m_pieceCount(pieceCount), m_color(color) {
}
Column::~Column() {
}
Color Column::getColor() {
	return m_color;
}
int Column::getPieceCount() {
	return m_pieceCount;
}

void Column::removePiece() {
	if (m_pieceCount > 0) {
		m_pieceCount--;
		if (m_pieceCount == 0) {
			m_color = NONE;
		}
	}
}
void Column::setColor(Color color) {
	m_color = color;
}
void Column::addPiece(Color color) {
    if (m_color == NONE || (m_pieceCount == 1 && m_color != color)) {
        m_color = color;
        m_pieceCount = 1;
    }
    else if (m_color == color) {
        m_pieceCount++;
    }
}