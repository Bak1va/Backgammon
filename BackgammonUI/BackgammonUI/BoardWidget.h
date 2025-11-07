#pragma once

#include <QWidget>
#include <vector>
#include "IGameObserver.h"
#include "IGame.h"
#include "GameStateDTO.h"

class BoardWidget : public QWidget, public IGameObserver
{
    Q_OBJECT

public:
    explicit BoardWidget(QWidget* parent, IGame* game);

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
    IGame* m_game;          // nu deținem, doar folosim
    GameStateDTO m_state;   // snapshot pentru desenare

    int m_selectedPoint;            // punctul sursă selectat, -1 dacă niciunul
    std::vector<int> m_legalTargets; // destinații posibile pt. punctul selectat

    void refreshState();
    void clearSelection();
    void selectPoint(int index);

    int pointIndexFromPosition(const QPoint& pos) const;
    void drawBoard(QPainter& p);
    void drawPieces(QPainter& p);
    void drawHighlights(QPainter& p);
};
