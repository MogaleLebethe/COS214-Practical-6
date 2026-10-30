#ifndef DECISIONTASK_H
#define DECISIONTASK_H

//The special task that picks a branch. It is a TASK that asks a "router" (STRATEGY pattern) which path the workflow should take, like either "approve" or "reject". Each path, called branch, has a label and list of task that belong to it. 

//Memory rules: 
// *router_ is BORROWED (the RouterRegistry owns the strategy).
// * the branch tasks are BORROWED (Project owns them).

#include "Task.h"
#include <map>
#include <string>
#include <vector> 

class RoutingStrategy;

class DecisionTask : public Task {

    

    public: 

        DecisionTask();
        virtual ~DecisionTask();
        void setRouter(RoutingStrategy* router); // stores a borrowed pointer to the routing strategy
        void addBranchTarget(const std::string& label, Task* target); 
        std::vector<Task*> getBranchTargets() const; //every target of every branch in one list(borrowed pointers)
        const std::map<std::string,std::vector<Task*>>& getBranches() const; //returns the map of branches (borrowed pointers)
        std::string decide(const nlohmann::json& context);
        
        void clearDecision();
        const std::string& getSelectedBranch() const; 

        nlohmann::json toJson() const override; //returns a json representation of the DecisionTask
        WorkflowComponent* clone() const override; 


    protected:
        DecisionTask(const DecisionTask& other);
    
    private:
        RoutingStrategy* router_;
        std::map<std::string,std::vector<Task*>> branches_; //map of branches, each branch has a label and a list of tasks
        std::string selectedBranch_; 


};

#endif 