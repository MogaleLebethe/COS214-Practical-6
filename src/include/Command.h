#pragma once

#include <string>

class Command {
public:
    Command() {}
    virtual ~Command();

    // Must leave the system unchanged when it throws. Redo calls execute() again.
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual std::string describe() const = 0;

private:
    Command(const Command&) = delete;
    Command& operator=(const Command&) = delete;
};
