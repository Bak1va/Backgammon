/**
 * @file Player.hpp
 * @brief Defines the Player class representing a player in the Backgammon game.
 */

#pragma once
#include "Color.hpp"

/**
 * @class Player
 * @brief Represents a player in the Backgammon game.
 *
 * Each player has a color (WHITE or BLACK) that identifies them
 * and determines their pieces and movement direction.
 */
class Player {
public:
    /**
     * @brief Constructor creating a player with the specified color.
     * @param color The color assigned to this player (WHITE or BLACK)
     */
    Player(Color color);

    /**
     * @brief Destructor for the Player.
     */
    ~Player();

    /**
     * @brief Gets the color of this player.
     * @return The player's color
     */
    Color getColor() const;

private:
    Color playerColor;  ///< The color of this player
};
