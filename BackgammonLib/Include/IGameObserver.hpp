 /**
 * @file IGameObserver.hpp
 * @brief Defines the IGameObserver interface for the Observer design pattern.
 */

#pragma once
#include "Color.hpp"
#include "MoveResult.hpp"

/**
 * @interface IGameObserver
 * @brief Observer interface for receiving notifications about game events.
 *
 * This interface implements the Observer design pattern, allowing objects
 * to subscribe to game events and react accordingly. All methods have default
 * empty implementations, so observers can choose which events to handle.
 */
class IGameObserver {
public:
	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~IGameObserver() = default;

	/**
	 * @brief Called when the game has started.
	 */
	virtual void onGameStarted() {}

	/**
	 * @brief Called when dice have been rolled.
	 * @param player The player who rolled the dice
	 * @param d1 Value of the first die
	 * @param d2 Value of the second die
	 */
	virtual void onDiceRolled(Color player, int d1, int d2) {}

	/**
	 * @brief Called when a move has been made.
	 * @param player The player who made the move
	 * @param fromIndex Source column index
	 * @param toIndex Destination column index
	 * @param result Result of the move attempt
	 */
	virtual void onMoveMade(Color player, int fromIndex, int toIndex, MoveResult result) {}

	/**
	 * @brief Called when the turn changes to another player.
	 * @param currentPlayer The player whose turn it now is
	 */
	virtual void onTurnChanged(Color currentPlayer) {}

	/**
	 * @brief Called when the game has finished.
	 * @param winner The color of the winning player
	 */
	virtual void onGameFinished(Color winner) {}
};


