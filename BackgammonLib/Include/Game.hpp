/**
 * @file Game.hpp
 * @brief Defines the Game class implementing the complete Backgammon game logic.
 */

#pragma once
#include <vector>
#include "IGame.hpp"
#include "IGameObserver.hpp"
#include "Board.hpp"
#include "GameStateDTO.hpp"

/**
 * @class Game
 * @brief Concrete implementation of the IGame interface for Backgammon.
 *
 * This class implements all the game rules for Backgammon including:
 * - Opening dice rolls to determine starting player
 * - Regular dice rolls and move validation
 * - Hitting opponent pieces and placing them on the bar
 * - Bearing off pieces when all are in the home board
 * - Win detection
 * - Observer pattern for UI notifications
 */
class Game : public IGame {
public:
    /**
     * @brief Special index constant representing the bar.
     */
    static constexpr int BAR_INDEX = 25;

    /**
     * @brief Constructor creating a new game instance.
     */
    Game();

    /**
     * @brief Destructor for the Game.
     */
    ~Game() override;

	/**
	 * @brief Starts a new game with the standard starting position.
	 *
	 * Resets the board and enters the opening roll phase.
	 */
	void start() override;

	/**
	 * @brief Gets the current phase of the game.
	 * @return The current GamePhase
	 */
    GamePhase getPhase() const override;

    /**
     * @brief Gets the current player whose turn it is.
     * @return The color of the current player
     */
    Color getCurrentPlayer() const override;

    /**
     * @brief Rolls two dice for the current player.
     *
     * Generates random values 1-6 for each die. Only valid during IN_PROGRESS phase.
     */
    void rollDice() override;

    /**
     * @brief Gets the current dice values.
     * @return Array with two die values (0 if used or not rolled)
     */
    const std::array<int, 2> getDice() const override;

    /**
     * @brief Checks if the current player has any legal moves.
     * @return True if legal moves exist, false otherwise
     */
    bool hasMovesAvailable() const override;

    /**
     * @brief Attempts to make a move on the board.
     * @param fromIndex Source column (0-23 or BAR_INDEX)
     * @param toIndex Destination column (0-23 or special bear-off value)
     * @return MoveResult indicating success or failure reason
     */
    MoveResult makeMove(int fromIndex, int toIndex) override;

    /**
     * @brief Rolls one die for each player to determine who starts.
     *
     * Progresses through opening roll phases. If tied, both players roll again.
     */
    void rollOpeningDice() override;

    /**
     * @brief Gets white player's opening die value.
     * @return Opening die value for white
     */
    int getOpeningDiceWhite() const override;

    /**
     * @brief Gets black player's opening die value.
     * @return Opening die value for black
     */
    int getOpeningDiceBlack() const override;

    /**
     * @brief Transitions from opening phase to the main game.
     */
    void startGameAfterOpening() override;

    /**
     * @brief Passes the turn to the opponent.
     *
     * Clears dice and switches the current player.
     */
    void passTurn() override;

	/**
	 * @brief Gets the number of pieces on a column.
	 * @param index Column index (0-23)
	 * @return Number of pieces
	 */
    int getColumnCount(int index) const override;

    /**
     * @brief Gets the color of pieces on a column.
     * @param index Column index (0-23)
     * @return Color of pieces (NONE if empty)
     */
    Color getColumnColor(int index) const override;

    /**
     * @brief Gets the number of pieces on the bar for a player.
     * @param player Player color
     * @return Number of pieces on bar
     */
    int getBarCount(Color player) const override;

    /**
     * @brief Gets the number of pieces borne off for a player.
     * @param player Player color
     * @return Number of pieces borne off
     */
    int getBorneOffCount(Color player) const override;

	/**
	 * @brief Gets the complete current game state.
	 * @return GameStateDTO with all state information
	 */
    GameStateDTO getState() const override;

    /**
     * @brief Checks if a point can be selected for moving.
     * @param index Column index
     * @return True if selectable by current player
     */
    bool canSelectPoint(int index) const override;

    /**
     * @brief Gets all legal destination columns for a piece.
     * @param fromIndex Source column index
     * @return Vector of legal destination indices
     */
    std::vector<int> getLegalTargets(int fromIndex) const override;

	/**
	 * @brief Adds an observer for game events.
	 * @param observer Observer to add
	 */
    void addObserver(IGameObserver *observer) override;

    /**
     * @brief Removes an observer from notifications.
     * @param observer Observer to remove
     */
    void removeObserver(IGameObserver *observer) override;

private:
    Board m_board;                          ///< The game board
    GamePhase m_phase;                      ///< Current phase of the game
    Color m_currentPlayer;                  ///< Current player's turn
    std::array<int, 2> m_dice;              ///< Current dice values
    bool m_diceRolled;                      ///< Whether dice have been rolled this turn
    std::vector<IGameObserver *> m_observers; ///< Registered observers

    int m_openingDiceWhite;  ///< White's opening die value
    int m_openingDiceBlack;  ///< Black's opening die value

    /**
     * @brief Notifies all observers that the game has started.
     */
    void notifyGameStarted();

    /**
     * @brief Notifies all observers that dice were rolled.
     */
    void notifyDiceRolled();

    /**
     * @brief Notifies all observers that a move was made.
     * @param fromIndex Source column
     * @param toIndex Destination column
     * @param result Result of the move
     */
    void notifyMoveMade(int fromIndex, int toIndex, MoveResult result);

    /**
     * @brief Notifies all observers that the turn changed.
     */
    void notifyTurnChanged();

    /**
     * @brief Notifies all observers that the game finished.
     * @param winner Winning player color
     */
    void notifyGameFinished(Color winner);

    /**
     * @brief Switches to the other player's turn.
     */
    void switchTurn();

	/**
	 * @brief Rolls a single die (1-6).
	 * @return Random value 1-6
	 */
    int rollSingleDie();

	/**
	 * @brief Checks if a destination is blocked by opponent pieces.
	 * @param toIndex Destination column
	 * @param player Player attempting the move
	 * @return True if blocked
	 */
    bool isMoveBlocked(int toIndex, Color player) const;

    /**
     * @brief Checks if a piece can hit an opponent at the destination.
     * @param toIndex Destination column
     * @param player Player attempting the move
     * @return True if can hit
     */
    bool canHit(int toIndex, Color player) const;

    /**
     * @brief Converts player color to index (0 or 1).
     * @param player Player color
     * @return 0 for WHITE, 1 for BLACK
     */
    int playerIndex(Color player) const;

	/**
	 * @brief Checks if a player can bear off pieces.
	 * @param player Player color
	 * @return True if player can bear off
	 */
    bool canBearOff(Color player) const;

    /**
     * @brief Checks if a column is in the home board for a player.
     * @param index Column index
     * @param player Player color
     * @return True if in home board
     */
    bool isHomeBoard(int index, Color player) const;

    /**
     * @brief Checks if all of a player's pieces are in the home board.
     * @param player Player color
     * @return True if all pieces are home
     */
    bool hasAllPiecesHome(Color player) const;
};
