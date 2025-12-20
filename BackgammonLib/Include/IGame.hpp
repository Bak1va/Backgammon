/**
 * @file IGame.hpp
 * @brief Defines the IGame interface for the Backgammon game logic.
 */

#pragma once
#include "Color.hpp"
#include "MoveResult.hpp"
#include "GameStateDTO.hpp"
#include <vector>

class IGameObserver;

/**
 * @enum GamePhase
 * @brief Represents the current phase of the game.
 */
enum class GamePhase {
	NOT_STARTED,          ///< Game has not started yet
	OPENING_ROLL_WHITE,   ///< White player is rolling the opening die
	OPENING_ROLL_BLACK,   ///< Black player is rolling the opening die
	OPENING_ROLL_COMPARE, ///< Comparing opening rolls to determine who starts
	IN_PROGRESS,          ///< Game is in progress
	FINISHED              ///< Game has finished
};

/**
 * @interface IGame
 * @brief Abstract interface for the Backgammon game logic.
 *
 * This interface defines all the operations needed to play a game of Backgammon,
 * including starting the game, rolling dice, making moves, and querying game state.
 * It also supports the Observer pattern for notifying UI components of game events.
 */
class IGame {
public:
	/**
	 * @brief Virtual destructor for proper cleanup of derived classes.
	 */
	virtual ~IGame() = default;

	/**
	 * @brief Starts a new game.
	 *
	 * Initializes the board to the standard starting position and begins the opening roll phase.
	 */
	virtual void start() = 0;

	/**
	 * @brief Gets the current phase of the game.
	 * @return The current GamePhase
	 */
	virtual GamePhase getPhase() const = 0;

	/**
	 * @brief Gets the current player whose turn it is.
	 * @return The color of the current player
	 */
	virtual Color getCurrentPlayer() const = 0;

	/**
	 * @brief Rolls the dice for the current player.
	 *
	 * This method generates two random die values (1-6) for the current player.
	 * Can only be called during the IN_PROGRESS phase.
	 */
	virtual void rollDice() = 0;

	/**
	 * @brief Gets the current dice values.
	 * @return Array containing the two die values (0 if not rolled or already used)
	 */
	virtual const std::array<int, 2> getDice() const = 0;

	/**
	 * @brief Checks if the current player has any legal moves available.
	 * @return True if moves are available, false otherwise
	 */
	virtual bool hasMovesAvailable() const = 0;

	/**
	 * @brief Rolls the opening dice for determining who goes first.
	 *
	 * Each player rolls one die. The player with the higher value starts.
	 * If values are equal, both players roll again.
	 */
	virtual void rollOpeningDice() = 0;

	/**
	 * @brief Gets the opening die value for the white player.
	 * @return White's opening die value
	 */
	virtual int getOpeningDiceWhite() const = 0;

	/**
	 * @brief Gets the opening die value for the black player.
	 * @return Black's opening die value
	 */
	virtual int getOpeningDiceBlack() const = 0;

	/**
	 * @brief Transitions from opening roll phase to the main game.
	 *
	 * Should be called after the opening rolls have been compared and a
	 * starting player has been determined.
	 */
	virtual void startGameAfterOpening() = 0;

	/**
	 * @brief Passes the turn to the other player.
	 *
	 * Used when the current player has no legal moves or chooses to pass.
	 */
	virtual void passTurn() = 0;

	/**
	 * @brief Attempts to make a move from one column to another.
	 * @param fromIndex Source column index (0-23), or BAR_INDEX for pieces on the bar
	 * @param toIndex Destination column index (0-23), or special value for bearing off
	 * @return MoveResult indicating success or the reason for failure
	 */
	virtual MoveResult makeMove(int fromIndex, int toIndex) = 0;

	/**
	 * @brief Gets the number of pieces on a specific column.
	 * @param index Column index (0-23)
	 * @return Number of pieces on that column
	 */
	virtual int getColumnCount(int index) const = 0;

	/**
	 * @brief Gets the color of pieces on a specific column.
	 * @param index Column index (0-23)
	 * @return Color of pieces on that column (NONE if empty)
	 */
	virtual Color getColumnColor(int index) const = 0;

	/**
	 * @brief Gets the number of pieces on the bar for a player.
	 * @param player The player color
	 * @return Number of pieces on the bar
	 */
	virtual int getBarCount(Color player) const = 0;

	/**
	 * @brief Gets the number of pieces borne off for a player.
	 * @param player The player color
	 * @return Number of pieces borne off
	 */
	virtual int getBorneOffCount(Color player) const = 0;

	/**
	 * @brief Adds an observer to receive game event notifications.
	 * @param observer Pointer to the observer to add
	 */
	virtual void addObserver(IGameObserver* observer) = 0;

	/**
	 * @brief Removes an observer from the notification list.
	 * @param observer Pointer to the observer to remove
	 */
	virtual void removeObserver(IGameObserver* observer) = 0;

	/**
	 * @brief Gets the complete current state of the game.
	 * @return GameStateDTO containing all game state information
	 */
	virtual GameStateDTO getState() const = 0;

	/**
	 * @brief Checks if a point can be selected for moving.
	 * @param index Column index to check
	 * @return True if the point can be selected by the current player
	 */
	virtual bool canSelectPoint(int index) const = 0;

	/**
	 * @brief Gets all legal destination columns for a piece at the given position.
	 * @param fromIndex Source column index
	 * @return Vector of legal destination indices
	 */
	virtual std::vector<int> getLegalTargets(int fromIndex) const = 0;
};


