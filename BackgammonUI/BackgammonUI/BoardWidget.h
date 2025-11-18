#pragma once

#include <QWidget>
#include <vector>
#include "IGameObserver.h"
#include "IGame.h"
#include "GameStateDTO.h"

class BackgammonUI;

class BoardWidget : public QWidget, public IGameObserver
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent, IGame* game, BackgammonUI* mainWindow = nullptr);

    // Observer callbacks
    void onGameStarted() override;
    void onDiceRolled(Color player, int d1, int d2) override;
    void onMoveMade(Color player, int fromIndex, int toIndex, MoveResult result) override;
    void onTurnChanged(Color currentPlayer) override;
    void onGameFinished(Color winner) override;

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;

private:
    IGame* m_game;
    BackgammonUI* m_mainWindow;
    GameStateDTO m_state;

    int m_selectedPoint;
    std::vector<int> m_legalTargets;

    // Variabila noua pentru a stoca castigatorul
    Color m_winner;

    void refreshState();
    void clearSelection();
    void selectPoint(int index);

    // Drawing helpers
    void drawTriangle(QPainter& painter, int x, int y, int width, int height, bool pointUp, int pointIndex);
    void drawPiecesAtPoint(QPainter& painter, int centerX, int centerY, int count, Color color, int radius, bool isTopRow);
    void drawBarPieces(QPainter& painter, const GameStateDTO& state);
    void drawBoard(QPainter& p);
    void drawPieces(QPainter& p);
    void drawHighlights(QPainter& p);

    // Metoda noua pentru desenarea panoului de final
    void drawGameOverPanel(QPainter& p);

    int pointIndexFromPosition(const QPoint& pos) const;
};