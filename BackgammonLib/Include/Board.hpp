/**
 * @file Board.hpp
 * @brief Defines the Board class representing the Backgammon game board.
 */

#pragma once
#include <array>
#include "Column.hpp"

/**
 * @class Board
 * @brief Represents the Backgammon game board with 24 columns, bars, and bear-off areas.
 *
 * The board maintains the state of all 24 columns (points), the bar for each player,
 * and the bear-off count for each player. The board is initialized with the standard
 * Backgammon starting position.
 */
class Board {
public:
    /**
     * @brief Constructor creating a board with the standard starting position.
     */
    Board();

    /**
     * @brief Destructor for the Board.
     */
    ~Board();

    /**
     * @brief Gets a reference to the column at the specified index.
     * @param index Column index (0-23)
     * @return Reference to the column
     */
    Column &getColumn(int index);

    /**
     * @brief Gets a const reference to the column at the specified index.
     * @param index Column index (0-23)
     * @return Const reference to the column
     */
    const Column &getColumn(int index) const;

    /**
     * @brief Gets the number of pieces on the bar for a player.
     * @param playerIndex Player index (0 for WHITE, 1 for BLACK)
     * @return Number of pieces on the bar
     */
    int getBarCount(int playerIndex) const;

    /**
     * @brief Gets the number of pieces borne off for a player.
     * @param playerIndex Player index (0 for WHITE, 1 for BLACK)
     * @return Number of pieces borne off
     */
    int getBorneOffCount(int playerIndex) const;

    /**
     * @brief Increments the bar count for a player.
     * @param playerIndex Player index (0 for WHITE, 1 for BLACK)
     */
    void incrementBarCount(int playerIndex);

    /**
     * @brief Decrements the bar count for a player.
     * @param playerIndex Player index (0 for WHITE, 1 for BLACK)
     */
    void decrementBarCount(int playerIndex);

    /**
     * @brief Increments the bear-off count for a player.
     * @param playerIndex Player index (0 for WHITE, 1 for BLACK)
     */
    void incrementBorneOffCount(int playerIndex);

private:
    std::array<Column, 24> m_columns;  ///< The 24 columns (points) on the board
    int m_barCount[2];                 ///< Number of pieces on the bar for each player
    int m_borneOffCount[2];            ///< Number of pieces borne off for each player
};
