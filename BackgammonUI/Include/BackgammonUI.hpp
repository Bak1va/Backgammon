/**
 * @file BackgammonUI.hpp
 * @brief Defines the main window class for the Backgammon game UI.
 */

#pragma once

#include <memory>

#include <QMainWindow>

#include "IGame.hpp"

class BoardWidget;
class QPushButton;
class QLabel;

/**
 * @class BackgammonUI
 * @brief Main window class for the Backgammon game application.
 *
 * This class manages the main game window including:
 * - Game controls (roll dice, new game buttons)
 * - Status displays (current player, dice values, game messages)
 * - Board widget for visual game representation
 * - Menu bar with game options
 */
class BackgammonUI : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the main window.
     * @param parent Optional parent widget
     */
    explicit BackgammonUI(QWidget* parent = nullptr);

    /**
     * @brief Destructor for the main window.
     */
    ~BackgammonUI() override;

    /**
     * @brief Updates all UI elements to reflect the current game state.
     *
     * This method refreshes dice displays, player labels, and status messages.
     */
    void updateUI();

    private slots:
        /**
         * @brief Slot called when the roll dice button is clicked.
         *
         * Handles both opening rolls and regular game rolls.
         */
        void onRollDice();

        /**
         * @brief Slot called when the new game menu action is triggered.
         *
         * Starts a fresh game with the opening roll phase.
         */
    void onNewGame();

private:
    /**
     * @brief Sets up the main UI layout and widgets.
     */
    void setupUi();

    /**
     * @brief Sets up the menu bar with game actions.
     */
    void setupMenu();

    /**
     * @brief Updates the dice image displays.
     * @param dice1 Value of the first die
     * @param dice2 Value of the second die
     */
    void updateDiceDisplay(int dice1, int dice2) const;

    /**
     * @brief Updates the current player display label.
     * @param currentPlayer Color of the current player
     */
    void updatePlayerDisplay(Color currentPlayer) const;

    /**
     * @brief Updates the status message display.
     * @param diceConsumed Whether all dice have been used
     * @param currentPlayer Color of the current player
     */
    void updateStatusDisplay(bool diceConsumed, Color currentPlayer) const;

    /**
     * @brief Updates the opening roll display labels.
     */
    void updateOpeningRollDisplay() const;

    /**
     * @brief Creates a pixmap for a die with the specified value.
     * @param value Die value (1-6)
     * @return QPixmap showing the die face
     */
    [[nodiscard]] QPixmap dicePixmap(int value) const;

    std::unique_ptr<IGame> m_game;  ///< The game logic instance

    BoardWidget* m_boardWidget;              ///< Visual board representation
    QPushButton* m_rollButton;               ///< Button to roll dice
    QLabel* m_playerLabel;                   ///< Label showing current player
    QLabel* m_statusLabel;                   ///< Label showing game status messages
    QLabel* m_diceImg1;                      ///< Image label for first die
    QLabel* m_diceImg2;                      ///< Image label for second die
    QLabel* m_openingDiceWhiteLabel;         ///< Label for white's opening die
    QLabel* m_openingDiceBlackLabel;         ///< Label for black's opening die
};