// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
//#include "WorkflowPlan.h"

#include <QString>
#include <QJsonDocument>
#include <QPointer>

#include "WorkflowPlan.h"

namespace mv
{
    class Task;
}

namespace mv::util {

class WorkflowExecutionContext;

/**
 * Serializable class
 * 
 * Serialize objects using Qt variants.
 * 
 * @author Thomas Kroes
 */
class CORE_EXPORT Serializable
{
public:

    /** Determines the state of the serializable object */
    enum class State {
        Idle,       /** The serializable object is not being read or written */
        Reading,    /** The serializable object is being read */
        Writing     /** The serializable object is being written */
    };

    /** Describes the serialization direction */
    enum class Direction {
        From,   /** From variant map to serializable object */
        To      /** From serializable object to variant map */
    };

public:

    /**
     * Construct with serialization name
     * @param serializationName Serialization name
     */
    Serializable(const QString& serializationName = "");

    /** Destructor */
    virtual ~Serializable() { }

    /**
     * Get globally unique identifier, possibly \p truncated
     * @param truncated Whether to only return the first six characters of the ID
     * @return Globally unique identifier of the serializable object
     */
    QString getId(bool truncated = false) const;

    /**
     * Set globally unique identifier (only use this function when strictly necessary and when the ramifications are understood, undefined behaviour might happen otherwise)
     * @param id Globally unique identifier of the serializable object
     */
    void setId(const QString& id);

    /**
     * Get serialization name
     * @return Serialization name
     */
    QString getSerializationName() const;

    /**
     * Set serialization name to \p name
     * @param serializationName Serialization name
     */
    void setSerializationName(const QString& serializationName);

    /**
     * Load the object state from a variant map.
     *
     * This function is strictly synchronous. When this function returns, all state
     * represented by the variant map must have been fully applied to the object.
     *
     * Implementations must not start background work, schedule workflow jobs, or
     * return before the object is in a fully restored and usable state.
     *
     * If loading requires long-running, parallel, or asynchronous work, implement
     * fromVariantMapWorkflow() instead and keep this function as the blocking
     * fallback.
     *
     * @param variantMap Variant map representation of the object state.
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
     * Create a workflow plan that loads the object state from a variant map.
     *
     * This function only constructs and returns a workflow plan. It must not execute
     * the plan, schedule background work, or modify object state except for trivial
     * preparation required to build the plan.
     *
     * The caller owns the scheduling decision and may execute the returned workflow
     * blocking, asynchronously, or as a child of another workflow execution context.
     *
     * Implement this function when restoring the object involves long-running,
     * parallelizable, staged, or progress-reporting work.
     *
     * The default implementation may return a simple workflow that calls
     * fromVariantMap() synchronously.
     *
     * @param variantMap Variant map representation of the object state.
     * @param parentExecutionContext Optional parent workflow execution context to which the loading workflow will be attached as a child. This can be used to integrate the loading workflow into a larger workflow hierarchy, allowing it to report progress and messages in the context of the parent workflow execution.
     * @return Workflow plan that loads the object state when executed.
     */
    virtual UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& variantMap, SharedWorkflowExecutionContext parentExecutionContext = nullptr);

    /**
     * Load the object state from a parent variant map.
     *
     * This utility function looks up the variant map entry associated with the
     * object's serialization name, extracts the corresponding child variant map,
     * and forwards it to fromVariantMap().
     *
     * If no entry with the serialization name exists:
     *
     * - an exception is thrown when ignoreLoadingErrors is false
     * - the function returns silently when ignoreLoadingErrors is true
     *
     * This function is synchronous and blocking, matching the contract of
     * fromVariantMap().
     *
     * @param parentVariantMap Parent variant map containing serialized child objects.
     * @param ignoreLoadingErrors Whether missing or invalid serialized data should
     * be ignored instead of causing a loading failure (default: false).
     */
    virtual void fromParentVariantMap(const QVariantMap& parentVariantMap, bool ignoreLoadingErrors = false);

    /**
	 * Serialize the object state to a variant map.
	 *
	 * This function returns a complete and self-contained variant map
	 * representation of the object state suitable for persistence,
	 * reconstruction, or transfer.
	 *
	 * The returned variant map should contain all data required by
	 * fromVariantMap() to fully restore the object.
	 *
	 * This function is synchronous and blocking. Implementations must not
	 * schedule asynchronous work or defer serialization to background tasks.
	 *
	 * For large, staged, or parallelizable serialization operations,
	 * implement toVariantMapWorkflow() instead and keep this function as
	 * the blocking fallback.
	 *
	 * @return Variant map representation of the object state.
	 */
    virtual QVariantMap toVariantMap() const;

    /**
	 * Create a workflow plan that serializes the object state to a variant map.
	 *
	 * This function only constructs and returns a workflow plan. It must not
	 * execute the plan, schedule background work, or perform deferred execution
	 * outside the returned workflow structure.
	 *
	 * The caller owns the execution and scheduling strategy of the returned
	 * workflow plan and may execute it blocking, asynchronously, or as part
	 * of another workflow hierarchy.
	 *
	 * Implement this function when serialization involves long-running,
	 * staged, parallelizable, or progress-reporting work.
	 *
	 * The default implementation may return a simple workflow that calls
	 * toVariantMap() synchronously.
	 *
	 * The resulting workflow is expected to produce a complete and
	 * self-contained variant map representation of the object state
	 * equivalent to the result of toVariantMap().
	 *
	 * @return Workflow plan that serializes the object state when executed.
	 */
    virtual UniqueWorkflowPlan toVariantMapWorkflow() const;

    /**
     * Save into \p variantMap
     * @param variantMap Variant map
     */
    void insertIntoVariantMap(QVariantMap& variantMap) const;

    /**
     * Load widget action from JSON document
     * @param jsonDocument The JSON document
     */
    void fromJsonDocument(const QJsonDocument& jsonDocument);

    /**
     * Save widget action to JSON document
     * @return JSON document
     */
    QJsonDocument toJsonDocument() const;

    /**
     * Load from JSON file
     * @param filePath Path to the JSON file
     */
    void fromJsonFile(const QString& filePath = "");

    /**
     * Save to JSON file
     * @param filePath Path to the JSON file
     */
    void toJsonFile(const QString& filePath = "") const;

    /** Assigns a fresh new identifier to the serializable object */
    void makeUnique();

    /**
     * Creates a new globally unique identifier for a serializable object
     * @return Globally unique identifier
     */
    static QString createId();

    /**
     * Get serialization count
     * @param direction Serialization direction
     * @return Serialization count
     */
    std::int32_t getSerializationCount(const Direction& direction) const;

    /**
     * Get serialization count from
     * @return Serialization count from
     */
    std::int32_t getSerializationCountFrom() const;

    /**
     * Get serialization count to
     * @return Serialization count to
     */
    std::int32_t getSerializationCountTo() const;

    /**
     * Handle the case where an item with a specific key is not found in a variant map (e.g. by throwing an exception with an informative error message)
     * @param serializable Serializable object for which the key was not found in the map
     * @param map QVariantMap that should contain the key
     * @param key Item name that was not found in the map
     */
    static void handleKeyNotFoundInVariantMap(const Serializable& serializable, const QVariantMap& map, const QString& key);

public: // Task related

    /**
     * Get task that the serializable object belongs to (if any)
     * @return Task that the serializable object belongs to (if any)
     */
    QPointer<Task> getTask() const;

    /**
     * Whether the serializable object belongs to a task
     * @return Whether the serializable object belongs to a task
     */
    bool hasTask() const;

    /**
     * Set task that the serializable object belongs to (if any)
     * @param task Task that the serializable object belongs to (if any)
     */
    void setTask(Task* task);

protected: // Serialization

    /**
     * Load serializable object from variant map
     * @param serializable Pointer to serializable object
     * @param variantMap The Variant map
     */
    static void fromVariantMap(Serializable* serializable, const QVariantMap& variantMap);

    /**
     * Load from variant map
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     * @param key Variant map key
     */
    static void fromVariantMap(Serializable& serializable, const QVariantMap& variantMap, const QString& key);

    /**
     * Save serializable object to variant map
     * @param serializable Pointer to serializable object
     * @return Variant map
     */
    static QVariantMap toVariantMap(const Serializable* serializable);

    /**
     * Save \p serializable object in \p variantMap with \p key
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     * @param key Variant map key
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap, const QString& key);

    /**
     * Save \p serializable object in \p variantMap
     * @param serializable Reference to serializable object
     * @param variantMap Variant map
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap);

public: // Operators

    /**
     * Equality operator
     * @param rhs Right-hand-side operator
     */
    bool operator == (const Serializable& rhs) const {
        return rhs.getId() == getId();
    }
    
    /**
     * Inequality operator
     * @param rhs Right-hand-side operator
     */
    bool operator != (const Serializable& rhs) const {
        return rhs.getId() != getId();
    }

private:
    QString             _id;                        /** Globally unique identifier of the serializable object */
    QString             _serializationName;         /** Serialization name */
    std::int32_t        _serializationCounter[2];   /** Serialization counter (used to query how many times the action is serialized to- and from) */
    QPointer<Task>      _task;                      /** Task that the serializable object belongs to (if any) */
};

}

