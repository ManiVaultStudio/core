// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "ManiVaultGlobals.h"
//#include "WorkflowPlan.h"

#include <QString>
#include <QJsonDocument>
#include <QPointer>

namespace mv
{
    class Task;
}

namespace mv::util {

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

    /** Result of an asynchronous toVariantMap() operation */
    struct AsyncToVariantMapResult
    {
        //WorkflowPlan                    _workflow;      /** Workflow associated with the asynchronous operation */
        std::shared_ptr<QVariantMap>    _result;        /** Resulting QVariantMap from the asynchronous operation */
    };

    /** Result of an asynchronous fromVariantMap() operation */
    struct AsyncFromVariantMapResult
    {
        //WorkflowPlan                    _workflow;      /** Workflow associated with the asynchronous operation */
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
     * Synchronously restores the object from a QVariantMap.
     *
     * This function blocks until deserialization has fully completed.
     *
     * Implementations may internally use parallelism or worker threads, but all
     * work must complete before this function returns.
     *
     * Implementations must not start asynchronous work that outlives the function
     * call. Use fromVariantMapAsync() for externally observable asynchronous
     * execution.
     *
     * Deserialization semantics:
     * - Implementations should focus on restoring persistent object state.
     * - External side effects unrelated to state restoration should generally be
     *   avoided during deserialization.
     *
     * @param variantMap Serialized object state.
     */
    virtual void fromVariantMap(const QVariantMap& variantMap);

    /**
     * Asynchronously restores the object from a QVariantMap.
     *
     * Unlike fromVariantMap(), this function may schedule concurrent or
     * asynchronous work and return before deserialization has completed.
     *
     * The returned async result contains the workflow and/or completion handle
     * required to track progress, cancellation, warnings/errors, and completion.
     *
     * The object must not be considered fully restored until the asynchronous
     * operation has completed successfully.
     *
     * The default implementation wraps the synchronous fromVariantMap() call in
     * a workflow job executed asynchronously.
     *
     * Implementations may internally use multiple threads, nested workflows,
     * deferred decoding, background decompression, or staged reconstruction.
     *
     * Thread-safety:
     * - Implementations must ensure that all asynchronous work remains valid for
     *   the lifetime of the async operation.
     * - Implementations must synchronize access to shared mutable state.
     *
     * Progress reporting:
     * - Implementations are encouraged to expose nested workflow stages and
     *   meaningful progress information where possible.
     *
     * Deserialization semantics:
     * - Implementations should focus on restoring persistent object state.
     * - External side effects unrelated to state restoration should generally be
     *   avoided during deserialization.
     *
     * @param map Serialized object state.
     *
     * @return Async deserialization result containing workflow state.
     */
    virtual AsyncFromVariantMapResult fromVariantMapAsync(const QVariantMap& map);

    /**
     * Load from variant map located in \p parentVariantMap at the serialization name
     * @param parentVariantMap Parent variant map
     * @param ignoreLoadingErrors Whether to ignore loading errors (default: false)
     */
    virtual void fromParentVariantMap(const QVariantMap& parentVariantMap, bool ignoreLoadingErrors = false);

    /**
     * Synchronously serializes the object to a QVariantMap.
     *
     * This function blocks until serialization has fully completed and returns
     * a complete serialized representation of the object.
     *
     * Implementations may internally use parallelism, worker threads, temporary
     * tasks, or concurrent processing, but all work must complete before this
     * function returns.
     *
     * Implementations must not start asynchronous work that outlives the function
     * call. Use toVariantMapAsync() for externally observable asynchronous
     * serialization, progress reporting, cancellation, or workflow-based
     * execution.
     *
     * The returned QVariantMap should contain all state required to restore the
     * object using fromVariantMap().
     *
     * Thread-safety:
     * - Implementations are responsible for synchronizing access to shared mutable
     *   state during serialization.
     * - Returned QVariantMap data must remain valid independently of temporary
     *   buffers or background tasks created during serialization.
     *
     * Error handling:
     * - Implementations may throw serialization-related exceptions or report
     *   workflow messages when serialization fails.
     *
     * Serialization semantics:
     * - Implementations should serialize persistent object state only.
     * - Serialization should avoid mutating observable application state where
     *   possible.
     *
     * @return Complete serialized object state.
     */
    virtual QVariantMap toVariantMap() const;

    /**
     * Asynchronously serializes the object to a QVariantMap.
     *
     * Unlike toVariantMap(), this function may schedule concurrent or asynchronous
     * work and return before serialization has completed.
     *
     * The returned async result contains the workflow and/or completion handle
     * required to track progress, cancellation, warnings/errors, and completion.
     *
     * Implementations may internally use multiple threads, nested workflows,
     * deferred compression, streaming IO, or background encoding tasks.
     *
     * The serialized QVariantMap must not be accessed until the asynchronous
     * operation has completed successfully.
     *
     * The default implementation wraps the synchronous toVariantMap() call in
     * a workflow job executed asynchronously.
     *
     * Thread-safety:
     * - Implementations must ensure that all asynchronous work remains valid for
     *   the lifetime of the async operation.
     * - Implementations must not access destroyed objects or temporary buffers
     *   after returning from this function.
     *
     * Progress reporting:
     * - Implementations are encouraged to expose nested workflow stages and
     *   meaningful progress information where possible.
     *
     * Serialization semantics:
     * - Implementations should serialize persistent object state only.
     * - Serialization should avoid mutating observable application state where
     *   possible.
     *
     * @return Async serialization result containing workflow state and the
     *         serialized QVariantMap result.
     */
    virtual AsyncToVariantMapResult toVariantMapAsync() const;

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

public:

    /**
     * Convenience wrapper that reports to the currently active operation context, if any.
     * @param scope Scope of the warning (e.g. the name of the serializable object or the name of the property that is being serialized)
     * @param message Warning message
     */
    static void reportSerializationWarning(const QString& scope, const QString& message);

    /**
     * Convenience wrapper that reports to the currently active operation context, if any.
     * @param scope Scope of the error (e.g. the name of the serializable object or the name of the property that is being serialized)
     * @param message Error message
     */
    static void reportSerializationError(QString scope, QString message);

    /**
     * Convenience wrapper that reports to the currently active operation context, if any.
     * @param scope Scope of the fatal error (e.g. the name of the serializable object or the name of the property that is being serialized)
     * @param message Fatal error message
     */
    static void reportFatalSerializationError(QString scope, QString message);

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

