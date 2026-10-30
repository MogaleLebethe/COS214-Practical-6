#ifndef WORKFLOWCOMPONENT_H
#define WORKFLOWCOMPONENT_H

#include <map>
#include <string>
#include <vector>
#include "nlohmann/json.hpp"
#include "Participant.h"
#include "DateTime.h"
#include "Comment.h"
#include "WorkItemView.h"

class TaskBuilder;
class ProjectBuilder;
class DecisionTaskBuilder;
class Project;
class TaskMemento;

class WorkflowComponent {

    public:
        virtual ~WorkflowComponent();
        std::string getId() const;
        std::string getName() const;
        void setName(const std::string& name);
        std::string getDescription() const;
        void setDescription(const std::string& desc);
        void setExecutor(const Participant& p);
        std::string getExecutorId() const;

        std::string addParticipant(Participant p);
        void setDeadline(const DateTime& d);
        void addComment(Comment comment);
        void setCustomField(const std::string& key, const std::string& value);
        std::string getCustomField(const std::string& key) const;

        bool addDependency(WorkflowComponent* prerequisite);
        void removeDependency(WorkflowComponent* prerequisite);

        void addDependent(WorkflowComponent* component);
        void removeDependent(WorkflowComponent* component);

        const std::vector<WorkflowComponent*>& getDependencies() const;
        void notifyDependents();
        virtual void update(WorkflowComponent* changedComponent);

        virtual bool isReady() const;
        virtual void evaluateDependencies() = 0;
        virtual bool isCompleted() const = 0;
        virtual std::string getStatusSummary() const = 0;
        virtual void collectWork(std::vector<WorkItemView>& out) const = 0;
        virtual nlohmann::json toJson() const = 0;
        virtual WorkflowComponent* clone() const = 0;
    
    protected:
        WorkflowComponent();
        WorkflowComponent(const WorkflowComponent& other);
        void recomputeReadiness();
        virtual void unlinkAll();

        std::string id_;
        std::string name_;
        std::string description_;
        Participant executor_;
        DateTime deadline_;
        std::map<std::string, std::string> customFields_;
        std::vector<Comment> comments_;
        std::vector<Participant> participants_;
        std::vector<WorkflowComponent*>dependencies_;
        std::vector<WorkflowComponent*> dependents_;
        bool ready_;

        friend class TaskBuilder;
        friend class ProjectBuilder;
        friend class DecisionTaskBuilder;
        friend class Project;
        friend class TaskMemento;

    private:
        WorkflowComponent& operator=(const WorkflowComponent&) = delete;
};

#endif