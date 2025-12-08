#pragma once
#include <vector>
#include "IGame.hpp"
#include "IGameObserver.hpp"
#include "Board.hpp"
#include "GameStateDTO.hpp"

class Game : public IGame {
public:
    static constexpr int BAR_INDEX = 25; // Index special pentru selectia barei

    Game();
    ~Game() override;

    // IGame implementation
    void start() override;
    GamePhase getPhase() const override;
    Color getCurrentPlayer() const override;
    void rollDice() override;
    const int *getDice() const override;
    bool hasMovesAvailable() const override;
    MoveResult makeMove(int fromIndex, int toIndex) override;

    int getColumnCount(int index) const override;
    Color getColumnColor(int index) const override;
    int getBarCount(Color player) const override;
    int getBorneOffCount(Color player) const override;

    GameStateDTO getState() const override;
    bool canSelectPoint(int index) const override;
    std::vector<int> getLegalTargets(int fromIndex) const override;

    void addObserver(IGameObserver *observer) override;
    void removeObserver(IGameObserver *observer) override;

private:
    Board m_board;
    GamePhase m_phase;
    Color m_currentPlayer;
    int m_dice[2];
    bool m_diceRolled;
    std::vector<IGameObserver *> m_observers;

    // Helper methods logic
    void notifyGameStarted();
    void notifyDiceRolled();
    void notifyMoveMade(int fromIndex, int toIndex, MoveResult result);
    void notifyTurnChanged();
    void notifyGameFinished(Color winner);
    void switchTurn();

    bool isMoveBlocked(int toIndex, Color player) const;
    bool canHit(int toIndex, Color player) const;
    int playerIndex(Color player) const;

    // Metode noi pentru logica avansata
    bool canBearOff(Color player) const;
    bool isHomeBoard(int index, Color player) const;
    bool areAllPiecesHome(Color player) const;
};
