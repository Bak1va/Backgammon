#pragma once

#include <memory>

#include <QMainWindow>

#include "IGame.hpp"

class BoardWidget;
class QPushButton;
class QLabel;

class BackgammonUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit BackgammonUI(QWidget* parent = nullptr);
    ~BackgammonUI() override;

    void updateUI();

    private slots:
        void onRollDice();
    void onNewGame();

private:
    void setupUi();
    void setupMenu();
    void updateDiceDisplay(int dice1, int dice2) const;
    void updatePlayerDisplay(Color currentPlayer) const;
    void updateStatusDisplay(bool diceConsumed, Color currentPlayer) const;

    [[nodiscard]] QPixmap dicePixmap(int value) const;

    std::unique_ptr<IGame> m_game;

    BoardWidget* m_boardWidget;
    QPushButton* m_rollButton;
    QLabel* m_playerLabel;
    QLabel* m_statusLabel;
    QLabel* m_diceImg1;
    QLabel* m_diceImg2;
};