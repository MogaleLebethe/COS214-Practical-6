#include "ActionRegistry.h"
#include "TaskStateCommand.h"
#include "Task.h"
#include "WorkflowError.h"
#include <stdexcept>

namespace {
ActionCreator lifecycle(const std::string& label, void (Task::*event)()) {
    return [label, event](Task* task, const nlohmann::json&) -> Command* {
        return new TaskStateCommand(task, std::mem_fn(event), label);
    };
}
}

ActionRegistry::ActionRegistry() {
    registerAction("start", lifecycle("start", &Task::start));
    registerAction("complete", lifecycle("complete", &Task::complete));
    registerAction("block", lifecycle("block", &Task::block));
    registerAction("fail", lifecycle("fail", &Task::fail));
    registerAction("cancel", lifecycle("cancel", &Task::cancel));
}

void ActionRegistry::registerAction(const std::string& name, ActionCreator creator) {
    if (!creator) throw std::logic_error("ActionRegistry: empty creator for " + name);
    creators[name] = creator;
}

Command* ActionRegistry::create(const std::string& name, Task* task, const nlohmann::json& args) const {
    std::map<std::string, ActionCreator>::const_iterator it = creators.find(name);
    if (it == creators.end()) throw NotFoundError("Unknown action: " + name);
    return it->second(task, args);
}

bool ActionRegistry::has(const std::string& name) const {
    return creators.find(name) != creators.end();
}

std::vector<std::string> ActionRegistry::names() const {
    std::vector<std::string> out;
    for (std::map<std::string, ActionCreator>::const_iterator it = creators.begin(); it != creators.end(); ++it)
        out.push_back(it->first);
    return out;
}
