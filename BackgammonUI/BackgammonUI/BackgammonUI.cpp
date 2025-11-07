#include "BackgammonUI.h"
#include "BoardWidget.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QAction>

BackgammonUI::BackgammonUI(QWidget* parent)
    : QMainWindow(parent),
    m_game(std::make_unique<Game>()),
    m_boardWidget(nullptr)
{
    setupUi();
    setupMenu();

    // înregistrăm BoardWidget ca observer
    m_game->addObserver(m_boardWidget);

    // start joc
    m_game->start();
}

BackgammonUI::~BackgammonUI()
{
    if (m_boardWidget) {
        m_game->removeObserver(m_boardWidget);
    }
}

void BackgammonUI::setupUi()
{
    resize(1000, 600);
    setWindowTitle("Backgammon");

    auto* central = new QWidget(this);
    auto* layout = new QVBoxLayout(central);

    // BoardWidget primește pointer la IGame (GameLogic)
    m_boardWidget = new BoardWidget(central, m_game.get());
    layout->addWidget(m_boardWidget);

    central->setLayout(layout);
    setCentralWidget(central);
}

void BackgammonUI::setupMenu()
{
    auto* gameMenu = menuBar()->addMenu("Game");

    auto* newGameAction = new QAction("New Game", this);
    connect(newGameAction, &QAction::triggered, [this]() {
        m_game->start();        // resetează logica
        m_boardWidget->update(); // se va updata prin observer, dar e ok și asta
        });
    gameMenu->addAction(newGameAction);
}
