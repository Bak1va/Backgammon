#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BackgammonUI.h"

class BackgammonUI : public QMainWindow
{
    Q_OBJECT

public:
    BackgammonUI(QWidget *parent = nullptr);
    ~BackgammonUI();

private:
    Ui::BackgammonUIClass ui;
};

