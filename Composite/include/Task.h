#ifndef TASK_H
#define TASK_H

#include "WorkflowComponent.h"
#include "TaskMemento.h"

class TaskState;

class Task : public WorkflowComponent {
    public:
         Task();
         virtual ~Task();
         static std::string generateId();

         //State patter: these just pass event onto the current state and throw an InvalidTransitionError when the state does not allow it. 
         void start();
         void complete();
         void block();
         void fail();
         void cancel();
         void skip();

         TaskState* getState() const;
         void setState(TaskState* state);

         TaskMemento createMemento() const;
         void restoreMemento(const TaskMemento& memento);
         
         void evaluateDependencies() override;
         bool isCompleted() const override;
         std::string getStatusSummary() const override;
         void collectWork(std::vector<WorkItemView>& out) const override;
         nlohmann::json toJson() const override;
         WorkflowComponent* clone() const override;
    
    protected:
        Task(const Task& other);
    
    private:
        TaskState* currentState_;
        static int counter_;     
};

#endif