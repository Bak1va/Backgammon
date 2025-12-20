/**
 * @file Column.hpp
 * @brief Defines the Column class representing a single point on the Backgammon board.
 */

#pragma once
#include "Color.hpp"

/**
 * @class Column
 * @brief Represents a single column (point) on the Backgammon board.
 *
 * Each column can hold zero or more pieces of a single color.
 * The column tracks the number of pieces and their color.
 */
class Column {
public:
    /**
     * @brief Default constructor creating an empty column.
     */
    Column();

    /**
     * @brief Constructor creating a column with specified pieces.
     * @param pieceCount Initial number of pieces on this column
     * @param color Color of the pieces on this column
     */
    Column(int pieceCount, Color color);

    /**
     * @brief Destructor for the Column.
     */
    ~Column();

    /**
     * @brief Adds a piece of the specified color to the column.
     * @param color Color of the piece to add
     *
     * If the column is empty, the color is set to the specified color.
     * If the column already has pieces of the same color, the count is incremented.
     */
    void addPiece(Color color);

    /**
     * @brief Removes one piece from the column.
     *
     * If the column becomes empty after removal, the color is reset to NONE.
     */
    void removePiece();

    /**
     * @brief Gets the number of pieces on this column.
     * @return The number of pieces
     */
    int getPieceCount() const;

    /**
     * @brief Gets the color of pieces on this column.
     * @return The color of the pieces (NONE if empty)
     */
    Color getColor() const;

    /**
     * @brief Sets the color of this column.
     * @param color The new color for the column
     */
    void setColor(Color color);

private:
    int m_pieceCount;   ///< Number of pieces on this column
    Color m_color;      ///< Color of pieces on this column
};
