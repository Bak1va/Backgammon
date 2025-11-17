#pragma once

#include <QMainWindow>
#include "IGame.h"
#include <memory>

class BoardWidget;
class QPushButton;
class QLabel;

class BackgammonUI : public QMainWindow
{
    Q_OBJECT

public:
    BackgammonUI(QWidget *parent = nullptr);
    ~BackgammonUI();
    
    void updateUI();  

private:
    std::unique_ptr<IGame> m_game;  
    BoardWidget* m_boardWidget;     
    
    // UI controls
    QPushButton* m_rollButton;
    QLabel* m_diceLabel;
    QLabel* m_playerLabel;
    QLabel* m_statusLabel;

    void setupUi();
    void setupMenu();
    void onRollDice();
};

