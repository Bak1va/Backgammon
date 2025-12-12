#include "BackgammonUI.hpp"

#include <QAction>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "BoardWidget.hpp"
#include "Game.hpp"

namespace {
    constexpr int WINDOW_WIDTH = 1000;
    constexpr int WINDOW_HEIGHT = 650;
    constexpr int DICE_SIZE = 48;
    constexpr int DICE_SPACING = 8;

    const QString PLAYER_LABEL_STYLE = "font-size: 14pt; font-weight: bold; padding: 5px;";
    const QString ROLL_BUTTON_STYLE = "font-size: 12pt; padding: 10px 20px; background-color: #4CAF50; color: white;";
    const QString STATUS_LABEL_STYLE = "font-size: 11pt; padding: 5px; background-color: #f0f0f0;";
    const QString DICE_PATH_TEMPLATE = ":/BackgammonUI/assets/assets/dice%1.png";
}

BackgammonUI::BackgammonUI(QWidget *parent)
    : QMainWindow(parent)
      , m_game(std::make_unique<Game>())
      , m_boardWidget(nullptr)
      , m_rollButton(nullptr)
      , m_playerLabel(nullptr)
      , m_statusLabel(nullptr)
      , m_diceImg1(nullptr)
      , m_diceImg2(nullptr) {
    setupUi();
    setupMenu();

    if (m_boardWidget) {
        m_game->addObserver(m_boardWidget);
    }

    m_game->start();
}

BackgammonUI::~BackgammonUI() {
    // Remove observer before game is destroyed
    if (m_boardWidget && m_game) {
        m_game->removeObserver(m_boardWidget);
    }
    // Qt parent-child relationship handles widget cleanup automatically
}

QPixmap BackgammonUI::dicePixmap(int value) const
{
	if (value >= 1 && value <= 6) {
		qDebug() << "Loading dice:" << value;
		QString path = QString(":/BackgammonUI/assets/dice%1.png").arg(value);
		QPixmap pixmap(path);
		return pixmap;
	}
	return QPixmap();
}

void BackgammonUI::setupUi() {
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    setWindowTitle("Backgammon");

    auto *central = new QWidget(this); // ReSharper disable once CppDFAMemoryLeak
    auto *mainLayout = new QVBoxLayout(central); // ReSharper disable once CppDFAMemoryLeak

    // Control panel
    auto *controlPanel = new QWidget(central); // ReSharper disable once CppDFAMemoryLeak
    auto *controlLayout = new QHBoxLayout(controlPanel); // ReSharper disable once CppDFAMemoryLeak

    // Player label
    m_playerLabel = new QLabel("Current Player: White", controlPanel);
    m_playerLabel->setStyleSheet(PLAYER_LABEL_STYLE);
    controlLayout->addWidget(m_playerLabel);

    controlLayout->addStretch();

    // Dice display
    // ReSharper disable once CppDFAMemoryLeak
    auto *diceBox = new QWidget(controlPanel); // ReSharper disable once CppDFAMemoryLeak
    auto *diceLayout = new QHBoxLayout(diceBox);
    diceLayout->setContentsMargins(0, 0, 0, 0);
    diceLayout->setSpacing(DICE_SPACING);

    m_diceImg1 = new QLabel(diceBox);
    m_diceImg2 = new QLabel(diceBox);

    m_diceImg1->setFixedSize(DICE_SIZE, DICE_SIZE);
    m_diceImg2->setFixedSize(DICE_SIZE, DICE_SIZE);
    m_diceImg1->setScaledContents(true);
    m_diceImg2->setScaledContents(true);

    diceLayout->addWidget(m_diceImg1);
    diceLayout->addWidget(m_diceImg2);

    controlLayout->addWidget(diceBox);

    // Roll dice button
    m_rollButton = new QPushButton("Roll Dice", controlPanel);
    m_rollButton->setStyleSheet(ROLL_BUTTON_STYLE);
    m_rollButton->setEnabled(true);
    connect(m_rollButton, &QPushButton::clicked, this, &BackgammonUI::onRollDice);
    controlLayout->addWidget(m_rollButton);

    mainLayout->addWidget(controlPanel);

    // Board widget
    m_boardWidget = new BoardWidget(central, m_game.get(), this);
    mainLayout->addWidget(m_boardWidget);

    // Status label
    m_statusLabel = new QLabel("Welcome! Click 'Roll Dice' to start your turn.", central);
    m_statusLabel->setStyleSheet(STATUS_LABEL_STYLE);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(central);

    updateUI();
}

void BackgammonUI::setupMenu() {
    auto *gameMenu = menuBar()->addMenu("Game");

    auto *newGameAction = new QAction("New Game", this); // ReSharper disable once CppDFAMemoryLeak
    connect(newGameAction, &QAction::triggered, this, &BackgammonUI::onNewGame);
    gameMenu->addAction(newGameAction);
}

void BackgammonUI::updateUI() {
    if (!m_game) {
        return;
    }

    const GameStateDTO state = m_game->getState();

    updatePlayerDisplay(state.currentPlayer);
    updateDiceDisplay(state.dice1, state.dice2);

    const bool diceConsumed = (state.dice1 == 0 && state.dice2 == 0);
    m_rollButton->setEnabled(diceConsumed);

    updateStatusDisplay(diceConsumed, state.currentPlayer);
}

void BackgammonUI::updateDiceDisplay(int dice1, int dice2) const {
    if (!m_diceImg1 || !m_diceImg2) {
        return;
    }

    if (dice1 > 0) {
        m_diceImg1->setPixmap(dicePixmap(dice1));
    } else {
        m_diceImg1->clear();
    }

    if (dice2 > 0) {
        m_diceImg2->setPixmap(dicePixmap(dice2));
    } else {
        m_diceImg2->clear();
    }
}

void BackgammonUI::updatePlayerDisplay(Color currentPlayer) const {
    if (!m_playerLabel) {
        return;
    }

    const QString playerText = (currentPlayer == Color::WHITE) ? "White" : "Black";
    m_playerLabel->setText("Current Player: " + playerText);
}

void BackgammonUI::updateStatusDisplay(bool diceConsumed, Color currentPlayer) const {
    if (!m_statusLabel) {
        return;
    }

    const QString playerText = (currentPlayer == Color::WHITE) ? "White" : "Black";

    if (diceConsumed) {
        m_statusLabel->setText(playerText + "'s turn - Roll the dice!");
    } else {
        m_statusLabel->setText(playerText + " - Make your moves");
    }
}

void BackgammonUI::onRollDice() {
    if (!m_game) {
        return;
    }

    if (m_game->getPhase() != GamePhase::IN_PROGRESS) {
        QMessageBox::information(this, "Backgammon", "Please start a new game first!");
        return;
    }

    m_game->rollDice();
    updateUI();

    if (!m_game->hasMovesAvailable()) {
        const Color currentPlayer = m_game->getCurrentPlayer();
        const QString playerText = (currentPlayer == Color::WHITE) ? "White" : "Black";
        m_statusLabel->setText(playerText + " has no legal moves. Turn will pass...");
    }
}

void BackgammonUI::onNewGame() {
    if (!m_game) {
        return;
    }

    m_game->start();
    updateUI();
}
