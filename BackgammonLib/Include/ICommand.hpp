/**
 * @file ICommand.hpp
 * @brief Defines the ICommand interface for the Command design pattern.
 */

#pragma once

/**
 * @interface ICommand
 * @brief Abstract base class for the Command design pattern.
 *
 * This interface allows encapsulating actions as objects, enabling
 * deferred execution, queuing, and undo/redo functionality.
 */
class ICommand {
public:
    /**
     * @brief Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~ICommand() = default;

    /**
     * @brief Executes the command.
     *
     * This method should be implemented by derived classes to perform
     * the specific action encapsulated by the command.
     */
    virtual void execute() = 0;
};

