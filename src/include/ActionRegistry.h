/**
 * @file ActionRegistry.h
 * @brief Command factory: maps action names to command creators.
 */
#pragma once

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "nlohmann/json.hpp"

class Command;
class Task;

/**
 * @brief Type definition for action factory functions (`ActionCreator`).
 *
 * Takes a target task raw pointer and a JSON arguments payload, returning a newly
 * allocated concrete Command pointer that the caller owns.
 */
typedef std::function<Command*(Task*, const nlohmann::json&)> ActionCreator;

/**
 * @brief Registry mapping action names to command creators.
 *
 * Built-in lifecycle actions are registered on construction. Industry modules register
 * extra actions (e.g. approve, reject, escalate) without modifying the engine or the Facade.
 */
class ActionRegistry {
private:
    std::map<std::string, ActionCreator> creators;  ///< Map linking action names to their creators.

public:
    /**
     * @brief Constructs the registry and pre-loads the built-in actions (start, complete, block, fail, cancel).
     */
    ActionRegistry();

    /**
     * @brief Registers an action, replacing any existing action with the same name.
     *
     * @param name The string identifier for the action (e.g. "approve").
     * @param creator The `ActionCreator` factory function or lambda.
     * @throws `std::logic_error` If creator is empty.
     */
    void registerAction(const std::string& name, ActionCreator creator);

    /**
     * @brief Creates a command by looking up its action name.
     *
     * TRANSFERS ownership to caller, who must pass it straight to `CommandHistory::run`.
     *
     * @param name The string identifier of the action.
     * @param task Raw pointer to the target task (borrowed).
     * @param args JSON arguments payload passed to the creator.
     * @return `Command*` Raw pointer to the newly allocated command.
     * @throws `NotFoundError` If the action name is not registered.
     */
    Command* create(const std::string& name, Task* task, const nlohmann::json& args) const;

    /**
     * @brief Checks if an action name is registered.
     *
     * @param name The string identifier of the action.
     * @return `bool` `true` If the action exists, `false` otherwise.
     */
    bool has(const std::string& name) const;

    /**
     * @brief Retrieves all registered action names.
     *
     * @return `std::vector<std::string>` Registered action names in alphabetical order.
     */
    std::vector<std::string> names() const;
};
