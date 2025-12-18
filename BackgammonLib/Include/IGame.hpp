#pragma once
#include "Color.hpp"
#include "MoveResult.hpp"
#include "GameStateDTO.hpp"
#include <vector>

class IGameObserver;

enum class GamePhase {
	NOT_STARTED,
	OPENING_ROLL_WHITE,
	OPENING_ROLL_BLACK,
	OPENING_ROLL_COMPARE,
	IN_PROGRESS,
	FINISHED
};

class IGame {
public:
	virtual ~IGame() = default;

	// lifecycle
	virtual void start() = 0;
	virtual GamePhase getPhase() const = 0;

	// turn/dice
	virtual Color getCurrentPlayer() const = 0;
	virtual void rollDice() = 0;
	virtual const std::array<int, 2> getDice() const = 0; // returns pointer to array[2]
	virtual bool hasMovesAvailable() const = 0;

	// opening roll
	virtual void rollOpeningDice() = 0;
	virtual int getOpeningDiceWhite() const = 0;
	virtual int getOpeningDiceBlack() const = 0;
	virtual void startGameAfterOpening() = 0;

	// moves (indices 0..23)
	virtual MoveResult makeMove(int fromIndex, int toIndex) = 0;

	// board queries
	virtual int getColumnCount(int index) const = 0;
	virtual Color getColumnColor(int index) const = 0;
	virtual int getBarCount(Color player) const = 0;
	virtual int getBorneOffCount(Color player) const = 0;

	// observer
	virtual void addObserver(IGameObserver* observer) = 0;
	virtual void removeObserver(IGameObserver* observer) = 0;

	virtual GameStateDTO getState() const = 0;

	// helpers for UI
	virtual bool canSelectPoint(int index) const = 0;
	virtual std::vector<int> getLegalTargets(int fromIndex) const = 0;
};


