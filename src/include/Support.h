#ifndef TIVIDY_SUPPORT_H
#define TIVIDY_SUPPORT_H

#include <string>
#include <vector>
#include <stack>
#include <memory>
#include <nlohmann/json.hpp>

// Forward declarations
class Command;
class Project;

/**
 * @brief Represents date and time information for workflow deadlines, audit timestamps, and comments.
 */
class DateTime {
private:
    int year;  ///< Four-digit year.
    int month; ///< Month of the year (1-12).
    int day;   ///< Day of the month (1-31).
    int hour;  ///< Hour of the day (0-23).
    int min;   ///< Minute of the hour (0-59).

public:
    /**
     * @brief Constructs a `DateTime` instance.
     * 
     * @param y Four-digit year.
     * @param m Month (1-12).
     * @param d Day (1-31).
     * @param h Hour (0-23), default is 0.
     * @param min Minute (0-59), default is 0.
     */
    DateTime(int y, int m, int d, int h = 0, int min = 0);

    /**
     * @brief Virtual destructor.
     */
    virtual ~DateTime() = default;

    /**
     * @brief Retrieves the current system date and time.
     * 
     * @return `DateTime` Current date and time instance.
     */
    static DateTime now();

    /**
     * @brief Formats the date and time into a string representation.
     * 
     * @param format Format string (default is `"yyyy/mm/dd"`).
     * @return `std::string` Formatted date and time string.
     */
    std::string toString(const std::string& format = "yyyy/mm/dd") const;

    /**
     * @brief Retrieves the year.
     * 
     * @return `int` Year.
     */
    int getYear() const;

    /**
     * @brief Retrieves the month.
     * 
     * @return `int` Month.
     */
    int getMonth() const;

    /**
     * @brief Retrieves the day.
     * 
     * @return `int` Day.
     */
    int getDay() const;

    /**
     * @brief Retrieves the hour.
     * 
     * @return `int` Hour.
     */
    int getHour() const;

    /**
     * @brief Retrieves the minute.
     * 
     * @return `int` Minute.
     */
    int getMinute() const;
};

/**
 * @brief Represents a human participant or system actor involved in workflow components.
 */
class Participant {
private:
    std::string id;   ///< Unique identifier of the participant.
    std::string name; ///< Display name of the participant.
    std::string role; ///< Assigned role of the participant within the workflow.

public:
    /**
     * @brief Constructs a `Participant` instance.
     * 
     * @param id Unique participant ID.
     * @param name Participant display name.
     * @param role Participant role.
     */
    Participant(const std::string& id, const std::string& name, const std::string& role);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Participant() = default;

    /**
     * @brief Retrieves the participant identifier.
     * 
     * @return `std::string` Participant ID.
     */
    std::string getId() const;

    /**
     * @brief Retrieves the participant name.
     * 
     * @return `std::string` Participant name.
     */
    std::string getName() const;

    /**
     * @brief Retrieves the participant role.
     * 
     * @return `std::string` Participant role.
     */
    std::string getRole() const;
};

/**
 * @brief Represents an annotated comment added to a workflow component by a participant.
 */
class Comment {
private:
    std::string authorId; ///< Identifier of the author who posted the comment.
    std::string text;     ///< Content of the comment.
    DateTime timestamp;   ///< Date and time when the comment was created.

public:
    /**
     * @brief Constructs a `Comment` instance.
     * 
     * @param authorId Identifier of the comment author.
     * @param text Comment text content.
     * @param timestamp Timestamp of creation.
     */
    Comment(const std::string& authorId, const std::string& text, const DateTime& timestamp);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Comment() = default;

    /**
     * @brief Retrieves the author identifier.
     * 
     * @return `std::string` Author ID.
     */
    std::string getAuthorId() const;

    /**
     * @brief Retrieves the comment text.
     * 
     * @return `std::string` Comment text.
     */
    std::string getText() const;

    /**
     * @brief Retrieves the timestamp of the comment.
     * 
     * @return `DateTime` Creation timestamp.
     */
    DateTime getTimestamp() const;
};

/**
 * @brief Data Transfer Object (DTO) providing a summary view of a work item.
 */
struct WorkItemView {
    std::string id;       ///< Unique work item identifier.
    std::string name;     ///< Name of the work item.
    std::string status;   ///< Current state summary string.
    std::string executor; ///< Assigned executor ID or name.
    bool ready;           ///< Flag indicating whether dependencies are satisfied.
};

/**
 * @brief Encapsulates a reusable workflow definition template categorized by industry and ID.
 */
class WorkflowDefinition {
private:
    std::string id;             ///< Unique definition identifier.
    std::string name;           ///< Human-readable name of the workflow definition.
    std::string industry;       ///< Industry domain category.
    std::string description;    ///< Detailed description of the definition template.
    nlohmann::json templateJson;///< JSON template structure used to instantiate projects/tasks.

public:
    /**
     * @brief Constructs a `WorkflowDefinition` instance.
     * 
     * @param id Unique definition ID.
     * @param name Definition name.
     * @param industry Industry classification.
     * @param description Template description.
     * @param templateJson JSON template payload.
     */
    WorkflowDefinition(const std::string& id, const std::string& name, 
                       const std::string& industry, const std::string& description, 
                       const nlohmann::json& templateJson);

    /**
     * @brief Virtual destructor.
     */
    virtual ~WorkflowDefinition() = default;

    /**
     * @brief Retrieves the definition identifier.
     * 
     * @return `std::string` Definition ID.
     */
    std::string getId() const;

    /**
     * @brief Retrieves the definition name.
     * 
     * @return `std::string` Definition name.
     */
    std::string getName() const;

    /**
     * @brief Retrieves the industry classification.
     * 
     * @return `std::string` Industry category.
     */
    std::string getIndustry() const;

    /**
     * @brief Retrieves the template description.
     * 
     * @return `std::string` Description.
     */
    std::string getDescription() const;

    /**
     * @brief Retrieves the underlying JSON template.
     * 
     * @return `const nlohmann::json&` Const reference to the JSON template.
     */
    const nlohmann::json& getTemplate() const;
};

/**
 * @brief Represents a single immutable audit log record for tracking workflow and project actions.
 */
class AuditEntry {
public:
    std::string kind;        ///< Category or kind of audit action (e.g., `EXECUTE`, `UNDO`, `REJECTED`).
    std::string description; ///< Detailed description of the action event.
    DateTime timestamp;      ///< Timestamp of when the event occurred.

    /**
     * @brief Constructs an audit entry record.
     * 
     * @param kind Action kind category.
     * @param description Event description.
     * @param timestamp Timestamp of occurrence.
     */
    AuditEntry(const std::string& kind, const std::string& description, const DateTime& timestamp);

    /**
     * @brief Virtual destructor.
     */
    virtual ~AuditEntry() = default;

    /**
     * @brief Serializes the audit entry into a JSON representation.
     * 
     * @return `nlohmann::json` JSON object representing the entry.
     */
    nlohmann::json toJson() const;
};

/**
 * @brief Manages a collection of immutable `AuditEntry` records for system auditing and compliance.
 */
class AuditLog {
private:
    std::vector<AuditEntry> entries; ///< Collection of recorded audit entries.

public:
    /**
     * @brief Constructs an empty audit log.
     */
    AuditLog();

    /**
     * @brief Virtual destructor.
     */
    virtual ~AuditLog() = default;

    /**
     * @brief Records a new audit entry in the log.
     * 
     * @param kind Category kind of the audit action.
     * @param description Description of the event.
     * @param at Timestamp of the event.
     */
    void record(const std::string& kind, const std::string& description, const DateTime& at);

    /**
     * @brief Retrieves all audit entries currently recorded.
     * 
     * @return `const std::vector<AuditEntry>&` Const reference to the vector of audit entries.
     */
    const std::vector<AuditEntry>& getEntries() const;

    /**
     * @brief Generates a vector of formatted string representations of all audit entries.
     * 
     * @return `std::vector<std::string>` Vector of audit log strings.
     */
    std::vector<std::string> toStrings() const;

    /**
     * @brief Serializes the complete audit log history into a JSON array representation.
     * 
     * @return `nlohmann::json` JSON array of audit entries.
     */
    nlohmann::json toJson() const;
};

/**
 * @brief Manages execution and rollback history stacks supporting undo and redo operations.
 */
class CommandHistory {
private:
    std::stack<std::shared_ptr<Command>> undoStack; ///< Stack of executed commands available for undo.
    std::stack<std::shared_ptr<Command>> redoStack; ///< Stack of undone commands available for redo.
    AuditLog* audit;                                ///< Non-owning raw pointer to the associated audit log.

public:
    /**
     * @brief Constructs a command history container.
     * 
     * @param audit Raw pointer to the `AuditLog` instance for recording history events.
     */
    explicit CommandHistory(AuditLog* audit);

    /**
     * @brief Virtual destructor.
     */
    virtual ~CommandHistory() = default;

    /**
     * @brief Executes a command, records it onto the undo stack, and clears the redo stack if successful.
     * 
     * @param cmd Shared pointer to the `Command` to run.
     * @throws `WorkflowError` If execution fails (audits as `REJECTED` and rethrows).
     */
    void run(std::shared_ptr<Command> cmd);

    /**
     * @brief Reverts the most recent command on the undo stack and moves it to the redo stack.
     * 
     * @throws `WorkflowError` If the undo stack is empty.
     */
    void undo();

    /**
     * @brief Re-executes the most recent command on the redo stack and moves it back to the undo stack.
     * 
     * @throws `WorkflowError` If the redo stack is empty.
     */
    void redo();

    /**
     * @brief Checks whether an undo operation can be performed.
     * 
     * @return `bool` `true` if the undo stack is non-empty, `false` otherwise.
     */
    bool canUndo() const;

    /**
     * @brief Checks whether a redo operation can be performed.
     * 
     * @return `bool` `true` if the redo stack is non-empty, `false` otherwise.
     */
    bool canRedo() const;
};

/**
 * @brief Encapsulates a project instance along with its isolated audit log and command history.
 */
class ProjectSession {
public:
    std::shared_ptr<Project> project;       ///< Shared pointer to the active project instance.
    std::unique_ptr<AuditLog> audit;        ///< Unique pointer to the session audit log.
    std::unique_ptr<CommandHistory> history;///< Unique pointer to the command history manager.

    /**
     * @brief Constructs a project session container.
     * 
     * @param proj Shared pointer to the project instance.
     */
    explicit ProjectSession(std::shared_ptr<Project> proj);

    /**
     * @brief Virtual destructor.
     */
    virtual ~ProjectSession() = default;
};

#endif // TIVIDY_SUPPORT_H