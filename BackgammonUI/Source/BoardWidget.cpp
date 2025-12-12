#include "BoardWidget.hpp"
#include "BackgammonUI.hpp"
#include "Game.hpp"
#include <QPainter>
#include <QMouseEvent>
#include <QMessageBox> 
#include <algorithm>

constexpr int BEAR_OFF_WIDTH = 60;

BoardWidget::BoardWidget(QWidget* parent, IGame* game, BackgammonUI* mainWindow)
    : QWidget(parent),
    m_game(game),
    m_mainWindow(mainWindow),
    m_selectedPoint(-1),
    m_winner(Color::NONE)
{
    setMinimumSize(960, 500);
    if (m_game) {
        m_state = m_game->getState();
    }
}

void BoardWidget::refreshState() {
    if (m_game) {
        m_state = m_game->getState();
    }
    update();
}

void BoardWidget::clearSelection() {
    m_selectedPoint = -1;
    m_legalTargets.clear();
    update();
}

void BoardWidget::selectPoint(int index) {
    if (!m_game) return;
    if (m_winner != Color::NONE) return;

    m_selectedPoint = index;
    m_legalTargets = m_game->getLegalTargets(index);
    update();
}

void BoardWidget::onGameStarted() {
    m_winner = Color::NONE; 
    clearSelection();
    refreshState();
}

void BoardWidget::onDiceRolled(Color, int, int) { refreshState(); }

void BoardWidget::onMoveMade(Color, int, int, MoveResult) {
    refreshState();
    if (m_mainWindow) m_mainWindow->updateUI();
}

void BoardWidget::onTurnChanged(Color) { clearSelection(); refreshState(); }

void BoardWidget::onGameFinished(Color winner) {
    m_winner = winner;
    clearSelection();
    refreshState();

    QString winnerName = (winner == Color::WHITE) ? "Color::WHITE" : "BLACK";
    QMessageBox::information(this, "Game Over",
        "Congratulations! Player " + winnerName + " has won the game!");
}

void BoardWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor(30, 90, 50));

    drawBoard(p);
    drawPieces(p);
    drawHighlights(p);

    // If the game is over we draw the panel.
    if (m_winner != Color::NONE) {
        drawGameOverPanel(p);
    }
}

void BoardWidget::drawGameOverPanel(QPainter& p) {
    p.fillRect(rect(), QColor(0, 0, 0, 180));
    p.setPen(Qt::white);
    QFont font("Arial", 32, QFont::Bold);
    p.setFont(font);

    QString text = (m_winner == Color::WHITE) ? "Color::WHITE WINS!" : "BLACK WINS!";

    p.drawText(rect(), Qt::AlignCenter, text);

    QFont subFont("Arial", 16);
    p.setFont(subFont);
    p.setPen(QColor(200, 200, 200));
    QRect subRect = rect();
    subRect.setTop(subRect.top() + 80); 
    p.drawText(subRect, Qt::AlignCenter, "Go to Game -> New Game to restart");
}

void BoardWidget::drawBoard(QPainter& p) {
    const int w = width() - BEAR_OFF_WIDTH;
    const int h = height();
    int barWidth = 40;
    int barX = w / 2 - barWidth / 2;
    int leftSideWidth = barX;
    int rightSideWidth = w - (barX + barWidth);
    int triangleWidth = leftSideWidth / 6;
    int triangleHeight = h / 2 - 20;

    QRect barRect(barX, 0, barWidth, h);
    QLinearGradient barGradient(barX, 0, barX + barWidth, 0);
    barGradient.setColorAt(0, QColor(101, 67, 33));
    barGradient.setColorAt(0.5, QColor(139, 69, 19));
    barGradient.setColorAt(1, QColor(101, 67, 33));
    p.fillRect(barRect, barGradient);

    // TOP ROW (left to right visually): 11 10 9 8 7 6   |   5 4 3 2 1 0
    // BOTTOM ROW (left to right visually): 12 13 14 15 16 17   |   18 19 20 21 22 23
    for (int i = 0; i < 6; i++) {
        // Left half: top -> indices 11..6 (reversed), bottom -> 12..17
        int topLeftIndex = 11 - i;
        int bottomLeftIndex = 12 + i;
        drawTriangle(p, i * triangleWidth, 0, triangleWidth, triangleHeight, false, topLeftIndex);
        drawTriangle(p, i * triangleWidth, h, triangleWidth, triangleHeight, true, bottomLeftIndex);
    }

    int rightStartX = barX + barWidth;
    triangleWidth = rightSideWidth / 6;
    for (int i = 0; i < 6; i++) {
        // Right half: top -> indices 5..0 (reversed), bottom -> 18..23
        int topRightIndex = 5 - i;
        int bottomRightIndex = 18 + i;
        drawTriangle(p, rightStartX + i * triangleWidth, 0, triangleWidth, triangleHeight, false, topRightIndex);
        drawTriangle(p, rightStartX + i * triangleWidth, h, triangleWidth, triangleHeight, true, bottomRightIndex);
    }

    QRect bearOffRect(w, 0, BEAR_OFF_WIDTH, h);
    p.fillRect(bearOffRect, QColor(60, 30, 10));
    p.setPen(QPen(QColor(100, 70, 30), 2));
    p.drawRect(bearOffRect);

    p.setPen(QPen(Qt::black, 2));
    p.drawLine(w, 0, w, h);
}

void BoardWidget::drawTriangle(QPainter& p, int x, int y, int width, int height, bool pointUp, int pointIndex) {
    QPolygon triangle;
    if (pointUp) {
        triangle << QPoint(x, y) << QPoint(x + width, y) << QPoint(x + width / 2, y - height);
    }
    else {
        triangle << QPoint(x, y) << QPoint(x + width, y) << QPoint(x + width / 2, y + height);
    }
    QColor color = (pointIndex % 2 == 0) ? QColor(100, 50, 20) : QColor(220, 180, 140);
    p.setBrush(color);
    p.setPen(QPen(Qt::black, 1));
    p.drawPolygon(triangle);
}

void BoardWidget::drawPieces(QPainter& painter) {
    int boardWidth = width() - BEAR_OFF_WIDTH;
    int boardHeight = height();
    int barWidth = 40;
    int barX = boardWidth / 2 - barWidth / 2;
    int leftSideWidth = barX;
    int rightSideWidth = boardWidth - (barX + barWidth);
    int leftTriangleWidth = leftSideWidth / 6;
    int rightTriangleWidth = rightSideWidth / 6;

    int pieceRadius = leftTriangleWidth / 2 - 5;
    if (pieceRadius > 22) pieceRadius = 22;

    const GameStateDTO& state = m_state;

    for (int pointIndex = 0; pointIndex < 24; pointIndex++) {
        int pieceCount = state.pieceCounts[pointIndex];
        Color color = state.colors[pointIndex];

        if (pieceCount > 0 && color != Color::NONE) {
            int x, y;
            bool isTopRow;

            // Mapping to match visual layout:
            // Top row: 11 10 9 8 7 6  |  5 4 3 2 1 0
            // Bottom row: 12 13 14 15 16 17  |  18 19 20 21 22 23
            if (pointIndex >= 6 && pointIndex <= 11) {
                // Top-left half: indices 11..6
                int col = 11 - pointIndex; // 0..5
                x = col * leftTriangleWidth + leftTriangleWidth / 2;
                y = pieceRadius + 5;
                isTopRow = true;
            }
            else if (pointIndex >= 0 && pointIndex <= 5) {
                // Top-right half: indices 5..0
                int col = 5 - pointIndex; // 0..5
                x = barX + barWidth + col * rightTriangleWidth + rightTriangleWidth / 2;
                y = pieceRadius + 5;
                isTopRow = true;
            }
            else if (pointIndex >= 12 && pointIndex <= 17) {
                // Bottom-left half: indices 12..17
                int col = pointIndex - 12; // 0..5
                x = col * leftTriangleWidth + leftTriangleWidth / 2;
                y = boardHeight - pieceRadius - 5;
                isTopRow = false;
            }
            else if (pointIndex >= 18 && pointIndex <= 23) {
                // Bottom-right half: indices 18..23
                int col = pointIndex - 18; // 0..5
                x = barX + barWidth + col * rightTriangleWidth + rightTriangleWidth / 2;
                y = boardHeight - pieceRadius - 5;
                isTopRow = false;
            }
            else {
                continue;
            }
            drawPiecesAtPoint(painter, x, y, pieceCount, color, pieceRadius, isTopRow);
        }
    }

    drawBarPieces(painter, state);

    int bearOffX = boardWidth + BEAR_OFF_WIDTH / 2;
    if (state.borneOffWhite > 0) {
        drawPiecesAtPoint(painter, bearOffX, pieceRadius + 10, state.borneOffWhite, Color::WHITE, 12, true);
    }
    if (state.borneOffBlack > 0) {
        drawPiecesAtPoint(painter, bearOffX, boardHeight - pieceRadius - 10, state.borneOffBlack, Color::BLACK, 12, false);
    }
}

void BoardWidget::drawPiecesAtPoint(QPainter& painter, int centerX, int centerY,
    int count, Color color, int radius, bool isTopRow) {
    QColor pieceColor = (color == Color::WHITE) ? QColor(255, 255, 255) : QColor(0, 0, 0);
    QColor borderColor = (color == Color::WHITE) ? QColor(200, 200, 200) : QColor(50, 50, 50);
    painter.setBrush(pieceColor);
    painter.setPen(QPen(borderColor, 2));

    int spacing = radius * 2 + 2;
    if (count > 5) spacing = (radius * 2 * 5) / count;

    int maxVisible = 15;

    for (int i = 0; i < std::min(count, maxVisible); i++) {
        int yOffset = isTopRow ? (i * spacing) : -(i * spacing);
        int pieceY = centerY + yOffset;
        painter.drawEllipse(QPoint(centerX, pieceY), radius, radius);
    }
}

void BoardWidget::drawBarPieces(QPainter& painter, const GameStateDTO& state) {
    int boardWidth = width() - BEAR_OFF_WIDTH;
    int boardHeight = height();
    int barWidth = 40;
    int barX = boardWidth / 2 - barWidth / 2;
    int pieceRadius = 18;
    int centerX = barX + barWidth / 2;

    if (state.barWhite > 0) {
        int startY = boardHeight / 2 - 50;
        for (int i = 0; i < state.barWhite; i++) {
            painter.setBrush(Qt::white);
            painter.setPen(QPen(QColor(200, 200, 200), 2));
            painter.drawEllipse(QPoint(centerX, startY - i * 20), pieceRadius, pieceRadius);
        }
    }
    if (state.barBlack > 0) {
        int startY = boardHeight / 2 + 50;
        for (int i = 0; i < state.barBlack; i++) {
            painter.setBrush(Qt::black);
            painter.setPen(QPen(QColor(50, 50, 50), 2));
            painter.drawEllipse(QPoint(centerX, startY + i * 20), pieceRadius, pieceRadius);
        }
    }
}

void BoardWidget::drawHighlights(QPainter& p) {
    if (m_winner != Color::NONE) return;

    auto getPointRect = [&](int pointIndex) -> QRect {
        if (pointIndex == Game::BAR_INDEX) {
            int barW = 40;
            int barX = (width() - BEAR_OFF_WIDTH) / 2 - barW / 2;
            return QRect(barX, 0, barW, height());
        }

        const int w = width() - BEAR_OFF_WIDTH;
        const int h = height();
        const int barWidth = 40;
        const int barX = w / 2 - barWidth / 2;
        const int leftTriangleWidth = barX / 6;
        const int rightTriangleWidth = (w - (barX + barWidth)) / 6;

        bool top = (pointIndex < 12);
        int x = 0;
        int triangleWidth = 0;

        // Match the same visual mapping as drawBoard/drawPieces:
        // Top: 11 10 9 8 7 6 | 5 4 3 2 1 0
        // Bottom: 12 13 14 15 16 17 | 18 19 20 21 22 23
        if (pointIndex >= 6 && pointIndex <= 11) {
            // Top-left
            int col = 11 - pointIndex; // 0..5
            x = col * leftTriangleWidth;
            triangleWidth = leftTriangleWidth;
        }
        else if (pointIndex >= 0 && pointIndex <= 5) {
            // Top-right
            int col = 5 - pointIndex; // 0..5
            x = barX + barWidth + col * rightTriangleWidth;
            triangleWidth = rightTriangleWidth;
        }
        else if (pointIndex >= 12 && pointIndex <= 17) {
            // Bottom-left
            int col = pointIndex - 12; // 0..5
            x = col * leftTriangleWidth;
            triangleWidth = leftTriangleWidth;
        }
        else if (pointIndex >= 18 && pointIndex <= 23) {
            // Bottom-right
            int col = pointIndex - 18; // 0..5
            x = barX + barWidth + col * rightTriangleWidth;
            triangleWidth = rightTriangleWidth;
        }
        else {
            return QRect();
        }

        return QRect(x, top ? 0 : h / 2, triangleWidth, h / 2);
        };

    if (m_selectedPoint != -1) {
        p.setBrush(QColor(255, 255, 0, 80));
        p.setPen(Qt::NoPen);
        p.drawRect(getPointRect(m_selectedPoint));

        p.setBrush(QColor(0, 255, 0, 80));
        for (int targetPoint : m_legalTargets) {
            if (targetPoint == 24 || targetPoint == -1) {
                QRect bearOffZone(width() - BEAR_OFF_WIDTH, 0, BEAR_OFF_WIDTH, height());
                p.drawRect(bearOffZone);
            }
            else {
                p.drawRect(getPointRect(targetPoint));
            }
        }
    }
}

int BoardWidget::pointIndexFromPosition(const QPoint& pos) const {
    const int w = width() - BEAR_OFF_WIDTH;
    const int h = height();
    const int barWidth = 40;
    const int barX = w / 2 - barWidth / 2;
    const int leftTriangleWidth = barX / 6;
    const int rightTriangleWidth = (w - (barX + barWidth)) / 6;

    if (pos.x() > w) {
        if (m_state.currentPlayer == Color::WHITE) return 24;
        if (m_state.currentPlayer == Color::BLACK) return -1;
    }

    if (pos.x() >= barX && pos.x() <= barX + barWidth) {
        if (m_state.currentPlayer == Color::WHITE && m_state.barWhite > 0) return Game::BAR_INDEX;
        if (m_state.currentPlayer == Color::BLACK && m_state.barBlack > 0) return Game::BAR_INDEX;
        return -1;
    }

    bool top = (pos.y() < h / 2);
    int pointIndex = -1;

    if (pos.x() < barX) {
        int col = pos.x() / leftTriangleWidth;
        if (col >= 0 && col < 6) {
            if (top) {
                // Top-left: 11 10 9 8 7 6
                pointIndex = 11 - col;
            }
            else {
                // Bottom-left: 12 13 14 15 16 17
                pointIndex = 12 + col;
            }
        }
    }
    else if (pos.x() > barX + barWidth && pos.x() < w) {
        int col = (pos.x() - (barX + barWidth)) / rightTriangleWidth;
        if (col >= 0 && col < 6) {
            if (top) {
                // Top-right: 5 4 3 2 1 0
                pointIndex = 5 - col;
            }
            else {
                // Bottom-right: 18 19 20 21 22 23
                pointIndex = 18 + col;
            }
        }
    }
    return pointIndex;
}

void BoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_game) return;
    if (m_winner != Color::NONE) return; 

    int index = pointIndexFromPosition(event->pos());

    if (index == -1 && (event->pos().x() <= width() - BEAR_OFF_WIDTH)) {
        clearSelection();
        return;
    }

    if (m_selectedPoint == -1) {
        if (index == 24 || index == -1) return;
        if (m_game->canSelectPoint(index)) selectPoint(index);
        else clearSelection();
        return;
    }

    if (index == m_selectedPoint) {
        clearSelection();
        return;
    }

    if (std::find(m_legalTargets.begin(), m_legalTargets.end(), index) != m_legalTargets.end()) {
        MoveResult result = m_game->makeMove(m_selectedPoint, index);
        if (result == MoveResult::SUCCESS) clearSelection();
        else refreshState();
        return;
    }

    if (m_game->canSelectPoint(index)) selectPoint(index);
    else clearSelection();
}