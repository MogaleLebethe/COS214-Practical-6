#pragma once 
#include <string>

class Task; 

class TaskState {
    public: 
        virtual ~TaskState();
        virtual void start(Task* task) = 0; 
        virtual void complete(Task* task) =0;
        virtual void block(Task* task) =0; 
        virtual void fail(Task* task) =0; 
        virtual void cancel(Task* task) =0; 
        virtual void skip(Task* task) =0; 
        virtual std::string toString() const =0;
        virtual bool isSatisfied() const =0;  

}; 

class PendingState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        PendingState(const PendingState&) = delete;
        PendingState& operator=(const PendingState&) = delete; 

    private:    
        PendingState() {}

}; 

class InProgressState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        InProgressState(const InProgressState&) = delete;
        InProgressState& operator=(const InProgressState&) = delete; 

    private:    
        InProgressState() {}

}; 

class BlockedState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        BlockedState(const BlockedState&) = delete;
        BlockedState& operator=(const BlockedState&) = delete; 

    private:    
        BlockedState() {}

}; 

class CompletedState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        CompletedState(const CompletedState&) = delete;
        CompletedState& operator=(const CompletedState&) = delete; 

    private:    
        CompletedState() {}

}; 

class FailedState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        FailedState(const FailedState&) = delete;
        FailedState& operator=(const FailedState&) = delete; 

    private:    
        FailedState() {}

}; 

class CancelledState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        CancelledState(const CancelledState&) = delete;
        CancelledState& operator=(const CancelledState&) = delete; 

    private:    
        CancelledState() {}

}; 

class SkippedState : public TaskState{
    public:
        static TaskState* instance();
        void start(Task* task) override; 
        void complete(Task* task) override; 
        void block(Task* task) override; 
        void fail(Task* task) override;
        void cancel(Task* task) override;
        void skip(Task* task) override;
        std::string toString() const override; 
        bool isSatisfied() const override; 
        SkippedState(const SkippedState&) = delete;
        SkippedState& operator=(const SkippedState&) = delete; 

    private:    
        SkippedState() {}

}; 