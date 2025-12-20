/**
 * @file MoveResult.hpp
 * @brief Defines the MoveResult enum for move validation in the Backgammon game.
 */

#pragma once

/**
 * @enum MoveResult
 * @brief Represents the result of attempting to make a move in the game.
 */
enum class MoveResult {
    SUCCESS,                 ///< Move was successful
    INVALID_MOVE,            ///< The move is not valid according to game rules
    INVALID_FROM_COLUMN,     ///< The source column index is invalid
    INVALID_TO_COLUMN,       ///< The destination column index is invalid
    NOT_CURRENT_PLAYER,      ///< The piece does not belong to the current player
    DICE_NOT_ROLLED,         ///< Dice must be rolled before making a move
    NO_AVAILABLE_MOVES,      ///< No legal moves are available
    CANNOT_BEAR_OFF,         ///< Cannot bear off pieces yet (not all pieces in home board)
    BLOCKED_BY_OPPONENT,     ///< The destination is blocked by opponent pieces
    GAME_NOT_STARTED,        ///< The game has not started yet
    GAME_ALREADY_OVER        ///< The game has already finished
};

