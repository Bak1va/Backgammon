/**
 * @file Color.hpp
 * @brief Defines the Color enum for representing player colors in the Backgammon game.
 */

#pragma once

/**
 * @enum Color
 * @brief Represents the color of a player or piece in the Backgammon game.
 */
enum class Color {
	BLACK,  ///< Represents the black player
	WHITE,  ///< Represents the white player
	NONE    ///< Represents no color (empty column)
};
