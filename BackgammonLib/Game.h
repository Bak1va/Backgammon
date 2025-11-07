#pragma once
#include <vector>
#include "IGame.h"
#include "IGameObserver.h"
#include "Board.h"

class Game : public IGame {
private:
	Board m_board;
	GamePhase m_phase;
	Color m_currentPlayer;
	int m_dice[2];
	bool m_diceRolled;
	std::vector<IGameObserver*> m_observers;

private:
	void notifyGameStarted();
	void notifyDiceRolled();
	void notifyMoveMade(int fromIndex, int toIndex, MoveResult result);
	void notifyTurnChanged();
	void notifyGameFinished(Color winner);
	void switchTurn();
	bool isMoveBlocked(int toIndex, Color player) const;
	bool canHit(int toIndex, Color player) const;
	int playerIndex(Color player) const;

public:
	Game();
	~Game() override;

	// IGame
	void start() override;
	GamePhase getPhase() const override;
	Color getCurrentPlayer() const override;
	void rollDice() override;
	const int* getDice() const override;
	bool hasMovesAvailable() const override;
	MoveResult makeMove(int fromIndex, int toIndex) override;
	int getColumnCount(int index) const override;
	Color getColumnColor(int index) const override;
	int getBarCount(Color player) const override;
	int getBorneOffCount(Color player) const override;
	void addObserver(IGameObserver* observer) override;
	void removeObserver(IGameObserver* observer) override;
};


