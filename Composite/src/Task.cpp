#include "../include/"
#include "../include/TaskState.h"
#include "../include/PendingState.h"
#include "../include/CancelledState.h"
#include <sstream>
#include <stdexcept>

int Task::counter_ = 0;

Task::Task() : currentState_(PendingState::instance()) {
    id_ = generateId();
}

Task::Task(const Task& other)
    : WorkflowComponent(other),
      currentState_(PendingState::instance()) {
    id_ = generateId();
}

Task::~Task() {
}

std::string Task::generateId() {
    std::ostringstream out;
    out << "T-" << ++counter_;
    return out.str();
}

void Task::start()    { currentState_->start(this); }
void Task::complete() { currentState_->complete(this); }
void Task::block()    { currentState_->block(this); }
void Task::fail()     { currentState_->fail(this); }
void Task::cancel()   { currentState_->cancel(this); }
void Task::skip()     { currentState_->skip(this); }

TaskState* Task::getState() const {
    return currentState_;
}

void Task::setState(TaskState* state) {
    if (state == nullptr) {
        throw std::logic_error("Task::setState: state must not be null");
    }
    currentState_ = state;
    notifyDependents();
}

TaskMemento Task::createMemento() const {
    return TaskMemento(*this);
}

void Task::restoreMemento(const TaskMemento& memento) {
    if (!memento.isValid()) {
        throw std::logic_error("Task::restoreMemento: memento is empty");
    }
    name_ = memento.name_;
    description_ = memento.description_;
    executor_ = memento.executor_;
    deadline_ = memento.deadline_;
    participants_ = memento.participants_;
    customFields_ = memento.customFields_;
}

void Task::evaluateDependencies() {
    recomputeReadiness();
}

bool Task::isCompleted() const {
    return currentState_->isSatisfied();
}

std::string Task::getStatusSummary() const {
    return "Task [" + id_ + "] " + name_ + " - Status: " + currentState_->toString();
}

void Task::collectWork(std::vector<WorkItemView>& out) const {
    bool finished = isCompleted() || currentState_ == CancelledState::instance();
    if (!isReady() || finished) {
        return;
    }
    WorkItemView view;
    view.id = id_;
    view.name = name_;
    view.status = currentState_->toString();
    view.executor = executor_.getName();
    view.ready = isReady();
    out.push_back(view);
}

nlohmann::json Task::toJson() const {
    nlohmann::json j;
    j["type"] = "Task";
    j["id"] = id_;
    j["name"] = name_;
    j["description"] = description_;
    j["status"] = currentState_->toString();

    nlohmann::json executor;
    executor["id"] = executor_.getId();
    executor["name"] = executor_.getName();
    executor["role"] = executor_.getRole();
    j["executor"] = executor;

    j["deadline"] = deadline_.toString("%Y-%m-%d %H:%M");

    nlohmann::json fields = nlohmann::json::object();
    for (std::map<std::string, std::string>::const_iterator it = customFields_.begin();
         it != customFields_.end(); ++it) {
        fields[it->first] = it->second;
    }
    j["customFields"] = fields;

    nlohmann::json prereqIds = nlohmann::json::array();
    for (std::size_t i = 0; i < dependencies_.size(); ++i) {
        prereqIds.push_back(dependencies_[i]->getId());
    }
    j["dependsOn"] = prereqIds;
    return j;
}

WorkflowComponent* Task::clone() const {
    return new Task(*this);
}