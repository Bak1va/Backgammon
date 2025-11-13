#include <algorithm>
#include <random>
#include "Game.h"

Game::Game()
	: m_phase(GamePhase::NotStarted), m_currentPlayer(WHITE), m_dice{0, 0}, m_diceRolled(false) {
}

Game::~Game() {
}

void Game::start() {
	m_board = Board(); // reset board
	m_phase = GamePhase::InProgress;
	m_currentPlayer = WHITE;
	m_dice[0] = m_dice[1] = 0;
	m_diceRolled = false;
	notifyGameStarted();
}

GamePhase Game::getPhase() const {
	return m_phase;
}

Color Game::getCurrentPlayer() const {
	return m_currentPlayer;
}

void Game::rollDice() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> dist(1, 6);
	m_dice[0] = dist(gen);
	m_dice[1] = dist(gen);
	m_diceRolled = true;
	notifyDiceRolled();
}

const int* Game::getDice() const {
	return m_dice;
}

bool Game::hasMovesAvailable() const {
	// keep it simple: if dice rolled, assume at least one move exists unless all entries are blocked
	if (!m_diceRolled) return false;
	for (int i = 0; i < 24; ++i) {
		if (m_board.getColumn(i).getPieceCount() > 0 && m_board.getColumn(i).getColor() == m_currentPlayer) {
			int d1Target = (m_currentPlayer == WHITE) ? i + m_dice[0] : i - m_dice[0];
			int d2Target = (m_currentPlayer == WHITE) ? i + m_dice[1] : i - m_dice[1];
			if (d1Target >= 0 && d1Target < 24 && !isMoveBlocked(d1Target, m_currentPlayer)) return true;
			if (d2Target >= 0 && d2Target < 24 && !isMoveBlocked(d2Target, m_currentPlayer)) return true;
		}
	}
	return false;
}

MoveResult Game::makeMove(int fromIndex, int toIndex) {
	if (m_phase != GamePhase::InProgress) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::GameNotStarted);
		return MoveResult::GameNotStarted;
	}
	if (!m_diceRolled) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::DiceNotRolled);
		return MoveResult::DiceNotRolled;
	}
	if (fromIndex < 0 || fromIndex >= 24) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::InvalidFromColumn);
		return MoveResult::InvalidFromColumn;
	}
	if (toIndex < 0 || toIndex >= 24) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::InvalidToColumn);
		return MoveResult::InvalidToColumn;
	}
	Column& fromCol = m_board.getColumn(fromIndex);
	if (fromCol.getPieceCount() == 0 || fromCol.getColor() != m_currentPlayer) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::InvalidMove);
		return MoveResult::InvalidMove;
	}

	// validate dice distance (simple, no doubling handling beyond equality to either die)
	int distance = (m_currentPlayer == WHITE) ? (toIndex - fromIndex) : (fromIndex - toIndex);
	if (!(distance == m_dice[0] || distance == m_dice[1])) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::InvalidMove);
		return MoveResult::InvalidMove;
	}

	if (isMoveBlocked(toIndex, m_currentPlayer)) {
		notifyMoveMade(fromIndex, toIndex, MoveResult::BlockedByOpponent);
		return MoveResult::BlockedByOpponent;
	}

	// execute move; handle hit if exactly one opponent piece
	Column& toCol = m_board.getColumn(toIndex);
	if (canHit(toIndex, m_currentPlayer)) {
		Color opponent = (m_currentPlayer == WHITE) ? BLACK : WHITE;
		toCol.removePiece();
		m_board.incrementBarCount(playerIndex(opponent));
	}
	// move piece
	fromCol.removePiece();
	toCol.addPiece(m_currentPlayer);

	// consume die used
	if (distance == m_dice[0]) m_dice[0] = 0; else m_dice[1] = 0;
	if (m_dice[0] == 0 && m_dice[1] == 0 || !hasMovesAvailable()) {
		m_diceRolled = false;
		switchTurn();
	}

	// simplistic win: all 15 borne off (we do not implement bearing off fully here)
	if (m_board.getBorneOffCount(playerIndex(m_currentPlayer)) >= 15) {
		m_phase = GamePhase::Finished;
		notifyGameFinished(m_currentPlayer);
	}

	notifyMoveMade(fromIndex, toIndex, MoveResult::Success);
	return MoveResult::Success;
}

int Game::getColumnCount(int index) const {
	return m_board.getColumn(index).getPieceCount();
}

Color Game::getColumnColor(int index) const {
	return m_board.getColumn(index).getColor();
}

int Game::getBarCount(Color player) const {
	return m_board.getBarCount(playerIndex(player));
}

int Game::getBorneOffCount(Color player) const {
	return m_board.getBorneOffCount(playerIndex(player));
}

GameStateDTO Game::getState() const {
	GameStateDTO s;

	for (int i = 0; i < 24; ++i) {
		const Column& col = m_board.getColumn(i);
		s.pieceCounts[i] = col.getPieceCount();
		s.colors[i] = col.getColor();
	}

	s.barWhite = m_board.getBarCount(0);
	s.barBlack = m_board.getBarCount(1);

	s.borneOffWhite = m_board.getBorneOffCount(0);
	s.borneOffBlack = m_board.getBorneOffCount(1);

	s.currentPlayer = m_currentPlayer;

	s.dice1 = m_dice[0];
	s.dice2 = m_dice[1];

	return s;
}

bool Game::canSelectPoint(int index) const {
	if (m_phase != GamePhase::InProgress) return false;
	if (index < 0 || index >= 24) return false;

	const Column& col = m_board.getColumn(index);
	if (col.getPieceCount() == 0) return false;
	if (col.getColor() != m_currentPlayer) return false;
	if (!m_diceRolled) return false;

	int distances[2] = { m_dice[0], m_dice[1] };
	for (int d : distances) {
		if (d <= 0) continue;

		int toIndex = (m_currentPlayer == WHITE)
			? index + d
			: index - d;

		if (toIndex < 0 || toIndex >= 24) {

			continue;
		}

		if (!isMoveBlocked(toIndex, m_currentPlayer)) {
			return true;
		}
	}

	return false;
}


std::vector<int> Game::getLegalTargets(int fromIndex) const {
	std::vector<int> targets;

	if (!canSelectPoint(fromIndex)) {
		return targets;
	}

	if (!m_diceRolled) {
		return targets;
	}

	int dirs[2] = { m_dice[0], m_dice[1] };

	for (int d : dirs) {
		if (d <= 0) continue;

		int toIndex = (m_currentPlayer == WHITE)
			? fromIndex + d
			: fromIndex - d;

		if (toIndex < 0 || toIndex >= 24) {

			continue;
		}

		if (isMoveBlocked(toIndex, m_currentPlayer)) {
			continue;
		}

		if (std::find(targets.begin(), targets.end(), toIndex) == targets.end()) {
			targets.push_back(toIndex);
		}
	}

	return targets;
}


void Game::addObserver(IGameObserver* observer) {
	if (!observer) return;
	if (std::find(m_observers.begin(), m_observers.end(), observer) == m_observers.end()) {
		m_observers.push_back(observer);
	}
}

void Game::removeObserver(IGameObserver* observer) {
	auto it = std::remove(m_observers.begin(), m_observers.end(), observer);
	m_observers.erase(it, m_observers.end());
}

void Game::notifyGameStarted() {
	for (auto* o : m_observers) o->onGameStarted();
}

void Game::notifyDiceRolled() {
	for (auto* o : m_observers) o->onDiceRolled(m_currentPlayer, m_dice[0], m_dice[1]);
}

void Game::notifyMoveMade(int fromIndex, int toIndex, MoveResult result) {
	for (auto* o : m_observers) o->onMoveMade(m_currentPlayer, fromIndex, toIndex, result);
}

void Game::notifyTurnChanged() {
	for (auto* o : m_observers) o->onTurnChanged(m_currentPlayer);
}

void Game::notifyGameFinished(Color winner) {
	for (auto* o : m_observers) o->onGameFinished(winner);
}

void Game::switchTurn() {
	m_currentPlayer = (m_currentPlayer == WHITE) ? BLACK : WHITE;
	notifyTurnChanged();
}

bool Game::isMoveBlocked(int toIndex, Color player) const {
	const Column& toCol = m_board.getColumn(toIndex);
	if (toCol.getPieceCount() >= 2 && toCol.getColor() != player) return true;
	return false;
}

bool Game::canHit(int toIndex, Color player) const {
	const Column& toCol = m_board.getColumn(toIndex);
	return toCol.getPieceCount() == 1 && toCol.getColor() != player && toCol.getColor() != NONE;
}

int Game::playerIndex(Color player) const {
	return (player == WHITE) ? 0 : 1;
}


