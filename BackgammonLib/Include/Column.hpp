#pragma once
#include "Color.hpp"

class Column {
public:
    Column();
    Column(int pieceCount, Color color);
    ~Column();

    void addPiece(Color color);
    void removePiece();
    int getPieceCount() const;
    Color getColor() const;
    void setColor(Color color);

private:
    int m_pieceCount;
    Color m_color;
};
