#include "../include/DecisionTask.h"
#include "../include/RoutingStrategy.h"
#include "../include/WorkflowComponent.h"
#include <algorithm>
#include <stdexcept>

DecisionTask::DecisionTask() : router_(nullptr){}

DecisionTask::DecisionTask(const DecisionTask& other) : Task(other), router_(other.router_){}

DecisionTask::~DecisionTask(){}

void DecisionTask::setRouter(RoutingStrategy* router){
    router_ router;
}

void DecisionTask::addBranchTarget(const std::string& label, Task* target){
    if(target == nullptr){
        throw std::logic_error("DecisionTask::addBranchTarget: target must not be null");
    }
    if(label.empty()){
        throw WorkflowError("DecisionTask::addBranchTarget:branch label must not be empty");
    }

    target-addDependency(this);

    std::vector<Task*>& targets = branches_[label];

    if(std::find(targets.begin(), targets.end(), target) == targets.end()){
        targets.push_back(target);
    }
}

std::vector<Task*> DecisionTask::getBranchTargets() const {
    std::vector<Task*> all;
    for (std::map<std::string, std::vector<Task*> >::const_iterator it = branches_.begin();
         it != branches_.end(); ++it) {
        all.insert(all.end(), it->second.begin(), it->second.end());
    }
    return all;
}

const std::map<std::string, std::vector<Task*> >& DecisionTask::getBranches() const {
    return branches_;
}

std::string DecisionTask::decide(const nlohmann::json& context) {
    if (router_ == nullptr) {
        throw NotFoundError("DecisionTask::decide: no router set for " + getId());
    }
    std::string label = router_->route(*this, context);   // STRATEGY: the router chooses
    if (branches_.find(label) == branches_.end()) {
        throw NotFoundError("DecisionTask::decide: unknown branch \"" + label + "\"");
    }
    selectedBranch_ = label;
    return selectedBranch_;
}

void DecisionTask::clearDecision() {
    selectedBranch_.clear();
}

const std::string& DecisionTask::getSelectedBranch() const {
    return selectedBranch_;
}

nlohmann::json DecisionTask::toJson() const {
    nlohmann::json j = Task::toJson();     // reuse everything a Task already exports
    j["type"] = "Decision";
    j["selectedBranch"] = selectedBranch_;

    nlohmann::json branches = nlohmann::json::object();
    for (std::map<std::string, std::vector<Task*> >::const_iterator it = branches_.begin();
         it != branches_.end(); ++it) {
        nlohmann::json ids = nlohmann::json::array();
        for (std::size_t i = 0; i < it->second.size(); ++i) {
            ids.push_back(it->second[i]->getId());
        }
        branches[it->first] = ids;
    }
    j["branches"] = branches;
    return j;
}

WorkflowComponent* DecisionTask::clone() const {
    return new DecisionTask(*this);
}
