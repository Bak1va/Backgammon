#include "BoardWidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <algorithm>

BoardWidget::BoardWidget(QWidget* parent, IGame* game)
    : QWidget(parent),
    m_game(game),
    m_selectedPoint(-1)
{
    setMinimumSize(900, 500);

    if (m_game) {
        m_state = m_game->getState();
    }
}

void BoardWidget::refreshState()
{
    if (m_game) {
        m_state = m_game->getState();
    }
    update(); // redraw
}

void BoardWidget::clearSelection()
{
    m_selectedPoint = -1;
    m_legalTargets.clear();
    update();
}

void BoardWidget::selectPoint(int index)
{
    if (!m_game) return;

    m_selectedPoint = index;
    m_legalTargets = m_game->getLegalTargets(index);
    update();
}

/* ==== IGameObserver callbacks ==== */

void BoardWidget::onGameStarted()
{
    clearSelection();
    refreshState();
}

void BoardWidget::onDiceRolled(Color, int, int)
{
    refreshState();
}

void BoardWidget::onMoveMade(Color, int, int, MoveResult)
{
    // Indiferent dacă e success sau nu, re-citim starea
    refreshState();
}

void BoardWidget::onTurnChanged(Color)
{
    clearSelection();
    refreshState();
}

void BoardWidget::onGameFinished(Color)
{
    clearSelection();
    refreshState();
}

/* ==== Painting ==== */

void BoardWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    // fundal
    p.fillRect(rect(), QColor(30, 90, 50)); // verde inchis

    drawBoard(p);
    drawPieces(p);
    drawHighlights(p);
}

void BoardWidget::drawBoard(QPainter& p)
{
    const int w = width();
    const int h = height();
	int barWidth = 40;
    int barX = w/ 2 - barWidth / 2;
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
    // Draw left side triangles 
    for (int i = 0; i < 6; i++) {

        drawTriangle(p, i * triangleWidth, 0, triangleWidth, triangleHeight, false, i);

        drawTriangle(p, i * triangleWidth, h, triangleWidth, triangleHeight, true, 18 + i);
    }

    int rightStartX = barX + barWidth;
    triangleWidth = rightSideWidth / 6;  

    for (int i = 0; i < 6; i++) {
        drawTriangle(p, rightStartX + i * triangleWidth, 0, triangleWidth, triangleHeight, false, 6 + i);

        drawTriangle(p, rightStartX + i * triangleWidth, h, triangleWidth, triangleHeight, true, 12 + i);
    }

}
void BoardWidget::drawTriangle(QPainter& p, int x, int y, int width, int height, bool pointUp, int pointIndex) {
    QPolygon triangle;

    if (pointUp) {
        triangle << QPoint(x, y)
            << QPoint(x + width, y)
            << QPoint(x + width / 2, y - height);
    }
    else {
        triangle << QPoint(x, y)
            << QPoint(x + width, y)
            << QPoint(x + width / 2, y + height);
    }

    // Alternate colors
    QColor color = (pointIndex % 2 == 0) ? QColor(100, 50, 20) : QColor(220, 180, 140);
    p.setBrush(color);
    p.setPen(QPen(Qt::black, 1));
    p.drawPolygon(triangle);
}

void BoardWidget::drawPieces(QPainter& painter) {
    int boardWidth = width();
    int boardHeight = height();
    int barWidth = 40;
    int barX = boardWidth / 2 - barWidth / 2;

    int leftSideWidth = barX;
    int triangleWidth = leftSideWidth / 6;
    int pieceRadius = triangleWidth / 2 - 15;  // Piece slightly smaller than triangle width


    GameStateDTO state = m_game->getState();

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

    drawBarPieces(painter, state);
}

void BoardWidget::drawPiecesAtPoint(QPainter& painter, int centerX, int centerY,
    int count, Color color, int radius, bool isTopRow) {
    // Set piece color
    QColor pieceColor = (color == WHITE) ? QColor(255, 255, 255) : QColor(0, 0, 0);
    QColor borderColor = (color == WHITE) ? QColor(200, 200, 200) : QColor(50, 50, 50);

    painter.setBrush(pieceColor);
    painter.setPen(QPen(borderColor, 2));


    int spacing = radius * 2 + 3;  
    int maxVisiblePieces = 5;  // Show max 5 pieces, then show count

    for (int i = 0; i < std::min(count, maxVisiblePieces); i++) {
        int yOffset = isTopRow ? (i * spacing) : -(i * spacing);
        int pieceY = centerY + yOffset;

        painter.drawEllipse(QPoint(centerX, pieceY), radius, radius);
    }

    // If more than maxVisiblePieces, draw count on top piece
    if (count > maxVisiblePieces) {
        int yOffset = isTopRow ? ((maxVisiblePieces - 1) * spacing) : -((maxVisiblePieces - 1) * spacing);
        int textY = centerY + yOffset;

        painter.setPen(QPen(color == WHITE ? Qt::black : Qt::white));
        painter.setFont(QFont("Arial", 10, QFont::Bold));
        painter.drawText(QRect(centerX - radius, textY - radius, radius * 2, radius * 2),
            Qt::AlignCenter, QString::number(count));
    }
}
void BoardWidget::drawBarPieces(QPainter& painter, const GameStateDTO& state) {
    int boardWidth = width();
    int boardHeight = height();
    int barWidth = 40;
    int barX = boardWidth / 2 - barWidth / 2;
    int pieceRadius = 15;

    int centerX = barX + barWidth / 2;

    // Draw white pieces on bar
    if (state.barWhite > 0) {
        int startY = boardHeight / 4;
        for (int i = 0; i < state.barWhite; i++) {
            painter.setBrush(Qt::white);
            painter.setPen(QPen(QColor(200, 200, 200), 2));
            painter.drawEllipse(QPoint(centerX, startY + i * (pieceRadius * 2 + 2)),
                pieceRadius, pieceRadius);
        }
    }

    // Draw black pieces on bar
    if (state.barBlack > 0) {
        int startY = boardHeight * 3 / 4;
        for (int i = 0; i < state.barBlack; i++) {
            painter.setBrush(Qt::black);
            painter.setPen(QPen(QColor(50, 50, 50), 2));
            painter.drawEllipse(QPoint(centerX, startY + i * (pieceRadius * 2 + 2)),
                pieceRadius, pieceRadius);
        }
    }
}

void BoardWidget::drawHighlights(QPainter& p)
{
    if (m_selectedPoint < 0) return;

    const int w = width();
    const int h = height();
    const int colWidth = w / 12;

    // highlight pentru punctul selectat
    int from = m_selectedPoint;
    bool top = (from < 12);
    int colIndex = from % 12;
    QRect fromRect(colIndex * colWidth, top ? 0 : h / 2,
        colWidth, h / 2);
    p.setBrush(QColor(255, 255, 0, 60));
    p.setPen(Qt::NoPen);
    p.drawRect(fromRect);

    // highlight pentru ținte
    p.setBrush(QColor(0, 255, 0, 60));
    for (int t : m_legalTargets) {
        bool tTop = (t < 12);
        int tCol = t % 12;
        QRect r(tCol * colWidth, tTop ? 0 : h / 2,
            colWidth, h / 2);
        p.drawRect(r);
    }
}

/* ==== Input: click pentru selectare si mutare ==== */

int BoardWidget::pointIndexFromPosition(const QPoint& pos) const
{
    const int w = width();
    const int h = height();
    const int colWidth = w / 12;

    int col = pos.x() / colWidth;
    if (col < 0 || col >= 12) return -1;

    bool top = (pos.y() < h / 2);
    int index = top ? col : (12 + col);

    if (index < 0 || index >= 24) return -1;
    return index;
}

void BoardWidget::mousePressEvent(QMouseEvent* event)
{
    if (!m_game) return;

    int index = pointIndexFromPosition(event->pos());
    if (index == -1) {
        clearSelection();
        return;
    }

    // Nimic selectat încă → încercăm să selectăm sursa
    if (m_selectedPoint == -1) {
        if (m_game->canSelectPoint(index)) {
            selectPoint(index);
        }
        else {
            clearSelection();
        }
        return;
    }

    // Dacă dai click pe aceeași coloană → deselectăm
    if (index == m_selectedPoint) {
        clearSelection();
        return;
    }

    // Dacă index este o țintă legală → facem mutarea
    if (std::find(m_legalTargets.begin(), m_legalTargets.end(), index) != m_legalTargets.end()) {
        MoveResult result = m_game->makeMove(m_selectedPoint, index);

        if (result == MoveResult::Success) {
            clearSelection();
            // onMoveMade / onTurnChanged vor face refreshState prin observer
        }
        else {
            // mutare invalidă: reîmprospătăm ca să reflectăm starea corectă
            refreshState();
        }
        return;
    }

    // Altfel: încercăm să schimbăm selecția pe un alt punct valid
    if (m_game->canSelectPoint(index)) {
        selectPoint(index);
    }
    else {
        clearSelection();
    }
}
