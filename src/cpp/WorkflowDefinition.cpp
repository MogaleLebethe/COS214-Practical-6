#include "WorkflowDefinition.h"
#include "WorkflowComponent.h"
#include "WorkflowError.h"
#include "Task.h"
#include <functional>
#include <map>

WorkflowDefinition::WorkflowDefinition(const std::string& defId, const std::string& defName, const std::string& defVersion)
    : id(defId), name(defName), version(defVersion) {}

WorkflowDefinition::~WorkflowDefinition() = default;

std::string WorkflowDefinition::getId() const {
    return id;
}

std::string WorkflowDefinition::getName() const {
    return name;
}

std::string WorkflowDefinition::getVersion() const {
    return version;
}

void WorkflowDefinition::setSchemaPayload(const nlohmann::json& schema) {
    schemaPayload = schema;
}

nlohmann::json WorkflowDefinition::getSchemaPayload() const {
    return schemaPayload;
}

WorkflowComponent* WorkflowDefinition::instantiate() const {
    if (schemaPayload.is_null() || !schemaPayload.contains("root")) {
        throw WorkflowError("Cannot instantiate workflow: schema payload is null or missing 'root' node definition.");
    }

    std::map<std::string, WorkflowComponent*> componentMap;

    std::function<WorkflowComponent*(const nlohmann::json&)> parseComponentNode = 
        [&](const nlohmann::json& nodeJson) -> WorkflowComponent* {
        
        if (!nodeJson.contains("type") || !nodeJson.contains("id")) {
            throw WorkflowError("Invalid schema node: missing 'type' or 'id' field.");
        }

        std::string type = nodeJson["type"].get<std::string>();
        std::string compId = nodeJson["id"].get<std::string>();

        WorkflowComponent* component = nullptr;

        if (type == "Task") {
            Task* newTask = new Task();
            if (nodeJson.contains("name")) {
                newTask->setCustomField("name", nodeJson["name"].get<std::string>());
            }
            component = newTask;
        } 
        else if (type == "Project" || type == "Container") {
            throw WorkflowError("Project container instantiation from schema requires project factory context.");
        } 
        else {
            throw WorkflowError("Unknown workflow component type in schema: " + type);
        }

        componentMap[compId] = component;

        if (nodeJson.contains("customFields") && nodeJson["customFields"].is_object()) {
            for (auto it = nodeJson["customFields"].begin(); it != nodeJson["customFields"].end(); ++it) {
                component->setCustomField(it.key(), it.value().get<std::string>());
            }
        }

        if (nodeJson.contains("children") && nodeJson["children"].is_array()) {
            for (const auto& childJson : nodeJson["children"]) {
                parseComponentNode(childJson);
            }
        }

        return component;
    };

    WorkflowComponent* rootComponent = parseComponentNode(schemaPayload["root"]);

    if (schemaPayload.contains("dependencies") && schemaPayload["dependencies"].is_array()) {
        for (const auto& depJson : schemaPayload["dependencies"]) {
            if (!depJson.contains("dependent") || !depJson.contains("prerequisite")) {
                continue;
            }
            std::string depId = depJson["dependent"].get<std::string>();
            std::string preId = depJson["prerequisite"].get<std::string>();

            if (componentMap.find(depId) != componentMap.end() && componentMap.find(preId) != componentMap.end()) {
                WorkflowComponent* dependentComp = componentMap[depId];
                WorkflowComponent* prerequisiteComp = componentMap[preId];
                
                dependentComp->addDependency(prerequisiteComp);
                prerequisiteComp->addDependent(dependentComp);
            } else {
                throw NotFoundError("Dependency linking failed: Component ID not found in instantiation map (" + depId + " -> " + preId + ")");
            }
        }
    }

    return rootComponent;
}

nlohmann::json WorkflowDefinition::toJson() const {
    nlohmann::json j;
    j["id"] = id;
    j["name"] = name;
    j["version"] = version;
    j["description"] = description;
    j["schema"] = schemaPayload;
    j["metadata"] = metadata;
    return j;
}

void WorkflowDefinition::fromJson(const nlohmann::json& j) {
    if (j.contains("id")) id = j["id"].get<std::string>();
    if (j.contains("name")) name = j["name"].get<std::string>();
    if (j.contains("version")) version = j["version"].get<std::string>();
    if (j.contains("description")) description = j["description"].get<std::string>();
    if (j.contains("schema")) schemaPayload = j["schema"];
    if (j.contains("metadata") && j["metadata"].is_object()) {
        metadata = j["metadata"].get<std::map<std::string, std::string>>();
    }
}