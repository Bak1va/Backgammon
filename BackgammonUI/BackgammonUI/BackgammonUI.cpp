#include "BackgammonUI.h"
#include "BoardWidget.h"
#include "Game.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QAction>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <iostream>

BackgammonUI::BackgammonUI(QWidget* parent)
	: QMainWindow(parent),
	m_game(std::make_unique<Game>()),
	m_boardWidget(nullptr),
	m_rollButton(nullptr),
	m_diceLabel(nullptr),
	m_playerLabel(nullptr),
	m_statusLabel(nullptr)
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

QPixmap BackgammonUI::dicePixmap(int value) const
{

	if (value >= 1 && value <= 6) {
		qDebug() << value;
		return QPixmap(QString(":/BackgammonUI/assets/assets/dice%1.png").arg(value));
	}
	return QPixmap();


}

void BackgammonUI::setupUi()
{
	resize(1000, 650);
	setWindowTitle("Backgammon");

	auto* central = new QWidget(this);
	auto* mainLayout = new QVBoxLayout(central);

	// Control panel at the top
	auto* controlPanel = new QWidget(central);
	auto* controlLayout = new QHBoxLayout(controlPanel);

	m_playerLabel = new QLabel("Current Player: White", controlPanel);
	m_playerLabel->setStyleSheet("font-size: 14pt; font-weight: bold; padding: 5px;");
	controlLayout->addWidget(m_playerLabel);

	controlLayout->addStretch();

	auto* diceBox = new QWidget(controlPanel);
	auto* diceLayout = new QHBoxLayout(diceBox);
	diceLayout->setContentsMargins(0, 0, 0, 0);
	diceLayout->setSpacing(8);

	m_diceImg1 = new QLabel(diceBox);
	m_diceImg2 = new QLabel(diceBox);

	// dimensiuni vizuale — ajustează după preferințe
	m_diceImg1->setFixedSize(48, 48);
	m_diceImg2->setFixedSize(48, 48);

	// dacă vrei, poți lăsa ScaledContents și setezi direct pixmapul
	m_diceImg1->setScaledContents(true);
	m_diceImg2->setScaledContents(true);

	// placeholder inițial (gol)
	m_diceImg1->clear();
	m_diceImg2->clear();

	diceLayout->addWidget(m_diceImg1);
	diceLayout->addWidget(m_diceImg2);

	controlLayout->addWidget(diceBox);

	// Roll dice button
	m_rollButton = new QPushButton("Roll Dice", controlPanel);
	m_rollButton->setStyleSheet("font-size: 12pt; padding: 10px 20px; background-color: #4CAF50; color: white;");
	m_rollButton->setEnabled(true);
	connect(m_rollButton, &QPushButton::clicked, this, &BackgammonUI::onRollDice);
	controlLayout->addWidget(m_rollButton);

	controlPanel->setLayout(controlLayout);
	mainLayout->addWidget(controlPanel);

	m_boardWidget = new BoardWidget(central, m_game.get(), this);
	mainLayout->addWidget(m_boardWidget);

	m_statusLabel = new QLabel("Welcome! Click 'Roll Dice' to start your turn.", central);
	m_statusLabel->setStyleSheet("font-size: 11pt; padding: 5px; background-color: #f0f0f0;");
	m_statusLabel->setAlignment(Qt::AlignCenter);
	mainLayout->addWidget(m_statusLabel);

	central->setLayout(mainLayout);
	setCentralWidget(central);

	updateUI();
}

void BackgammonUI::setupMenu()
{
	auto* gameMenu = menuBar()->addMenu("Game");

	auto* newGameAction = new QAction("New Game", this);
	connect(newGameAction, &QAction::triggered, [this]() {
		m_game->start();
		updateUI();
		});
	gameMenu->addAction(newGameAction);
}

void BackgammonUI::updateUI()
{
	if (!m_game) return;

	GameStateDTO state = m_game->getState();

	QString playerText = (state.currentPlayer == WHITE) ? "White" : "Black";
	m_playerLabel->setText("Current Player: " + playerText);

	if (state.dice1 > 0) {
		QPixmap p1 = dicePixmap(state.dice1);
		m_diceImg1->setPixmap(p1);
	}
	else {
		m_diceImg1->clear(); // sau placeholder
	}

	if (state.dice2 > 0) {
		QPixmap p2 = dicePixmap(state.dice2);
		m_diceImg2->setPixmap(p2);
	}
	else {
		m_diceImg2->clear();
	}

	// Update button state - disable if dice already rolled and not used
	bool diceRolled = (state.dice1 > 0 && state.dice2 > 0);
	m_rollButton->setEnabled(!diceRolled);

	// Update status
	if (!diceRolled) {
		m_statusLabel->setText(playerText + "'s turn - Roll the dice!");
	}
	else {
		m_statusLabel->setText(playerText + " - Make your moves");
	}
}

void BackgammonUI::onRollDice()
{
	if (!m_game) return;

	if (m_game->getPhase() != GamePhase::InProgress) {
		QMessageBox::information(this, "Backgammon", "Please start a new game first!");
		return;
	}

	m_game->rollDice();
	updateUI();

	// Check if player has any moves
	if (!m_game->hasMovesAvailable()) {
		QString playerText = (m_game->getCurrentPlayer() == WHITE) ? "White" : "Black";
		m_statusLabel->setText(playerText + " has no legal moves. Turn will pass...");
		// Note: You'll need to add logic to automatically pass turn here
	}
}
