/**
 * @file BoardWidget.hpp
 * @brief Defines the BoardWidget class for rendering the Backgammon board.
 */

#pragma once

#include <vector>

#include <QWidget>

#include "GameStateDTO.hpp"
#include "IGame.hpp"
#include "IGameObserver.hpp"

class BackgammonUI;

/**
 * @class BoardWidget
 * @brief Custom Qt widget that renders the Backgammon board and handles user interaction.
 *
 * This widget implements the Observer pattern to automatically update when game
 * events occur. It provides:
 * - Visual representation of the 24 points, bar, and bear-off areas
 * - Piece rendering with appropriate colors
 * - Move highlighting for selected pieces
 * - Mouse click handling for piece selection and movement
 * - Game over screen display
 */
class BoardWidget : public QWidget, public IGameObserver
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for the board widget.
     * @param parent Parent widget
     * @param game Pointer to the game logic instance
     * @param mainWindow Pointer to the main window for UI updates
     */
    explicit BoardWidget(QWidget* parent, IGame* game, BackgammonUI* mainWindow = nullptr);

    /**
     * @brief Observer callback when the game starts.
     */
    void onGameStarted() override;

    /**
     * @brief Observer callback when dice are rolled.
     * @param player Player who rolled
     * @param d1 First die value
     * @param d2 Second die value
     */
    void onDiceRolled(Color player, int d1, int d2) override;

    /**
     * @brief Observer callback when a move is made.
     * @param player Player who made the move
     * @param fromIndex Source column index
     * @param toIndex Destination column index
     * @param result Result of the move
     */
    void onMoveMade(Color player, int fromIndex, int toIndex, MoveResult result) override;

    /**
     * @brief Observer callback when the turn changes.
     * @param currentPlayer New current player
     */
    void onTurnChanged(Color currentPlayer) override;

    /**
     * @brief Observer callback when the game finishes.
     * @param winner Winning player color
     */
    void onGameFinished(Color winner) override;

protected:
    /**
     * @brief Qt paint event handler - renders the board.
     * @param event Paint event information
     */
    void paintEvent(QPaintEvent* event) override;

    /**
     * @brief Qt mouse press event handler - handles piece selection and movement.
     * @param event Mouse event information
     */
    void mousePressEvent(QMouseEvent* event) override;

private:
    IGame* m_game;               ///< Pointer to the game logic
    BackgammonUI* m_mainWindow;  ///< Pointer to the main window
    GameStateDTO m_state;        ///< Cached game state for rendering

    int m_selectedPoint;              ///< Currently selected point index (-1 if none)
    std::vector<int> m_legalTargets;  ///< Legal destination points for selected piece

    Color m_winner;  ///< Winner color when game is finished

    /**
     * @brief Refreshes the cached game state from the game logic.
     */
    void refreshState();

    /**
     * @brief Clears the current piece selection.
     */
    void clearSelection();

    /**
     * @brief Selects a point and highlights legal moves.
     * @param index Point index to select
     */
    void selectPoint(int index);

    /**
     * @brief Draws a triangular point on the board.
     * @param painter QPainter for drawing
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Width of the triangle
     * @param height Height of the triangle
     * @param pointUp Whether triangle points up or down
     * @param pointIndex Index of this point (0-23)
     */
    void drawTriangle(QPainter& painter, int x, int y, int width, int height, bool pointUp, int pointIndex);

    /**
     * @brief Draws pieces stacked at a point.
     * @param painter QPainter for drawing
     * @param centerX X coordinate of point center
     * @param centerY Y coordinate of point center
     * @param count Number of pieces
     * @param color Color of pieces
     * @param radius Radius of each piece
     * @param isTopRow Whether this is the top row of the board
     */
    void drawPiecesAtPoint(QPainter& painter, int centerX, int centerY, int count, Color color, int radius, bool isTopRow);

    /**
     * @brief Draws pieces on the bar.
     * @param painter QPainter for drawing
     * @param state Game state containing bar counts
     */
    void drawBarPieces(QPainter& painter, const GameStateDTO& state);

    /**
     * @brief Draws the board structure (points, bar, bear-off areas).
     * @param p QPainter for drawing
     */
    void drawBoard(QPainter& p);

    /**
     * @brief Draws all pieces on the board.
     * @param p QPainter for drawing
     */
    void drawPieces(QPainter& p);

    /**
     * @brief Draws selection and legal move highlights.
     * @param p QPainter for drawing
     */
    void drawHighlights(QPainter& p);

    /**
     * @brief Draws the game over panel with winner announcement.
     * @param p QPainter for drawing
     */
    void drawGameOverPanel(QPainter& p);

    /**
     * @brief Converts screen coordinates to point index.
     * @param pos Mouse position
     * @return Point index (0-23) or special value
     */
    int pointIndexFromPosition(const QPoint& pos) const;
};