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
    QLabel* m_diceImg1;
    QLabel* m_diceImg2;

    QPixmap dicePixmap(int value) const;


    void setupUi();
    void setupMenu();
    void onRollDice();
};

