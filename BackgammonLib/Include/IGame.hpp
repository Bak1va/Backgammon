#pragma once
#include "Color.hpp"
#include "MoveResult.hpp"
#include "GameStateDTO.hpp"
#include <vector>

class IGameObserver;

enum class GamePhase {
	NOT_STARTED,
	IN_PROGRESS,
	FINISHED
};

class IGame {
public:
	virtual ~IGame() = default;

virtual void start() = 0;
	virtual GamePhase getPhase() const = 0;

	virtual Color getCurrentPlayer() const = 0;
	virtual void rollDice() = 0;
	virtual const std::array<int, 2> getDice() const = 0;
	virtual bool hasMovesAvailable() const = 0;

	virtual MoveResult makeMove(int fromIndex, int toIndex) = 0;

	virtual int getColumnCount(int index) const = 0;
	virtual Color getColumnColor(int index) const = 0;
	virtual int getBarCount(Color player) const = 0;
	virtual int getBorneOffCount(Color player) const = 0;

	virtual void addObserver(IGameObserver* observer) = 0;
	virtual void removeObserver(IGameObserver* observer) = 0;

	virtual GameStateDTO getState() const = 0;

	virtual bool canSelectPoint(int index) const = 0;
	virtual std::vector<int> getLegalTargets(int fromIndex) const = 0;
};


