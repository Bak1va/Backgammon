/**
 * @file BackgammonUI.cpp
 * @brief Implementation of the BackgammonUI main window class.
 *
 * This file implements the main window UI including layout, controls,
 * menu bar, and all event handlers for the Backgammon game.
 */

#include "BackgammonUI.hpp"

#include <QAction>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "BoardWidget.hpp"
#include "Game.hpp"

namespace {
    constexpr int WINDOW_WIDTH = 1000;     ///< Default window width
    constexpr int WINDOW_HEIGHT = 650;     ///< Default window height
    constexpr int DICE_SIZE = 48;          ///< Size of dice images in pixels
    constexpr int DICE_SPACING = 8;        ///< Spacing between dice images

    /// Stylesheet for the player label
    const QString PLAYER_LABEL_STYLE = "font-size: 14pt; font-weight: bold; padding: 5px;";

    /// Stylesheet for the roll dice button
    const QString ROLL_BUTTON_STYLE = "font-size: 12pt; padding: 10px 20px; background-color: #4CAF50; color: white;";

    /// Stylesheet for the status message label
    const QString STATUS_LABEL_STYLE = "font-size: 11pt; padding: 5px; background-color: #000000; color: white;";

    /// Template for dice image resource paths
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
      , m_diceImg2(nullptr)
      , m_openingDiceWhiteLabel(nullptr)
      , m_openingDiceBlackLabel(nullptr) {

    m_game->start();

    setupUi();
    setupMenu();

    if (m_boardWidget) {
        m_game->addObserver(m_boardWidget);
    }
}

BackgammonUI::~BackgammonUI() {
    // Remove observer before game is destroyed
    if (m_boardWidget && m_game) {
        m_game->removeObserver(m_boardWidget);
    }
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

    // Opening dice display (for determining first player)
    m_openingDiceWhiteLabel = new QLabel("White: -", controlPanel);
    m_openingDiceWhiteLabel->setStyleSheet("font-size: 11pt; padding: 5px;");
    controlLayout->addWidget(m_openingDiceWhiteLabel);

    m_openingDiceBlackLabel = new QLabel("Black: -", controlPanel);
    m_openingDiceBlackLabel->setStyleSheet("font-size: 11pt; padding: 5px;");
    controlLayout->addWidget(m_openingDiceBlackLabel);

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
    m_statusLabel = new QLabel("", central);
    m_statusLabel->setStyleSheet(STATUS_LABEL_STYLE);
    m_statusLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(m_statusLabel);

    setCentralWidget(central);

    updateUI();
}

void BackgammonUI::setupMenu() {
    auto *gameMenu = menuBar()->addMenu("Game");

    // ReSharper disable once CppDFAMemoryLeak
    auto *newGameAction = new QAction("New Game", this);
    connect(newGameAction, &QAction::triggered, this, &BackgammonUI::onNewGame);
    gameMenu->addAction(newGameAction);
}

void BackgammonUI::updateUI() {
    if (!m_game) {
        return;
    }

    const GameStateDTO state = m_game->getState();
    const GamePhase phase = m_game->getPhase();


    // Handle opening roll phases
    if (phase == GamePhase::OPENING_ROLL_WHITE) {
        updateOpeningRollDisplay();
        m_rollButton->setEnabled(true);
        m_rollButton->setText("Roll Dice");
        m_statusLabel->setText("Roll opening dice for White");
        updatePlayerDisplay(Color::WHITE);
        updateDiceDisplay(0, 0);
        return;
    }

    if (phase == GamePhase::OPENING_ROLL_BLACK) {
        updateOpeningRollDisplay();
        m_rollButton->setEnabled(true);
        m_rollButton->setText("Roll Dice");
        const QString whiteRoll = QString::number(m_game->getOpeningDiceWhite());
        m_statusLabel->setText("White rolled " + whiteRoll + ". Roll opening dice for Black");
        updatePlayerDisplay(Color::BLACK);
        updateDiceDisplay(m_game->getOpeningDiceWhite(), 0);
        return;
    }

    if (phase == GamePhase::OPENING_ROLL_COMPARE) {
        updateOpeningRollDisplay();
        m_rollButton->setEnabled(true);
        m_rollButton->setText("Roll Dice");
        const int whiteRoll = m_game->getOpeningDiceWhite();
        const int blackRoll = m_game->getOpeningDiceBlack();

        // Check for tie situation
        if (whiteRoll == blackRoll) {
            m_statusLabel->setText(QString("Tie! Both rolled %1. Press button to roll again").arg(whiteRoll));
            updatePlayerDisplay(Color::WHITE);
        } else {
            const Color startingPlayer = m_game->getCurrentPlayer();
            const QString playerText = (startingPlayer == Color::WHITE) ? "White" : "Black";
            m_statusLabel->setText(QString("White: %1, Black: %2. %3 starts - Press to roll for first move").arg(whiteRoll).arg(blackRoll).arg(playerText));
            updatePlayerDisplay(startingPlayer);
        }

        updateDiceDisplay(whiteRoll, blackRoll);
        if (m_openingDiceWhiteLabel) m_openingDiceWhiteLabel->hide();
        if (m_openingDiceBlackLabel) m_openingDiceBlackLabel->hide();
        return;
    }

    updatePlayerDisplay(state.currentPlayer);
    updateDiceDisplay(state.dice1, state.dice2);

    const bool diceConsumed = (state.dice1 == 0 && state.dice2 == 0);
    m_rollButton->setEnabled(diceConsumed);
    m_rollButton->setText("Roll Dice");

    if (diceConsumed) {
        const QString playerText = (state.currentPlayer == Color::WHITE) ? "White" : "Black";
        m_statusLabel->setText(playerText + "'s turn - Roll dice");
    } else {
        const QString playerText = (state.currentPlayer == Color::WHITE) ? "White" : "Black";
        m_statusLabel->setText(playerText + " - Make your moves");
    }
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

void BackgammonUI::updateOpeningRollDisplay() const {
    if (!m_game) {
        return;
    }

    const int whiteRoll = m_game->getOpeningDiceWhite();
    const int blackRoll = m_game->getOpeningDiceBlack();

    if (m_openingDiceWhiteLabel) {
        if (whiteRoll > 0) {
            m_openingDiceWhiteLabel->setText(QString("White: %1").arg(whiteRoll));
        } else {
            m_openingDiceWhiteLabel->setText("White: -");
        }
    }

    if (m_openingDiceBlackLabel) {
        if (blackRoll > 0) {
            m_openingDiceBlackLabel->setText(QString("Black: %1").arg(blackRoll));
        } else {
            m_openingDiceBlackLabel->setText("Black: -");
        }
    }
}

void BackgammonUI::onRollDice() {
    if (!m_game) {
        return;
    }

    const GamePhase phase = m_game->getPhase();

    if (phase == GamePhase::OPENING_ROLL_WHITE || phase == GamePhase::OPENING_ROLL_BLACK) {
        m_game->rollOpeningDice();
        updateUI();
        return;
    }

    if (phase == GamePhase::OPENING_ROLL_COMPARE) {
        const int whiteRoll = m_game->getOpeningDiceWhite();
        const int blackRoll = m_game->getOpeningDiceBlack();

        if (whiteRoll == blackRoll) {
            m_game->start();
            updateUI();
            return;
        }

        m_game->startGameAfterOpening();
        m_game->rollDice();
        updateUI();
        return;
    }

    if (phase != GamePhase::IN_PROGRESS) {
        m_statusLabel->setText("Please start a new game first!");
        return;
    }

    m_game->rollDice();
    updateUI();

    if (!m_game->hasMovesAvailable()) {
        const Color currentPlayer = m_game->getCurrentPlayer();
        const QString playerText = (currentPlayer == Color::WHITE) ? "White" : "Black";
        m_statusLabel->setText(playerText + " has no legal moves. Turn passed.");

        // Automatically pass the turn
        m_game->passTurn();
        updateUI();
    }
}

void BackgammonUI::onNewGame() {
    if (!m_game) {
        return;
    }


    if (m_openingDiceWhiteLabel) m_openingDiceWhiteLabel->show();
    if (m_openingDiceBlackLabel) m_openingDiceBlackLabel->show();

    m_game->start();
    updateUI();
}
