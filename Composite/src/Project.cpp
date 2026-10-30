#include "../include/Project.h"
#include "../include/DecisionTask.h"
#include "../include/WorkflowError.h"
#include <algorithm>
#include <map>
#include <sstream>

int Project::counter_ = 0;

Project::Project() {
    id_ = generateId();
}

Project::Project(const Project& other) : WorkflowComponent(other) {
    id_ = generateId();   // the copy is a new project, so it gets a new id
}

Project::~Project() {
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        delete workItems_[i];
    }
    workItems_.clear();
}

std::string Project::generateId() {
    std::ostringstream out;
    out << "P-" << ++counter_;
    return out.str();
}


bool Project::contains(const WorkflowComponent* component) const {
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        if (workItems_[i] == component) {
            return true;
        }
        // If the child is itself a project, look inside it too.
        const Project* inner = dynamic_cast<const Project*>(workItems_[i]);
        if (inner != nullptr && inner->contains(component)) {
            return true;
        }
    }
    return false;
}

void Project::addWorkItem(WorkflowComponent* item) {
    // Check everything BEFORE changing anything. If a check fails, we throw and the caller still owns the item.
    if (item == nullptr) {
        throw WorkflowError("Project::addWorkItem: item must not be null");
    }
    if (item == this) {
        throw WorkflowError("Project::addWorkItem: a project cannot contain itself");
    }
    if (contains(item)) {
        throw WorkflowError("Project::addWorkItem: item is already part of this project");
    }
    
    const Project* itemAsProject = dynamic_cast<const Project*>(item);
    if (itemAsProject != nullptr && itemAsProject->contains(this)) {
        throw WorkflowError("Project::addWorkItem: that project already contains this project");
    }

    workItems_.push_back(item);   
}

WorkflowComponent* Project::removeWorkItem(WorkflowComponent* item) {
    std::vector<WorkflowComponent*>::iterator it =
        std::find(workItems_.begin(), workItems_.end(), item);
    if (it == workItems_.end()) {
        throw NotFoundError("Project::removeWorkItem: item is not a direct child of this project");
    }
    
    item->unlinkAll();
    workItems_.erase(it);   
    return item;            
}

const std::vector<WorkflowComponent*>& Project::getWorkItems() const {
    return workItems_;
}

WorkflowComponent* Project::findById(const std::string& id) const {
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        if (workItems_[i]->getId() == id) {
            return workItems_[i];
        }
        const Project* inner = dynamic_cast<const Project*>(workItems_[i]);
        if (inner != nullptr) {
            WorkflowComponent* found = inner->findById(id);
            if (found != nullptr) {
                return found;
            }
        }
    }
    return nullptr;
}

void Project::unlinkAll() {
    WorkflowComponent::unlinkAll();                 
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        workItems_[i]->unlinkAll();               
    }
}


void Project::evaluateDependencies() {
    recomputeReadiness();
}

bool Project::isCompleted() const {
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        if (!workItems_[i]->isCompleted()) {
            return false;
        }
    }
    return true;  
}

std::string Project::getStatusSummary() const {
    std::size_t done = 0;
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        if (workItems_[i]->isCompleted()) {
            ++done;
        }
    }

    std::ostringstream out;
    out << "Project [" << id_ << "] " << name_ << " - "
        << done << "/" << workItems_.size() << " complete";

   
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        std::string childText = workItems_[i]->getStatusSummary();
        std::string indented;
        for (std::size_t k = 0; k < childText.size(); ++k) {
            indented += childText[k];
            if (childText[k] == '\n') {
                indented += "  ";
            }
        }
        out << "\n  " << indented;
    }
    return out.str();
}

void Project::collectWork(std::vector<WorkItemView>& out) const {
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        workItems_[i]->collectWork(out);
    }
}

nlohmann::json Project::toJson() const {
    nlohmann::json j;
    j["type"] = "Project";
    j["id"] = id_;
    j["name"] = name_;
    j["description"] = description_;
    j["completed"] = isCompleted();

    nlohmann::json fields = nlohmann::json::object();
    for (std::map<std::string, std::string>::const_iterator it = customFields_.begin();
         it != customFields_.end(); ++it) {
        fields[it->first] = it->second;
    }
    j["customFields"] = fields;

    nlohmann::json items = nlohmann::json::array();
    for (std::size_t i = 0; i < workItems_.size(); ++i) {
        items.push_back(workItems_[i]->toJson());
    }
    j["workItems"] = items;
    return j;
}


WorkflowComponent* Project::clone() const {
   
    Project* copy = new Project(*this);
    try {
        // Step 1: clone every child and remember "original -> copy".
        std::map<const WorkflowComponent*, WorkflowComponent*> copyOf;
        for (std::size_t i = 0; i < workItems_.size(); ++i) {
            WorkflowComponent* childCopy = workItems_[i]->clone();
            try {
                copy->addWorkItem(childCopy);   // the copy project owns it now
            } catch (...) {
                delete childCopy;               // not added, so we must free it
                throw;
            }
            copyOf[workItems_[i]] = childCopy;
        }

        // Step 2: re-create the links, but between the COPIES.
        // A link to something outside this project is dropped, so the copy never depends on the original's tasks.
        for (std::size_t i = 0; i < workItems_.size(); ++i) {
            const WorkflowComponent* original = workItems_[i];
            WorkflowComponent* duplicate = copyOf[original];

            // 2a. dependency links
            const std::vector<WorkflowComponent*>& prereqs = original->getDependencies();
            for (std::size_t j = 0; j < prereqs.size(); ++j) {
                std::map<const WorkflowComponent*, WorkflowComponent*>::const_iterator found =
                    copyOf.find(prereqs[j]);
                if (found != copyOf.end()) {
                    duplicate->addDependency(found->second);
                }
            }

            // 2b. decision branches (only DecisionTasks have them)
            const DecisionTask* originalDecision = dynamic_cast<const DecisionTask*>(original);
            DecisionTask* duplicateDecision = dynamic_cast<DecisionTask*>(duplicate);
            if (originalDecision != nullptr && duplicateDecision != nullptr) {
                const std::map<std::string, std::vector<Task*> >& branches =
                    originalDecision->getBranches();
                for (std::map<std::string, std::vector<Task*> >::const_iterator b = branches.begin();
                     b != branches.end(); ++b) {
                    for (std::size_t t = 0; t < b->second.size(); ++t) {
                        std::map<const WorkflowComponent*, WorkflowComponent*>::const_iterator found =
                            copyOf.find(b->second[t]);
                        Task* targetCopy =
                            (found == copyOf.end()) ? nullptr : dynamic_cast<Task*>(found->second);
                        if (targetCopy != nullptr) {
                            duplicateDecision->addBranchTarget(b->first, targetCopy);
                        }
                    }
                }
            }
        }
    } catch (...) {
        delete copy;   
        throw;
    }
    return copy;
}
