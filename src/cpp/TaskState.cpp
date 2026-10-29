#include "TaskState.h"
#include "Task.h"
#include "WorkflowError.h"



TaskState::~TaskState() {}

//PendingState
TaskState* PendingState::instance(){
    static PendingState s; 
    return &s; 
}

void PendingState::start(Task* task){
    if (!task->isReady()){
        throw InvalidTransitionError("PendingState::start: Prereq not satisfied");
    }
    task->setState(InProgressState::instance());
}

void PendingState::complete(Task*){
    throw InvalidTransitionError("PendingState::complete: task has not been started yet");
}

void PendingState::block(Task* task){
    task->setState(BlockedState::instance());
}

void PendingState::fail(Task*){
    throw InvalidTransitionError("PendingState::fail: not a valid event from pending");

}

void PendingState::cancel(Task* task){
    task->setState(CancelledState::instance());
}

void PendingState::skip(Task* task){
    task->setState(SkippedState::instance());
}

std::string PendingState::toString() const{
    return "Pending";
}

bool PendingState::isSatisfied() const{
    return false; 
}

//InProgressState


TaskState* InProgressState::instance(){
    static InProgressState s; 
    return &s; 
}

void InProgressState::start(Task* task){

    throw InvalidTransitionError("InProgressState::start:task is already in progress");
}

void InProgressState::complete(Task* task){
    task->setState(CompletedState::instance());
}

void InProgressState::block(Task* task){
    task->setState(BlockedState::instance());
}

void InProgressState::fail(Task* task){
    task->setState(FailedState::instance());
}

void InProgressState::cancel(Task* task){
    task->setState(CancelledState::instance());
}

void InProgressState::skip(Task* task){
    throw InvalidTransitionError("InProgressState::skip: not a valid event from In Progress");
}

std::string InProgressState::toString() const{
    return "In Progress";
}

bool InProgressState::isSatisfied() const{
    return false; 
}

//BlockedState

TaskState* BlockedState::instance(){
    static BlockedState s; 
    return &s; 
}

void BlockedState::start(Task* task){
    task->setState(InProgressState::instance());
}

void BlockedState::complete(Task* task){
    throw InvalidTransitionError("BlockedState::complete: task is blocked until resume");
}

void BlockedState::block(Task* task){
    throw InvalidTransitionError("BlockedState::block: task is already blocked");
}

void BlockedState::fail(Task* task){
    task->setState(FailedState::instance());
}

void BlockedState::cancel(Task* task){
    task->setState(CancelledState::instance());
}

void BlockedState::skip(Task* task){
    throw InvalidTransitionError("BlockedState::skip: not a valid event from blocked");
}

std::string BlockedState::toString() const{
    return "Blocked";
}

bool BlockedState::isSatisfied() const{
    return false; 
}

//CompletedState
TaskState* CompletedState::instance(){
    static CompletedState s; 
    return &s; 
}

void CompletedState::start(Task* task){
    task->setState(InProgressState::instance());
}

void CompletedState::complete(Task* task){
    throw InvalidTransitionError("CompletedState::complete: task is already completed");
}

void CompletedState::block(Task* task){
    throw InvalidTransitionError("CompletedState::block: a completed task cannot be blocked");
}

void CompletedState::fail(Task* task){
    throw InvalidTransitionError("CompletedState::fail: a completed task cannot fail");
}

void CompletedState::cancel(Task* task){
    throw InvalidTransitionError("CompletedState::cancel: a complete task cant be cancelled");
}

void CompletedState::skip(Task* task){
    throw InvalidTransitionError("CompletedState::skip: a completed task cant be skipped");
}

std::string CompletedState::toString() const{
    return "Complete";
}

bool CompletedState::isSatisfied() const{
    return true; 
}

//FailedState
TaskState* FailedState::instance(){
    static FailedState s; 
    return &s; 
}

void FailedState::start(Task* task){
    task->setState(InProgressState::instance());
}

void FailedState::complete(Task* task){
    throw InvalidTransitionError("FailedState::complete:failed task cant completed");
}

void FailedState::block(Task* task){
    throw InvalidTransitionError("FailedState::block: not a valid event from Failed");
}

void FailedState::fail(Task* task){
    throw InvalidTransitionError("FailedState::fail: task already failed");
}

void FailedState::cancel(Task* task){
    task->setState(CancelledState::instance());
}

void FailedState::skip(Task* task){
    throw InvalidTransitionError("FailedState::skip: not valid event for Failed");
}

std::string FailedState::toString() const{
    return "Failed";
}

bool FailedState::isSatisfied() const{
    return false; 
}

//CancelledState
TaskState* CancelledState::instance(){
    static CancelledState s; 
    return &s; 
}

void CancelledState::start(Task* task){
    throw InvalidTransitionError("CancelledState::start: Cancelled is terminal state");
}

void CancelledState::complete(Task* task){
    throw InvalidTransitionError("CancelledState::complete:Cancelled is terminal state");
}

void CancelledState::block(Task* task){
    throw InvalidTransitionError("CancelledState::block: Cancelled is terminal state");
}

void CancelledState::fail(Task* task){
    throw InvalidTransitionError("CancelledState::fail: Cancelled is terminal state");
}

void CancelledState::cancel(Task* task){
    throw InvalidTransitionError("CancelledState::cancel: Cancelled is terminal state");
}

void CancelledState::skip(Task* task){
    throw InvalidTransitionError("CancelledState::skip: Cancelled is terminal state");
}

std::string CancelledState::toString() const{
    return "Cancelled";
}

bool CancelledState::isSatisfied() const{
    return false; 
}

//SkippedState
TaskState* SkippedState::instance(){
    static SkippedState s; 
    return &s; 
}

void SkippedState::start(Task* task){
    throw InvalidTransitionError("SkippedState::start: Skipped is terminal state");
}

void SkippedState::complete(Task* task){
    throw InvalidTransitionError("SkippedState::complete:Skipped is terminal state");
}

void SkippedState::block(Task* task){
    throw InvalidTransitionError("SkippedState::block: Skipped is terminal state");
}

void SkippedState::fail(Task* task){
    throw InvalidTransitionError("SkippedState::fail: Skipped is terminal state");
}

void SkippedState::cancel(Task* task){
    throw InvalidTransitionError("SkippedState::cancel: Skipped is terminal state");
}

void SkippedState::skip(Task* task){
    throw InvalidTransitionError("SkippedState::skip: Skipped is terminal state");
}

std::string SkippedState::toString() const{
    return "Skipped";
}

bool SkippedState::isSatisfied() const{
    return true; 
}