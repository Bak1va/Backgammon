#pragma once
#include "Color.h"
#include "MoveResult.h"

class IGameObserver;

enum class GamePhase {
	NotStarted,
	InProgress,
	Finished
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
	virtual const int* getDice() const = 0; // returns pointer to array[2]
	virtual bool hasMovesAvailable() const = 0;

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
};


