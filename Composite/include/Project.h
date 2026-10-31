#ifndef PROJECT_H
#define PROJECT_H

//A projector is a container.It holds a lift of WorkflowComponents. And other projects can sit inside another project. 

//Memory rules:
// * A Project OWNS its children(workItems_). So it destructor deletes them. 
// *addWOrkItem(item) -> the project TAKES ownership of item.
// *removeWorkItem(item) -> ownership goes BACK to the cellar. 

#include "WorkflowComponent.h"

class Project : public WorkflowComponent {
    public:
        Project();
        virtual ~Project();
        static std::string generateId();
        void addWorkItem(WorkflowComponent* item); //takes ownership of item and appends it
        WorkflowComponent* removeWorkItem(WorkflowComponent* item); //removes item from the list and returns it to the caller (caller takes ownership)
        const std::vector<WorkflowComponent*>& getWorkItems() const; //returns a list of borrowed pointers to the work items

        WorkflowComponent* findById(const std::string& id) const;

        void evaluateDependencies() override; //evaluates the dependencies of all the work items in the project
        bool isCompleted() const override;
        std::string getStatusSummary() const override;
        void collectWork(std::vector<WorkItemView>& out) const override;
        
        nlohmann::json toJson() const override;
        WorkflowComponent* clone() const override;

    protected:
        Project(const Project& other);
        void unlinkAll() override;
    
    private:
        bool contains(const WorkflowComponent* component) const;
        std::vector <WorkflowComponent*> workItems_; //the list of work items in the project
        static int counter_;       
};

#endif
