#include "Support.h"
#include "Command.h"
#include "Project.h"
#include "WorkflowError.h"
#include <ctime>
#include <iomanip>
#include <sstream>

// ==================== DateTime ====================

DateTime::DateTime(int y, int m, int d, int h, int min)
    : year(y), month(m), day(d), hour(h), min(min) {}

DateTime DateTime::now() {
    std::time_t t = std::time(nullptr);
    std::tm* nowTime = std::localtime(&t);
    return DateTime(
        nowTime->tm_year + 1900,
        nowTime->tm_mon + 1,
        nowTime->tm_mday,
        nowTime->tm_hour,
        nowTime->tm_min
    );
}

std::string DateTime::toString(const std::string& format) const {
    std::ostringstream oss;
    if (format == "yyyy/mm/dd") {
        oss << std::setfill('0') << std::setw(4) << year << "/"
            << std::setw(2) << month << "/"
            << std::setw(2) << day;
    } else {
        oss << std::setfill('0') << std::setw(4) << year << "-"
            << std::setw(2) << month << "-"
            << std::setw(2) << day << " "
            << std::setw(2) << hour << ":"
            << std::setw(2) << min;
    }
    return oss.str();
}

int DateTime::getYear() const { return year; }
int DateTime::getMonth() const { return month; }
int DateTime::getDay() const { return day; }
int DateTime::getHour() const { return hour; }
int DateTime::getMinute() const { return min; }

// ==================== Participant ====================

Participant::Participant(const std::string& id, const std::string& name, const std::string& role)
    : id(id), name(name), role(role) {}

std::string Participant::getId() const { return id; }
std::string Participant::getName() const { return name; }
std::string Participant::getRole() const { return role; }

// ==================== Comment ====================

Comment::Comment(const std::string& authorId, const std::string& text, const DateTime& timestamp)
    : authorId(authorId), text(text), timestamp(timestamp) {}

std::string Comment::getAuthorId() const { return authorId; }
std::string Comment::getText() const { return text; }
DateTime Comment::getTimestamp() const { return timestamp; }

// ==================== WorkflowDefinition ====================

WorkflowDefinition::WorkflowDefinition(const std::string& id, const std::string& name, 
                                     const std::string& industry, const std::string& description, 
                                     const nlohmann::json& templateJson)
    : id(id), name(name), industry(industry), description(description), templateJson(templateJson) {}

std::string WorkflowDefinition::getId() const { return id; }
std::string WorkflowDefinition::getName() const { return name; }
std::string WorkflowDefinition::getIndustry() const { return industry; }
std::string WorkflowDefinition::getDescription() const { return description; }
const nlohmann::json& WorkflowDefinition::getTemplate() const { return templateJson; }

// ==================== AuditEntry ====================

AuditEntry::AuditEntry(const std::string& kind, const std::string& description, const DateTime& timestamp)
    : kind(kind), description(description), timestamp(timestamp) {}

nlohmann::json AuditEntry::toJson() const {
    nlohmann::json j;
    j["kind"] = kind;
    j["description"] = description;
    j["timestamp"] = timestamp.toString("yyyy-mm-dd HH:MM");
    return j;
}

// ==================== AuditLog ====================

AuditLog::AuditLog() = default;

void AuditLog::record(const std::string& kind, const std::string& description, const DateTime& at) {
    entries.push_back(AuditEntry(kind, description, at));
}

const std::vector<AuditEntry>& AuditLog::getEntries() const { return entries; }

std::vector<std::string> AuditLog::toStrings() const {
    std::vector<std::string> result;
    for (const auto& entry : entries) {
        result.push_back("[" + entry.timestamp.toString() + "] " + entry.kind + ": " + entry.description);
    }
    return result;
}

nlohmann::json AuditLog::toJson() const {
    nlohmann::json jArray = nlohmann::json::array();
    for (const auto& entry : entries) {
        jArray.push_back(entry.toJson());
    }
    return jArray;
}

// ==================== CommandHistory ====================

CommandHistory::CommandHistory(AuditLog* audit) : audit(audit) {}

void CommandHistory::run(std::shared_ptr<Command> cmd) {
    if (!cmd) return;

    try {
        cmd->execute();
        undoStack.push(cmd);
        
        while (!redoStack.empty()) {
            redoStack.pop();
        }

        if (audit) {
            audit->record("EXECUTE", cmd->describe(), DateTime::now());
        }
    } catch (const WorkflowError& e) {
        if (audit) {
            audit->record("REJECTED", cmd->describe() + " (" + e.what() + ")", DateTime::now());
        }
        throw;
    }
}

void CommandHistory::undo() {
    if (!canUndo()) {
        throw WorkflowError("Command history undo stack is empty.");
    }

    auto cmd = undoStack.top();
    undoStack.pop();

    cmd->undo();
    redoStack.push(cmd);

    if (audit) {
        audit->record("UNDO", cmd->describe(), DateTime::now());
    }
}

void CommandHistory::redo() {
    if (!canRedo()) {
        throw WorkflowError("Command history redo stack is empty.");
    }

    auto cmd = redoStack.top();
    redoStack.pop();

    cmd->execute();
    undoStack.push(cmd);

    if (audit) {
        audit->record("REDO", cmd->describe(), DateTime::now());
    }
}

bool CommandHistory::canUndo() const { return !undoStack.empty(); }
bool CommandHistory::canRedo() const { return !redoStack.empty(); }

// ==================== ProjectSession ====================

ProjectSession::ProjectSession(std::shared_ptr<Project> proj)
    : project(proj),
      audit(std::make_unique<AuditLog>()),
      history(std::make_unique<CommandHistory>(audit.get())) {}