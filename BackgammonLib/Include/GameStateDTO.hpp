/**
 * @file GameStateDTO.hpp
 * @brief Defines the GameStateDTO structure for transferring game state data.
 */

#pragma once

#include <array>
#include "Color.hpp"

/**
 * @struct GameStateDTO
 * @brief Data Transfer Object containing the complete state of the Backgammon game.
 *
 * This structure is used to pass game state information between different
 * components of the application, particularly from the game logic to the UI.
 */
struct GameStateDTO {
    std::array<int, 24> pieceCounts{};  ///< Number of pieces on each of the 24 columns (0-23)
    std::array<Color, 24> colors{};     ///< Color of pieces on each column

    int barWhite = 0;     ///< Number of white pieces on the bar
    int barBlack = 0;     ///< Number of black pieces on the bar

    int borneOffWhite = 0;  ///< Number of white pieces that have been borne off
    int borneOffBlack = 0;  ///< Number of black pieces that have been borne off

    Color currentPlayer = Color::NONE;  ///< The player whose turn it is

    int dice1 = 0;  ///< Value of the first die (0 if not rolled or already used)
    int dice2 = 0;  ///< Value of the second die (0 if not rolled or already used)

    int openingDiceWhite = 0;  ///< White player's opening die value
    int openingDiceBlack = 0;  ///< Black player's opening die value
};
