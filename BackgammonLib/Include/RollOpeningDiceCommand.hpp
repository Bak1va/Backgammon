#pragma once
#include "ICommand.hpp"
#include "Color.hpp"
#include <functional>

// Command to roll opening dice for a player
class RollOpeningDiceCommand : public ICommand {
public:
    RollOpeningDiceCommand(std::function<int()> rollFunc, Color player, int& outDiceValue)
        : m_rollFunc(rollFunc), m_player(player), m_outDiceValue(outDiceValue) {}

    void execute() override {
        m_outDiceValue = m_rollFunc();
    }

private:
    std::function<int()> m_rollFunc;
    Color m_player;
    int& m_outDiceValue;
};

