#ifndef TIVIDY_WORKFLOW_DEFINITION_H
#define TIVIDY_WORKFLOW_DEFINITION_H

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

// Forward declarations
class WorkflowComponent;

/**
 * @brief Represents a reusable template blueprint or schema for workflow instantiation.
 * 
 * Encapsulates structural definitions, default component hierarchies, metadata parameters, 
 * and versioning information used by the definition repository to instantiate project instances.
 */
class WorkflowDefinition {
private:
    std::string id;                                     ///< Unique identifier for the workflow definition.
    std::string name;                                   ///< Human-readable name of the definition.
    std::string version;                                ///< Version string of the template schema.
    std::string description;                            ///< Detailed description of the workflow template purpose.
    nlohmann::json schemaPayload;                       ///< Raw JSON schema layout describing tasks and routing nodes.
    std::map<std::string, std::string> metadata;        ///< Additional key-value metadata attributes.

public:
    /**
     * @brief Constructs a new WorkflowDefinition with specified identifiers and metadata.
     * 
     * @param defId Unique identifier string for the definition.
     * @param defName Human-readable name of the workflow blueprint.
     * @param defVersion Version tag string.
     */
    WorkflowDefinition(const std::string& defId, const std::string& defName, const std::string& defVersion);

    /**
     * @brief Virtual destructor for the workflow definition.
     */
    virtual ~WorkflowDefinition();

    /**
     * @brief Retrieves the unique definition identifier.
     * 
     * @return `std::string` The definition ID string.
     */
    std::string getId() const;

    /**
     * @brief Retrieves the human-readable name of the workflow definition.
     * 
     * @return `std::string` The definition name string.
     */
    std::string getName() const;

    /**
     * @brief Retrieves the version string of the template schema.
     * 
     * @return `std::string` The version string.
     */
    std::string getVersion() const;

    /**
     * @brief Sets or updates the raw JSON schema payload defining the workflow layout.
     * 
     * @param schema The `nlohmann::json` object containing component templates and connections.
     */
    void setSchemaPayload(const nlohmann::json& schema);

    /**
     * @brief Retrieves the JSON schema payload.
     * 
     * @return `nlohmann::json` The schema payload object.
     */
    nlohmann::json getSchemaPayload() const;

    /**
     * @brief Instantiates a concrete workflow component hierarchy from this definition schema.
     * 
     * @return `WorkflowComponent*` Raw pointer to the newly allocated root component instance.
     * @throws `WorkflowError` If schema parsing or component instantiation fails.
     */
    WorkflowComponent* instantiate() const;

    /**
     * @brief Serializes the workflow definition into a structured JSON object.
     * 
     * @return `nlohmann::json` The serialized JSON representation of the definition.
     */
    nlohmann::json toJson() const;

    /**
     * @brief Deserializes and populates definition attributes from a JSON object.
     * 
     * @param j Const reference to the `nlohmann::json` source object.
     */
    void fromJson(const nlohmann::json& j);
};

#endif // TIVIDY_WORKFLOW_DEFINITION_H