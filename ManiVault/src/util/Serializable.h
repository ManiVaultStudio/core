// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"

#include "workflow/WorkflowPlan.h"

#include <QString>
#include <QJsonDocument>
#include <QPointer>

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
 * Serializable provides a workflow-first serialization framework for
 * persisting and restoring object state.
 *
 * Objects may participate in serialization through either:
 *
 * - the synchronous serialization contract:
 *   fromVariantMap() / toVariantMap()
 *
 * - the workflow-based serialization contract:
 *   fromVariantMapWorkflow() / toVariantMapWorkflow()
 *
 * Framework code should generally use the scoped entry points:
 *
 * - fromVariantMapScoped()
 * - toVariantMapScoped()
 * - fromJsonDocumentScoped()
 * - fromJsonFileScoped()
 * - toJsonDocumentScoped()
 * - toJsonFileScoped()
 *
 * These functions execute the workflow-based serialization contract and
 * automatically fall back to the synchronous implementation when no
 * custom workflow is provided.
 *
 * Plugin developers should typically:
 *
 * - override fromVariantMap() and toVariantMap() for simple objects
 * - override fromVariantMapWorkflow() and/or
 *   toVariantMapWorkflow() for expensive, staged, parallelizable,
 *   or progress-reporting serialization operations
 *
 * The synchronous serialization functions remain the fundamental
 * implementation layer, while workflow-based serialization adds
 * progress reporting, nested workflows, parallel execution,
 * diagnostics, and integration into larger workflow hierarchies.
 *
 * In addition to the primary serialization contract, Serializable
 * provides convenience helpers for:
 *
 * - working with parent QVariantMaps
 * - inserting serialized child objects into QVariantMaps
 * - reading and writing JSON documents
 * - reading and writing JSON files
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
	 * Most framework code should not call this function directly.
	 * Instead, use fromVariantMapScoped(), which executes the workflow-based
	 * loading path and automatically falls back to this function through the
	 * default workflow implementation.
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
	 * Load the object state from a variant map using the workflow-based
	 * serialization contract.
	 *
	 * This is the preferred loading entry point for framework code.
	 *
	 * The function obtains a workflow from fromVariantMapWorkflow(),
	 * executes it synchronously, and returns only after the object has been
	 * fully restored.
	 *
	 * The default workflow implementation delegates to fromVariantMap(),
	 * ensuring that all Serializable objects support this entry point even
	 * when they do not provide a custom workflow.
	 *
	 * @param variantMap Serialized object state.
	 * @param parentExecutionContext Optional parent workflow context.
	 */
    virtual void fromVariantMapScoped(const QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& parentExecutionContext);

    /**
	 * Create a workflow that restores the object state.
	 *
	 * This function constructs a workflow but does not execute it.
	 *
	 * The returned workflow must fully restore the object when executed.
	 *
	 * Implementations may use sequential, parallel, nested, or batched
	 * stages and may publish progress, warnings, messages, and results.
	 *
	 * The default implementation creates a simple workflow that invokes
	 * fromVariantMap().
	 *
	 * Framework code should generally call fromVariantMapScoped() rather
	 * than executing this workflow directly.
	 *
	 * @param variantMap Serialized object state.
	 * @return Workflow plan that restores the object state.
	 */
    virtual workflow::UniqueWorkflowPlan fromVariantMapWorkflow(const QVariantMap& variantMap);

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
	 * Use fromVariantMapScoped() when workflow-based loading behavior is
	 * required.
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
	 * Restore this object from a parent variant map using the workflow-based
	 * serialization contract.
	 *
	 * This convenience function looks up the child variant map associated
	 * with this object's serialization name and forwards it to
	 * fromVariantMapScoped().
	 *
	 * Unlike fromParentVariantMap(), this function executes the
	 * workflow-based loading contract and therefore honors custom
	 * implementations of fromVariantMapWorkflow().
	 *
	 * @param parentVariantMap Parent variant map containing serialized child
	 * objects.
	 * @param parentExecutionContext Optional parent workflow context.
	 */
    virtual void fromParentVariantMapScoped(const QVariantMap& parentVariantMap, const workflow::SharedWorkflowExecutionContext& parentExecutionContext = nullptr);

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
	 * Most framework code should prefer fromJsonDocumentScoped(), which
	 * executes the workflow-based serialization contract and automatically
	 * honors custom implementations of fromVariantMapWorkflow().
	 *
	 * @param jsonDocument JSON document containing the serialized object
	 * state.
	 *
	 * @throws ManiVaultException If the required serialization entry is
	 * missing or invalid.
	 */
    void fromJsonDocument(const QJsonDocument& jsonDocument);

    /**
	 * Restore the object state from a JSON document.
	 *
	 * This is the preferred JSON document loading entry point for
	 * framework code.
	 *
	 * The function extracts the object's serialized variant map from the
	 * JSON document and forwards it to fromVariantMapScoped().
	 *
	 * @param jsonDocument JSON document containing serialized object state.
	 * @param parentExecutionContext Optional parent workflow context.
	 */
    void fromJsonDocumentScoped(const QJsonDocument& jsonDocument, const workflow::SharedWorkflowExecutionContext& parentExecutionContext = nullptr);

    /**
	 * Restore the object state from a JSON file.
	 *
	 * This function reads a JSON document from disk, validates that the file
	 * exists, can be opened, contains data, and contains a valid JSON
	 * document, then forwards the document to fromJsonDocument().
	 *
	 * This function follows the same loading contract as fromJsonDocument().
	 * If fromJsonDocument() uses the synchronous serialization path, this
	 * function also bypasses workflow-based loading. If fromJsonDocument()
	 * delegates to fromJsonDocumentScoped(), this function honors the
	 * workflow-based serialization contract.
	 *
	 * Prefer fromJsonFileScoped() when an explicit parent workflow context is
	 * required.
	 *
	 * @param filePath Path to the JSON file to load.
	 *
	 * @throws std::runtime_error If the file does not exist.
	 * @throws std::runtime_error If the file cannot be opened for reading.
	 * @throws std::runtime_error If no data could be read.
	 * @throws std::runtime_error If the JSON document is invalid or empty.
	 * @throws std::exception Any exception propagated by fromJsonDocument().
	 */
    void fromJsonFile(const QString& filePath = "");

    /**
	 * Restore the object state from a JSON file using the workflow-based
	 * serialization contract.
	 *
	 * This is the preferred JSON file loading entry point for framework
	 * code.
	 *
	 * The function reads a JSON document from disk, validates the file
	 * contents, and restores the object through
	 * fromJsonDocumentScoped().
	 *
	 * Custom implementations of fromVariantMapWorkflow() are therefore
	 * honored automatically, including staged, parallel, and
	 * progress-reporting loading workflows.
	 *
	 * The function returns only after the loading workflow has completed.
	 *
	 * @param filePath Path of the JSON file to read.
	 * @param parentExecutionContext Optional parent workflow context into
	 * which the loading workflow will be integrated.
	 *
	 * @throws std::runtime_error If the file does not exist.
	 * @throws std::runtime_error If the file cannot be opened for reading.
	 * @throws std::runtime_error If no data could be read.
	 * @throws std::runtime_error If the JSON document is invalid or empty.
	 * @throws std::exception Any exception propagated by the loading
	 * workflow execution.
	 */
    void fromJsonFileScoped(const QString& filePath = "", const workflow::SharedWorkflowExecutionContext& parentExecutionContext = nullptr);

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
	 * Most framework code should prefer toVariantMapScoped(), which
	 * executes the workflow-based serialization contract and automatically
	 * honors custom implementations of toVariantMapWorkflow().
	 *
	 * @return Complete variant map representation of the object state.
	*/
    virtual QVariantMap toVariantMap() const;

    /**
	 * Serialize the object state using the workflow-based serialization
	 * contract.
	 *
	 * This is the preferred serialization entry point for framework code.
	 *
	 * The function obtains a workflow from toVariantMapWorkflow(),
	 * executes it synchronously, and returns the resulting variant map.
	 *
	 * The default workflow implementation delegates to toVariantMap(),
	 * ensuring that all Serializable objects support this entry point even
	 * when they do not provide a custom workflow.
	 *
	 * @param parentExecutionContext Optional parent workflow context.
	 * @return Serialized object state.
	 */
    virtual QVariantMap toVariantMapScoped(const workflow::SharedWorkflowExecutionContext& parentExecutionContext) const;

    /**
	 * Create a workflow that serializes the object state to a variant map.
	 *
	 * This is the workflow-based serialization hook.
	 *
	 * The function only constructs and returns a workflow plan. It does not
	 * execute the workflow.
	 *
	 * The returned workflow must publish the serialized QVariantMap as the
	 * result value for this object's serialization name.
	 *
	 * The base implementation creates a simple GUI-thread workflow stage
	 * that calls toVariantMap() synchronously and publishes the result.
	 *
	 * Derived classes may override this function when serialization is
	 * expensive, staged, parallelizable, or requires progress reporting.
	 *
	 * Implementations must not perform the serialization immediately unless
	 * that work is part of a job in the returned workflow.
	 *
	 * Most framework code should prefer toVariantMapScoped(), which executes
	 * this workflow and returns the published variant map.
	 *
	 * @return Workflow plan that serializes the object state when executed.
	 */
    virtual workflow::UniqueWorkflowPlan toVariantMapWorkflow() const;

public: // Writing helpers

    /**
	 * Insert this object's serialized state into a parent variant map.
	 *
	 * This convenience function serializes the object using toVariantMap()
	 * and stores the resulting child variant map under the object's
	 * serialization name.
	 *
	 * The function requires:
	 * - a non-empty serialization name
	 * - that the parent variant map does not already contain an entry with
	 *   that name
	 *
	 * This function uses the synchronous serialization contract and
	 * therefore invokes toVariantMap() directly. It does not execute
	 * toVariantMapWorkflow() and does not participate in workflow
	 * execution.
	 *
	 * Use toVariantMapScoped() when workflow-based serialization behavior
	 * is required.
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
	 * Insert this object's serialized state into a parent variant map using
	 * the workflow-based serialization contract.
	 *
	 * This convenience function serializes the object using
	 * toVariantMapScoped() and stores the resulting child variant map under
	 * the object's serialization name.
	 *
	 * Unlike insertIntoVariantMap(), this function executes the
	 * workflow-based serialization contract and therefore honors custom
	 * implementations of toVariantMapWorkflow().
	 *
	 * The function requires:
	 * - a non-empty serialization name
	 * - that the parent variant map does not already contain an entry with
	 *   that name
	 *
	 * Example:
	 *
	 * @code
	 * QVariantMap parentMap;
	 * child.insertIntoVariantMapScoped(parentMap, executionContext);
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
	 * @param parentExecutionContext Optional parent workflow context used
	 * when executing the serialization workflow.
	 *
	 * @throws std::runtime_error If the serialization name is empty.
	 * @throws std::runtime_error If the parent variant map already contains
	 * an entry with the same serialization name.
	 */
    void insertIntoVariantMapScoped(QVariantMap& variantMap, const workflow::SharedWorkflowExecutionContext& parentExecutionContext = nullptr) const;
    
    // TODO
    QJsonDocument toJsonDocument() const;

    /**
	 * Serialize the object state to a JSON file.
	 *
	 * This convenience overload of toJsonFileScoped() executes the
	 * workflow-based serialization contract using the default workflow
	 * execution context.
	 *
	 * @param filePath Path of the JSON file to write.
	 *
	 * @throws std::exception Any exception propagated by
	 * toJsonFileScoped().
	 */
    void toJsonFile(const QString& filePath = "") const;

    // TODO
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
     */
    static void handleKeyNotFoundInVariantMap(const Serializable& serializable, const QVariantMap& map, const QString& key);

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

