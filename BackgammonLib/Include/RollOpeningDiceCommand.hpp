/**
 * @file RollOpeningDiceCommand.hpp
 * @brief Defines the RollOpeningDiceCommand class implementing the Command pattern for opening dice rolls.
 */

#pragma once
#include "ICommand.hpp"
#include "Color.hpp"
#include <functional>

/**
 * @class RollOpeningDiceCommand
 * @brief Command to roll the opening die for a player.
 *
 * This class encapsulates the action of rolling a single die during the opening phase
 * of the game. It implements the Command design pattern to allow deferred execution
 * of the dice roll.
 */
class RollOpeningDiceCommand : public ICommand {
public:
    /**
     * @brief Constructor for the RollOpeningDiceCommand.
     * @param rollFunc Function that performs the actual die roll (returns 1-6)
     * @param player The player who is rolling the die
     * @param outDiceValue Reference to the variable where the result will be stored
     */
    RollOpeningDiceCommand(std::function<int()> rollFunc, Color player, int& outDiceValue)
        : m_rollFunc(rollFunc), m_player(player), m_outDiceValue(outDiceValue) {}

    /**
     * @brief Executes the command by rolling the die and storing the result.
     */
    void execute() override {
        m_outDiceValue = m_rollFunc();
    }

private:
    std::function<int()> m_rollFunc;  ///< Function that generates the die roll
    Color m_player;                    ///< The player rolling the die
    int& m_outDiceValue;              ///< Reference to store the die roll result
};

