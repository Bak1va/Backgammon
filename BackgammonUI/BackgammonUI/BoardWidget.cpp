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
    const int midY = h / 2;
    const int colWidth = w / 12;

    // Triunghiuri sus (0..11)
    for (int i = 0; i < 12; ++i) {
        QPoint pts[3] = {
            QPoint(i * colWidth, 0),
            QPoint((i + 1) * colWidth, 0),
            QPoint(i * colWidth + colWidth / 2, midY - 10)
        };
        QColor c = (i % 2 == 0) ? QColor(200, 180, 120) : QColor(80, 50, 30);
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawPolygon(pts, 3);
    }

    // Triunghiuri jos (12..23)
    for (int i = 0; i < 12; ++i) {
        QPoint pts[3] = {
            QPoint(i * colWidth, h),
            QPoint((i + 1) * colWidth, h),
            QPoint(i * colWidth + colWidth / 2, midY + 10)
        };
        QColor c = (i % 2 == 0) ? QColor(80, 50, 30) : QColor(200, 180, 120);
        p.setBrush(c);
        p.setPen(Qt::NoPen);
        p.drawPolygon(pts, 3);
    }
}

void BoardWidget::drawPieces(QPainter& p)
{
    const int w = width();
    const int h = height();
    const int midY = h / 2;
    const int colWidth = w / 12;
    const int pieceRadius = std::min(colWidth, h / 12) / 2 - 2;

    // indexe 0..11 => sus, 12..23 => jos
    for (int idx = 0; idx < 24; ++idx) {
        int count = m_state.pieceCounts[idx];
        Color col = m_state.colors[idx];
        if (count <= 0 || col == NONE) continue;

        bool top = (idx < 12);
        int colIndex = idx % 12;

        int xCenter = colIndex * colWidth + colWidth / 2;

        for (int i = 0; i < count && i < 5; ++i) { // max 5 desenate, restul pot fi "număr"
            int yCenter;
            if (top) {
                yCenter = 10 + pieceRadius + i * (pieceRadius * 2 + 2);
            }
            else {
                yCenter = h - 10 - pieceRadius - i * (pieceRadius * 2 + 2);
            }

            QColor qc = (col == WHITE) ? QColor(240, 240, 240) : QColor(20, 20, 20);
            p.setBrush(qc);
            p.setPen(QPen(Qt::black, 1));
            p.drawEllipse(QPoint(xCenter, yCenter), pieceRadius, pieceRadius);
        }

        // dacă sunt mai mult de 5, scriem numărul
        if (count > 5) {
            int textY = top ? (10 + pieceRadius) : (h - 10 - pieceRadius);
            p.setPen(Qt::yellow);
            p.drawText(xCenter - 8, textY, QString::number(count));
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
