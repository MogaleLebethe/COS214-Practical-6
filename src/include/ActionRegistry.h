#pragma once

#include "nlohmann/json.hpp"
#include <functional>
#include <map>
#include <string>
#include <vector>

class Command;
class Task;

typedef std::function<Command*(Task*, const nlohmann::json&)> ActionCreator;

class ActionRegistry {
private:
    std::map<std::string, ActionCreator> creators;

public:
    // Registers the built-ins: start, complete, block, fail, cancel
    ActionRegistry();

    void registerAction(const std::string& name, ActionCreator creator);

    // TRANSFERS ownership to caller (pass straight to CommandHistory::run)
    Command* create(const std::string& name, Task* task, const nlohmann::json& args) const;

    bool has(const std::string& name) const;
    std::vector<std::string> names() const;
};
