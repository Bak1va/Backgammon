#pragma once

#include <QMainWindow>
#include "ui_BackgammonUI.h"
#include "Game.h"
#include <memory>

class BoardWidget;

class BackgammonUI : public QMainWindow
{
    Q_OBJECT

public:
    BackgammonUI(QWidget *parent = nullptr);
    ~BackgammonUI();

private:
    std::unique_ptr<Game> m_game;  // logica jocului
    BoardWidget* m_boardWidget;    // widgetul care afișează tabla

    void setupUi();
    void setupMenu();
};

