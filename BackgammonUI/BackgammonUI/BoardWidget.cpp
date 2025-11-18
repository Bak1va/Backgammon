#include "BoardWidget.h"
#include "BackgammonUI.h"
#include "Game.h" // pt Game::BAR_INDEX
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>

// Constante pentru UI
const int BEAR_OFF_WIDTH = 60; // Latimea zonei din dreapta unde scoatem piese

BoardWidget::BoardWidget(QWidget* parent, IGame* game, BackgammonUI* mainWindow)
    : QWidget(parent),
    m_game(game),
    m_mainWindow(mainWindow),
    m_selectedPoint(-1)
{
    setMinimumSize(960, 500); // Putin mai lat pentru a acomoda zona de bear-off
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
    m_selectedPoint = index;
    m_legalTargets = m_game->getLegalTargets(index);
    update();
}

void BoardWidget::onGameStarted() { clearSelection(); refreshState(); }
void BoardWidget::onDiceRolled(Color, int, int) { refreshState(); }
void BoardWidget::onMoveMade(Color, int, int, MoveResult) {
    refreshState();
    if (m_mainWindow) m_mainWindow->updateUI();
}
void BoardWidget::onTurnChanged(Color) { clearSelection(); refreshState(); }
void BoardWidget::onGameFinished(Color) { clearSelection(); refreshState(); }

void BoardWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), QColor(30, 90, 50)); // Fundal verde inchis

    drawBoard(p);
    drawPieces(p);
    drawHighlights(p);
}

void BoardWidget::drawBoard(QPainter& p) {
    const int w = width() - BEAR_OFF_WIDTH; // Rezervam spatiu in dreapta
    const int h = height();
    int barWidth = 40;
    int barX = w / 2 - barWidth / 2;

    int leftSideWidth = barX;
    int rightSideWidth = w - (barX + barWidth);
    int triangleWidth = leftSideWidth / 6;
    int triangleHeight = h / 2 - 20;

    // Desenare Bara Centrala
    QRect barRect(barX, 0, barWidth, h);
    QLinearGradient barGradient(barX, 0, barX + barWidth, 0);
    barGradient.setColorAt(0, QColor(101, 67, 33));
    barGradient.setColorAt(0.5, QColor(139, 69, 19));
    barGradient.setColorAt(1, QColor(101, 67, 33));
    p.fillRect(barRect, barGradient);

    // Triunghiuri Stanga
    for (int i = 0; i < 6; i++) {
        drawTriangle(p, i * triangleWidth, 0, triangleWidth, triangleHeight, false, i);
        drawTriangle(p, i * triangleWidth, h, triangleWidth, triangleHeight, true, 18 + i);
    }

    // Triunghiuri Dreapta
    int rightStartX = barX + barWidth;
    triangleWidth = rightSideWidth / 6;
    for (int i = 0; i < 6; i++) {
        drawTriangle(p, rightStartX + i * triangleWidth, 0, triangleWidth, triangleHeight, false, 6 + i);
        drawTriangle(p, rightStartX + i * triangleWidth, h, triangleWidth, triangleHeight, true, 12 + i);
    }

    // --- DESENARE ZONA BEAR OFF (Dreapta) ---
    QRect bearOffRect(w, 0, BEAR_OFF_WIDTH, h);
    p.fillRect(bearOffRect, QColor(60, 30, 10)); // Maro inchis
    p.setPen(QPen(QColor(100, 70, 30), 2));
    p.drawRect(bearOffRect);

    // Linie despartitoare
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
    int triangleWidth = leftSideWidth / 6;
    int pieceRadius = triangleWidth / 2 - 5;
    if (pieceRadius > 22) pieceRadius = 22; // Limitare marime

    const GameStateDTO& state = m_state;

    // 1. Desenare piese pe tabla (0-23)
    for (int pointIndex = 0; pointIndex < 24; pointIndex++) {
        int pieceCount = state.pieceCounts[pointIndex];
        Color color = state.colors[pointIndex];

        if (pieceCount > 0 && color != NONE) {
            int x, y;
            bool isTopRow;

            if (pointIndex >= 0 && pointIndex <= 5) {
                x = pointIndex * triangleWidth + triangleWidth / 2;
                y = pieceRadius + 5;
                isTopRow = true;
            }
            else if (pointIndex >= 6 && pointIndex <= 11) {
                x = barX + barWidth + (pointIndex - 6) * triangleWidth + triangleWidth / 2;
                y = pieceRadius + 5;
                isTopRow = true;
            }
            else if (pointIndex >= 12 && pointIndex <= 17) {
                x = barX + barWidth + (pointIndex - 12) * triangleWidth + triangleWidth / 2;
                y = boardHeight - pieceRadius - 5;
                isTopRow = false;
            }
            else {
                x = (pointIndex - 18) * triangleWidth + triangleWidth / 2;
                y = boardHeight - pieceRadius - 5;
                isTopRow = false;
            }
            drawPiecesAtPoint(painter, x, y, pieceCount, color, pieceRadius, isTopRow);
        }
    }

    // 2. Desenare piese pe BARA (Middle Bar)
    drawBarPieces(painter, state);

    // 3. Desenare piese SCOASE (Bear Off - Zona dreapta)
    int bearOffX = boardWidth + BEAR_OFF_WIDTH / 2;

    // Piese Albe scoase (sus in dreapta, arbitrar)
    if (state.borneOffWhite > 0) {
        drawPiecesAtPoint(painter, bearOffX, pieceRadius + 10, state.borneOffWhite, WHITE, 12, true);
    }
    // Piese Negre scoase (jos in dreapta)
    if (state.borneOffBlack > 0) {
        drawPiecesAtPoint(painter, bearOffX, boardHeight - pieceRadius - 10, state.borneOffBlack, BLACK, 12, false);
    }
}

void BoardWidget::drawPiecesAtPoint(QPainter& painter, int centerX, int centerY,
    int count, Color color, int radius, bool isTopRow) {
    QColor pieceColor = (color == WHITE) ? QColor(255, 255, 255) : QColor(0, 0, 0);
    QColor borderColor = (color == WHITE) ? QColor(200, 200, 200) : QColor(50, 50, 50);
    painter.setBrush(pieceColor);
    painter.setPen(QPen(borderColor, 2));

    int spacing = radius * 2 + 2;
    // Comprimare daca sunt multe piese
    if (count > 5) spacing = (radius * 2 * 5) / count;

    int maxVisible = 15; // Desenam fizic maxim 15

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
    auto getPointRect = [&](int pointIndex) -> QRect {
        // Special: BARA
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
        int x, triangleWidth;

        if (pointIndex >= 0 && pointIndex <= 5) {
            x = pointIndex * leftTriangleWidth;
            triangleWidth = leftTriangleWidth;
        }
        else if (pointIndex >= 6 && pointIndex <= 11) {
            x = barX + barWidth + (pointIndex - 6) * rightTriangleWidth;
            triangleWidth = rightTriangleWidth;
        }
        else if (pointIndex >= 12 && pointIndex <= 17) {
            x = barX + barWidth + (pointIndex - 12) * rightTriangleWidth;
            triangleWidth = rightTriangleWidth;
        }
        else {
            x = (pointIndex - 18) * leftTriangleWidth;
            triangleWidth = leftTriangleWidth;
        }
        return QRect(x, top ? 0 : h / 2, triangleWidth, h / 2);
        };

    if (m_selectedPoint != -1) {
        // Highlight Sursa
        p.setBrush(QColor(255, 255, 0, 80)); // Galben transparent
        p.setPen(Qt::NoPen);
        p.drawRect(getPointRect(m_selectedPoint));

        // Highlight Tinte
        p.setBrush(QColor(0, 255, 0, 80)); // Verde transparent
        for (int targetPoint : m_legalTargets) {
            // VERIFICARE BEARING OFF (Scoate piesa)
            if (targetPoint == 24 || targetPoint == -1) {
                // Desenam highlight pe zona din dreapta
                QRect bearOffZone(width() - BEAR_OFF_WIDTH, 0, BEAR_OFF_WIDTH, height());
                p.drawRect(bearOffZone);
            }
            else {
                // Miscare normala
                p.drawRect(getPointRect(targetPoint));
            }
        }
    }
}

int BoardWidget::pointIndexFromPosition(const QPoint& pos) const {
    const int w = width() - BEAR_OFF_WIDTH; // excludem bear off din calculul triunghiurilor
    const int h = height();
    const int barWidth = 40;
    const int barX = w / 2 - barWidth / 2;
    const int leftTriangleWidth = barX / 6;
    const int rightTriangleWidth = (w - (barX + barWidth)) / 6;

    // 0. Check BEAR OFF Zone click
    if (pos.x() > w) {
        // Daca jucatorul curent este WHITE, zona de scoatere este index 24
        if (m_state.currentPlayer == WHITE) return 24;
        // Daca este BLACK, zona de scoatere este index -1
        if (m_state.currentPlayer == BLACK) return -1;
    }

    // 1. Check BAR click
    if (pos.x() >= barX && pos.x() <= barX + barWidth) {
        if (m_state.currentPlayer == WHITE && m_state.barWhite > 0) return Game::BAR_INDEX;
        if (m_state.currentPlayer == BLACK && m_state.barBlack > 0) return Game::BAR_INDEX;
        return -1;
    }

    bool top = (pos.y() < h / 2);
    int pointIndex = -1;

    if (pos.x() < barX) {
        int col = pos.x() / leftTriangleWidth;
        if (col >= 0 && col < 6) {
            pointIndex = top ? col : (18 + col);
        }
    }
    else if (pos.x() > barX + barWidth && pos.x() < w) { // pana in bear off zone
        int col = (pos.x() - (barX + barWidth)) / rightTriangleWidth;
        if (col >= 0 && col < 6) {
            pointIndex = top ? (6 + col) : (12 + col);
        }
    }
    return pointIndex;
}

void BoardWidget::mousePressEvent(QMouseEvent* event) {
    if (!m_game) return;

    int index = pointIndexFromPosition(event->pos());

    // Daca nu e o zona valida si nu e bear-off
    if (index == -1 && (event->pos().x() <= width() - BEAR_OFF_WIDTH)) {
        clearSelection();
        return;
    }

    // Selectie Noua
    if (m_selectedPoint == -1) {
        // Nu poti selecta zona de bear off ca sursa
        if (index == 24 || index == -1) return;

        if (m_game->canSelectPoint(index)) {
            selectPoint(index);
        }
        else {
            clearSelection();
        }
        return;
    }

    // Deselectare
    if (index == m_selectedPoint) {
        clearSelection();
        return;
    }

    // Executare Miscare (inclusiv Bear Off daca index este 24 sau -1)
    if (std::find(m_legalTargets.begin(), m_legalTargets.end(), index) != m_legalTargets.end()) {
        MoveResult result = m_game->makeMove(m_selectedPoint, index);
        if (result == MoveResult::Success) {
            clearSelection();
        }
        else {
            refreshState();
        }
        return;
    }

    // Schimbare selectie (daca dau click pe alta piesa a mea)
    if (m_game->canSelectPoint(index)) {
        selectPoint(index);
    }
    else {
        clearSelection();
    }
}