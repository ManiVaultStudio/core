// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "workflow/WorkflowPlan.h"

#include <QString>
#include <QJsonDocument>
#include <QPointer>
#include <source_location>

namespace mv
{
    class Task;

	namespace workflow
    {
        class WorkflowExecutionContext;
    }
}

namespace mv::util {

/**
 * Serializable base class.
 *
 * Serializable provides the serialization contract for persisting and
 * restoring object state.
 *
 * Serialization is exposed through two layers:
 *
 * - the synchronous contract:
 *   fromVariantMap() / toVariantMap()
 *
 * - the workflow contract:
 *   fromVariantMapWorkflow() / toVariantMapWorkflow()
 *
 * The synchronous functions are intended for simple, immediate
 * serialization work. They must complete all work before returning and
 * must not start background work, execute workflows, or defer state
 * changes.
 *
 * The workflow functions are intended for expensive, staged,
 * parallelizable, or progress-reporting serialization work. They only
 * construct workflow plans; they do not execute them. Execution,
 * scheduling, nesting, progress reporting, cancellation, diagnostics,
 * and output collection are the responsibility of the workflow executor
 * and the caller that composes the workflow.
 *
 * The default workflow implementations delegate to the synchronous
 * functions, so simple objects only need to override fromVariantMap() and
 * toVariantMap().
 *
 * Framework code that participates in a larger workflow should compose
 * the workflow returned by fromVariantMapWorkflow() or
 * toVariantMapWorkflow(), typically as a nested workflow. It should not
 * call helper functions that hide blocking workflow execution.
 *
 * Convenience JSON and parent-map helpers are provided for simple
 * synchronous use cases. They are not workflow orchestration APIs.
 *
 * Plugin developers should typically:
 *
 * - override fromVariantMap() and toVariantMap() for simple objects
 * - override fromVariantMapWorkflow() and/or toVariantMapWorkflow() for
 *   expensive, staged, parallelizable, or progress-reporting operations
 *
 * In addition to the primary serialization contract, Serializable
 * provides helpers for:
 *
 * - working with parent QVariantMaps
 * - inserting serialized child objects into QVariantMaps
 * - reading and writing JSON documents and files
 * - object identity management and serialization diagnostics
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
     * Construct a serializable object.
     *
     * @param serializationName Serialization name used when storing
     * and retrieving this object from parent variant maps and JSON
     * documents.
     */
    Serializable(const QString& serializationName = "");

    /**
     * Destructor.
     */
    virtual ~Serializable() {}

    /**
     * Get this object's globally unique identifier.
     *
     * The identifier represents the persistent identity of the object
     * and is used by the equality operators.
     *
     * @param truncated Whether to return a shortened representation of
     * the identifier for diagnostic or display purposes.
     *
     * @return Globally unique identifier of this object.
     */
    QString getId(bool truncated = false) const;

    /**
     * Set this object's globally unique identifier.
     *
     * Changing the identifier changes the logical identity of the
     * object and should therefore only be done when the implications
     * are fully understood.
     *
     * In most cases, makeUnique() should be preferred when a new
     * identity is required.
     *
     * @param id Globally unique identifier to assign.
     */
    void setId(const QString& id);

    /**
     * Get this object's serialization name.
     *
     * The serialization name is used as the key under which the object
     * is stored in parent variant maps and JSON documents.
     *
     * @return Serialization name.
     */
    QString getSerializationName() const;

    /**
	 * Set this object's serialization name.
	 *
	 * The serialization name is used as the key under which the object is
	 * stored in parent variant maps and JSON documents.
	 *
	 * Changing the serialization name after serialized data has already
	 * been persisted may render that data unreadable by this object,
	 * because the expected serialization key no longer matches the key
	 * stored in the serialized representation.
	 *
	 * Changing the serialization name may also introduce collisions with
	 * other serialized objects or make existing serialized state
	 * unreachable.
	 *
	 * Use this function sparingly and only when the object's serialization
	 * identity is intentionally being changed and the implications are
	 * fully understood.
	 *
	 * @param serializationName Serialization name to assign.
	 */
    void setSerializationName(const QString& serializationName);

public: // Reading

    /**
	 * Load the object state from a variant map.
	 *
	 * This is the low-level synchronous loading implementation.
	 *
	 * When this function returns, all state represented by the variant map
	 * must have been fully applied to the object.
	 *
	 * Implementations must not:
	 * - start background work
	 * - schedule workflow jobs
	 * - return before restoration is complete
	 *
	 * Framework code that participates in workflow execution should generally
	 * compose the workflow returned by fromVariantMapWorkflow() instead of
	 * calling this function directly.
	 *
	 * Override this function for simple objects whose state can be restored
	 * synchronously.
	 *
	 * Override fromVariantMapWorkflow() instead when loading is expensive,
	 * staged, parallelizable, or requires progress reporting.
	 *
	 * @param variantMap Serialized object state.
	 */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
	 * Create a workflow that restores the object state.
	 *
	 * This function constructs a workflow but does not execute it.
	 *
	 * The returned workflow must fully restore the object when executed.
	 *
	 * Implementations may use sequential, parallel, nested, or batched
	 * stages and may publish progress, warnings, messages, and outputs.
	 *
	 * The default implementation creates a simple workflow that invokes
	 * fromVariantMap().
	 *
	 * Framework code may compose this workflow directly, for example as a
	 * nested workflow within a larger serialization workflow.
	 *
	 * @param variantMap Serialized object state.
	 * @return Workflow plan that restores the object state.
	 */
    virtual workflow::UniqueWorkflowPlan fromVariantMapWorkflow(QVariantMap variantMap);

public: // Reading helpers

    /**
	 * Restore this object from a parent variant map.
	 *
	 * This convenience function looks up the child variant map associated
	 * with this object's serialization name and forwards it to
	 * fromVariantMap().
	 *
	 * The function expects the parent variant map to contain an entry whose
	 * key matches getSerializationName(). The associated value must be a
	 * QVariantMap representing the serialized state of this object.
	 *
	 * This function uses the synchronous serialization contract and
	 * therefore invokes fromVariantMap() directly. It does not execute
	 * fromVariantMapWorkflow() and does not participate in workflow
	 * execution, progress reporting, or parent workflow hierarchies.
	 *
	 * Use fromVariantMapWorkflow() when deserialization needs to participate
	 * in a larger workflow hierarchy.
	 *
	 * @param parentVariantMap Parent variant map containing serialized child
	 * objects.
	 * @param ignoreLoadingErrors Whether missing serialized data should be
	 * ignored. When false, missing entries result in an exception. When
	 * true, missing entries may be handled according to the application's
	 * loading error policy.
	 *
	 * @throws std::runtime_error If the serialization name is empty.
	 * @throws ManiVaultException If the required entry is not present.
	 */
    virtual void fromParentVariantMap(const QVariantMap& parentVariantMap, bool ignoreLoadingErrors = false);

    /**
	 * Restore the object state from a JSON document.
	 *
	 * This function extracts the variant map associated with this object's
	 * serialization name from the JSON document and forwards it directly to
	 * fromVariantMap().
	 *
	 * This function uses the synchronous serialization contract and does not
	 * execute fromVariantMapWorkflow(). As a result, custom workflow-based
	 * loading behavior, progress reporting, nested workflows, and parent
	 * workflow hierarchies are not used.
	 *
	 * @param jsonDocument JSON document containing the serialized object
	 * state.
	 *
	 * @throws ManiVaultException If the required serialization entry is
	 * missing or invalid.
	 */
    virtual void fromJsonDocument(const QJsonDocument& jsonDocument);

    /**
	 * Restore the object state from a JSON file.
	 *
	 * This function reads a JSON document from disk, validates that the file
	 * exists, can be opened, contains data, and contains a valid JSON
	 * document, then forwards the document to fromJsonDocument().
	 *
	 * This function performs synchronous file loading and delegates to
	 * fromJsonDocument() after validating and parsing the JSON document.
	 *
	 * @param filePath Path to the JSON file to load.
	 *
	 * @throws std::runtime_error If the file does not exist.
	 * @throws std::runtime_error If the file cannot be opened for reading.
	 * @throws std::runtime_error If no data could be read.
	 * @throws std::runtime_error If the JSON document is invalid or empty.
	 * @throws std::exception Any exception propagated by fromJsonDocument().
	 */
    virtual void fromJsonFile(const QString& filePath = "");

public: // Writing

    /**
     * Serialize the object state to a variant map.
     *
     * This is the low-level synchronous serialization implementation.
     *
     * The returned variant map must contain all information required to
     * restore the object through fromVariantMap().
     *
     * The base implementation serializes only the object's unique
     * identifier and updates the serialization counter.
     *
     * Derived classes should extend the returned variant map with their
     * own serialized state and should generally invoke the base
     * implementation first.
     *
     * Implementations must not schedule asynchronous work, create
     * workflows, or defer serialization beyond the lifetime of this call.
     *
     * Framework code that participates in workflow execution should
     * generally compose the workflow returned by toVariantMapWorkflow()
     * instead of calling this function directly.
     *
     * The default implementation of toVariantMapWorkflow() delegates to
     * this function, allowing simple objects to participate in the
     * workflow-based serialization contract without implementing a custom
     * workflow.
     *
     * @return Complete variant map representation of the object state.
     */
    virtual QVariantMap toVariantMap() const;

    /**
     * Create a workflow that serializes the object state to a variant map.
     *
     * This is the workflow-based serialization hook.
     *
     * The function only constructs and returns a workflow plan. It does not
     * execute the workflow.
     *
     * The returned workflow must publish the serialized QVariantMap as an
     * output using this object's serialization name.
     *
     * The base implementation creates a simple workflow that invokes
     * toVariantMap() and publishes the resulting variant map as an output.
     *
     * Derived classes may override this function when serialization is
     * expensive, staged, parallelizable, or requires progress reporting.
     *
     * Implementations must not perform the serialization immediately unless
     * that work is part of a job in the returned workflow.
     *
     * Framework code may compose this workflow directly, for example as a
     * nested workflow within a larger serialization workflow.
     *
     * @return Workflow plan that serializes the object state when executed.
     */
	virtual workflow::UniqueWorkflowPlan toVariantMapWorkflow() const;

public: // Writing helpers

    /**
	 * Insert this object's serialized state into a parent variant map.
	 *
	 * This convenience function serializes the object using
	 * toVariantMap() and stores the resulting child variant map under the
	 * object's serialization name.
	 *
	 * The function requires:
	 * - a non-empty serialization name
	 * - that the parent variant map does not already contain an entry with
	 *   that name
	 *
	 * This function uses the synchronous serialization contract and
	 * therefore invokes toVariantMap() directly. It does not participate
	 * in workflow execution and does not invoke toVariantMapWorkflow().
	 *
	 * Use toVariantMapWorkflow() when serialization needs to participate in
	 * a larger workflow hierarchy.
	 *
	 * Example:
	 *
	 * @code
	 * QVariantMap parentMap;
	 * child.insertIntoVariantMap(parentMap);
	 * @endcode
	 *
	 * Result:
	 *
	 * @code
	 * {
	 *     "ChildSerializationName" : { ... }
	 * }
	 * @endcode
	 *
	 * @param variantMap Parent variant map into which the serialized object
	 * state will be inserted.
	 *
	 * @throws std::runtime_error If the serialization name is empty.
	 * @throws std::runtime_error If the parent variant map already contains
	 * an entry with the same serialization name.
	 */
    void insertIntoVariantMap(QVariantMap& variantMap) const;

    /**
	 * Serialize the object state to a JSON document.
	 *
	 * This convenience function serializes the object using the
	 * synchronous serialization contract and converts the resulting
	 * QVariantMap to a JSON document.
	 *
	 * This function invokes toVariantMap() directly. It does not
	 * participate in workflow execution and does not invoke
	 * toVariantMapWorkflow().
	 *
	 * Use toVariantMapWorkflow() when serialization needs to participate in
	 * a larger workflow hierarchy.
	 *
	 * @return JSON document containing the serialized object state.
	 */
    virtual QJsonDocument toJsonDocument() const;

    /**
	 * Serialize the object state to a JSON file.
	 *
	 * This convenience function serializes the object using the
	 * synchronous serialization contract, converts the resulting variant
	 * map to a JSON document, and writes it to disk.
	 *
	 * This function invokes toVariantMap() directly. It does not
	 * participate in workflow execution and does not invoke
	 * toJsonFileWorkflow() or toVariantMapWorkflow().
	 *
	 * Use toJsonFileWorkflow() when file serialization needs to participate
	 * in a larger workflow hierarchy.
	 *
	 * @param filePath Path of the JSON file to write.
	 *
	 * @throws std::exception Any exception raised during serialization,
	 * JSON conversion, or file writing.
	 */
    virtual void toJsonFile(const QString& filePath = "") const;

    /**
	 * Create a workflow that serializes the object state to a JSON file.
	 *
	 * This function only constructs and returns a workflow plan. It does
	 * not execute the workflow.
	 *
	 * The returned workflow must serialize the object state and write the
	 * resulting JSON document to the specified file when executed.
	 *
	 * The base implementation may compose existing serialization workflows
	 * and additional stages required to write the JSON file.
	 *
	 * Derived classes may override this function when JSON file
	 * serialization requires custom staging, progress reporting, or
	 * additional workflow integration.
	 *
	 * Implementations must not write the file immediately unless that work
	 * is part of a job in the returned workflow.
	 *
	 * @param filePath Path of the JSON file to write.
	 * @return Workflow plan that writes the serialized object state to
	 * disk when executed.
	 */
    workflow::UniqueWorkflowPlan toJsonFileWorkflow(const QString& filePath = "") const;

public: // Identity and diagnostics

    /**
     * Assign a fresh globally unique identifier to this object.
     *
     * This function replaces the current identifier with a newly
     * generated identifier produced by createId().
     *
     * Use this function when creating a logically distinct copy of a
     * serializable object that must not share identity with the
     * original object.
     */
    void makeUnique();

    /**
     * Create a new globally unique identifier.
     *
     * The returned identifier is suitable for use as the persistent
     * identity of a Serializable instance.
     *
     * @return Newly generated globally unique identifier.
     */
    static QString createId();

    /**
     * Get the number of times this object has been serialized in the
     * specified direction.
     *
     * Serialization counters are maintained automatically by the base
     * Serializable implementation and may be useful for debugging,
     * diagnostics, and testing.
     *
     * @param direction Serialization direction to query.
     * @return Number of serialization operations performed in the
     * specified direction.
     */
    std::int32_t getSerializationCount(const Direction& direction) const;

    /**
	 * Get the number of times fromVariantMap() has been invoked.
	 *
	 * This counter is maintained automatically by the base Serializable
	 * implementation and may be useful for diagnostics and testing.
	 *
	 * @return Number of deserialization operations performed through the
	 * low-level deserialization implementation.
	 */
    std::int32_t getSerializationCountFrom() const;

    /**
	 * Get the number of times toVariantMap() has been invoked.
	 *
	 * This counter is maintained automatically by the base Serializable
	 * implementation and may be useful for diagnostics and testing.
	 *
	 * @return Number of serialization operations performed through the
	 * low-level serialization implementation.
	 */
    std::int32_t getSerializationCountTo() const;

    /**
     * Handle a missing required key in a variant map.
     *
     * This helper centralizes error reporting for serialization-related
     * lookup failures and typically throws a descriptive exception that
     * includes contextual information about the missing key and the
     * available entries in the variant map.
     *
     * @param serializable Serializable object for which the lookup
     * failed.
     * @param map Variant map that was expected to contain the key.
     * @param key Required key that could not be found.
     * @param sourceLocation Source location of the call site, used for diagnostics. Defaults to the current source location.
     */
    static void handleKeyNotFoundInVariantMap(const Serializable& serializable, const QVariantMap& map, const QString& key, std::source_location sourceLocation = std::source_location::current());

public: // Task related

    /**
     * Get the task associated with this object.
     *
     * @return Associated task, or a null pointer if no task is
     * associated with this object.
     */
    QPointer<Task> getTask() const;

    /**
     * Determine whether this object is associated with a task.
     *
     * @return True if a task is associated with this object, false
     * otherwise.
     */
    bool hasTask() const;

    /**
     * Associate this object with a task.
     *
     * The task association may be used by higher-level infrastructure
     * for ownership, lifecycle management, or contextual integration.
     *
     * @param task Task to associate with this object, or nullptr to
     * clear the association.
     */
    void setTask(Task* task);

protected: // Serialization

    /**
     * Restore a serializable object from a variant map.
     *
     * This convenience helper forwards the supplied variant map to the
     * object's synchronous deserialization implementation.
     *
     * This function is not part of the primary serialization contract.
     *
     * @param serializable Serializable object to restore.
     * @param variantMap Serialized object state.
     */
    static void fromVariantMap(Serializable* serializable, const QVariantMap& variantMap);

    /**
     * Restore a serializable object from a child variant map contained
     * within a parent variant map.
     *
     * This convenience helper extracts the child variant map associated
     * with the specified key and forwards it to the object's synchronous
     * deserialization implementation.
     *
     * This function is not part of the primary serialization contract.
     *
     * @param serializable Serializable object to restore.
     * @param variantMap Parent variant map.
     * @param key Key identifying the serialized child object.
     */
    static void fromVariantMap(Serializable& serializable, const QVariantMap& variantMap, const QString& key);

    /**
     * Serialize a serializable object to a variant map.
     *
     * This convenience helper invokes the object's synchronous
     * serialization implementation.
     *
     * This function is not part of the primary serialization contract.
     *
     * @param serializable Serializable object to serialize.
     * @return Serialized object state.
     */
    static QVariantMap toVariantMap(const Serializable* serializable);

    /**
     * Serialize a serializable object and insert the resulting child
     * variant map into a parent variant map under the specified key.
     *
     * This convenience helper is not part of the primary serialization
     * contract.
     *
     * @param serializable Serializable object to serialize.
     * @param variantMap Parent variant map.
     * @param key Key under which the serialized object state will be
     * stored.
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap, const QString& key);

    /**
     * Serialize a serializable object and insert the resulting child
     * variant map into a parent variant map using the object's
     * serialization name as the key.
     *
     * This convenience helper is not part of the primary serialization
     * contract.
     *
     * @param serializable Serializable object to serialize.
     * @param variantMap Parent variant map.
     */
    static void insertIntoVariantMap(const Serializable& serializable, QVariantMap& variantMap);

public: // Operators

    /**
     * Compare two serializable objects for identity equality.
     *
     * Two Serializable instances are considered equal when their
     * globally unique identifiers are equal.
     *
     * @param rhs Object to compare against.
     * @return True if both objects have the same identifier, false
     * otherwise.
     */
    bool operator == (const Serializable& rhs) const {
        return rhs.getId() == getId();
    }

    /**
     * Compare two serializable objects for identity inequality.
     *
     * Two Serializable instances are considered different when their
     * globally unique identifiers differ.
     *
     * @param rhs Object to compare against.
     * @return True if the identifiers differ, false otherwise.
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

