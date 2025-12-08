#pragma once
#include "Color.hpp"
#include "MoveResult.hpp"

class IGameObserver {
public:
	virtual ~IGameObserver() = default;
	virtual void onGameStarted() {}
	virtual void onDiceRolled(Color player, int d1, int d2) {}
	virtual void onMoveMade(Color player, int fromIndex, int toIndex, MoveResult result) {}
	virtual void onTurnChanged(Color currentPlayer) {}
	virtual void onGameFinished(Color winner) {}
};


