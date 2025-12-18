#pragma once

// Command Design Pattern
class ICommand {
public:
    virtual ~ICommand() = default;
    virtual void execute() = 0;
};

